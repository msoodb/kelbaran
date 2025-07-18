
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_TYPES_H
#define KLBN_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define KLBN_OLED_WIDTH 128
#define KLBN_OLED_HEIGHT 32
#define KLBN_OLED_PAGES (KLBN_OLED_HEIGHT / 8)
#define KLBN_OLED_MAX_SMALL_TEXT_LEN 12
#define KLBN_OLED_MAX_BIG_TEXT_LEN 16

//-----------------------
//  sensors
//-----------------------
typedef enum {
  KLBN_MODE_BUTTON_EVENT_PRESSED,
  KLBN_MODE_BUTTON_EVENT_RELEASED
} klbn_mode_button_event_type_t;

typedef struct {
  klbn_mode_button_event_type_t event_type;
  uint32_t timestamp;
} klbn_mode_button_event_t;


typedef struct {
  uint32_t timestamp;
} klbn_sensor_data_t;

//-----------------------
//  actuators
//-----------------------
typedef enum {
  KLBN_OLED_ICON_NONE = 0,
  KLBN_OLED_ICON_HEART,
  KLBN_OLED_ICON_SMILEY,
  KLBN_OLED_ICON_STAR,
} klbn_oled_icon_t;

typedef struct {
  klbn_oled_icon_t icon1;  // top-left icon
  klbn_oled_icon_t icon2;  // top-left icon (next to icon1)
  klbn_oled_icon_t icon3;  // top-right icon
  klbn_oled_icon_t icon4;  // top-right icon (next to icon3)
  char smalltext1[KLBN_OLED_MAX_SMALL_TEXT_LEN];
  char bigtext[KLBN_OLED_MAX_BIG_TEXT_LEN];
  char smalltext2[KLBN_OLED_MAX_SMALL_TEXT_LEN];
  uint8_t invert;          // invert display
  uint8_t progress_percent; // optional progress bar
} klbn_oled_command_t;


typedef enum {
  KLBN_LED_MODE_OFF = 0,
  KLBN_LED_MODE_ON,
  KLBN_LED_MODE_BLINK,
} klbn_led_mode_t;

typedef struct {
  klbn_led_mode_t mode;    // off, on, or blink
  uint16_t blink_speed_ms; // blinking speed in milliseconds (period)
  uint8_t pattern_id; // blink pattern id (e.g., 0=solid, 1=short pulses, 2=SOS,
  uint8_t
      brightness; // brightness level (0-255), optional if hardware supports PWM
} klbn_led_command_t;

typedef struct {
  klbn_oled_command_t oled;
  klbn_led_command_t led;
} klbn_actuator_command_t;

#endif // KLBN_TYPES_H
