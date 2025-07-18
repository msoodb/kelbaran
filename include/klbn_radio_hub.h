/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_RADIO_HUB_H
#define KLBN_RADIO_HUB_H

#include <stdbool.h>
#include "klbn_types.h"

/**
 * @brief Initialize radio hub
 */
void klbn_radio_hub_init(void);

/**
 * @brief Read radio data (sensor-like)
 * @param out Output data structure
 * @return true if data available, false otherwise
 */
bool klbn_radio_hub_read(klbn_radio_data_t *out);

/**
 * @brief Send radio command (actuator-like)
 * @param cmd Command structure
 */
void klbn_radio_hub_send(const klbn_radio_command_t *cmd);

/**
 * @brief Tick function for pairing state machine
 */
void klbn_radio_hub_tick(void);

/**
 * @brief Start pairing process
 */
void klbn_radio_hub_start_pairing(void);

#endif /* KLBN_RADIO_HUB_H */