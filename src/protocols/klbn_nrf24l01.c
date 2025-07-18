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
#include "klbn_pins.h"
#include "klbn_gpio.h"
#include "klbn_delay.h"
#include "libc_stubs.h"

// Static variables
static klbn_radio_mode_t current_mode = KLBN_RADIO_MODE_RX;
static const uint8_t rx_addr_p0[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
static const uint8_t rx_addr_p1[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
static const uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};

// Internal functions
static uint8_t nrf24_read_register(uint8_t reg);
static void nrf24_write_register(uint8_t reg, uint8_t value);
static void nrf24_write_register_multi(uint8_t reg, const uint8_t *data, uint8_t length);
static uint8_t nrf24_get_status(void);
static void nrf24_ce_high(void);
static void nrf24_ce_low(void);

void klbn_nrf24l01_init(void) {
    // Initialize SPI
    klbn_spi_init();
    
    // Configure CE pin as output
    klbn_gpio_config_output((uint32_t)KLBN_RADIO_CE_PORT, KLBN_RADIO_CE_PIN);
    
    // Configure IRQ pin as input with pull-up
    klbn_gpio_config_input_pullup((uint32_t)KLBN_RADIO_IRQ_PORT, KLBN_RADIO_IRQ_PIN);
    
    // Ensure CE is low initially
    nrf24_ce_low();
    
    // Wait for power-up
    klbn_delay_ms(100);
    
    // Configure NRF24L01
    // Power up, enable CRC, 2-byte CRC, RX mode
    nrf24_write_register(NRF24_REG_CONFIG, 
                        NRF24_CONFIG_PWR_UP | NRF24_CONFIG_EN_CRC | NRF24_CONFIG_CRCO);
    
    // Enable auto-acknowledgment on pipe 0 and 1
    nrf24_write_register(NRF24_REG_EN_AA, 0x03);
    
    // Enable RX addresses for pipe 0 and 1
    nrf24_write_register(NRF24_REG_EN_RXADDR, 0x03);
    
    // Setup address width (5 bytes)
    nrf24_write_register(NRF24_REG_SETUP_AW, 0x03);
    
    // Setup automatic retransmission (750us delay, 3 retries)
    nrf24_write_register(NRF24_REG_SETUP_RETR, 0x23);
    
    // Set RF channel (2.4GHz + 76 = 2.476GHz)
    nrf24_write_register(NRF24_REG_RF_CH, 76);
    
    // Set RF power (0dBm) and data rate (2Mbps)
    nrf24_write_register(NRF24_REG_RF_SETUP, 
                        NRF24_RF_SETUP_RF_PWR_0DBM | NRF24_RF_SETUP_RF_DR_2MBPS);
    
    // Set RX addresses
    nrf24_write_register_multi(NRF24_REG_RX_ADDR_P0, rx_addr_p0, 5);
    nrf24_write_register_multi(NRF24_REG_RX_ADDR_P1, rx_addr_p1, 5);
    
    // Set TX address
    nrf24_write_register_multi(NRF24_REG_TX_ADDR, tx_addr, 5);
    
    // Set payload size for pipe 0 and 1
    nrf24_write_register(NRF24_REG_RX_PW_P0, NRF24_MAX_PAYLOAD_SIZE);
    nrf24_write_register(NRF24_REG_RX_PW_P1, NRF24_MAX_PAYLOAD_SIZE);
    
    // Clear interrupts
    nrf24_write_register(NRF24_REG_STATUS, 0x70);
    
    // Flush buffers
    klbn_nrf24l01_flush_buffers();
    
    // Set to RX mode initially
    klbn_nrf24l01_set_mode(KLBN_RADIO_MODE_RX);
    
    // Wait for configuration to settle
    klbn_delay_ms(2);
}

bool klbn_nrf24l01_is_present(void) {
    // Try to read and write to SETUP_AW register
    uint8_t original = nrf24_read_register(NRF24_REG_SETUP_AW);
    nrf24_write_register(NRF24_REG_SETUP_AW, 0x55);
    uint8_t test = nrf24_read_register(NRF24_REG_SETUP_AW);
    nrf24_write_register(NRF24_REG_SETUP_AW, original);
    
    return (test == 0x55);
}

void klbn_nrf24l01_set_mode(klbn_radio_mode_t mode) {
    current_mode = mode;
    
    // Set CE low during mode change
    nrf24_ce_low();
    
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    
    if (mode == KLBN_RADIO_MODE_RX) {
        // Set PRIM_RX bit for RX mode
        config |= NRF24_CONFIG_PRIM_RX;
        nrf24_write_register(NRF24_REG_CONFIG, config);
        
        // Set CE high to start listening
        nrf24_ce_high();
    } else {
        // Clear PRIM_RX bit for TX mode
        config &= ~NRF24_CONFIG_PRIM_RX;
        nrf24_write_register(NRF24_REG_CONFIG, config);
        
        // CE stays low until transmit
    }
    
    // Allow time for mode change
    klbn_delay_us(150);
}

klbn_radio_mode_t klbn_nrf24l01_get_mode(void) {
    return current_mode;
}

bool klbn_nrf24l01_transmit(const klbn_radio_packet_t *packet) {
    if (!packet || packet->length > NRF24_MAX_PAYLOAD_SIZE) {
        return false;
    }
    
    // Ensure we're in TX mode
    if (current_mode != KLBN_RADIO_MODE_TX) {
        klbn_nrf24l01_set_mode(KLBN_RADIO_MODE_TX);
    }
    
    // Clear TX interrupts
    nrf24_write_register(NRF24_REG_STATUS, NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
    
    // Write payload
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24_CMD_W_TX_PAYLOAD);
    klbn_spi_transfer_multi(packet->data, NULL, packet->length);
    klbn_spi_cs_high();
    
    // Pulse CE to start transmission
    nrf24_ce_high();
    klbn_delay_us(15); // At least 10us
    nrf24_ce_low();
    
    // Wait for transmission to complete (with timeout)
    uint32_t timeout = 1000; // 1ms timeout
    while (timeout--) {
        uint8_t status = nrf24_get_status();
        if (status & (NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT)) {
            // Clear interrupt flags
            nrf24_write_register(NRF24_REG_STATUS, status & 0x70);
            return (status & NRF24_STATUS_TX_DS) != 0;
        }
        klbn_delay_us(1);
    }
    
    return false; // Timeout
}

bool klbn_nrf24l01_receive(klbn_radio_packet_t *packet) {
    if (!packet) {
        return false;
    }
    
    // Ensure we're in RX mode
    if (current_mode != KLBN_RADIO_MODE_RX) {
        klbn_nrf24l01_set_mode(KLBN_RADIO_MODE_RX);
    }
    
    uint8_t status = nrf24_get_status();
    
    // Check if data is available
    if (status & NRF24_STATUS_RX_DR) {
        // Get payload size
        uint8_t payload_size;
        klbn_spi_cs_low();
        klbn_spi_transfer(0x60); // R_RX_PL_WID command
        payload_size = klbn_spi_transfer(0x00);
        klbn_spi_cs_high();
        
        if (payload_size <= NRF24_MAX_PAYLOAD_SIZE) {
            // Read payload
            klbn_spi_cs_low();
            klbn_spi_transfer(NRF24_CMD_R_RX_PAYLOAD);
            klbn_spi_transfer_multi(NULL, packet->data, payload_size);
            klbn_spi_cs_high();
            
            packet->length = payload_size;
            
            // Clear RX interrupt
            nrf24_write_register(NRF24_REG_STATUS, NRF24_STATUS_RX_DR);
            
            return true;
        } else {
            // Invalid payload size, flush RX buffer
            klbn_spi_cs_low();
            klbn_spi_transfer(NRF24_CMD_FLUSH_RX);
            klbn_spi_cs_high();
        }
    }
    
    return false;
}

klbn_radio_status_t klbn_nrf24l01_get_status(void) {
    uint8_t status = nrf24_get_status();
    
    if (status & NRF24_STATUS_RX_DR) {
        return KLBN_RADIO_STATUS_RX_READY;
    } else if (status & NRF24_STATUS_TX_DS) {
        return KLBN_RADIO_STATUS_TX_SUCCESS;
    } else if (status & NRF24_STATUS_MAX_RT) {
        return KLBN_RADIO_STATUS_TX_FAILED;
    }
    
    return KLBN_RADIO_STATUS_IDLE;
}

void klbn_nrf24l01_flush_buffers(void) {
    // Flush TX buffer
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24_CMD_FLUSH_TX);
    klbn_spi_cs_high();
    
    // Flush RX buffer
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24_CMD_FLUSH_RX);
    klbn_spi_cs_high();
}

void klbn_nrf24l01_power_up(void) {
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    config |= NRF24_CONFIG_PWR_UP;
    nrf24_write_register(NRF24_REG_CONFIG, config);
    klbn_delay_ms(2); // Wait for power-up
}

void klbn_nrf24l01_power_down(void) {
    nrf24_ce_low();
    uint8_t config = nrf24_read_register(NRF24_REG_CONFIG);
    config &= ~NRF24_CONFIG_PWR_UP;
    nrf24_write_register(NRF24_REG_CONFIG, config);
}

// Internal functions
static uint8_t nrf24_read_register(uint8_t reg) {
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24_CMD_R_REGISTER | (reg & 0x1F));
    uint8_t value = klbn_spi_transfer(0x00);
    klbn_spi_cs_high();
    return value;
}

static void nrf24_write_register(uint8_t reg, uint8_t value) {
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    klbn_spi_transfer(value);
    klbn_spi_cs_high();
}

static void nrf24_write_register_multi(uint8_t reg, const uint8_t *data, uint8_t length) {
    klbn_spi_cs_low();
    klbn_spi_transfer(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    klbn_spi_transfer_multi(data, NULL, length);
    klbn_spi_cs_high();
}

static uint8_t nrf24_get_status(void) {
    klbn_spi_cs_low();
    uint8_t status = klbn_spi_transfer(NRF24_CMD_NOP);
    klbn_spi_cs_high();
    return status;
}

static void nrf24_ce_high(void) {
    klbn_gpio_set_pin((uint32_t)KLBN_RADIO_CE_PORT, KLBN_RADIO_CE_PIN);
}

static void nrf24_ce_low(void) {
    klbn_gpio_clear_pin((uint32_t)KLBN_RADIO_CE_PORT, KLBN_RADIO_CE_PIN);
}