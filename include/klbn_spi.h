/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_SPI_H
#define KLBN_SPI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief SPI error codes
 */
typedef enum {
    KLBN_SPI_OK = 0,
    KLBN_SPI_ERROR_TIMEOUT,
    KLBN_SPI_ERROR_BUSY,
    KLBN_SPI_ERROR_NULL_PTR,
    KLBN_SPI_ERROR_NOT_INITIALIZED
} klbn_spi_error_t;

/**
 * @brief SPI configuration structure
 */
typedef struct {
    uint32_t prescaler;     // SPI_BaudRatePrescaler_X (2, 4, 8, 16, 32, 64, 128, 256)
    uint32_t cpol;          // Clock polarity: 0 = low when idle, 1 = high when idle
    uint32_t cpha;          // Clock phase: 0 = first edge, 1 = second edge
    uint32_t data_size;     // Data size: 8 or 16 bits
    bool msb_first;         // true = MSB first, false = LSB first
} klbn_spi_config_t;

/**
 * @brief Initialize SPI1 peripheral
 * @param config SPI configuration (NULL for default)
 * @return Error code
 */
klbn_spi_error_t klbn_spi_init(const klbn_spi_config_t *config);

/**
 * @brief Deinitialize SPI1 peripheral
 * @return Error code
 */
klbn_spi_error_t klbn_spi_deinit(void);

/**
 * @brief Transfer a single byte
 * @param tx_data Data to transmit
 * @return Received data
 */
uint8_t klbn_spi_transfer(uint8_t tx_data);

/**
 * @brief Transfer multiple bytes
 * @param tx_data Transmit buffer (NULL for receive-only)
 * @param rx_data Receive buffer (NULL for transmit-only)
 * @param length Number of bytes to transfer
 * @return Error code
 */
klbn_spi_error_t klbn_spi_transfer_multi(const uint8_t *tx_data, uint8_t *rx_data, size_t length);

/**
 * @brief Set chip select low (start communication)
 */
void klbn_spi_cs_low(void);

/**
 * @brief Set chip select high (end communication)
 */
void klbn_spi_cs_high(void);

/**
 * @brief Check if SPI is busy
 * @return true if busy, false if idle
 */
bool klbn_spi_is_busy(void);

/**
 * @brief Get SPI status
 * @return SPI status register value
 */
uint32_t klbn_spi_get_status(void);

#endif /* KLBN_SPI_H */