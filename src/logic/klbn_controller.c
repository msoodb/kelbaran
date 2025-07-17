/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_controller.h"
#include "FreeRTOS.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "klbn_types.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

#define MODE_BUTTON_DEBOUNCE_MS 100
#define IR_CONTROL_TIMEOUT_MS 200

/* -------------------- Main Controller Initialization -------------------- */
void klbn_controller_init(void) {  
  // TODO: Initialize radio communication
}

/* -------------------- Main Processing Functions -------------------- */
void klbn_controller_process(const klbn_sensor_data_t *in,
                             klbn_actuator_command_t *out) {
  (void)in; 

  out->led.mode = KLBN_LED_MODE_BLINK;
  out->led.blink_speed_ms = 200;
 }

/* -------------------- Mode Button -------------------- */
void klbn_controller_process_mode_button(const klbn_mode_button_event_t *event,
                                         klbn_actuator_command_t *out) {
  (void) out;
  static uint32_t last_press_time = 0;  
  if (event->event_type == KLBN_MODE_BUTTON_EVENT_PRESSED) {
    uint32_t current_time = xTaskGetTickCount();
    
    // Debounce
    if (current_time - last_press_time < pdMS_TO_TICKS(MODE_BUTTON_DEBOUNCE_MS)) {
      return;
    }
    last_press_time = current_time;

    klbn_gpio_toggle_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);    
  }
}
