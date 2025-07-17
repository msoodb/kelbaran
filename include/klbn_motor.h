
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_MOTOR_H
#define KLBN_MOTOR_H

#include "klbn_types.h"

void klbn_motor_init(void);
void klbn_motor_apply(const klbn_motor_command_t *cmd);

#endif // KLBN_MOTOR_H

