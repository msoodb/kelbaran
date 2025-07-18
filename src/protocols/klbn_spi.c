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

void klbn_spi_init(void) {
    // Enable SPI1 and GPIOA clocks
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    
    // Configure SPI pins
    // PA5 (SCK) - Alternate function push-pull
    klbn_gpio_config_alternate_pushpull((uint32_t)GPIOA, 5);
    
    // PA6 (MISO) - Input floating
    klbn_gpio_config_input((uint32_t)GPIOA, 6);
    
    // PA7 (MOSI) - Alternate function push-pull
    klbn_gpio_config_alternate_pushpull((uint32_t)GPIOA, 7);
    
    // Configure CSN pin (PA3) as output
    klbn_gpio_config_output((uint32_t)KLBN_RADIO_CSN_PORT, KLBN_RADIO_CSN_PIN);
    klbn_gpio_set_pin((uint32_t)KLBN_RADIO_CSN_PORT, KLBN_RADIO_CSN_PIN); // CSN high by default
    
    // Configure SPI1
    SPI1->CR1 = 0; // Reset SPI1
    SPI1->CR1 |= SPI_CR1_MSTR;        // Master mode
    SPI1->CR1 |= SPI_CR1_BR_2;        // Baud rate = fPCLK/32 (72MHz/32 = 2.25MHz)
    SPI1->CR1 |= SPI_CR1_CPOL;        // Clock polarity high when idle
    SPI1->CR1 |= SPI_CR1_CPHA;        // Clock phase - data captured on second edge
    SPI1->CR1 |= SPI_CR1_SSM;         // Software slave management
    SPI1->CR1 |= SPI_CR1_SSI;         // Internal slave select
    
    // Enable SPI1
    SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t klbn_spi_transfer(uint8_t data) {
    // Wait for TX buffer to be empty
    while (!(SPI1->SR & SPI_SR_TXE));
    
    // Send data
    SPI1->DR = data;
    
    // Wait for RX buffer to have data
    while (!(SPI1->SR & SPI_SR_RXNE));
    
    // Return received data
    return (uint8_t)SPI1->DR;
}

void klbn_spi_transfer_multi(const uint8_t *tx_data, uint8_t *rx_data, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        uint8_t tx_byte = tx_data ? tx_data[i] : 0x00;
        uint8_t rx_byte = klbn_spi_transfer(tx_byte);
        if (rx_data) {
            rx_data[i] = rx_byte;
        }
    }
}

void klbn_spi_cs_low(void) {
    klbn_gpio_clear_pin((uint32_t)KLBN_RADIO_CSN_PORT, KLBN_RADIO_CSN_PIN);
}

void klbn_spi_cs_high(void) {
    klbn_gpio_set_pin((uint32_t)KLBN_RADIO_CSN_PORT, KLBN_RADIO_CSN_PIN);
}