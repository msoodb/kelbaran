/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belfhym.
 *
 * Belfhym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

/**
 * @file klbn_taskmanager.c
 * @brief FreeRTOS task setup and startup manager.
 *
 * Registers and launches key tasks, such as sensor polling,
 * actuator control, and safety monitoring.
 */

#include "klbn_taskmanager.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "klbn_actuator_hub.h"
#include "klbn_controller.h"
#include "klbn_sensor_hub.h"

#include "klbn_mode_button.h"
#include "klbn_ir_remote.h"

// --- Task declarations ---
static void vSensorHubTask(void *pvParameters);
static void vControllerTask(void *pvParameters);
static void vActuatorHubTask(void *pvParameters);

// --- Event Handlers ---
static void handle_sensor_data(void);

static void handle_ir_remote_event(void);
static void handle_mode_button_event(void);

// --- Task and queue settings ---
#define SENSOR_HUB_TASK_STACK 256
#define CONTROLLER_TASK_STACK 256
#define ACTUATOR_HUB_TASK_STACK 256

#define SENSOR_HUB_TASK_PRIORITY 2
#define CONTROLLER_TASK_PRIORITY 2
#define ACTUATOR_HUB_TASK_PRIORITY 2

// --- Queues ---
static QueueHandle_t xSensorDataQueue = NULL;
static QueueHandle_t xActuatorCmdQueue = NULL;

static QueueHandle_t xIRRemoteQueue = NULL;
static QueueHandle_t xModeButtonQueue = NULL;

static QueueSetHandle_t xControllerQueueSet = NULL;

void klbn_taskmanager_setup(void) {
  // Always create sensor + actuator command queues
  xSensorDataQueue = xQueueCreate(5, sizeof(klbn_sensor_data_t));
  configASSERT(xSensorDataQueue != NULL);

  xActuatorCmdQueue = xQueueCreate(5, sizeof(klbn_actuator_command_t));
  configASSERT(xActuatorCmdQueue != NULL);

  // Event queues

  xIRRemoteQueue = xQueueCreate(5, sizeof(klbn_ir_remote_event_t));
  configASSERT(xIRRemoteQueue != NULL);

  xModeButtonQueue = xQueueCreate(5, sizeof(klbn_mode_button_event_t));
  configASSERT(xModeButtonQueue != NULL);


  // Queue set
  xControllerQueueSet = xQueueCreateSet(10);
  configASSERT(xControllerQueueSet != NULL);

  xQueueAddToSet(xSensorDataQueue, xControllerQueueSet);
  xQueueAddToSet(xIRRemoteQueue, xControllerQueueSet);
  xQueueAddToSet(xModeButtonQueue, xControllerQueueSet);

  // Init all modules
  klbn_sensor_hub_init();
  klbn_actuator_hub_init();
  klbn_controller_init();

  klbn_mode_button_init(xModeButtonQueue);
  //klbn_ir_remote_init(xIRRemoteQueue);

  // Tasks (always run sensor and actuator hub)
  xTaskCreate(vSensorHubTask, "SensorHub", SENSOR_HUB_TASK_STACK, NULL,
              SENSOR_HUB_TASK_PRIORITY, NULL);

  xTaskCreate(vControllerTask, "Controller", CONTROLLER_TASK_STACK, NULL,
              CONTROLLER_TASK_PRIORITY, NULL);

  xTaskCreate(vActuatorHubTask, "ActuatorHub", ACTUATOR_HUB_TASK_STACK, NULL,
              ACTUATOR_HUB_TASK_PRIORITY, NULL);
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
    else if (activated == xIRRemoteQueue) {
      handle_ir_remote_event();
    }
    else if (activated == xModeButtonQueue) {
      handle_mode_button_event();
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

// --- Event Handlers ---
static void handle_sensor_data(void) {
  klbn_sensor_data_t sensor_data;
  klbn_actuator_command_t command;

  if (xQueueReceive(xSensorDataQueue, &sensor_data, 0) == pdPASS) {
    klbn_controller_process(&sensor_data, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}


static void handle_ir_remote_event(void) {
  klbn_ir_remote_event_t event;
  klbn_actuator_command_t command;

  if (xQueueReceive(xIRRemoteQueue, &event, 0) == pdPASS) {
    klbn_controller_process_ir_remote(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}

static void handle_mode_button_event(void) {
  klbn_mode_button_event_t event;
  klbn_actuator_command_t command;

  if (xQueueReceive(xModeButtonQueue, &event, 0) == pdPASS) {
    klbn_controller_process_mode_button(&event, &command);
    xQueueSendToBack(xActuatorCmdQueue, &command, 0);
  }
}

