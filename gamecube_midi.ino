#define LED_PIN 13
#define GC1_TX 12
#define GC1_RX 11
#define GC2_TX 14
#define GC2_RX 15

#include "gamecube.h"

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

  // LED
  digitalWrite(LED_PIN, HIGH);
}


void loop(void)
{ 
  GameCube gc1;
  gc1.id = 1;
  gc1.tx_pin = GC1_TX;
  gc1.rx_pin = GC1_RX;

  GameCube gc2;
  gc2.id = 2;
  gc2.tx_pin = GC2_TX;
  gc2.rx_pin = GC2_RX;

  bool st_last = 0;
  
  while(1)
  {
    gc_get_status(&gc1);
    gc_print_status(&gc1);

    if(gc1.btn_start != st_last)
    {
      if(gc1.btn_start)
      {
        usbMIDI.sendNoteOn(60, 127, 1);  
      }
      else
      {
        usbMIDI.sendNoteOff(60, 0, 1);
      }
    }
    st_last = gc1.btn_start;
    
    delay(6);

    // MIDI Controllers should discard incoming MIDI messages.
    while (usbMIDI.read()) {}
  }
}
