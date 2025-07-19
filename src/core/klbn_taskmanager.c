/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "klbn_taskmanager.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "klbn_actuator_hub.h"
#include "klbn_controller.h"
#include "klbn_sensor_hub.h"
#include "klbn_radio_hub.h"

#include "klbn_mode_button.h"

// --- Task declarations ---
static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);
static void vRadioHubTask(void *pvParameters);

// --- Event Handlers ---
static void handle_sensor_data(void);
static void handle_mode_button_event(void);
static void handle_radio_data(void);

// --- Task and queue settings ---
#define SENSOR_HUB_TASK_STACK 256
#define CONTROLLER_TASK_STACK 256
#define ACTUATOR_HUB_TASK_STACK 256
#define RADIO_HUB_TASK_STACK 256

#define SENSOR_HUB_TASK_PRIORITY 2
#define CONTROLLER_TASK_PRIORITY 2
#define ACTUATOR_HUB_TASK_PRIORITY 2
#define RADIO_HUB_TASK_PRIORITY 2

// --- Queues ---
static QueueHandle_t xSensorDataQueue = NULL;
static QueueHandle_t xActuatorCmdQueue = NULL;
static QueueHandle_t xModeButtonQueue = NULL;
static QueueHandle_t xRadioDataQueue = NULL;
static QueueHandle_t xRadioCmdQueue = NULL;

static QueueSetHandle_t xControllerQueueSet = NULL;

void klbn_taskmanager_setup(void) {
  // Always create sensor + actuator command queues
  xSensorDataQueue = xQueueCreate(5, sizeof(klbn_sensor_data_t));
  configASSERT(xSensorDataQueue != NULL);

  xActuatorCmdQueue = xQueueCreate(5, sizeof(klbn_actuator_command_t));
  configASSERT(xActuatorCmdQueue != NULL);

  // Event queues
  xModeButtonQueue = xQueueCreate(5, sizeof(klbn_mode_button_event_t));
  configASSERT(xModeButtonQueue != NULL);

  xRadioDataQueue = xQueueCreate(5, sizeof(klbn_radio_data_t));
  configASSERT(xRadioDataQueue != NULL);

  xRadioCmdQueue = xQueueCreate(5, sizeof(klbn_radio_command_t));
  configASSERT(xRadioCmdQueue != NULL);

  // Queue set
  xControllerQueueSet = xQueueCreateSet(15);
  configASSERT(xControllerQueueSet != NULL);

  xQueueAddToSet(xSensorDataQueue, xControllerQueueSet);
  xQueueAddToSet(xModeButtonQueue, xControllerQueueSet);
  xQueueAddToSet(xRadioDataQueue, xControllerQueueSet);

  // Init all modules
  klbn_sensor_hub_init();
  klbn_actuator_hub_init();
  klbn_radio_hub_init();
  klbn_controller_init();

  klbn_mode_button_init(xModeButtonQueue);

  // Tasks (always run sensor and actuator hub)
  xTaskCreate(vSensorHubTask, "SensorHub", SENSOR_HUB_TASK_STACK, NULL,
              SENSOR_HUB_TASK_PRIORITY, NULL);

  xTaskCreate(vControllerTask, "Controller", CONTROLLER_TASK_STACK, NULL,
              CONTROLLER_TASK_PRIORITY, NULL);

  xTaskCreate(vActuatorHubTask, "ActuatorHub", ACTUATOR_HUB_TASK_STACK, NULL,
              ACTUATOR_HUB_TASK_PRIORITY, NULL);

  xTaskCreate(vRadioHubTask, "RadioHub", RADIO_HUB_TASK_STACK, NULL,
              RADIO_HUB_TASK_PRIORITY, NULL);
}

void klbn_taskmanager_start(void) { vTaskStartScheduler(); }

// --- Tasks ---
static void vSensorHubTask(void *pvParameters) {
  (void)pvParameters;
  klbn_sensor_data_t sensor_data;

  for (;;) {
    if (klbn_sensor_hub_read(&sensor_data)) {
      xQueueSendToBack(xSensorDataQueue, &sensor_data, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static void vControllerTask(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    QueueSetMemberHandle_t activated =
        xQueueSelectFromSet(xControllerQueueSet, pdMS_TO_TICKS(100));

    if (activated == xSensorDataQueue) {
      handle_sensor_data();
    }
    else if (activated == xModeButtonQueue) {
      handle_mode_button_event();
    }
    else if (activated == xRadioDataQueue) {
      handle_radio_data();
    }
  }
}

static void vActuatorHubTask(void *pvParameters) {
  (void)pvParameters;

  klbn_actuator_command_t command;

  for (;;) {
    if (xQueueReceive(xActuatorCmdQueue, &command, pdMS_TO_TICKS(10)) ==
        pdPASS) {
      klbn_actuator_hub_apply(&command);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

static void vRadioHubTask(void *pvParameters) {
  (void)pvParameters;
  klbn_radio_data_t radio_data;
  klbn_radio_command_t radio_cmd;

  for (;;) {
    // Check for incoming radio data
    if (klbn_radio_hub_receive(&radio_data)) {
      xQueueSendToBack(xRadioDataQueue, &radio_data, 0);
    }
    
    // Check for outgoing radio commands
    if (xQueueReceive(xRadioCmdQueue, &radio_cmd, 0) == pdPASS) {
      klbn_radio_hub_send(&radio_cmd);
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// --- Event Handlers ---
static void handle_sensor_data(void) {
  klbn_sensor_data_t sensor_data;
  klbn_actuator_command_t command;

  if (xQueueReceive(xSensorDataQueue, &sensor_data, 0) == pdPASS) {
    klbn_controller_process(&sensor_data, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}



static void handle_mode_button_event(void) {
  klbn_mode_button_event_t event;
  klbn_actuator_command_t command;
  klbn_radio_command_t radio_cmd;

  if (xQueueReceive(xModeButtonQueue, &event, 0) == pdPASS) {
    klbn_controller_process_mode_button(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
    
    // Handle different button events
    if (event.event_type == KLBN_MODE_BUTTON_EVENT_PRESSED) {
      // Simple radio message
      const char* message = "Hello";
      radio_cmd.length = 5;
      for (uint8_t i = 0; i < 5; i++) {
        radio_cmd.data[i] = message[i];
      }
      xQueueSendToBack(xRadioCmdQueue, &radio_cmd, 0);
    }
  }
}

static void handle_radio_data(void) {
  klbn_radio_data_t radio_data;
  klbn_actuator_command_t command;

  if (xQueueReceive(xRadioDataQueue, &radio_data, 0) == pdPASS) {
    // Simple LED flash when receiving any message
    command.led.mode = KLBN_LED_MODE_BLINK;
    command.led.blink_speed_ms = 200;
    command.led.pattern_id = 1;
    command.led.brightness = 100;
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}

