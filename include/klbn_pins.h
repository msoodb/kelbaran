
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#ifndef KLBN_PINS_H
#define KLBN_PINS_H

// =============================
// === PORT A ==================
// =============================

// SPI1 pins (PA3, PA5, PA6, PA7)
#define KLBN_SPI_CS_PORT GPIOA
#define KLBN_SPI_CS_PIN 3

#define KLBN_SPI_SCK_PORT GPIOA
#define KLBN_SPI_SCK_PIN 5

#define KLBN_SPI_MISO_PORT GPIOA
#define KLBN_SPI_MISO_PIN 6

#define KLBN_SPI_MOSI_PORT GPIOA
#define KLBN_SPI_MOSI_PIN 7

// -----------------------------



// =============================
// === PORT B ==================
// =============================

// I2C1 pins (PB6, PB7)
#define KLBN_I2C1_SCL_PORT GPIOB
#define KLBN_I2C1_SCL_PIN 6

#define KLBN_I2C1_SDA_PORT GPIOB
#define KLBN_I2C1_SDA_PIN 7

// -----------------------------




// =============================
// === PORT C ==================
// =============================

// Onboard LED (PC13)
#define KLBN_LED_ONBOARD_PORT GPIOC
#define KLBN_LED_ONBOARD_PIN 13

// Debug LED (PC14)
#define KLBN_LED_DEBUG_PORT GPIOC
#define KLBN_LED_DEBUG_PIN 14

// Mode button (PC15)
#define KLBN_MODE_BUTTON_PORT GPIOC
#define KLBN_MODE_BUTTON_PIN 15

// -----------------------------

#endif /* KLBN_PINS_H */
