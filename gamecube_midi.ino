#define LED_PIN 13
#define GC1_TX 12
#define GC1_RX 11
#define GC2_TX 14
#define GC2_RX 15
#define ISR_STATUS_PIN 10


// GLOBALS
static bool gc_btn_start = 0;
static bool gc_btn_x = 0;
static bool gc_btn_y = 0;
static bool gc_btn_a = 0;
static bool gc_btn_b = 0;
static bool gc_btn_lt = 0;
static bool gc_btn_rt = 0;
static bool gc_btn_z = 0;
static bool gc_btn_d_up = 0;
static bool gc_btn_d_down = 0;
static bool gc_btn_d_left = 0;
static bool gc_btn_d_right = 0;
static uint8_t gc_joy_x = 0;
static uint8_t gc_joy_y = 0;
static uint8_t gc_cstick_x = 0;
static uint8_t gc_cstick_y = 0;
static uint8_t gc_ltrig = 0;
static uint8_t gc_rtrig = 0;


////////////////////////////////////////////////////////////////////////////////
// GAMECUBE CONTROLLER
////////////////////////////////////////////////////////////////////////////////

// PRIVATE VARIABLES
static volatile uint8_t gc_data_pointer = 0;
static volatile uint8_t gc_data_buffer[8 * 9];
static uint8_t gc_read_pin = 0;

// FUNCTION DECLARATIONS
void gc_low(uint8_t pin);
void gc_high(uint8_t pin);
void gc_stop_bit(uint8_t pin);
void gc_request_status(uint8_t pin);
void gc_read_status(uint8_t pin);
void gc_parse_status(void);
uint8_t gc_slice_byte(uint8_t b_start);
void gc_isr(void);
void gc_print_status(void);

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
void gc_request_status(uint8_t pin)
{
  // If there is too much slop here, replace the functions with #def macros
  noInterrupts();
  gc_low(pin);  gc_high(pin); gc_low(pin);  gc_low(pin);  // 0100
  gc_low(pin);  gc_low(pin);  gc_low(pin);  gc_low(pin);  // 0000
  gc_low(pin);  gc_low(pin);  gc_low(pin);  gc_low(pin);  // 0000
  gc_low(pin);  gc_low(pin);  gc_high(pin); gc_high(pin); // 0011
  gc_low(pin);  gc_low(pin);  gc_low(pin);  gc_low(pin);  // 0000
  gc_low(pin);  gc_low(pin);  gc_low(pin);  gc_low(pin);  // 000r   r=rumble
  gc_stop_bit(pin);                                       // stop bit
  interrupts();
}

void gc_read_status(uint8_t pin)
{
  gc_data_pointer = 0;
  gc_read_pin = pin;
  attachInterrupt(digitalPinToInterrupt(pin), gc_isr, FALLING);
  delayMicroseconds(300);
  detachInterrupt(digitalPinToInterrupt(pin));
}

void gc_parse_status(void)
{
  int8_t gc_offset = (gc_data_pointer % 8) - 1;
    
  gc_btn_start   = gc_data_buffer[3+gc_offset];
  gc_btn_y       = gc_data_buffer[4+gc_offset];
  gc_btn_x       = gc_data_buffer[5+gc_offset];
  gc_btn_b       = gc_data_buffer[6+gc_offset];
  gc_btn_a       = gc_data_buffer[7+gc_offset];
  gc_btn_lt      = gc_data_buffer[9+gc_offset];
  gc_btn_rt      = gc_data_buffer[10+gc_offset];
  gc_btn_z       = gc_data_buffer[11+gc_offset];
  gc_btn_d_up    = gc_data_buffer[12+gc_offset];
  gc_btn_d_down  = gc_data_buffer[13+gc_offset];
  gc_btn_d_right = gc_data_buffer[14+gc_offset];
  gc_btn_d_left  = gc_data_buffer[15+gc_offset];
  
  gc_joy_x    = gc_slice_byte((2*8)+gc_offset);
  gc_joy_y    = gc_slice_byte((3*8)+gc_offset);
  gc_cstick_x = gc_slice_byte((4*8)+gc_offset);
  gc_cstick_y = gc_slice_byte((5*8)+gc_offset);
  gc_ltrig    = gc_slice_byte((6*8)+gc_offset);
  gc_rtrig    = gc_slice_byte((7*8)+gc_offset);

//  gc_print_slice((2*8)+gc_offset);
//  gc_print_slice((3*8)+gc_offset);
//  gc_print_slice((4*8)+gc_offset);
//  gc_print_slice((5*8)+gc_offset);
//  gc_print_slice((6*8)+gc_offset);
//  gc_print_slice((7*8)+gc_offset);
//  Serial.println("----------");
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

void gc_print_slice(uint8_t b_start)
{
  for(uint8_t i = b_start; i < b_start + 8; i++)
  {
    Serial.print(gc_data_buffer[i]);
  }
  Serial.println();
}

void gc_isr(void)
{
  delayMicroseconds(1);   // adjust as needed
  digitalWriteFast(ISR_STATUS_PIN, HIGH);
  gc_data_buffer[gc_data_pointer] = digitalReadFast(gc_read_pin);
  digitalWriteFast(ISR_STATUS_PIN, LOW);
  gc_data_pointer++;
}

////////////////////////////////////////////////////////////////////////////////
// GAMECUBE CONTROLLER PUBLIC FUNCTIONS

void gc_print_status(void)
{
  Serial.print("OFFSET: ");     Serial.println(gc_data_pointer % 8);
  Serial.print("ST: ");     Serial.println(gc_btn_start);
  Serial.print("Y:  ");     Serial.println(gc_btn_y);
  Serial.print("X:  ");     Serial.println(gc_btn_x);
  Serial.print("B:  ");     Serial.println(gc_btn_b); 
  Serial.print("A:  ");     Serial.println(gc_btn_a);
  Serial.print("LT: ");     Serial.println(gc_btn_lt);
  Serial.print("RT: ");     Serial.println(gc_btn_rt);
  Serial.print("Z:  ");     Serial.println(gc_btn_z);
  Serial.print("DU: ");     Serial.println(gc_btn_d_up);
  Serial.print("DD: ");     Serial.println(gc_btn_d_down);
  Serial.print("DR: ");     Serial.println(gc_btn_d_right);
  Serial.print("DL: ");     Serial.println(gc_btn_d_left);
  Serial.print("JOY X: ");  Serial.println(gc_joy_x);
  Serial.print("JOY Y: ");  Serial.println(gc_joy_y);
  Serial.print("C X:   ");  Serial.println(gc_cstick_x);
  Serial.print("C Y:   ");  Serial.println(gc_cstick_y);
  Serial.print("LTRIG: ");  Serial.println(gc_ltrig);
  Serial.print("RTRIG: ");  Serial.println(gc_rtrig);
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
  while(1)
  {
    gc_request_status(GC1_TX);
    gc_read_status(GC1_RX);
    gc_parse_status();
    gc_print_status();
    delay(12);
  }
}
