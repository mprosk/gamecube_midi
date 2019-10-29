# gamecube_midi

GameCube controller to MIDI HID via Teensy 3.2



## Notes

### GameCube Protocol

 http://www.int03.co.uk/crema/hardware/gamecube/gc-control.html 

 https://os.mbed.com/users/christopherjwang/code/gamecube_controller/wiki/Homepage 

### Fast IO on Teensy

Source: https://forum.pjrc.com/threads/42865-A-set-of-scope-tested-100-500-ns-delay-macros

```C
// Empirically determined by Ed Suominen with an oscilloscope and a good deal of
// pressing Ctrl+U in the Arduino window. No guarantees expressed or implied. Dedicated
// to the public domain.

#define pinNum 13
void setup() {
  pinMode(pinNum, OUTPUT);
}

#define NOP3 "nop\n\t""nop\n\t""nop\n\t"
#define NOP4 "nop\n\t""nop\n\t""nop\n\t""nop\n\t"
#define NOP6 "nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t"

// P1-5 are 100-500 ns pauses, tested with an oscilloscope (2 second
// display persistence) and a Teensy 3.2 compiling with
// Teensyduino/Arduino 1.8.1, "faster" setting
#if F_CPU == 96000000
#define P1 __asm__(NOP4 NOP4)
#define P2 __asm__(NOP6 NOP6 NOP6)
#define P3 __asm__(NOP6 NOP6 NOP6 NOP6 NOP3)
#define P4 __asm__(NOP6 NOP6 NOP6 NOP6 NOP6 NOP4 NOP4)
#define P5 __asm__(NOP6 NOP6 NOP6 NOP6 NOP6 NOP6 NOP6 NOP4 NOP3)

#elif F_CPU == 72000000
#define P1 __asm__(NOP6)
#define P2 __asm__(NOP6 NOP6)
#define P3 __asm__(NOP6 NOP6 NOP6 NOP3)
#define P4 __asm__(NOP6 NOP6 NOP6 NOP6 NOP4)
#define P5 __asm__(NOP6 NOP6 NOP6 NOP6 NOP4 NOP4 NOP3)

#elif F_CPU == 48000000
#define P1 __asm__(NOP4)
#define P2 __asm__(NOP6 NOP3)
#define P3 __asm__(NOP6 NOP4 NOP3)
#define P4 __asm__(NOP6 NOP6 NOP6)
#define P5 __asm__(NOP6 NOP6 NOP4 NOP4 NOP3)

#endif

#define PAUSE P5

void loop() {
  noInterrupts();
  digitalWriteFast(pinNum, HIGH); // 1
  PAUSE;
  digitalWriteFast(pinNum, LOW);
  PAUSE;
  digitalWriteFast(pinNum, HIGH); // 2
  PAUSE;
  digitalWriteFast(pinNum, LOW);
  PAUSE;
  digitalWriteFast(pinNum, HIGH); // 3
  PAUSE;
  digitalWriteFast(pinNum, LOW);
  PAUSE;
  interrupts();
}
```

