#define LED_PIN 13
#define GC1_TX 12
#define GC1_RX 11
#define GC2_TX 14
#define GC2_RX 15
#define ISR_STATUS_PIN 10


////////////////////////////////////////////////////////////////////////////////
// GAMECUBE CONTROLLER
////////////////////////////////////////////////////////////////////////////////

// PRIVATE VARIABLES
static volatile uint8_t gc_data_pointer = 0;
static volatile uint8_t gc_data_buffer[8 * 9];

// STRUCTURE DECLARATION
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

// FUNCTION DECLARATIONS
void gc_low(uint8_t pin);
void gc_high(uint8_t pin);
void gc_stop_bit(uint8_t pin);
void gc_get_status(GameCube* gc);
uint8_t gc_slice_byte(uint8_t b_start);
void gc_print_status(GameCube* gc);

////////////////////////////////////////////////////////////////////////////////
// GAMECUBE CONTROLLER PRIVATE FUNCTIONS

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

// Send the 24-'byte' sequence to request the controller's status
void gc_get_status(GameCube* gc)
{ 
  unsigned long count = 0;
  gc_data_pointer = 0;
  gc->valid = 1;
  noInterrupts();   // Disable interrupts to prevent distractions during time-critical operation
  
  // PART 1: Send status request to controller
  // If there is too much slop here, replace the functions with #def macros
  gc_low(gc->tx_pin);  gc_high(gc->tx_pin); gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0100
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0000
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0000
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_high(gc->tx_pin); gc_high(gc->tx_pin); // 0011
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 0000
  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  gc_low(gc->tx_pin);  // 000r   r=rumble
  gc_stop_bit(gc->tx_pin);                                       // stop bit

  // PART 2: Retrieve status report from controller
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
  interrupts();   // Be sure to re-enable interrupts or the USB stuff breaks

  // PART 3: Parse status into data structure
  if(gc->valid)
  {
    gc->btn_start   = gc_data_buffer[3];
    gc->btn_y       = gc_data_buffer[4];
    gc->btn_x       = gc_data_buffer[5];
    gc->btn_b       = gc_data_buffer[6];
    gc->btn_a       = gc_data_buffer[7];
    gc->btn_lt      = gc_data_buffer[9];
    gc->btn_rt      = gc_data_buffer[10];
    gc->btn_z       = gc_data_buffer[11];
    gc->btn_d_up    = gc_data_buffer[12];
    gc->btn_d_down  = gc_data_buffer[13];
    gc->btn_d_right = gc_data_buffer[14];
    gc->btn_d_left  = gc_data_buffer[15];
  
    gc->joy_x    = gc_slice_byte(2*8) & (~0x01);
    gc->joy_y    = gc_slice_byte(3*8) & (~0x01);
    gc->cstick_x = gc_slice_byte(4*8) & (~0x01);
    gc->cstick_y = gc_slice_byte(5*8) & (~0x01);
    gc->ltrig    = gc_slice_byte(6*8) & (~0x01);
    gc->rtrig    = gc_slice_byte(7*8) & (~0x01);
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

void gc_print_status(GameCube* gc)
{
  Serial.print("ID: ");     Serial.println(gc->id);
  Serial.print("VALID: ");  Serial.println(gc->valid);
  Serial.print("ST: ");     Serial.println(gc->btn_start);
  Serial.print("Y:  ");     Serial.println(gc->btn_y);
  Serial.print("X:  ");     Serial.println(gc->btn_x);
  Serial.print("B:  ");     Serial.println(gc->btn_b); 
  Serial.print("A:  ");     Serial.println(gc->btn_a);
  Serial.print("LT: ");     Serial.println(gc->btn_lt);
  Serial.print("RT: ");     Serial.println(gc->btn_rt);
  Serial.print("Z:  ");     Serial.println(gc->btn_z);
  Serial.print("DU: ");     Serial.println(gc->btn_d_up);
  Serial.print("DD: ");     Serial.println(gc->btn_d_down);
  Serial.print("DR: ");     Serial.println(gc->btn_d_right);
  Serial.print("DL: ");     Serial.println(gc->btn_d_left);
  Serial.print("JOY X: ");  Serial.println(gc->joy_x);
  Serial.print("JOY Y: ");  Serial.println(gc->joy_y);
  Serial.print("C X:   ");  Serial.println(gc->cstick_x);
  Serial.print("C Y:   ");  Serial.println(gc->cstick_y);
  Serial.print("LTRIG: ");  Serial.println(gc->ltrig);
  Serial.print("RTRIG: ");  Serial.println(gc->rtrig);
  Serial.println("------------");
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

  // LED
  digitalWrite(LED_PIN, HIGH);
}


void loop(void)
{ 
  GameCube gc1;
  gc1.id = 1;
  gc1.tx_pin = GC1_TX;
  gc1.rx_pin = GC1_RX;
  
  while(1)
  {
    gc_get_status(&gc1);
    gc_print_status(&gc1);
    delay(6);
  }
}
