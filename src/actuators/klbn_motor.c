
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_motor.h"
#include "klbn_gpio.h"
#include "stm32f1xx.h"
#include "klbn_pins.h"

// TIM2 PWM channels
#define LEFT_PWM_CCR  TIM2->CCR1  // PA0 (CH1)
#define RIGHT_PWM_CCR TIM2->CCR2  // PA1 (CH2)

static void klbn_motor_set_side(const klbn_single_motor_command_t *cmd, bool is_left);

void klbn_motor_init(void) {
  // Configure direction pins
  klbn_gpio_config_output((uint32_t)KLBN_MOTOR_LEFT_IN1_PORT, KLBN_MOTOR_LEFT_IN1_PIN);
  klbn_gpio_config_output((uint32_t)KLBN_MOTOR_LEFT_IN2_PORT, KLBN_MOTOR_LEFT_IN2_PIN);
  klbn_gpio_config_output((uint32_t)KLBN_MOTOR_RIGHT_IN1_PORT, KLBN_MOTOR_RIGHT_IN1_PIN);
  klbn_gpio_config_output((uint32_t)KLBN_MOTOR_RIGHT_IN2_PORT, KLBN_MOTOR_RIGHT_IN2_PIN);

  // Enable peripherals
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // PA0 and PA1 (TIM2_CH1 and TIM2_CH2) as alternate function push-pull
  GPIOA->CRL &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)));
  GPIOA->CRL |=  ((0xB << (0 * 4)) | (0xB << (1 * 4)));

  // Timer setup
  TIM2->PSC = 72 - 1;   // 72 MHz / 72 = 1 MHz
  TIM2->ARR = 255;      // 8-bit resolution

  TIM2->CCR1 = 0;
  TIM2->CCR2 = 0;

  TIM2->CCMR1 |= (6 << 4) | (1 << 3);      // CH1 PWM1 + preload
  TIM2->CCMR1 |= (6 << 12) | (1 << 11);    // CH2 PWM1 + preload

  TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
  TIM2->CR1 |= TIM_CR1_CEN;
}

void klbn_motor_apply(const klbn_motor_command_t *cmd) {
  if (!cmd) return;
  klbn_motor_set_side(&cmd->left, true);
  klbn_motor_set_side(&cmd->right, false);
}

static void klbn_motor_set_side(const klbn_single_motor_command_t *cmd, bool is_left) {
  if (!cmd) return;

  GPIO_TypeDef *in1_port, *in2_port;
  uint8_t in1_pin, in2_pin;
  volatile uint32_t *pwm_ccr;

  if (is_left) {
    in1_port = KLBN_MOTOR_LEFT_IN1_PORT;
    in2_port = KLBN_MOTOR_LEFT_IN2_PORT;
    in1_pin  = KLBN_MOTOR_LEFT_IN1_PIN;
    in2_pin  = KLBN_MOTOR_LEFT_IN2_PIN;
    pwm_ccr  = &LEFT_PWM_CCR;
  } else {
    in1_port = KLBN_MOTOR_RIGHT_IN1_PORT;
    in2_port = KLBN_MOTOR_RIGHT_IN2_PORT;
    in1_pin  = KLBN_MOTOR_RIGHT_IN1_PIN;
    in2_pin  = KLBN_MOTOR_RIGHT_IN2_PIN;
    pwm_ccr  = &RIGHT_PWM_CCR;
  }

  // Direction
  if (cmd->direction == 0) {
    klbn_gpio_set_pin((uint32_t)in1_port, in1_pin);
    klbn_gpio_clear_pin((uint32_t)in2_port, in2_pin);
  } else {
    klbn_gpio_clear_pin((uint32_t)in1_port, in1_pin);
    klbn_gpio_set_pin((uint32_t)in2_port, in2_pin);
  }

  // Speed
  *pwm_ccr = (cmd->speed > 255) ? 255 : cmd->speed;
}

