
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_sensor_hub.h"
#include "klbn_radio.h"

#include <stdbool.h>

void klbn_sensor_hub_init(void) {
  // Initialize radio communication
  klbn_radio_init();
}

bool klbn_sensor_hub_read(klbn_sensor_data_t *out) {
  if (!out) {
    return false;
  }

  // TODO: Read radio communication status and data
  return true;
}
