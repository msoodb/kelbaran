
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_sensor_hub.h"
#include "klbn_ultrasonic.h"

#include <stdbool.h>

void klbn_sensor_hub_init(void) {
  //klbn_ultrasonic_init();
}

bool klbn_sensor_hub_read(klbn_sensor_data_t *out) {
  if (!out) {
    return false;
  }

  //return klbn_ultrasonic_read(&out->ultrasonic);
  return true;
}
