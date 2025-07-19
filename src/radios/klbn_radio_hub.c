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
#include "klbn_pairing.h"
#include "klbn_types.h"

void klbn_radio_hub_init(void) {
  klbn_nrf24l01_module_init();
  klbn_pairing_init();
}

bool klbn_radio_hub_read(klbn_radio_data_t *out) {
  if (!out) {
    return false;
  }

  // Don't read data during pairing mode - let pairing handle it directly
  klbn_pair_state_t pairing_state = klbn_pairing_get_state();
  if (pairing_state != KLBN_PAIR_STATE_NORMAL && pairing_state != KLBN_PAIR_STATE_PAIRED) {
    return false;
  }

  // Try to read radio data only when not pairing
  if (klbn_nrf24l01_module_read(out)) {
    // Normal data message
    return true;
  }
  
  return false;
}

void klbn_radio_hub_send(const klbn_radio_command_t *cmd) {
  if (!cmd) {
    return;
  }

  // Only send if paired (or use for pairing process)
  if (klbn_pairing_is_paired()) {
    klbn_nrf24l01_module_apply(cmd);
  }
}

void klbn_radio_hub_tick(void) {
  // Update pairing state machine
  klbn_pairing_tick();
}

void klbn_radio_hub_start_pairing(void) {
  klbn_pairing_start();
}