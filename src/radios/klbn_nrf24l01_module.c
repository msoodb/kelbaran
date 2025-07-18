/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_nrf24l01_module.h"
#include "klbn_nrf24l01.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdbool.h>

static bool module_initialized = false;

void klbn_nrf24l01_module_init(void) {
  // Initialize NRF24L01 with default settings
  if (klbn_nrf24l01_init(NULL) == KLBN_NRF24L01_OK) {
    // Set default addresses
    uint8_t address[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    klbn_nrf24l01_set_tx_address(address);
    klbn_nrf24l01_set_rx_address(address);
    
    // Start in RX mode
    klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
    
    module_initialized = true;
  }
}

bool klbn_nrf24l01_module_read(klbn_radio_data_t *out) {
  if (!out || !module_initialized) {
    return false;
  }

  // Check for incoming radio data
  if (klbn_nrf24l01_data_available()) {
    if (klbn_nrf24l01_receive(out->data, &out->length) == KLBN_NRF24L01_OK) {
      out->timestamp = xTaskGetTickCount();
      return true;
    }
  }
  
  return false;
}

void klbn_nrf24l01_module_apply(const klbn_radio_command_t *cmd) {
  if (!cmd || !module_initialized) {
    return;
  }

  // Switch to TX mode, send, then back to RX
  klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_TX);
  klbn_nrf24l01_transmit(cmd->data, cmd->length);
  klbn_nrf24l01_set_mode(KLBN_NRF24L01_MODE_RX);
}