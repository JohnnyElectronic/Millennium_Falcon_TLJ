/* Compiled with Board: ATTinyCore, ATtiny 25/45/85 (No bootloader) */
#define IR_SMALLD_NEC
#define IR_RX_PIN  PB2   /* ATTiny85 pin 7 */

#define SEARCH_LIGHTS_PIN   PB3  /* ATTiny85 pin 2 */
#define LANDING_LIGHTS_PIN  PB4  /* ATTiny85 pin 3 */
//#define HB_LIGHTS_PIN     PB1  /* ATTiny85 pin 6 */
#define MODE_CHANGE_PIN     PB0  /* ATTiny85 pin 5 */

#define MODE_CMD            28   /* Toggle Flight/Landing modes, OK */
#define LANDING_LIGHTS_CMD  22   /* Landing Lights, * */
#define SEARCH_LIGHTS_CMD   13   /* Front Search Lights, # */

#include <IRsmallDecoder.h>

IRsmallDecoder irDecoder(IR_RX_PIN); //assuming that the IR sensor is connected to digital pin X  
irSmallD_t irData;
int landingLightsState=LOW;  /* *, Command: 22, bottom landing lights */
int searchLightsState=LOW;   /* #, Command: 13, Forward facing dual search lights */
int modeChangeState=LOW;     /* OK, Command: 28, Mode toggle */


//int hllightsState=LOW;   /* Up arrow, Command: 24, Head lights */
//int blink=LOW;           /* State of 1 sec blinker */

// NEC Protocol
// Key 1:
// Command: 69
// P=8 A=0x0 C=0x45 Raw=0xBA45FF00
// 
// Key 2:
// Command: 70
// P=8 A=0x0 C=0x46 Raw=0xB946FF000
// 
// Key 3:
// Command: 71
// P=8 A=0x0 C=0x47 Raw=0xB847FF00
// 
// Key 4:
// Command: 68
// P=8 A=0x0 C=0x44 Raw=0xBB44FF00
// 
// Key 5:
// Command: 64
// P=8 A=0x0 C=0x40 Raw=0xBF40FF00
// 
// Key 6:
// Command: 67
// P=8 A=0x0 C=0x43 Raw=0xBC43FF00
// 
// Key 7:
// Command: 7
// P=8 A=0x0 C=0x7 Raw=0xF807FF00
// 
// Key 8:
// Command: 21
// P=8 A=0x0 C=0x15 Raw=0xEA15FF00
// 
// Key 9:
// Command: 9
// P=8 A=0x0 C=0x9 Raw=0xF609FF00
// 
// 
// Key 0:
// Command: 25
// P=8 A=0x0 C=0x19 Raw=0xE619FF00
// 
// Key *:
// Command: 22
// P=8 A=0x0 C=0x16 Raw=0xE916FF000
// 
// Key #:
// Command: 13
// P=8 A=0x0 C=0xD Raw=0xF20DFF000
//
// Key OK:
// Command: 28
// P=8 A=0x0 C=0x1C Raw=0xE31CFF00
// 
// Key UP:
// Command: 24
// P=8 A=0x0 C=0x18 Raw=0xE718FF00
// 
// Key DOWN:
// Command: 82
// P=8 A=0x0 C=0x52 Raw=0xAD52FF00
// 
// Key LEFT:
// Command: 8
// P=8 A=0x0 C=0x8 Raw=0xF708FF00
// 
// Key RIGHT:
// Command: 90
// P=8 A=0x0 C=0x5A Raw=0xA55AFF000
// 

void setup() {
  // setup code here, to run once:
   pinMode(SEARCH_LIGHTS_PIN, OUTPUT);      
   pinMode(LANDING_LIGHTS_PIN, OUTPUT);  
   pinMode(MODE_CHANGE_PIN, OUTPUT);
   digitalWrite(MODE_CHANGE_PIN, modeChangeState);
} /* emd setup */

int blinkCnt = 0;

void loop() {
      if(irDecoder.dataAvailable(irData)){
        if (irData.keyHeld) {
        } else {
            if (irData.cmd == MODE_CMD) {
                modeChangeState = HIGH ; //toggle lights
                digitalWrite(MODE_CHANGE_PIN, modeChangeState);
            } /* end lights cmd */

            if (irData.cmd == SEARCH_LIGHTS_CMD) {
                searchLightsState = (searchLightsState == LOW)? HIGH : LOW; //toggle lights
                digitalWrite(SEARCH_LIGHTS_PIN, searchLightsState);
            } /* end search lights cmd */

            if (irData.cmd == LANDING_LIGHTS_CMD) {
                landingLightsState = (landingLightsState == LOW)? HIGH : LOW; //toggle lights
                digitalWrite(LANDING_LIGHTS_PIN, landingLightsState);
            } /* end landing lights cmd */
        } /* end !IR Held */
      } /* end if IR data */

      delay (50);
      if (modeChangeState) {
          modeChangeState = LOW ; //toggle lights
          digitalWrite(MODE_CHANGE_PIN, modeChangeState);
      }
} /* end loop */
