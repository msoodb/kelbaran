
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_SENSOR_HUB_H
#define KLBN_SENSOR_HUB_H

#include "FreeRTOS.h"
#include "task.h"
#include "klbn_types.h"

void klbn_sensor_hub_init();
bool klbn_sensor_hub_read(klbn_sensor_data_t *out);

#endif // KLBN_SENSOR_HUB_H

