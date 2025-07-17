/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_IR_REMOTE_H
#define KLBN_IR_REMOTE_H

#include "FreeRTOS.h"
#include "klbn_types.h"
#include "queue.h"
#include <stdint.h>

void klbn_ir_remote_init(QueueHandle_t controller_queue);
void ir_exti_handler(void);

#endif // KLBN_IR_REMOTE_H
