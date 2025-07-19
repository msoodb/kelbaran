/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_RADIO_HUB_H
#define KLBN_RADIO_HUB_H

#include <stdbool.h>
#include "klbn_types.h"

void klbn_radio_hub_init(void);
bool klbn_radio_hub_receive(klbn_radio_data_t *out);
void klbn_radio_hub_send(const klbn_radio_command_t *cmd);

#endif /* KLBN_RADIO_HUB_H */