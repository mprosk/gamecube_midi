#define LED_PIN 13
#define GC1_TX 12
#define GC1_RX 11
#define GC2_TX 14
#define GC2_RX 15

#include "gamecube.h"
#include "mapping.h"


#define GC_CONTROLLER_COUNT   (2)
#define MIDI_CHANNEL               (1)


bool parse_action(uint8_t action, uint8_t param, uint8_t value)
{
  bool ret = true;
  switch (action)
  {
    case ACTION_NOTE:
      if(value)
      {
        usbMIDI.sendNoteOn(param, 100, MIDI_CHANNEL);
      }
      else
      {
        usbMIDI.sendNoteOff(param, 127, MIDI_CHANNEL);
      }
      break;

    case ACTION_CC:
      usbMIDI.sendControlChange(param, value, MIDI_CHANNEL);
      break;

    case ACTION_PANIC:
      usbMIDI.sendControlChange(123, 0, MIDI_CHANNEL);
      break;
    
    default:
      ret = false;
      break;
  }
  return ret;
}


////////////////////////////////////////////////////////////////////////////////
// ARDUINO HOOKS
////////////////////////////////////////////////////////////////////////////////

void setup(void)
{
  // System
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(ISR_STATUS_PIN, OUTPUT);
    
  // GameCube Pins
  pinMode(GC1_TX, OUTPUT);
  pinMode(GC1_RX, INPUT_PULLUP);
  pinMode(GC2_TX, OUTPUT);
  pinMode(GC2_RX, INPUT_PULLUP);
}


void loop(void)
{ 
  GameCube gc1;
  gc1.id = 1;
  gc1.tx_pin = GC1_TX;
  gc1.rx_pin = GC1_RX;

  GameCube gc1_last;
  gc1.id = 1;
  gc1.tx_pin = GC1_TX;
  gc1.rx_pin = GC1_RX;

  GameCube gc2;
  gc2.id = 2;
  gc2.tx_pin = GC2_TX;
  gc2.rx_pin = GC2_RX;

  GameCube gc2_last;
  gc2.id = 2;
  gc2.tx_pin = GC2_TX;
  gc2.rx_pin = GC2_RX;

  GameCube* gc_pointers[] = {&gc1, &gc2}; 
  GameCube* gc_last_pointers[] = {&gc1_last, &gc2_last};
  
  while(1)
  {
    for(uint8_t controller = 0; controller < GC_CONTROLLER_COUNT; controller++)
    {
      GameCube* gc_ptr = gc_pointers[controller];
      GameCube* gc_ptr_last = gc_last_pointers[controller];

      gc_get_status(gc_ptr);
      gc_print_status(gc_ptr);

      if(!gc_ptr->valid)
      {
        continue;
      }

      // Check for changes in the button states
      for( uint8_t i = 0; i < GC_BUTTON_COUNT; i++)
      {
        if( gc_ptr->buttons[i] != gc_ptr_last->buttons[i] )
        {
          uint8_t action = action_mapping[controller][i][0];
          uint8_t param =  action_mapping[controller][i][1];

          if ( parse_action(action, param, gc_ptr->buttons[i]) )
          {
            digitalWriteFast(LED_PIN, HIGH);    
          }
          
        }
        gc_ptr_last->buttons[i] = gc_ptr->buttons[i];
      }

      // Check for changes in the analog values
      for(uint8_t i = 0; i < GC_ANALOG_COUNT; i++)
      {
        if( gc_ptr->analog[i] != gc_ptr_last->analog[i] )
        {
          uint8_t action = action_mapping[controller][i + GC_BUTTON_COUNT][0];
          uint8_t param =  action_mapping[controller][i + GC_BUTTON_COUNT][1];

          if ( parse_action(action, param, gc_ptr->analog[i]) )
          {
           digitalWriteFast(LED_PIN, HIGH); 
          }
        }
        gc_ptr_last->analog[i] = gc_ptr->analog[i];
      }
    }

    //usbMIDI.send_now();

    digitalWriteFast(LED_PIN, LOW);
    
    delay(6);

    // MIDI Controllers should discard incoming MIDI messages.
    while (usbMIDI.read()) {}
  }
}
