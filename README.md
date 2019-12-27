# gamecube_midi

GameCube controller to MIDI HID via Teensy 3.2



# GameCube Controller Protocol

## Pinout

```
Socket on the front of the console

     #################
   #       |_7_|       #
  #                     #
 #    ___   ___   ___    #
#    | 1 | | 2 | | 3 |    #
#    █████████████████    #
#    █████████████████    #
#    |_4_| |_5_| |_6_|    #
 #                       #
  #                     #
   #                   #
     ##             ##
        ###########
```

| Pin  | Color  | Function                                                    |
| ---- | ------ | ----------------------------------------------------------- |
| 1    | Yellow | +5V for rumble motor                                        |
| 2    | Red    | Bi-directional data line. Pull up to 3.3V with 1k resistor. |
| 3    | Green  | Ground                                                      |
| 4    | White  | Ground (sometimes common with Pin 3)                        |
| 5    | -      | N/C                                                         |
| 6    | Blue   | +3.3V for logic                                             |
| 7    | Black  | Cable shield                                                |

Source:  http://www.int03.co.uk/crema/hardware/gamecube/gc-control.html 



## Protocol

|            | Bit 0 | Bit 1 | Bit 2 | Bit 3 | Bit 4 | Bit 5  | Bit 6   | Bit 7  |
| ---------- | ----- | ----- | ----- | ----- | ----- | ------ | ------- | ------ |
| **Byte 0** | 0     | 0     | 0     | Start | Y     | X      | B       | A      |
| **Byte 1** | 1     | L     | R     | Z     | D-Up  | D-Down | D-Right | D-Left |
| **Byte 2** | Joystick X Value MSB                                |Joy X Bit 6|Joy X Bit 5|Joy X Bit 4|Joy X Bit 3|Joy X Bit 2|Joy X Bit 1|Joy X LSB|
| **Byte 3** | Joystick Y Value MSB | Joy Y Bit 6 | Joy Y Bit 5 | Joy Y Bit 4 | Joy Y Bit 3 | Joy Y Bit 2 | Joy Y Bit 1 | Joy Y LSB |
| **Byte 4** | C-Stick X Value MSB | C-Stick X Bit 6 | C-Stick X Bit 5 | C-Stick X Bit 4 | C-Stick X Bit 3 | C-Stick X Bit 2 | C-Stick X Bit 1 | C-Stick X LSB |
| **Byte 5** | C-Stick Y Value MSB | C-Stick Y Bit 6 | C-Stick Y Bit 5 | C-Stick Y Bit 4 | C-Stick Y Bit 3 | C-Stick Y Bit 2 | C-Stick Y Bit 1 | C-Stick Y LSB |
| **Byte 6** | Left Trigger Value MSB | L-Trig Bit 6 | L-Trig Bit 5 | L-Trig Bit 4 | L-Trig Bit 3 | L-Trig Bit 2 | L-Trig Bit 1 | L-Trig LSB |
| **Byte 7** | Right Trigger Value MSB | R-Trig Bit 6 | R-Trig Bit 5 | R-Trig Bit 4 | R-Trig Bit 3 | R-Trig Bit 2 | R-Trig Bit 1 | R-Trig LSB |

Source:  http://www.int03.co.uk/crema/hardware/gamecube/gc-control.html 



# Reference

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

