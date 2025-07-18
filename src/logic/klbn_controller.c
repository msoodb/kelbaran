/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_controller.h"
#include "FreeRTOS.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "klbn_types.h"
#include "klbn_radio.h"
#include "libc_stubs.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

#define MODE_BUTTON_DEBOUNCE_MS 100

/* -------------------- Main Controller Initialization -------------------- */
void klbn_controller_init(void) {  
  // Controller initialization (radio is initialized in sensor hub)
}

/* -------------------- Main Processing Functions -------------------- */
void klbn_controller_process(const klbn_sensor_data_t *in,
                             klbn_actuator_command_t *out) {
  (void)in; 

  // Get radio state and set LED indicators
  klbn_radio_state_t radio_state = klbn_radio_get_current_state();
  
  switch (radio_state) {
    case KLBN_RADIO_STATE_TX_MODE:
      // TX mode - fast blink external LED
      out->led.mode = KLBN_LED_MODE_BLINK;
      out->led.blink_speed_ms = 100;
      break;
    
    case KLBN_RADIO_STATE_RX_MODE:
      // RX mode - slow blink external LED
      out->led.mode = KLBN_LED_MODE_BLINK;
      out->led.blink_speed_ms = 500;
      break;
    
    case KLBN_RADIO_STATE_TRANSMITTING:
      // Transmitting - solid on
      out->led.mode = KLBN_LED_MODE_ON;
      break;
    
    case KLBN_RADIO_STATE_LISTENING:
      // Listening - solid on
      out->led.mode = KLBN_LED_MODE_ON;
      break;
    
    case KLBN_RADIO_STATE_ERROR:
      // Error - very fast blink
      out->led.mode = KLBN_LED_MODE_BLINK;
      out->led.blink_speed_ms = 50;
      break;
    
    default:
      // Idle - normal blink
      out->led.mode = KLBN_LED_MODE_BLINK;
      out->led.blink_speed_ms = 200;
      break;
  }
  
  // Check for incoming radio data
  if (klbn_radio_is_data_available()) {
    uint8_t rx_data[32];
    uint8_t rx_length;
    
    if (klbn_radio_receive_data(rx_data, &rx_length)) {
      // Data received - pulse debug LED
      klbn_gpio_set_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
      // Note: LED will be turned off in next cycle
    }
  }
}

/* -------------------- Mode Button -------------------- */
void klbn_controller_process_mode_button(const klbn_mode_button_event_t *event,
                                         klbn_actuator_command_t *out) {
  (void) out;
  static uint32_t last_press_time = 0;
  static uint32_t press_count = 0;
  
  if (event->event_type == KLBN_MODE_BUTTON_EVENT_PRESSED) {
    uint32_t current_time = xTaskGetTickCount();
    
    // Debounce
    if (current_time - last_press_time < pdMS_TO_TICKS(MODE_BUTTON_DEBOUNCE_MS)) {
      return;
    }
    last_press_time = current_time;

    // Switch radio mode on button press
    klbn_radio_switch_mode();
    
    // In TX mode, send a test message
    klbn_radio_state_t current_state = klbn_radio_get_current_state();
    if (current_state == KLBN_RADIO_STATE_TX_MODE) {
      // Send test message with counter
      char test_msg[32];
      press_count++;
      simple_sprintf(test_msg, "Hello from board! Count: %lu", press_count);
      klbn_radio_send_data((uint8_t*)test_msg, strlen(test_msg));
    }
    
    // Toggle debug LED to show button press
    klbn_gpio_toggle_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
  }
}
