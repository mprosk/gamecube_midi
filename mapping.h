#ifndef _MAPPING_H_
#define _MAPPING_H_

#include "Arduino.h"


#define ACTION_NOP        (0)
#define ACTION_NOTE       (1)
#define ACTION_CC         (2)
#define ACTION_PITCH_BEND (3)
#define ACTION_PANIC      (4)


static const uint8_t action_mapping[][18][2] = {
  // Controller 1
  {
    // Start
    {ACTION_PANIC, 0},      // Panic (all notes off)
    // Y
    {ACTION_NOTE, 49},
    // X
    {ACTION_NOTE, 59},
    // B
    {ACTION_NOTE, 44},
    // A
    {ACTION_NOTE, 40},
    // L
    {ACTION_NOTE, 39},
    // R
    {ACTION_NOTE, 45},
    // Z
    {ACTION_NOTE, 54},
    // D-Up
    {ACTION_NOTE, 48},
    // D-Down
    {ACTION_NOTE, 36},
    // D-Right
    {ACTION_NOTE, 38},
    // D-Left
    {ACTION_NOTE, 43},
    // Joystick X
  	{ACTION_CC, 0},
  	// Joystick Y
  	{ACTION_CC, 1},
  	// C-Stick X
  	{ACTION_CC, 2},
  	// C-Stick Y
  	{ACTION_CC, 3},
  	// Left Trigger
  	{ACTION_CC, 4},
  	// Right Trigger
  	{ACTION_CC, 5}
  },
  
  // Controller 2
  {
    // Start
    {ACTION_NOP, 0},
    // Y
    {ACTION_NOP, 0},
    // X
    {ACTION_NOP, 0},
    // B
    {ACTION_NOP, 0},
    // A
    {ACTION_NOP, 0},
    // L
    {ACTION_NOP, 0},
    // R
    {ACTION_NOP, 0},
    // Z
    {ACTION_NOP, 0},
    // D-Up
    {ACTION_NOP, 0},
    // D-Down
    {ACTION_NOP, 0},
    // D-Right
    {ACTION_NOP, 0},
    // D-Left
    {ACTION_NOP, 0},
    // Joystick X
	  {ACTION_NOP, 0},
  	// Joystick Y
  	{ACTION_NOP, 0},
  	// C-Stick X
  	{ACTION_NOP, 0},
  	// C-Stick Y
  	{ACTION_NOP, 0},
  	// Left Trigger
  	{ACTION_NOP, 0},
  	// Right Trigger
  	{ACTION_NOP, 0}
  }
};

#endif /* _MAPPING_H_ */
