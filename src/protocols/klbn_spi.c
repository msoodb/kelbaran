/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_spi.h"
#include "klbn_pins.h"
#include "klbn_gpio.h"
#include "stm32f1xx.h"

// SPI1 timeout in loops (adjust based on system clock)
#define SPI_TIMEOUT_LOOPS 10000

// Default SPI configuration for most devices
static const klbn_spi_config_t default_config = {
    .prescaler = 16,        // 72MHz / 16 = 4.5MHz (safe for most devices)
    .cpol = 0,              // Clock low when idle
    .cpha = 0,              // First edge
    .data_size = 8,         // 8-bit data
    .msb_first = true       // MSB first
};

static bool spi_initialized = false;

/**
 * @brief Configure SPI1 GPIO pins
 */
static void klbn_spi_configure_gpio(void) {
    // Configure SPI1 pins:
    // PA5 (SCK) - Alternate function push-pull
    // PA6 (MISO) - Input floating
    // PA7 (MOSI) - Alternate function push-pull
    // PA3 (CS) - Output push-pull (GPIO controlled)
    
    klbn_gpio_config_alternate_pushpull((uint32_t)KLBN_SPI_SCK_PORT, KLBN_SPI_SCK_PIN);
    klbn_gpio_config_input((uint32_t)KLBN_SPI_MISO_PORT, KLBN_SPI_MISO_PIN);
    klbn_gpio_config_alternate_pushpull((uint32_t)KLBN_SPI_MOSI_PORT, KLBN_SPI_MOSI_PIN);
    klbn_gpio_config_output((uint32_t)KLBN_SPI_CS_PORT, KLBN_SPI_CS_PIN);
    
    // Set CS high initially (idle state)
    klbn_spi_cs_high();
}

/**
 * @brief Wait for SPI flag with timeout
 */
static klbn_spi_error_t klbn_spi_wait_flag(uint32_t flag, bool set) {
    volatile uint32_t timeout = SPI_TIMEOUT_LOOPS;
    
    while (timeout--) {
        if (set) {
            if (SPI1->SR & flag) {
                return KLBN_SPI_OK;
            }
        } else {
            if (!(SPI1->SR & flag)) {
                return KLBN_SPI_OK;
            }
        }
    }
    
    return KLBN_SPI_ERROR_TIMEOUT;
}

klbn_spi_error_t klbn_spi_init(const klbn_spi_config_t *config) {
    // Use default config if none provided
    if (config == NULL) {
        config = &default_config;
    }
    
    // Enable SPI1 clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    // Configure GPIO pins
    klbn_spi_configure_gpio();
    
    // Disable SPI during configuration
    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    // Configure SPI1
    uint32_t cr1 = 0;
    
    // Set prescaler
    switch (config->prescaler) {
        case 2:   cr1 |= (0 << 3); break;
        case 4:   cr1 |= (1 << 3); break;
        case 8:   cr1 |= (2 << 3); break;
        case 16:  cr1 |= (3 << 3); break;
        case 32:  cr1 |= (4 << 3); break;
        case 64:  cr1 |= (5 << 3); break;
        case 128: cr1 |= (6 << 3); break;
        case 256: cr1 |= (7 << 3); break;
        default:  cr1 |= (3 << 3); break; // Default to 16
    }
    
    // Set clock polarity and phase
    if (config->cpol) {
        cr1 |= SPI_CR1_CPOL;
    }
    if (config->cpha) {
        cr1 |= SPI_CR1_CPHA;
    }
    
    // Set data frame format
    if (config->data_size == 16) {
        cr1 |= SPI_CR1_DFF;
    }
    
    // Set bit order
    if (!config->msb_first) {
        cr1 |= SPI_CR1_LSBFIRST;
    }
    
    // Master mode, software NSS management
    cr1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
    
    // Write configuration
    SPI1->CR1 = cr1;
    
    // Enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
    
    spi_initialized = true;
    return KLBN_SPI_OK;
}

klbn_spi_error_t klbn_spi_deinit(void) {
    // Disable SPI
    SPI1->CR1 &= ~SPI_CR1_SPE;
    
    // Disable SPI clock
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    
    spi_initialized = false;
    return KLBN_SPI_OK;
}

uint8_t klbn_spi_transfer(uint8_t tx_data) {
    if (!spi_initialized) {
        return 0;
    }
    
    // Wait for TX buffer to be empty
    if (klbn_spi_wait_flag(SPI_SR_TXE, true) != KLBN_SPI_OK) {
        return 0;
    }
    
    // Send data
    SPI1->DR = tx_data;
    
    // Wait for RX buffer to have data
    if (klbn_spi_wait_flag(SPI_SR_RXNE, true) != KLBN_SPI_OK) {
        return 0;
    }
    
    // Return received data
    return (uint8_t)SPI1->DR;
}

klbn_spi_error_t klbn_spi_transfer_multi(const uint8_t *tx_data, uint8_t *rx_data, size_t length) {
    if (!spi_initialized) {
        return KLBN_SPI_ERROR_NOT_INITIALIZED;
    }
    
    if (length == 0) {
        return KLBN_SPI_OK;
    }
    
    if (tx_data == NULL && rx_data == NULL) {
        return KLBN_SPI_ERROR_NULL_PTR;
    }
    
    for (size_t i = 0; i < length; i++) {
        uint8_t tx_byte = (tx_data != NULL) ? tx_data[i] : 0x00;
        uint8_t rx_byte = klbn_spi_transfer(tx_byte);
        
        if (rx_data != NULL) {
            rx_data[i] = rx_byte;
        }
    }
    
    return KLBN_SPI_OK;
}

void klbn_spi_cs_low(void) {
    klbn_gpio_clear_pin((uint32_t)KLBN_SPI_CS_PORT, KLBN_SPI_CS_PIN);
}

void klbn_spi_cs_high(void) {
    klbn_gpio_set_pin((uint32_t)KLBN_SPI_CS_PORT, KLBN_SPI_CS_PIN);
}

bool klbn_spi_is_busy(void) {
    if (!spi_initialized) {
        return false;
    }
    
    return (SPI1->SR & SPI_SR_BSY) != 0;
}

uint32_t klbn_spi_get_status(void) {
    if (!spi_initialized) {
        return 0;
    }
    
    return SPI1->SR;
}