/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_exti_dispatcher.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "stm32f1xx.h"

#define MAX_EXTI_LINES 16

static klbn_exti_callback_t exti_callbacks[MAX_EXTI_LINES] = {0};

void klbn_exti_register_callback(uint8_t exti_line,
                                 klbn_exti_callback_t callback) {

  if (exti_line < MAX_EXTI_LINES) {
    exti_callbacks[exti_line] = callback;
  }
}

void EXTI4_IRQHandler(void) {
  if (EXTI->PR & (1U << 4)) {
    EXTI->PR = (1U << 4);
    if (exti_callbacks[4]) {
      exti_callbacks[4]();
    }
  }
}

void EXTI9_5_IRQHandler(void) {
  for (uint8_t line = 5; line <= 9; ++line) {
    if (EXTI->PR & (1U << line)) {
      EXTI->PR = (1U << line); // clear pending bit once here
      if (exti_callbacks[line]) {
        exti_callbacks[line]();
      }
    }
  }
}
