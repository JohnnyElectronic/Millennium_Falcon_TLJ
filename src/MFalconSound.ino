/* Compiled with Board: ATTinyCore, ATtiny 25/45/85 (No bootloader) */

#define IR_SMALLD_NEC
#include <IRsmallDecoder.h>
#include "SoftwareSerial.h"

#define IR_RX_PIN           PB2  /* ATTiny85 pin 7 */

#define DFP_BUSY_PIN        PB3  /* ATTiny85 pin 2 */
#define COCKPIT_LED         PB1  /* ATTiny85 pin 6 */
#define LANDING_MODE_PIN    PB0  /* ATTiny85 pin 5 */

// Use pins 2 and 3 to communicate with DFPlayer Mini
#define PIN_MP3_TX PB4    // Connects to module's RX, PB4 ATTiny85 pin 3
#define PIN_MP3_RX -1     // Not used

/* IR Commands */
/* Command used in other module
/* OK, Toggle Flight/Landing modes, 28 */
/* *, Lighting mode, 22 */
/* #, Lighting mode, 13 */

#define VOL_UP_CMD       24   /* UP,   Volume Up */
#define VOL_DWN_CMD      82   /* DOWN, Volume Down */
#define OK_CMD           28   /* OK, Toggle Modes */

#define PLAY_NEXT_CMD      90   /* RIGHT, Play Next */
#define PLAY_PREV_CMD      8    /* LEFT, Play Previous*/
#define PLAY_0_CMD         25   /* 0, Play Track 0*/
#define PLAY_1_CMD         69   /* 1, Play Track 1*/
#define PLAY_2_CMD         70   /* 2, Play Track 2*/
#define PLAY_3_CMD         71   /* 3, Play Track 3*/
#define PLAY_4_CMD         68   /* 4, Play Track 4*/
#define PLAY_5_CMD         64   /* 5, Play Track 5*/
#define PLAY_6_CMD         67   /* 6, Play Track 6*/
#define PLAY_7_CMD         7    /* 7, Play Track 7*/
#define PLAY_8_CMD         21   /* 8, Play Track 8*/
#define PLAY_9_CMD         9    /* 9, Play Track 9*/

# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]

# define CMD_DELAY 250
# define MAX_VOL   30
# define INIT_VOL  20
# define INIT_EQ   4

SoftwareSerial dfpSerial(PIN_MP3_RX, PIN_MP3_TX);  // RX, TX

IRsmallDecoder irDecoder(IR_RX_PIN); //assuming that the IR sensor is connected to digital pin X  
irSmallD_t irData;
int landingModeActive = LOW;      /* Status of flight mode, active if High */
int currentVol = INIT_VOL;

int cplDelay = 150;              /* delay after cockpit LED level changed */
int llev;
int soundEn = true;

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

    pinMode(LANDING_MODE_PIN, INPUT);
    pinMode(DFP_BUSY_PIN, INPUT);
    pinMode(COCKPIT_LED, OUTPUT);
    digitalWrite(COCKPIT_LED, 1);

    dfpSerial.begin (9600);
    delay (2000);    /* Default was 1000 */
    dfpInit();
   // delay (500);
    setVolume(currentVol);
   // delay (CMD_DELAY);
    setEq(INIT_EQ);
   // delay (CMD_DELAY);
    playTrack(1); /* Star Wars short version */
} /* emd setup */

void loop() {

    if(irDecoder.dataAvailable(irData)){
      if (irData.keyHeld) {
          if (irData.cmd == VOL_UP_CMD) {
             currentVol =  currentVol + 1;
             if (currentVol > MAX_VOL) {
                 currentVol = MAX_VOL;
             }
             setVolume(currentVol);
          }

          if (irData.cmd == VOL_DWN_CMD) {
             currentVol =  currentVol - 1;
             if (currentVol < 0) {
                 currentVol = 0;
             }
             setVolume(currentVol);
          }
      } else {
          if (irData.cmd == VOL_UP_CMD) {
             currentVol =  currentVol + 1;
             if (currentVol > MAX_VOL) {
                 currentVol = MAX_VOL;
             }
             setVolume(currentVol);
          }

          if (irData.cmd == VOL_DWN_CMD) {
             currentVol =  currentVol - 1;
             if (currentVol < 0) {
                 currentVol = 0;
             }
             setVolume(currentVol);
          }

          if (irData.cmd == OK_CMD) {
            delay (300);
            if (digitalRead(LANDING_MODE_PIN)) {
              if (soundEn) {playTrack(2);}
            } else {
              if (soundEn) { playTrack(3);}

              randCPLed(dtpause, 200);
              randCPLed(dtpause, 200);
              randCPLed(200, 200);

              while (!digitalRead(DFP_BUSY_PIN)) {
                randCPLed(dtpause, 150);
              };

              if (soundEn) {playTrack(8);}

              randCPLed(dtpause, 200);
              randCPLed(dtpause, 200);
              randCPLed(200, 200);
              
              while (!digitalRead(DFP_BUSY_PIN)) {
                  randCPLed(dtpause, 150);
              };
              analogWrite(COCKPIT_LED, 20);
              delay(dtpause);

              if (soundEn) {playTrack(4);}    
              digitalWrite(COCKPIT_LED, 1);
            }
          }

          if (irData.cmd == PLAY_NEXT_CMD) {
            if (soundEn) {playNext();}
          } 

          if (irData.cmd == PLAY_PREV_CMD) {
            if (soundEn) {playPrevious();}
          } 

          if (irData.cmd == PLAY_0_CMD) {
            // playTrack(0);
            if (soundEn) {
              playTrack(10);   /* Turn sound options off */
            } else {
              playTrack(11);   /* Turn sound options on */
            }
            soundEn = (soundEn == false)? true : false; //toggle soundEn
          } 

          if (irData.cmd == PLAY_1_CMD) {
            if (soundEn) {playTrack(1);}
          } 

          if (irData.cmd == PLAY_2_CMD) {
            if (soundEn) {playTrack(2);}
          } 

          if (irData.cmd == PLAY_3_CMD) {
            if (soundEn) {playTrack(3);}
          } 

          if (irData.cmd == PLAY_4_CMD) {
            if (soundEn) {playTrack(4);}
          } 

          if (irData.cmd == PLAY_5_CMD) {
            if (soundEn) {playTrack(5);}
          } 

          if (irData.cmd == PLAY_6_CMD) {
            if (soundEn) {playTrack(6);}
          } 

          if (irData.cmd == PLAY_7_CMD) {
            if (soundEn) {playTrack(7);}
          } 

          if (irData.cmd == PLAY_8_CMD) {
            if (soundEn) {playTrack(8);}
          } 

          if (irData.cmd == PLAY_9_CMD) {
            if (soundEn) {playTrack(9);}
          } 
      } /* end !IR Held */
    } /* end if IR data */


  if (hyperDrive) {
            delay (300);
            if (digitalRead(LANDING_MODE_PIN)) {
              if (soundEn) {playTrack(2);}
            } else {
              if (soundEn) { playTrack(3);}

              randCPLed(dtpause, 200);
              randCPLed(dtpause, 200);
              randCPLed(200, 200);

              while (!digitalRead(DFP_BUSY_PIN)) {
                randCPLed(dtpause, 150);
              };

              if (soundEn) {playTrack(8);}

              randCPLed(dtpause, 200);
              randCPLed(dtpause, 200);
              randCPLed(200, 200);
              
              while (!digitalRead(DFP_BUSY_PIN)) {
                  randCPLed(dtpause, 150);
              };
              analogWrite(COCKPIT_LED, 20);
              delay(dtpause);

              if (soundEn) {playTrack(4);}    
              digitalWrite(COCKPIT_LED, 1);
            }
  } else {delay (100);}

} /* end loop */


void randCPLed(int rtnDelay, int levRange)
{
    int ledLev = rand() % levRange;
    analogWrite(COCKPIT_LED, ledLev);
    delay(rtnDelay);
}

void dfpInit()
{
  execute_CMD(0x3F, 0, 0);
//  delay (CMD_DELAY);
}

void pause()
{
  execute_CMD(0x0E,0,0);
//  delay (CMD_DELAY);
}

void play()
{
  execute_CMD(0x0D,0,1); 
//  delay (CMD_DELAY);
}

void playNext()
{
  execute_CMD(0x01,0,0);
//  delay (CMD_DELAY);
}

void playPrevious()
{
  execute_CMD(0x02,0,0);
//  delay (CMD_DELAY);
}

void setVolume(int volume)
{
  execute_CMD(0x06, 0, volume); // Set the volume (0-30)
//  delay (CMD_DELAY);
}

void setEq(int eq)
{
  execute_CMD(0x07, 0, eq); // Set the EQ mode (0-5, norm|pop|rock|jazz|classic|base)
//  delay (CMD_DELAY);
}

void playTrack(int track)
{
  execute_CMD(0x03, 0, track); // Play the track number (0-255)
//  delay (CMD_DELAY);
}


void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
    // Calculate the checksum (2 bytes)
    word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
    // Build the command line
    byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
    Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
    //Send the command line to the module
    for (byte k=0; k<10; k++)
    {
        dfpSerial.write( Command_line[k]);
    }

    delay(CMD_DELAY);
}
