


#ifndef KLBN_STATE_H
#define KLBN_STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  KLBN_MODE_AUTO = 0,
  KLBN_MODE_MANUAL,
  KLBN_MODE_EMERGENCY
} klbn_mode_t;

typedef enum {
  KLBN_MOTION_FORWARD = 0,
  KLBN_MOTION_BACKWARD,
  KLBN_MOTION_ROTATE_LEFT,
  KLBN_MOTION_ROTATE_RIGHT,
  KLBN_MOTION_STOP
} klbn_motion_state_t;

typedef struct {
  klbn_mode_t current_mode;
  klbn_motion_state_t motion_state;
  bool overheat;
  bool obstacle_near;
  bool manual_override;
  uint32_t last_mode_change_time;
} klbn_system_state_t;

#endif // KLBN_STATE_H
