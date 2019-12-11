#ifndef _GAMECUBE_H_
#define _GAMECUBE_H_

#include "Arduino.h"

#define ISR_STATUS_PIN 10

////////////////////////////////////////////////////////////////////////////////
// STRUCTURE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////

typedef struct GameCube
{
  // Internal
  uint8_t id;         // Identifier
  uint8_t tx_pin;     // Pin to transmit messages on
  uint8_t rx_pin;     // Pin to receive messages on
  bool valid;         // Was the update valid
  
  // Controller State
  bool btn_start;     // Start button
  bool btn_y;         // Y button
  bool btn_x;         // X button
  bool btn_b;         // B button
  bool btn_a;         // A button
  bool btn_lt;        // Left Trigger button
  bool btn_rt;        // Right Trigger button
  bool btn_z;         // Z button
  bool btn_d_up;      // D-Pad Up
  bool btn_d_down;    // D-Pad Down
  bool btn_d_right;   // D-Pad Right
  bool btn_d_left;    // D-Pad Left
  uint8_t joy_x;      // Joystick X (Analog)
  uint8_t joy_y;      // Joystick Y (Analog)
  uint8_t cstick_x;   // C-Stick X (Analog)
  uint8_t cstick_y;   // C-Stick Y (Analog)
  uint8_t ltrig;      // Left Trigger (Analog)
  uint8_t rtrig;      // Right Trigger (Analog)
}
GameCube;


////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
void gc_get_status(GameCube* gc);
void gc_print_status(GameCube* gc);


#endif /* _GAMECUBE_H_ */
