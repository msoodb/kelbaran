

/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_mode_button.h"
#include "FreeRTOS.h"
#include "klbn_exti_dispatcher.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "queue.h"
#include "stm32f1xx.h"

#define DEBOUNCE_DELAY_MS 50
#define LONG_PRESS_DURATION_MS 3000

static QueueHandle_t mode_button_queue = NULL;
static uint32_t last_press_tick = 0;
static uint32_t press_start_time = 0;
static bool button_pressed = false;

static void mode_button_exti_handler(void) {
  if (mode_button_queue == NULL) {
    return;
  }

  uint32_t now = xTaskGetTickCountFromISR();
  if ((now - last_press_tick) < pdMS_TO_TICKS(DEBOUNCE_DELAY_MS)) {
    return; // Too soon, ignore as bounce
  }
  last_press_tick = now;

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  uint32_t pin_state = klbn_gpio_read_pin((uint32_t)KLBN_MODE_BUTTON_PORT, KLBN_MODE_BUTTON_PIN);
  klbn_mode_button_event_t event;

  if (pin_state) {
    // Button released
    if (button_pressed) {
      uint32_t press_duration = now - press_start_time;
      
      if (press_duration >= pdMS_TO_TICKS(LONG_PRESS_DURATION_MS)) {
        // Long press
        event.event_type = KLBN_MODE_BUTTON_EVENT_LONG_PRESS;
        event.press_duration = press_duration;
      } else {
        // Normal release
        event.event_type = KLBN_MODE_BUTTON_EVENT_RELEASED;
        event.press_duration = press_duration;
      }
      
      button_pressed = false;
      event.timestamp = now;
      xQueueSendFromISR(mode_button_queue, &event, &xHigherPriorityTaskWoken);
    }
  } else {
    // Button pressed
    if (!button_pressed) {
      event.event_type = KLBN_MODE_BUTTON_EVENT_PRESSED;
      event.timestamp = now;
      event.press_duration = 0;
      press_start_time = now;
      button_pressed = true;
      xQueueSendFromISR(mode_button_queue, &event, &xHigherPriorityTaskWoken);
    }
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void klbn_mode_button_init(QueueHandle_t controller_queue) {
  mode_button_queue = controller_queue;

  // Configure as input with Pull‑Up
  klbn_gpio_config_input_pullup((uint32_t)KLBN_MODE_BUTTON_PORT, KLBN_MODE_BUTTON_PIN);

  // EXTI mapping for PC15
  AFIO->EXTICR[3] &= ~(0xF << (4 * (KLBN_MODE_BUTTON_PIN - 12)));
  AFIO->EXTICR[3] |= (0x2 << (4 * (KLBN_MODE_BUTTON_PIN - 12)));

  // Trigger both edges
  EXTI->IMR |= (1 << KLBN_MODE_BUTTON_PIN);
  EXTI->RTSR |= (1 << KLBN_MODE_BUTTON_PIN);
  EXTI->FTSR |= (1 << KLBN_MODE_BUTTON_PIN);

  // Register callback
  klbn_exti_register_callback(KLBN_MODE_BUTTON_PIN, mode_button_exti_handler);

  // Enable IRQ (EXTI15_10 handles EXTI15)
  NVIC_EnableIRQ(EXTI15_10_IRQn);
}

