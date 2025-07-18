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

void klbn_spi_init(void);
uint8_t klbn_spi_transfer(uint8_t data);
void klbn_spi_transfer_multi(const uint8_t *tx_data, uint8_t *rx_data, uint8_t length);
void klbn_spi_cs_low(void);
void klbn_spi_cs_high(void);

#endif /* KLBN_SPI_H */