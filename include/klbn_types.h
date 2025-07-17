
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
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


typedef struct {
  uint16_t distance_mm;
} klbn_ultrasonic_data_t;



typedef enum {
  KLBN_IR_CMD_NONE = 0,
  KLBN_IR_CMD_1 = 0x45,
  KLBN_IR_CMD_2 = 0x46,
  KLBN_IR_CMD_3 = 0x47,
  KLBN_IR_CMD_4 = 0x44,
  KLBN_IR_CMD_5 = 0x40,
  KLBN_IR_CMD_6 = 0x43,
  KLBN_IR_CMD_7 = 0x07,
  KLBN_IR_CMD_8 = 0x15,
  KLBN_IR_CMD_9 = 0x09,
  KLBN_IR_CMD_0 = 0x19,
  KLBN_IR_CMD_STAR = 0x16,
  KLBN_IR_CMD_HASH = 0x0D,
  KLBN_IR_CMD_UP = 0x18,
  KLBN_IR_CMD_DOWN = 0x52,
  KLBN_IR_CMD_LEFT = 0x08,
  KLBN_IR_CMD_RIGHT = 0x5A,
  KLBN_IR_CMD_OK = 0x1C,
  KLBN_IR_CMD_REPEAT = 0xFFFFFFFF
} klbn_ir_command_t;

typedef struct {
  uint32_t timestamp;        // Tick count at event
  uint32_t pulse_us;         // Raw IR code received
  klbn_ir_command_t command; // Decoded command
} klbn_ir_remote_event_t;

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
  klbn_ultrasonic_data_t ultrasonic;
  klbn_joystick_data_t joystick;
  klbn_joystick_event_t joystick_event;     // Interpreted direction + event
} klbn_sensor_data_t;

// Raw ADC data from the joystick (X and Y axis)



//-----------------------
//  actuators
//-----------------------
typedef struct {
  uint16_t speed;     // 0–255
  uint8_t direction; // 0 = forward, 1 = backward
} klbn_single_motor_command_t;

typedef struct {
  klbn_single_motor_command_t left;
  klbn_single_motor_command_t right;
} klbn_motor_command_t;


typedef struct {
  uint8_t angle;           // Servo angle in degrees (0-180)
  uint16_t pulse_width_us; // Optional: pulse width in microseconds (e.g.,
                           // 1000-2000us)
} klbn_servomotor_command_t;

typedef enum {
  KLBN_SERVOMOTOR_NECK = 0,
  KLBN_SERVOMOTOR_COUNT
} klbn_servomotor_id_t;



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
  klbn_motor_command_t motor;
  klbn_oled_command_t oled;
  klbn_led_command_t led;
  klbn_servomotor_command_t servo;
} klbn_actuator_command_t;

#endif // KLBN_TYPES_H
