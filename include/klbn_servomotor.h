
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_SERVOMOTOR_H
#define KLBN_SERVOMOTOR_H

#include <stdint.h>
#include "klbn_types.h"

void klbn_servomotor_init(void);
void klbn_servomotor_apply(const klbn_servomotor_command_t *cmd);

#endif // KLBN_SERVOMOTOR_H
