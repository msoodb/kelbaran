
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_board.h"
#include "klbn_clock.h"
#include "klbn_gpio.h"
#include "klbn_i2c.h"
#include "klbn_delay.h"

void klbn_board_init(void) {
  klbn_clock_init();    // System clocks
  klbn_gpio_init();     // All GPIO modes

  // Peripheral inits
  klbn_i2c_init(NULL);
  // TODO: Add radio module initialization here

  // Enable cycle counter
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  klbn_delay_init();
}
