

/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_STATE_H
#define KLBN_STATE_H

#include <stdbool.h>
#include <stdint.h>

// Radio states
typedef enum {
    KLBN_RADIO_STATE_IDLE = 0,
    KLBN_RADIO_STATE_TX_MODE,
    KLBN_RADIO_STATE_RX_MODE,
    KLBN_RADIO_STATE_TRANSMITTING,
    KLBN_RADIO_STATE_LISTENING,
    KLBN_RADIO_STATE_ERROR
} klbn_radio_state_t;

#endif // KLBN_STATE_H
