
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

// Event definitions
typedef enum {
  KLBN_MODE_BUTTON_EVENT_PRESSED,
  KLBN_MODE_BUTTON_EVENT_RELEASED
} klbn_mode_button_event_type_t;

typedef struct {
  klbn_mode_button_event_type_t event_type;
  uint32_t timestamp;
} klbn_mode_button_event_t;


// TODO: Add radio communication data structures



// TODO: Add radio communication command types here

typedef struct {
  uint16_t x;  // ADC value from X-axis
  uint16_t y;  // ADC value from Y-axis
  bool pressed; // true if button is pressed
} klbn_joystick_data_t;

// Directional interpretation of the joystick position
typedef enum {
  KLBN_JOYSTICK_DIR_NONE = 0,
  KLBN_JOYSTICK_DIR_UP,
  KLBN_JOYSTICK_DIR_DOWN,
  KLBN_JOYSTICK_DIR_LEFT,
  KLBN_JOYSTICK_DIR_RIGHT
} klbn_joystick_direction_t;

// Type of joystick event (pressed/released)
typedef enum {
  KLBN_JOYSTICK_EVENT_NONE = 0,
  KLBN_JOYSTICK_EVENT_PRESSED,
  KLBN_JOYSTICK_EVENT_RELEASED
} klbn_joystick_event_type_t;

// Full interpreted joystick event
typedef struct {
  uint32_t timestamp;                        // Optional timestamp (e.g., from xTaskGetTickCount())
  klbn_joystick_event_type_t event_type;     // Button event type
  klbn_joystick_direction_t direction;       // Directional interpretation
} klbn_joystick_event_t;

typedef struct {
  klbn_joystick_data_t joystick;
  klbn_joystick_event_t joystick_event;     // Interpreted direction + event
  // TODO: Add radio communication data here
} klbn_sensor_data_t;

// Raw ADC data from the joystick (X and Y axis)



//-----------------------
//  actuators
//-----------------------
// TODO: Add radio communication command structures





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
                      // etc)
  uint8_t
      brightness; // brightness level (0-255), optional if hardware supports PWM
} klbn_led_command_t;


typedef struct {
  klbn_oled_command_t oled;
  klbn_led_command_t led;
  // TODO: Add radio communication commands here
} klbn_actuator_command_t;

#endif // KLBN_TYPES_H
