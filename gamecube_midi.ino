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
void gc_get_status(uint8_t tx_pin, uint8_t rx_pin);
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
void gc_get_status(uint8_t tx_pin, uint8_t rx_pin)
{
  noInterrupts();   // Disable interrupts to prevent distractions during time-critical operation
  
  // PART 1: Send status request to controller
  // If there is too much slop here, replace the functions with #def macros
  gc_low(tx_pin);  gc_high(tx_pin); gc_low(tx_pin);  gc_low(tx_pin);  // 0100
  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  // 0000
  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  // 0000
  gc_low(tx_pin);  gc_low(tx_pin);  gc_high(tx_pin); gc_high(tx_pin); // 0011
  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  // 0000
  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  gc_low(tx_pin);  // 000r   r=rumble
  gc_stop_bit(tx_pin);                                       // stop bit

  // PART 2: Retrieve status report from controller
  gc_data_pointer = 0;
  delayMicroseconds(1);
  //unsigned long break_time = micros() + 300;
  for(uint8_t i = 0; i < 64; i++)
  {
    delayMicroseconds(2);   // adjust as needed
    digitalWriteFast(ISR_STATUS_PIN, HIGH);
    gc_data_buffer[gc_data_pointer] = digitalReadFast(rx_pin);
    digitalWriteFast(ISR_STATUS_PIN, LOW);
    gc_data_pointer++;
    while(digitalReadFast(rx_pin) == 0)   // wait for pin to go high
    {
      //if(micros() > break_time) {break;}
    };  
    while(digitalReadFast(rx_pin) == 1)   // wait for pin to go low
    {
        //if(micros() > break_time) {break;}
    };
  }
  interrupts();   // Be sure to re-enable interrupts or the USB stuff breaks

  // PART 3: Parse status into data structure
  gc_btn_start   = gc_data_buffer[3];
  gc_btn_y       = gc_data_buffer[4];
  gc_btn_x       = gc_data_buffer[5];
  gc_btn_b       = gc_data_buffer[6];
  gc_btn_a       = gc_data_buffer[7];
  gc_btn_lt      = gc_data_buffer[9];
  gc_btn_rt      = gc_data_buffer[10];
  gc_btn_z       = gc_data_buffer[11];
  gc_btn_d_up    = gc_data_buffer[12];
  gc_btn_d_down  = gc_data_buffer[13];
  gc_btn_d_right = gc_data_buffer[14];
  gc_btn_d_left  = gc_data_buffer[15];
  
  gc_joy_x    = gc_slice_byte(2*8);
  gc_joy_y    = gc_slice_byte(3*8);
  gc_cstick_x = gc_slice_byte(4*8);
  gc_cstick_y = gc_slice_byte(5*8);
  gc_ltrig    = gc_slice_byte(6*8);
  gc_rtrig    = gc_slice_byte(7*8);
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
  noInterrupts();

}

////////////////////////////////////////////////////////////////////////////////
// GAMECUBE CONTROLLER PUBLIC FUNCTIONS

void gc_print_status(void)
{
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
    gc_get_status(GC1_TX, GC1_RX);
    
    if(1)
    {
      gc_print_status();
    }
    else
    {
      Serial.print(gc_joy_x);
      Serial.print(" ");
      Serial.print(gc_joy_y);
//      Serial.print(" ");
//      Serial.print(gc_cstick_x);
//      Serial.print(" ");
//      Serial.print(gc_cstick_y);
//      Serial.print(" ");
//      Serial.print(gc_ltrig);
//      Serial.print(" ");
//      Serial.print(gc_rtrig);
      Serial.println();
      Serial.flush();
    }
    delay(12);
  }
}
