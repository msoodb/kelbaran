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
#include "libc_stubs.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>
#include "klbn_types.h"

#define MODE_BUTTON_DEBOUNCE_MS 100
#define LED_ON_DURATION_MS 2000

#define BLFM_OLED_WIDTH 128
#define BLFM_OLED_HEIGHT 32
#define BLFM_OLED_PAGES (BLFM_OLED_HEIGHT / 8)
#define BLFM_OLED_MAX_SMALL_TEXT_LEN 12
#define BLFM_OLED_MAX_BIG_TEXT_LEN 16

/* -------------------- Main Controller Initialization -------------------- */
void klbn_controller_init(void) {  
  // Controller initialization
}

/* -------------------- Main Processing Functions -------------------- */
void klbn_controller_process(const klbn_sensor_data_t *in,
                             klbn_actuator_command_t *out) {
  (void)in; 
  
  // Simple LED control - normal blink
  out->led.mode = KLBN_LED_MODE_BLINK;
  out->led.blink_speed_ms = 500;

  out->oled.icon1 = KLBN_OLED_ICON_NONE;
  out->oled.icon2 = KLBN_OLED_ICON_NONE;
  out->oled.icon3 = KLBN_OLED_ICON_NONE;
  out->oled.icon4 = KLBN_OLED_ICON_NONE;

  safe_strncpy(out->oled.bigtext, "KELBARAN 2025", BLFM_OLED_MAX_BIG_TEXT_LEN);

  out->oled.invert = 0;
  out->oled.progress_percent = 75;

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

    // Toggle debug LED on button press
    klbn_gpio_toggle_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
  }
}
