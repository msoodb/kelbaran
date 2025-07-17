
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_ACTUATOR_HUB_H
#define KLBN_ACTUATOR_HUB_H

#include "klbn_types.h"

void klbn_actuator_hub_init(void);
void klbn_actuator_hub_apply(const klbn_actuator_command_t *cmd);

#endif // KLBN_ACTUATOR_HUB_H

