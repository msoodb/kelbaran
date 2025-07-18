/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_PAIRING_H
#define KLBN_PAIRING_H

#include "klbn_types.h"
#include <stdbool.h>

/**
 * @brief Initialize pairing system
 */
void klbn_pairing_init(void);

/**
 * @brief Start pairing process (called on long button press)
 */
void klbn_pairing_start(void);

/**
 * @brief Process pairing tick (called periodically)
 */
void klbn_pairing_tick(void);

/**
 * @brief Process received pairing message
 * @param data Received radio data
 * @return true if message was a pairing message
 */
bool klbn_pairing_process_message(const klbn_radio_data_t *data);

/**
 * @brief Get current pairing state
 * @return Current pairing state
 */
klbn_pair_state_t klbn_pairing_get_state(void);

/**
 * @brief Check if paired
 * @return true if paired with another board
 */
bool klbn_pairing_is_paired(void);

/**
 * @brief Get paired board address
 * @param address Output address buffer (5 bytes)
 * @return true if paired and address copied
 */
bool klbn_pairing_get_paired_address(uint8_t *address);

/**
 * @brief Reset pairing (unpair)
 */
void klbn_pairing_reset(void);

#endif /* KLBN_PAIRING_H */