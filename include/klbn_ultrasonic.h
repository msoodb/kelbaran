
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_ULTRASONIC_H
#define KLBN_ULTRASONIC_H

#include "klbn_types.h"
#include <stdbool.h>

void klbn_ultrasonic_init(void);
bool klbn_ultrasonic_read(klbn_ultrasonic_data_t *data);

#endif // KLBN_ULTRASONIC_H

