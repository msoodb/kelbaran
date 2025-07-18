/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_nrf24l01.h"
#include "klbn_spi.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "klbn_delay.h"
#include "stm32f1xx.h"

// Default configuration
static const klbn_nrf24l01_config_t default_config = {
    .channel = NRF24L01_DEFAULT_CHANNEL,
    .power = KLBN_NRF24L01_POWER_0DBM,
    .datarate = KLBN_NRF24L01_DATARATE_1MBPS,
    .retries = NRF24L01_DEFAULT_RETRIES,
    .delay = NRF24L01_DEFAULT_DELAY,
    .auto_ack = true,
    .dynamic_payload = false,
    .payload_size = 32
};

static bool nrf24l01_initialized = false;
static klbn_nrf24l01_config_t current_config;

/**
 * @brief Configure NRF24L01 GPIO pins
 */
static void klbn_nrf24l01_configure_gpio(void) {
    // Configure CE pin as output
    klbn_gpio_config_output((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
    
    // Configure IRQ pin as input with pull-up
    klbn_gpio_config_input_pullup((uint32_t)KLBN_NRF24L01_IRQ_PORT, KLBN_NRF24L01_IRQ_PIN);
    
    // Set CE low initially
    klbn_gpio_clear_pin((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
}

/**
 * @brief Read register from NRF24L01
 */
static uint8_t klbn_nrf24l01_read_register(uint8_t reg) {
    uint8_t value;
    
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24L01_CMD_R_REGISTER | reg);
    value = klbn_spi_transfer(0xFF);
    klbn_spi_cs_high();
    
    return value;
}

/**
 * @brief Write register to NRF24L01
 */
static void klbn_nrf24l01_write_register(uint8_t reg, uint8_t value) {
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24L01_CMD_W_REGISTER | reg);
    klbn_spi_transfer(value);
    klbn_spi_cs_high();
}

/**
 * @brief Read multiple bytes from register
 */
/*static void klbn_nrf24l01_read_register_multi(uint8_t reg, uint8_t *data, uint8_t length) {
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24L01_CMD_R_REGISTER | reg);
    
    for (uint8_t i = 0; i < length; i++) {
        data[i] = klbn_spi_transfer(0xFF);
    }
    
    klbn_spi_cs_high();
    }*/

/**
 * @brief Write multiple bytes to register
 */
static void klbn_nrf24l01_write_register_multi(uint8_t reg, const uint8_t *data, uint8_t length) {
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24L01_CMD_W_REGISTER | reg);
    
    for (uint8_t i = 0; i < length; i++) {
        klbn_spi_transfer(data[i]);
    }
    
    klbn_spi_cs_high();
}

/**
 * @brief Send command to NRF24L01
 */
static uint8_t klbn_nrf24l01_send_command(uint8_t cmd) {
    uint8_t status;
    
    klbn_spi_cs_low();
    status = klbn_spi_transfer(cmd);
    klbn_spi_cs_high();
    
    return status;
}

klbn_nrf24l01_error_t klbn_nrf24l01_init(const klbn_nrf24l01_config_t *config) {
    // Use default config if none provided
    if (config == NULL) {
        config = &default_config;
    }
    
    // Store current config
    current_config = *config;
    
    // Initialize SPI if not already done
    if (klbn_spi_init(NULL) != KLBN_SPI_OK) {
        return KLBN_NRF24L01_ERROR_NOT_FOUND;
    }
    
    // Configure GPIO pins
    klbn_nrf24l01_configure_gpio();
    
    // Power down mode
    klbn_nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x00);
    klbn_delay_ms(2);
    
    // Test if NRF24L01 is present
    if (!klbn_nrf24l01_test()) {
        return KLBN_NRF24L01_ERROR_NOT_FOUND;
    }
    
    // Configure auto acknowledgment
    uint8_t en_aa = config->auto_ack ? 0x01 : 0x00;
    klbn_nrf24l01_write_register(NRF24L01_REG_EN_AA, en_aa);
    
    // Enable RX pipe 0
    klbn_nrf24l01_write_register(NRF24L01_REG_EN_RXADDR, 0x01);
    
    // Set address width to 5 bytes
    klbn_nrf24l01_write_register(NRF24L01_REG_SETUP_AW, 0x03);
    
    // Set auto retransmit
    uint8_t setup_retr = (config->delay << 4) | (config->retries & 0x0F);
    klbn_nrf24l01_write_register(NRF24L01_REG_SETUP_RETR, setup_retr);
    
    // Set RF channel
    klbn_nrf24l01_write_register(NRF24L01_REG_RF_CH, config->channel & 0x7F);
    
    // Set RF setup (power and data rate)
    uint8_t rf_setup = 0;
    
    // Set power level
    switch (config->power) {
        case KLBN_NRF24L01_POWER_M18DBM:
            rf_setup |= 0x00;
            break;
        case KLBN_NRF24L01_POWER_M12DBM:
            rf_setup |= 0x02;
            break;
        case KLBN_NRF24L01_POWER_M6DBM:
            rf_setup |= 0x04;
            break;
        case KLBN_NRF24L01_POWER_0DBM:
            rf_setup |= 0x06;
            break;
    }
    
    // Set data rate
    switch (config->datarate) {
        case KLBN_NRF24L01_DATARATE_250KBPS:
            rf_setup |= NRF24L01_RF_SETUP_RF_DR_LOW;
            break;
        case KLBN_NRF24L01_DATARATE_1MBPS:
            // Default, no bits set
            break;
        case KLBN_NRF24L01_DATARATE_2MBPS:
            rf_setup |= NRF24L01_RF_SETUP_RF_DR_HIGH;
            break;
    }
    
    klbn_nrf24l01_write_register(NRF24L01_REG_RF_SETUP, rf_setup);
    
    // Set payload size for pipe 0
    klbn_nrf24l01_write_register(NRF24L01_REG_RX_PW_P0, config->payload_size);
    
    // Configure dynamic payload if enabled
    if (config->dynamic_payload) {
        klbn_nrf24l01_write_register(NRF24L01_REG_FEATURE, 0x04);
        klbn_nrf24l01_write_register(NRF24L01_REG_DYNPD, 0x01);
    }
    
    // Clear status flags
    klbn_nrf24l01_clear_status(NRF24L01_STATUS_RX_DR | NRF24L01_STATUS_TX_DS | NRF24L01_STATUS_MAX_RT);
    
    // Flush TX and RX FIFOs
    klbn_nrf24l01_send_command(NRF24L01_CMD_FLUSH_TX);
    klbn_nrf24l01_send_command(NRF24L01_CMD_FLUSH_RX);
    
    // Power up in standby mode
    klbn_nrf24l01_write_register(NRF24L01_REG_CONFIG, 
        NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_CRCO | NRF24L01_CONFIG_PWR_UP);
    
    klbn_delay_ms(2);
    
    nrf24l01_initialized = true;
    return KLBN_NRF24L01_OK;
}

klbn_nrf24l01_error_t klbn_nrf24l01_deinit(void) {
    if (!nrf24l01_initialized) {
        return KLBN_NRF24L01_ERROR_NOT_INITIALIZED;
    }
    
    // Power down
    klbn_nrf24l01_write_register(NRF24L01_REG_CONFIG, 0x00);
    klbn_gpio_clear_pin((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
    
    nrf24l01_initialized = false;
    return KLBN_NRF24L01_OK;
}

klbn_nrf24l01_error_t klbn_nrf24l01_set_mode(klbn_nrf24l01_mode_t mode) {
    if (!nrf24l01_initialized) {
        return KLBN_NRF24L01_ERROR_NOT_INITIALIZED;
    }
    
    uint8_t config = klbn_nrf24l01_read_register(NRF24L01_REG_CONFIG);
    
    switch (mode) {
        case KLBN_NRF24L01_MODE_POWER_DOWN:
            config &= ~NRF24L01_CONFIG_PWR_UP;
            klbn_gpio_clear_pin((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
            break;
            
        case KLBN_NRF24L01_MODE_STANDBY:
            config |= NRF24L01_CONFIG_PWR_UP;
            config &= ~NRF24L01_CONFIG_PRIM_RX;
            klbn_gpio_clear_pin((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
            break;
            
        case KLBN_NRF24L01_MODE_TX:
            config |= NRF24L01_CONFIG_PWR_UP;
            config &= ~NRF24L01_CONFIG_PRIM_RX;
            klbn_gpio_set_pin((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
            break;
            
        case KLBN_NRF24L01_MODE_RX:
            config |= NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_PRIM_RX;
            klbn_gpio_set_pin((uint32_t)KLBN_NRF24L01_CE_PORT, KLBN_NRF24L01_CE_PIN);
            break;
    }
    
    klbn_nrf24l01_write_register(NRF24L01_REG_CONFIG, config);
    klbn_delay_us(130); // Mode change delay
    
    return KLBN_NRF24L01_OK;
}

klbn_nrf24l01_error_t klbn_nrf24l01_set_tx_address(const uint8_t *address) {
    if (!nrf24l01_initialized || address == NULL) {
        return KLBN_NRF24L01_ERROR_INVALID_PARAM;
    }
    
    klbn_nrf24l01_write_register_multi(NRF24L01_REG_TX_ADDR, address, NRF24L01_ADDRESS_WIDTH);
    
    // For auto-ack, RX pipe 0 must have same address as TX
    if (current_config.auto_ack) {
        klbn_nrf24l01_write_register_multi(NRF24L01_REG_RX_ADDR_P0, address, NRF24L01_ADDRESS_WIDTH);
    }
    
    return KLBN_NRF24L01_OK;
}

klbn_nrf24l01_error_t klbn_nrf24l01_set_rx_address(const uint8_t *address) {
    if (!nrf24l01_initialized || address == NULL) {
        return KLBN_NRF24L01_ERROR_INVALID_PARAM;
    }
    
    klbn_nrf24l01_write_register_multi(NRF24L01_REG_RX_ADDR_P0, address, NRF24L01_ADDRESS_WIDTH);
    
    return KLBN_NRF24L01_OK;
}

klbn_nrf24l01_error_t klbn_nrf24l01_transmit(const uint8_t *data, uint8_t length) {
    if (!nrf24l01_initialized || data == NULL || length == 0 || length > NRF24L01_MAX_PAYLOAD_SIZE) {
        return KLBN_NRF24L01_ERROR_INVALID_PARAM;
    }
    
    // Set to standby mode
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_STANDBY);
    
    // Clear TX FIFO
    klbn_nrf24l01_send_command(NRF24L01_CMD_FLUSH_TX);
    
    // Write payload
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24L01_CMD_W_TX_PAYLOAD);
    for (uint8_t i = 0; i < length; i++) {
        klbn_spi_transfer(data[i]);
    }
    klbn_spi_cs_high();
    
    // Enter TX mode
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_TX);
    
    // Wait for transmission to complete (with timeout)
    uint32_t timeout = 10000;
    while (timeout--) {
        uint8_t status = klbn_nrf24l01_get_status();
        if (status & (NRF24L01_STATUS_TX_DS | NRF24L01_STATUS_MAX_RT)) {
            break;
        }
        klbn_delay_us(10);
    }
    
    if (timeout == 0) {
        return KLBN_NRF24L01_ERROR_TIMEOUT;
    }
    
    // Check if transmission was successful
    uint8_t status = klbn_nrf24l01_get_status();
    if (status & NRF24L01_STATUS_MAX_RT) {
        klbn_nrf24l01_clear_status(NRF24L01_STATUS_MAX_RT);
        return KLBN_NRF24L01_ERROR_TX_FAILED;
    }
    
    // Clear TX_DS flag
    klbn_nrf24l01_clear_status(NRF24L01_STATUS_TX_DS);
    
    return KLBN_NRF24L01_OK;
}

klbn_nrf24l01_error_t klbn_nrf24l01_receive(uint8_t *data, uint8_t *length) {
    if (!nrf24l01_initialized || data == NULL || length == NULL) {
        return KLBN_NRF24L01_ERROR_INVALID_PARAM;
    }
    
    // Check if data is available
    if (!klbn_nrf24l01_data_available()) {
        return KLBN_NRF24L01_ERROR_RX_EMPTY;
    }
    
    // Get payload width
    uint8_t payload_width;
    if (current_config.dynamic_payload) {
        klbn_spi_cs_low();
        klbn_spi_transfer(NRF24L01_CMD_R_RX_PL_WID);
        payload_width = klbn_spi_transfer(0xFF);
        klbn_spi_cs_high();
        
        if (payload_width > NRF24L01_MAX_PAYLOAD_SIZE) {
            // Invalid payload width, flush RX FIFO
            klbn_nrf24l01_send_command(NRF24L01_CMD_FLUSH_RX);
            return KLBN_NRF24L01_ERROR_RX_EMPTY;
        }
    } else {
        payload_width = current_config.payload_size;
    }
    
    // Read payload
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24L01_CMD_R_RX_PAYLOAD);
    for (uint8_t i = 0; i < payload_width; i++) {
        data[i] = klbn_spi_transfer(0xFF);
    }
    klbn_spi_cs_high();
    
    *length = payload_width;
    
    // Clear RX_DR flag
    klbn_nrf24l01_clear_status(NRF24L01_STATUS_RX_DR);
    
    return KLBN_NRF24L01_OK;
}

bool klbn_nrf24l01_data_available(void) {
    if (!nrf24l01_initialized) {
        return false;
    }
    
    uint8_t status = klbn_nrf24l01_get_status();
    return (status & NRF24L01_STATUS_RX_DR) != 0;
}

bool klbn_nrf24l01_tx_complete(void) {
    if (!nrf24l01_initialized) {
        return false;
    }
    
    uint8_t status = klbn_nrf24l01_get_status();
    return (status & NRF24L01_STATUS_TX_DS) != 0;
}

uint8_t klbn_nrf24l01_get_status(void) {
    return klbn_nrf24l01_send_command(NRF24L01_CMD_NOP);
}

void klbn_nrf24l01_clear_status(uint8_t flags) {
    klbn_nrf24l01_write_register(NRF24L01_REG_STATUS, flags);
}

bool klbn_nrf24l01_test(void) {
    // Test by reading CONFIG register (should be writable)
    uint8_t test_value = 0x55;
    uint8_t original = klbn_nrf24l01_read_register(NRF24L01_REG_CONFIG);
    
    klbn_nrf24l01_write_register(NRF24L01_REG_CONFIG, test_value);
    uint8_t read_value = klbn_nrf24l01_read_register(NRF24L01_REG_CONFIG);
    
    klbn_nrf24l01_write_register(NRF24L01_REG_CONFIG, original);
    
    return (read_value == test_value);
}
