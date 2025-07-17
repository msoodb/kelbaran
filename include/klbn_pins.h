
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_PINS_H
#define KLBN_PINS_H

#include "stm32f1xx.h"

// =============================
// === PORT A ==================
// =============================
#define KLBN_MOTOR_LEFT_EN_PORT GPIOA
#define KLBN_MOTOR_LEFT_EN_PIN 0

#define KLBN_MOTOR_RIGHT_EN_PORT GPIOA
#define KLBN_MOTOR_RIGHT_EN_PIN 1

#define KLBN_MODE_BUTTON_PORT GPIOA
#define KLBN_MODE_BUTTON_PIN 4



#define KLBN_SERVO_PWM_PORT GPIOA
#define KLBN_SERVO_PWM_PIN 7  // TIM3_CH2

#define KLBN_IR_REMOTE_PORT GPIOA
#define KLBN_IR_REMOTE_PIN 8

// =============================
// === PORT B ==================
// =============================
#define KLBN_MOTOR_LEFT_IN1_PORT GPIOB
#define KLBN_MOTOR_LEFT_IN1_PIN 0

#define KLBN_MOTOR_LEFT_IN2_PORT GPIOB
#define KLBN_MOTOR_LEFT_IN2_PIN 1

#define KLBN_ULTRASONIC_ECHO_PORT GPIOB
#define KLBN_ULTRASONIC_ECHO_PIN 3

#define KLBN_ULTRASONIC_TRIG_PORT GPIOB
#define KLBN_ULTRASONIC_TRIG_PIN 4

#define KLBN_LED_EXTERNAL_PORT GPIOB
#define KLBN_LED_EXTERNAL_PIN 5

#define KLBN_I2C1_SCL_PORT GPIOB
#define KLBN_I2C1_SCL_PIN 6

#define KLBN_I2C1_SDA_PORT GPIOB
#define KLBN_I2C1_SDA_PIN 7

#define KLBN_MOTOR_RIGHT_IN1_PORT GPIOB
#define KLBN_MOTOR_RIGHT_IN1_PIN 10

#define KLBN_MOTOR_RIGHT_IN2_PORT GPIOB
#define KLBN_MOTOR_RIGHT_IN2_PIN 11


// =============================
// === PORT C ==================
// =============================
#define KLBN_LED_ONBOARD_PORT GPIOC
#define KLBN_LED_ONBOARD_PIN 13


#define KLBN_LED_DEBUG_PORT GPIOC
#define KLBN_LED_DEBUG_PIN 15

#endif /* KLBN_PINS_H */
