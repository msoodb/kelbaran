
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
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
 * Cycle between MANUAL/AUTO/EMERGENCY modes.
 * Updates the LED and stops motors.
 */
void klbn_controller_cycle_mode(klbn_actuator_command_t *out);

/**
 * Process sensor data and update actuator commands.
 * Typically called periodically in AUTO mode.
 */
void klbn_controller_process(const klbn_sensor_data_t *in,
                             klbn_actuator_command_t *out);

/**
 * Handle IR remote events in MANUAL mode.
 * Sets motion state and actuator outputs.
 */
void klbn_controller_process_ir_remote(const klbn_ir_remote_event_t *in,
                                       klbn_actuator_command_t *out);

/**
 * Handle joystick direction events in MANUAL mode.
 * Sets motion state and actuator outputs.
 */
void klbn_controller_process_joystick(const klbn_joystick_event_t *evt,
                                      klbn_actuator_command_t *out);

/**
 * Handle joystick button press events.
 * Typically used to cycle modes or trigger actions.
 */
void klbn_controller_process_joystick_click(const klbn_joystick_event_t *event,
                                            klbn_actuator_command_t *command);

/**
 * Handle mode button events with debounce.
 * Typically cycles modes and can trigger servo motions.
 */
void klbn_controller_process_mode_button(const klbn_mode_button_event_t *event,
                                         klbn_actuator_command_t *command);

#endif /* KLBN_CONTROLLER_H */
