
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_led.h"
#include "klbn_oled.h"
#include "klbn_actuator_hub.h"
#include "klbn_types.h"

void klbn_actuator_hub_init(void) {
  klbn_led_init();
  klbn_oled_init();
}

void klbn_actuator_hub_apply(const klbn_actuator_command_t *cmd) {
  if (!cmd)
    return;

  klbn_led_apply(&cmd->led);
  klbn_oled_apply(&cmd->oled);
}
