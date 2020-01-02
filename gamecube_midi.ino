#define LED_PIN 13
#define GC1_TX 12
#define GC1_RX 11
#define GC2_TX 14
#define GC2_RX 15

#include "gamecube.h"
#include "mapping.h"


#define GC_CONTROLLER_COUNT   (2)
#define GC_POLLING_RATE_MS    (3)
#define GC_MIDI_CHANNEL       (1)
#define GC_LOWER_THRESH       (85)
#define GC_UPPER_THRESH       (255 - GC_LOWER_THRESH)


static int8_t gc_acc = 0;
static int8_t gc_acc_digital = 0;
static int8_t gc_acc_analog[] = {0, 0, 0, 0, 0, 0};
static int8_t gc_analog_latch[] = {0, 0, 0, 0, 0, 0};


bool parse_digital_action(int action, int param1, int param2, uint8_t value)
{
  bool ret = true;
  switch (action)
  {
    case ACTION_NOTE:
      if(value)
      {
        usbMIDI.sendNoteOn(param1 + gc_acc, 100, GC_MIDI_CHANNEL);
      }
      else
      {
        usbMIDI.sendNoteOff(param1 + gc_acc, 127, GC_MIDI_CHANNEL);
      }
      break;

    case ACTION_CC:
      usbMIDI.sendControlChange(param1, value, GC_MIDI_CHANNEL);
      break;

    case ACTION_PANIC:
      if(value)
      {
        usbMIDI.sendControlChange(123, 0, GC_MIDI_CHANNEL);
      }
      break;

    case ACTION_KEY:
      if(value)
      {
        Keyboard.press(KEY_ENTER);
      }
      else
      {
        Keyboard.release(KEY_ENTER);
      }
      break;

    case ACTION_ACC:
      if(value)
      {
        gc_acc_digital += param1;
      }
      else
      {
        gc_acc_digital -= param1;  
      }
      break;
    
    default:
      ret = false;
      break;
  }
  return ret;
}


bool parse_analog_action(uint8_t ctrl_idx, int action, int param1, int param2, uint8_t value)
{
  bool ret = true;
  switch (action)
  {
    case ACTION_NOTE:
      if(value <= GC_LOWER_THRESH)
      {
        if(gc_analog_latch[ctrl_idx] == 0)
        {
          gc_analog_latch[ctrl_idx] = 1;
          usbMIDI.sendNoteOn(param1 + gc_acc, 100, GC_MIDI_CHANNEL);
        }
      }
      else if(value >= GC_UPPER_THRESH)
      {
        if(gc_analog_latch[ctrl_idx] == 0)
        {
          gc_analog_latch[ctrl_idx] = 1;
          usbMIDI.sendNoteOn(param2 + gc_acc, 100, GC_MIDI_CHANNEL);
        }
      }
      else
      {
        if(gc_analog_latch[ctrl_idx] == 1)
        {
          gc_analog_latch[ctrl_idx] = 0;
          usbMIDI.sendNoteOff(param1 + gc_acc, 127, GC_MIDI_CHANNEL);
          usbMIDI.sendNoteOff(param2 + gc_acc, 127, GC_MIDI_CHANNEL);
        }
      }
      break;

    case ACTION_ACC:
      if(value <= GC_LOWER_THRESH)
      {
        gc_acc_analog[ctrl_idx] = param1;
      }
      else if(value >= GC_UPPER_THRESH)
      {
        gc_acc_analog[ctrl_idx] = param2;
      }
      else
      {
        gc_acc_analog[ctrl_idx] = 0;
      }
      break;

    case ACTION_CC:
      usbMIDI.sendControlChange(param1, value, GC_MIDI_CHANNEL);
      break;
    
    default:
      ret = false;
      break;
  }
  return ret;
}

void update_acc(void)
{
  gc_acc = gc_acc_digital;
  for(uint8_t i = 0; i < GC_ANALOG_COUNT; i++)
  {
    gc_acc += gc_acc_analog[i];
  }
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
  gc1_last.id = 1;
  gc1_last.tx_pin = GC1_TX;
  gc1_last.rx_pin = GC1_RX;

  GameCube gc2;
  gc2.id = 2;
  gc2.tx_pin = GC2_TX;
  gc2.rx_pin = GC2_RX;

  GameCube gc2_last;
  gc2_last.id = 2;
  gc2_last.tx_pin = GC2_TX;
  gc2_last.rx_pin = GC2_RX;

  GameCube* gc_pointers[] = {&gc1, &gc2}; 
  GameCube* gc_last_pointers[] = {&gc1_last, &gc2_last};
  
  while(1)
  {
    for(uint8_t controller = 0; controller < GC_CONTROLLER_COUNT; controller++)
    {
      GameCube* gc_ptr = gc_pointers[controller];
      GameCube* gc_ptr_last = gc_last_pointers[controller];

      gc_get_status(gc_ptr);
      //gc_print_status(gc_ptr);

      if(!gc_ptr->valid)
      {
        continue;
      }

      // Check for changes in the button states
      for( uint8_t i = 0; i < GC_BUTTON_COUNT; i++)
      {
        if( gc_ptr->buttons[i] != gc_ptr_last->buttons[i] )
        {
          int8_t action = action_mapping[controller][i][0];
          int8_t param1 =  action_mapping[controller][i][1];
          int8_t param2 =  action_mapping[controller][i][2];

          if ( parse_digital_action(action, param1, param2, gc_ptr->buttons[i]) )
          {
            digitalWriteFast(LED_PIN, HIGH);
            update_acc();
          }
          
        }
        gc_ptr_last->buttons[i] = gc_ptr->buttons[i];
      }

      // Check for changes in the analog values
      for(uint8_t i = 0; i < GC_ANALOG_COUNT; i++)
      {
        if( gc_ptr->analog[i] != gc_ptr_last->analog[i] )
        {
          int8_t action = action_mapping[controller][i + GC_BUTTON_COUNT][0];
          int8_t param1 =  action_mapping[controller][i + GC_BUTTON_COUNT][1];
          int8_t param2 =  action_mapping[controller][i + GC_BUTTON_COUNT][2];

          if ( parse_analog_action(i, action, param1, param2, gc_ptr->analog[i]) )
          {
           digitalWriteFast(LED_PIN, HIGH);
           update_acc();
          }
        }
        gc_ptr_last->analog[i] = gc_ptr->analog[i];
      }
    }

    //usbMIDI.send_now();

    delay(GC_POLLING_RATE_MS);
     
    digitalWriteFast(LED_PIN, LOW);

    // MIDI Controllers should discard incoming MIDI messages.
    while (usbMIDI.read()) {}
  }
}
