
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_DELAY_H
#define KLBN_DELAY_H

#include <stdint.h>

void klbn_delay_init(void);
void klbn_delay_ms(uint32_t ms);
void klbn_delay_us(uint32_t us);

#endif // KLBN_DELAY_H

