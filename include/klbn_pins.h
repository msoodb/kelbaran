
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_PINS_H
#define KLBN_PINS_H

#include "stm32f1xx.h"

// =============================
// === PORT A ==================
// =============================
#define KLBN_MODE_BUTTON_PORT GPIOA
#define KLBN_MODE_BUTTON_PIN 4

// TODO: Add radio module pins here
// Example for nRF24L01:
// #define KLBN_RADIO_CE_PORT GPIOA
// #define KLBN_RADIO_CE_PIN 2
// #define KLBN_RADIO_CSN_PORT GPIOA
// #define KLBN_RADIO_CSN_PIN 3

// =============================
// === PORT B ==================
// =============================
#define KLBN_LED_EXTERNAL_PORT GPIOB
#define KLBN_LED_EXTERNAL_PIN 5

#define KLBN_I2C1_SCL_PORT GPIOB
#define KLBN_I2C1_SCL_PIN 6

#define KLBN_I2C1_SDA_PORT GPIOB
#define KLBN_I2C1_SDA_PIN 7

// TODO: Add additional radio module pins here if needed


// =============================
// === PORT C ==================
// =============================
#define KLBN_LED_ONBOARD_PORT GPIOC
#define KLBN_LED_ONBOARD_PIN 13


#define KLBN_LED_DEBUG_PORT GPIOC
#define KLBN_LED_DEBUG_PIN 15

#endif /* KLBN_PINS_H */
