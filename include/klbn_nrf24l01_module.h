/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_NRF24L01_MODULE_H
#define KLBN_NRF24L01_MODULE_H

#include "klbn_types.h"

void klbn_nrf24l01_module_init(void);
bool klbn_nrf24l01_module_read(klbn_radio_data_t *out);
void klbn_nrf24l01_module_apply(const klbn_radio_command_t *cmd);

#endif // KLBN_NRF24L01_MODULE_H