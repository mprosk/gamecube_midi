#ifndef _MAPPING_H_
#define _MAPPING_H_

#include "Arduino.h"


#define ACTION_NOP        (0)
#define ACTION_NOTE       (1)
#define ACTION_CC         (2)
#define ACTION_PITCH_BEND (3)
#define ACTION_PANIC      (4)
#define ACTION_KEY        (5)
#define ACTION_ACC        (6)


static const int action_mapping[][18][3] = {
  // Controller 1
  {
    // Start
    {ACTION_KEY, KEY_ENTER, 0},      // enter key on keyboard
    // Y
    {ACTION_NOTE, 49, 0},
    // X
    {ACTION_NOTE, 59, 0},
    // B
    {ACTION_NOTE, 44, 0},
    // A
    {ACTION_NOTE, 40, 0},
    // L
    {ACTION_NOTE, 39, 0},
    // R
    {ACTION_NOTE, 45, 0},
    // Z
    {ACTION_NOTE, 54, 0},
    // D-Up
    {ACTION_NOTE, 48, 0},
    // D-Down
    {ACTION_NOTE, 36, 0},
    // D-Right
    {ACTION_NOTE, 38, 0},
    // D-Left
    {ACTION_NOTE, 43, 0},
    // Joystick X
  	{ACTION_CC, 0, 0},
  	// Joystick Y
  	{ACTION_CC, 1, 0},
  	// C-Stick X
  	{ACTION_CC, 2, 0},
  	// C-Stick Y
  	{ACTION_CC, 3, 0},
  	// Left Trigger
  	{ACTION_CC, 4, 0},
  	// Right Trigger
  	{ACTION_CC, 5, 0}
  },
  
  // Controller 2
  {
    // Start
    {ACTION_PANIC, 0, 0},      // All notes off
    // Y
    {ACTION_NOTE, 85, 0},
    // X
    {ACTION_NOTE, 83, 0},
    // B
    {ACTION_NOTE, 82, 0},
    // A
    {ACTION_NOTE, 74, 0},
    // L
    {ACTION_NOTE, 90, 0},
    // R
    {ACTION_ACC, 1, 0},    // All notes +1
    // Z
    {ACTION_ACC, -1, 0},    // All notes -1
    // D-Up
    {ACTION_NOTE, 78, 0},
    // D-Down
    {ACTION_NOTE, 73, 0},
    // D-Right
    {ACTION_NOTE, 80, 0},
    // D-Left
    {ACTION_NOTE, 75, 0},
    // Joystick X
    {ACTION_NOP, 0, 0},
    // Joystick Y
    {ACTION_ACC, -2, 2},      // UP = All notes +2, DOWN = All notes -2
    // C-Stick X
    {ACTION_NOTE, 83, 81},    // LEFT = 83, RIGHT = 81
    // C-Stick Y
    {ACTION_NOTE, 77, 86},    // UP = 86, DOWN = 77
    // Left Trigger
    {ACTION_NOP, 0, 0},
    // Right Trigger
    {ACTION_NOP, 0, 0}
  }
};

#endif /* _MAPPING_H_ */
