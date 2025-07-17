
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_LED_H
#define KLBN_LED_H

#include "klbn_types.h"

void klbn_led_init(void);
void klbn_led_apply(const klbn_led_command_t *cmd);

#endif /* KLBN_LED_H */
