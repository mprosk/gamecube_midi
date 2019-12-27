
#include "gamecube.h"

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////
static volatile uint8_t gc_data_pointer = 0;
static volatile uint8_t gc_data_buffer[8 * 9];
static const uint8_t gc_buffer_mapping[] = {3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15};


////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
void gc_low(uint8_t pin);
void gc_high(uint8_t pin);
void gc_stop_bit(uint8_t pin);
void gc_request_status(GameCube* gc);
void gc_read_status(GameCube* gc);
void gc_parse_status(GameCube* gc);
uint8_t gc_slice_byte(uint8_t b_start);


////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void gc_low(uint8_t pin)
{
  digitalWriteFast(pin, LOW);
  delayMicroseconds(3);
  digitalWriteFast(pin, HIGH);
  delayMicroseconds(1);
}


void gc_high(uint8_t pin)
{
  digitalWriteFast(pin, LOW);
  delayMicroseconds(1);
  digitalWriteFast(pin, HIGH);
  delayMicroseconds(3);
}


void gc_stop_bit(uint8_t pin)
{
  digitalWriteFast(pin, LOW);
  delayMicroseconds(1);
  digitalWriteFast(pin, HIGH);
}


void gc_request_status(GameCube* gc)
{
  // If there is too much slop here, replace the functions with #def macros
  gc_low(gc->tx_pin);  gc_high(gc->tx_pin); gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0100
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0000
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0000
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_high(gc->tx_pin); gc_high(gc->tx_pin); // 0011
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0000
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 000r   r=rumble
  gc_stop_bit(gc->tx_pin);                                                            // stop bit
}


void gc_read_status(GameCube* gc)
{
  unsigned long count = 0;
  gc_data_pointer = 0;
  gc->valid = 1;

  for(uint8_t i = 0; i < 64; i++)
  {
    count = 0;
    while(digitalReadFast(gc->rx_pin) == 1)   // wait for pin to go low
    {
      count++;
      if(count > 10000)
      {
        gc->valid = 0;
        break;
      }
    };
    delayMicroseconds(2);   // adjust as needed
    digitalWriteFast(ISR_STATUS_PIN, HIGH);
    gc_data_buffer[gc_data_pointer] = digitalReadFast(gc->rx_pin);
    digitalWriteFast(ISR_STATUS_PIN, LOW);
    gc_data_pointer++;
    count = 0;
    while(digitalReadFast(gc->rx_pin) == 0)   // wait for pin to go high
    {
      count++;
      if(count > 10000)
      {
        gc->valid = 0;
        break;
      }
    };  
  }
}


void gc_parse_status(GameCube* gc)
{
  for (uint8_t i = 0; i < GC_BUTTON_COUNT; i++)
  {
    gc->buttons[i] = gc_data_buffer[gc_buffer_mapping[i]];
  }

  for (uint8_t i = 0; i < GC_ANALOG_COUNT; i++)
  {
    gc->analog[i] = gc_slice_byte((i + 2) * 8) & (~0x01);
  }
}


uint8_t gc_slice_byte(uint8_t b_start)
{
  uint8_t out = 0;
  for(uint8_t i = 0; i < 8; i++)
  {
    out |= gc_data_buffer[b_start + i] << (7 - i);
  }
  return out;
}


////////////////////////////////////////////////////////////////////////////////
// GAMECUBE CONTROLLER PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// Send the 24-'byte' sequence to request the controller's status
void gc_get_status(GameCube* gc)
{ 
  
  noInterrupts();   // Disable interrupts to prevent distractions during time-critical operation
  
  // PART 1: Send status request to controller
  gc_request_status(gc);

  // PART 2: Retrieve status report from controller
  gc_read_status(gc);
  
  interrupts();   // Be sure to re-enable interrupts or the USB stuff breaks

  // PART 3: Parse status into data structure
  if(gc->valid)
  {
    gc_parse_status(gc);
  }
}


void gc_print_status(GameCube* gc)
{
  Serial.print("ID: ");     Serial.println(gc->id);
  Serial.print("VALID: ");  Serial.println(gc->valid);
  Serial.print("ST: ");     Serial.println(gc->buttons[0]);
  Serial.print("Y:  ");     Serial.println(gc->buttons[1]);
  Serial.print("X:  ");     Serial.println(gc->buttons[2]);
  Serial.print("B:  ");     Serial.println(gc->buttons[3]); 
  Serial.print("A:  ");     Serial.println(gc->buttons[4]);
  Serial.print("LT: ");     Serial.println(gc->buttons[5]);
  Serial.print("RT: ");     Serial.println(gc->buttons[6]);
  Serial.print("Z:  ");     Serial.println(gc->buttons[7]);
  Serial.print("DU: ");     Serial.println(gc->buttons[8]);
  Serial.print("DD: ");     Serial.println(gc->buttons[9]);
  Serial.print("DR: ");     Serial.println(gc->buttons[10]);
  Serial.print("DL: ");     Serial.println(gc->buttons[11]);
  Serial.print("JOY X: ");  Serial.println(gc->analog[0]);
  Serial.print("JOY Y: ");  Serial.println(gc->analog[1]);
  Serial.print("C X:   ");  Serial.println(gc->analog[2]);
  Serial.print("C Y:   ");  Serial.println(gc->analog[3]);
  Serial.print("LTRIG: ");  Serial.println(gc->analog[4]);
  Serial.print("RTRIG: ");  Serial.println(gc->analog[5]);
  Serial.println("------------");
}
