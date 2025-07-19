/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_radio_hub.h"
#include "klbn_nrf24l01_module.h"
#include "klbn_types.h"

void klbn_radio_hub_init(void) {
  klbn_nrf24l01_module_init();
}

bool klbn_radio_hub_read(klbn_radio_data_t *out) {
  if (!out) {
    return false;
  }

  return klbn_nrf24l01_module_read(out);
}

void klbn_radio_hub_apply(const klbn_radio_command_t *cmd) {
  if (!cmd) {
    return;
  }

  klbn_nrf24l01_module_apply(cmd);
}