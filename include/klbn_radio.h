/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_RADIO_H
#define KLBN_RADIO_H

#include "klbn_state.h"
#include "klbn_types.h"
#include "klbn_nrf24l01.h"
#include <stdint.h>
#include <stdbool.h>

// Radio message structure definition (now that we have all types)
struct klbn_radio_message_t {
    klbn_radio_event_t event;
    klbn_radio_packet_t packet;
    klbn_radio_state_t new_state;
};

// Function declarations
void klbn_radio_init(void);
void klbn_radio_switch_mode(void);
klbn_radio_state_t klbn_radio_get_current_state(void);
bool klbn_radio_send_data(const uint8_t *data, uint8_t length);
bool klbn_radio_is_data_available(void);
bool klbn_radio_receive_data(uint8_t *data, uint8_t *length);
const char* klbn_radio_get_mode_string(void);

#endif /* KLBN_RADIO_H */