
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_led.h"
#include "klbn_motor.h"
#include "klbn_oled.h"
#include "klbn_servomotor.h"
#include "klbn_actuator_hub.h"
#include "klbn_types.h"

void klbn_actuator_hub_init(void) {
  klbn_led_init();
  klbn_motor_init();
  klbn_oled_init();
  klbn_servomotor_init();
}

void klbn_actuator_hub_apply(const klbn_actuator_command_t *cmd) {
  if (!cmd)
    return;

  klbn_led_apply(&cmd->led);
  klbn_motor_apply(&cmd->motor);
  klbn_oled_apply(&cmd->oled);
  klbn_servomotor_apply(&cmd->servo);
}
