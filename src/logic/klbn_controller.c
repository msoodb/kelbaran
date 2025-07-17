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
#include "klbn_state.h"
#include "klbn_types.h"
#include "libc_stubs.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

#define SWEEP_MIN_ANGLE 0
#define SWEEP_MAX_ANGLE 180

#define ULTRASONIC_FORWARD_THRESH 20
#define MOTOR_BACKWARD_TICKS_MAX 2
#define MOTOR_MIN_ROTATE_TICKS 4
#define MOTOR_MAX_ROTATE_TICKS 6
#define MODE_BUTTON_DEBOUNCE_MS 100
#define IR_CONTROL_TIMEOUT_MS 200

#define MOTOR_DEFAULT_SPEED 255

static bool servo_direction = true;
static int motor_backward_ticks = 0;
static int motor_rotate_ticks = 0;
static int motor_rotate_duration = 0;

static klbn_system_state_t klbn_system_state = {
    .current_mode = KLBN_MODE_MANUAL, .motion_state = KLBN_MOTION_STOP};

/**
 * Helper: set motor motion from angle (-180 to 180) and speed.
 */
static void set_motor_motion_by_angle(int angle, int speed,
                                      klbn_motor_command_t *out);

/* -------------------- Utilities -------------------- */

static int pseudo_random(int min, int max) {
  static uint32_t seed = 123456789;
  seed = seed * 1664525 + 1013904223;
  return min + (seed % (max - min + 1));
}


/* -------------------- Motion Helpers -------------------- */

static int motion_state_to_angle(klbn_motion_state_t motion) {
  switch (motion) {
  case KLBN_MOTION_FORWARD:
  case KLBN_MOTION_STOP:
    return 0;
  case KLBN_MOTION_BACKWARD:
    return 180;
  case KLBN_MOTION_ROTATE_RIGHT:
    return 90;
  case KLBN_MOTION_ROTATE_LEFT:
    return -90;
  default:
    return 0;
  }
}

/**
 * Map angle (-180 to 180) and speed (0-255) to motor command.
 * Sets motor left/right direction and speed.
 *
 * No floating point used.
 */
static void set_motor_motion_by_angle(int angle, int speed,
                                      klbn_motor_command_t *out) {
  if (!out)
    return;

  // Normalize angle to -180..180
  while (angle > 180)
    angle -= 360;
  while (angle < -180)
    angle += 360;

  // Map angle to differential control
  // Example simple approach:
  // -90 = full left spin
  // +90 = full right spin
  // 0 = straight
  // ±180 = backward

  if (angle > 90) {
    // Hard right spin
    out->left.direction = KLBN_MOTION_BACKWARD;
    out->right.direction = KLBN_MOTION_BACKWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle < -90) {
    // Hard left spin
    out->left.direction = KLBN_MOTION_BACKWARD;
    out->right.direction = KLBN_MOTION_BACKWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle > 45) {
    // Right turn
    out->left.direction = KLBN_MOTION_FORWARD;
    out->right.direction = KLBN_MOTION_BACKWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle < -45) {
    // Left turn
    out->left.direction = KLBN_MOTION_BACKWARD;
    out->right.direction = KLBN_MOTION_FORWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else if (angle > -45 && angle < 45) {
    // Forward
    out->left.direction = KLBN_MOTION_FORWARD;
    out->right.direction = KLBN_MOTION_FORWARD;
    out->left.speed = speed;
    out->right.speed = speed;
  } else {
    // fallback stop
    out->left.direction = KLBN_MOTION_FORWARD;
    out->right.direction = KLBN_MOTION_FORWARD;
    out->left.speed = 0;
    out->right.speed = 0;
  }
}

/* -------------------- Public Controller -------------------- */

void klbn_controller_init(void) {
  // Servo initialization is handled in klbn_controller_process function
}

// --- Mode-setting functions ---
static void klbn_set_mode_auto(klbn_actuator_command_t *out) {
  klbn_system_state.current_mode = KLBN_MODE_AUTO;
  out->led.mode = KLBN_LED_MODE_BLINK;
  out->led.blink_speed_ms = 500;
}

static void klbn_set_mode_manual(klbn_actuator_command_t *out) {
  klbn_system_state.current_mode = KLBN_MODE_MANUAL;
  out->led.mode = KLBN_LED_MODE_OFF;
}

static void klbn_set_mode_emergency(klbn_actuator_command_t *out) {
  klbn_system_state.current_mode = KLBN_MODE_EMERGENCY;
  out->led.mode = KLBN_LED_MODE_ON;
}

// --- Public interface to change mode ---
void klbn_controller_change_mode(klbn_mode_t mode,
                                 klbn_actuator_command_t *out) {
  switch (mode) {
  case KLBN_MODE_AUTO:
    klbn_set_mode_auto(out);
    break;
  case KLBN_MODE_MANUAL:
    klbn_set_mode_manual(out);
    break;
  case KLBN_MODE_EMERGENCY:
  default:
    klbn_set_mode_emergency(out);
    break;
  }

  set_motor_motion_by_angle(0, 0, &out->motor);
  klbn_system_state.motion_state = KLBN_MOTION_STOP;
}

void klbn_controller_process(const klbn_sensor_data_t *in,
                             klbn_actuator_command_t *out) {
  if (!in || !out)
    return;

  // Initialize servo to center position on first call
  static bool servo_initialized = false;
  static uint8_t current_servo_angle = 90;
  if (!servo_initialized) {
    current_servo_angle = 90;
    servo_initialized = true;
  }

  if (klbn_system_state.current_mode == KLBN_MODE_AUTO) {
    switch (klbn_system_state.motion_state) {
    case KLBN_MOTION_STOP:
    case KLBN_MOTION_FORWARD:
      if (in->ultrasonic.distance_mm <= ULTRASONIC_FORWARD_THRESH) {
        motor_backward_ticks = 0;
        klbn_system_state.motion_state = KLBN_MOTION_BACKWARD;
      } else {
        set_motor_motion_by_angle(0, MOTOR_DEFAULT_SPEED, &out->motor);
      }
      break;

    case KLBN_MOTION_BACKWARD:
      set_motor_motion_by_angle(180, MOTOR_DEFAULT_SPEED, &out->motor);
      motor_backward_ticks++;
      if (motor_backward_ticks >= MOTOR_BACKWARD_TICKS_MAX) {
        motor_rotate_ticks = 0;
        motor_rotate_duration =
            pseudo_random(MOTOR_MIN_ROTATE_TICKS, MOTOR_MAX_ROTATE_TICKS);
        klbn_system_state.motion_state = KLBN_MOTION_ROTATE_LEFT;
      }
      break;

    case KLBN_MOTION_ROTATE_LEFT:
      set_motor_motion_by_angle(-90, MOTOR_DEFAULT_SPEED, &out->motor);
      motor_rotate_ticks++;
      if (motor_rotate_ticks >= motor_rotate_duration) {
        klbn_system_state.motion_state = KLBN_MOTION_FORWARD;
      }
      break;

    default:
      set_motor_motion_by_angle(0, 0, &out->motor);
      break;
    }
  }


  out->led.mode = KLBN_LED_MODE_BLINK;
  out->led.blink_speed_ms = 200;

  if (klbn_system_state.current_mode != KLBN_MODE_EMERGENCY) {
    // Debug: Toggle debug LED to show servo logic is running
    static uint8_t debug_counter = 0;
    if (++debug_counter >= 10) {
      debug_counter = 0;
    }
    
    if (servo_direction)
      current_servo_angle += 5;
    else
      current_servo_angle -= 5;

    if (current_servo_angle >= SWEEP_MAX_ANGLE)
      servo_direction = false;
    else if (current_servo_angle <= SWEEP_MIN_ANGLE)
      servo_direction = true;
  }

  // Always set servo angle to current value
  out->servo.angle = current_servo_angle;


  out->oled.icon1 = KLBN_OLED_ICON_NONE;
  out->oled.icon2 = KLBN_OLED_ICON_NONE;
  out->oled.icon3 = KLBN_OLED_ICON_NONE;
  out->oled.icon4 = KLBN_OLED_ICON_NONE;

  safe_strncpy(out->oled.bigtext, "BELFHYM VZA-993", KLBN_OLED_MAX_BIG_TEXT_LEN);

  out->oled.invert = 0;
  out->oled.progress_percent = 75;
}

void klbn_controller_process_ir_remote(const klbn_ir_remote_event_t *in,
                                       klbn_actuator_command_t *out) {
  if (!in || !out)
    return;

  // Handle mode changes (available in all modes)
  switch (in->command) {
  case KLBN_IR_CMD_1:
    klbn_controller_change_mode(KLBN_MODE_MANUAL, out);
    return;
  case KLBN_IR_CMD_2:
    klbn_controller_change_mode(KLBN_MODE_AUTO, out);
    return;
  case KLBN_IR_CMD_3:
    klbn_controller_change_mode(KLBN_MODE_EMERGENCY, out);
    return;
  default:
    break;
  }

  // Handle motion commands (only in manual mode)
  if (klbn_system_state.current_mode != KLBN_MODE_MANUAL)
    return;

  switch (in->command) {
  case KLBN_IR_CMD_UP:
    klbn_system_state.motion_state = KLBN_MOTION_FORWARD;
    break;
  case KLBN_IR_CMD_DOWN:
    klbn_system_state.motion_state = KLBN_MOTION_BACKWARD;
    break;
  case KLBN_IR_CMD_LEFT:
    klbn_system_state.motion_state = KLBN_MOTION_ROTATE_LEFT;
    break;
  case KLBN_IR_CMD_RIGHT:
    klbn_system_state.motion_state = KLBN_MOTION_ROTATE_RIGHT;
    break;
  case KLBN_IR_CMD_OK:
  default:
    klbn_system_state.motion_state = KLBN_MOTION_STOP;
    break;
  }

  int angle = motion_state_to_angle(klbn_system_state.motion_state);
  int motor_speed = (klbn_system_state.motion_state == KLBN_MOTION_STOP) ? 0 : MOTOR_DEFAULT_SPEED;
  set_motor_motion_by_angle(angle, motor_speed, &out->motor);
}

void klbn_controller_process_joystick(const klbn_joystick_event_t *evt,
                                      klbn_actuator_command_t *out) {
  if (!evt || !out)
    return;

  if (klbn_system_state.current_mode == KLBN_MODE_MANUAL) {
    switch (evt->direction) {
    case KLBN_JOYSTICK_DIR_UP:
      klbn_system_state.motion_state = KLBN_MOTION_FORWARD;
      break;
    case KLBN_JOYSTICK_DIR_DOWN:
      klbn_system_state.motion_state = KLBN_MOTION_BACKWARD;
      break;
    case KLBN_JOYSTICK_DIR_LEFT:
      klbn_system_state.motion_state = KLBN_MOTION_ROTATE_LEFT;
      break;
    case KLBN_JOYSTICK_DIR_RIGHT:
      klbn_system_state.motion_state = KLBN_MOTION_ROTATE_RIGHT;
      break;
    default:
      klbn_system_state.motion_state = KLBN_MOTION_STOP;
      break;
    }
  }

  int angle = motion_state_to_angle(klbn_system_state.motion_state);
  set_motor_motion_by_angle(angle, MOTOR_DEFAULT_SPEED, &out->motor);
}

void klbn_controller_process_mode_button(const klbn_mode_button_event_t *event,
                                         klbn_actuator_command_t *command) {
  static uint32_t last_press_tick = 0;
  (void)command;

  if (event && event->event_type == KLBN_MODE_BUTTON_EVENT_PRESSED) {
    uint32_t now = xTaskGetTickCount();
    if ((now - last_press_tick) > pdMS_TO_TICKS(MODE_BUTTON_DEBOUNCE_MS)) {
      klbn_gpio_toggle_pin((uint32_t)KLBN_LED_DEBUG_PORT, KLBN_LED_DEBUG_PIN);
      last_press_tick = now;
    }
  }
}
