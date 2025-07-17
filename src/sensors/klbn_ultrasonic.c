
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_ultrasonic.h"
#include "FreeRTOS.h"
#include "klbn_gpio.h"
#include "klbn_pins.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

#define ULTRASONIC_TASK_STACK_SIZE 256
#define ULTRASONIC_TASK_PRIORITY 2
#define ULTRASONIC_QUEUE_LENGTH 1

static QueueHandle_t ultrasonic_data_queue = NULL;
static TaskHandle_t ultrasonic_task_handle = NULL;

static void delay_us(uint32_t us) {
  for (uint32_t i = 0; i < us * 8; ++i) {
    __asm volatile("nop");
  }
}

static void vUltrasonicTask(void *pvParameters);
static bool wait_for_pin(uint32_t port, uint32_t pin, int target_state,
                         uint32_t timeout_ms);

void klbn_ultrasonic_init(void) {
  klbn_gpio_config_output((uint32_t)KLBN_ULTRASONIC_TRIG_PORT,
                          KLBN_ULTRASONIC_TRIG_PIN);
  klbn_gpio_config_input((uint32_t)KLBN_ULTRASONIC_ECHO_PORT,
                         KLBN_ULTRASONIC_ECHO_PIN);
  klbn_gpio_clear_pin((uint32_t)KLBN_ULTRASONIC_TRIG_PORT,
                      KLBN_ULTRASONIC_TRIG_PIN);

  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  }
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  if (ultrasonic_data_queue == NULL) {
    ultrasonic_data_queue =
        xQueueCreate(ULTRASONIC_QUEUE_LENGTH, sizeof(klbn_ultrasonic_data_t));
    configASSERT(ultrasonic_data_queue != NULL);
  }

  if (ultrasonic_task_handle == NULL) {
    BaseType_t result = xTaskCreate(
        vUltrasonicTask, "UltrasonicTask", ULTRASONIC_TASK_STACK_SIZE, NULL,
        ULTRASONIC_TASK_PRIORITY, &ultrasonic_task_handle);
    configASSERT(result == pdPASS);
    if (result != pdPASS) {
    }
  }
}

bool klbn_ultrasonic_read(klbn_ultrasonic_data_t *data) {
  if (!data || !ultrasonic_data_queue)
    return false;

  xTaskNotifyGive(ultrasonic_task_handle);

  if (xQueueReceive(ultrasonic_data_queue, data, pdMS_TO_TICKS(50)) == pdPASS) {
    return true;
  }
  return false;
}

static bool wait_for_pin(uint32_t port, uint32_t pin, int target_state,
                         uint32_t timeout_ms) {
  uint32_t start = DWT->CYCCNT;
  uint32_t timeout_cycles = (SystemCoreClock / 1000) * timeout_ms;

  while (klbn_gpio_read_pin(port, pin) != target_state) {
    if ((DWT->CYCCNT - start) > timeout_cycles) {
      return false;
    }
  }
  return true;
}

static bool klbn_ultrasonic_action(klbn_ultrasonic_data_t *data) {
  if (!data)
    return false;

  uint32_t start, end, duration;

  klbn_gpio_clear_pin((uint32_t)KLBN_ULTRASONIC_TRIG_PORT,
                      KLBN_ULTRASONIC_TRIG_PIN);
  delay_us(2);
  klbn_gpio_set_pin((uint32_t)KLBN_ULTRASONIC_TRIG_PORT,
                    KLBN_ULTRASONIC_TRIG_PIN);
  delay_us(10);
  klbn_gpio_clear_pin((uint32_t)KLBN_ULTRASONIC_TRIG_PORT,
                      KLBN_ULTRASONIC_TRIG_PIN);

  if (!wait_for_pin((uint32_t)KLBN_ULTRASONIC_ECHO_PORT,
                    KLBN_ULTRASONIC_ECHO_PIN, 1, 30)) {
    return false;
  }

  start = DWT->CYCCNT;
  if (!wait_for_pin((uint32_t)KLBN_ULTRASONIC_ECHO_PORT,
                    KLBN_ULTRASONIC_ECHO_PIN, 0, 30)) {
    return false;
  }
  end = DWT->CYCCNT;

  duration = end - start;
  uint32_t us = duration / (SystemCoreClock / 1000000);
  data->distance_mm = (uint16_t)(us / 58);

  return true;
}

static void vUltrasonicTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    klbn_ultrasonic_data_t data;
    if (klbn_ultrasonic_action(&data)) {
      xQueueOverwrite(ultrasonic_data_queue, &data);
    }
  }
}

