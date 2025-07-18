
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_CONTROLLER_H
#define KLBN_CONTROLLER_H

#include "klbn_types.h"

/**
 * Initialize the controller state. Call once at startup.
 */
void klbn_controller_init(void);

/**
 * Process sensor data and update actuator commands.
 * Handles radio communication and LED status indicators.
 */
void klbn_controller_process(const klbn_sensor_data_t *in,
                             klbn_actuator_command_t *out);

/**
 * Handle mode button events with debounce.
 * Switches radio modes and sends test messages.
 */
void klbn_controller_process_mode_button(const klbn_mode_button_event_t *event,
                                         klbn_actuator_command_t *command);

#endif /* KLBN_CONTROLLER_H */
