
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_MODE_BUTTON_H
#define KLBN_MODE_BUTTON_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include "stm32f1xx.h"
#include "klbn_types.h"
#include "klbn_pins.h"

void klbn_mode_button_init(QueueHandle_t controller_queue);

#endif // KLBN_MODE_BUTTON_H
