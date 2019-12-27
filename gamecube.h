#ifndef _GAMECUBE_H_
#define _GAMECUBE_H_

#include "Arduino.h"

#define ISR_STATUS_PIN    (10)    
#define GC_BUTTON_COUNT   (12)    // Number of digital signals (butotns) the GameCube controller has 
#define GC_ANALOG_COUNT   (6)     // Number of analog signals (joysticks) the GameCube controller has

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
  uint8_t buttons[GC_BUTTON_COUNT];
  uint8_t analog[GC_ANALOG_COUNT];
}
GameCube;


////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
void gc_get_status(GameCube* gc);
void gc_print_status(GameCube* gc);


#endif /* _GAMECUBE_H_ */
