/* Written by Eric Olsen in 2019
 *  
 *  code to turn a commodore 64 into a bluetooth keyboard, mainly made to play nice with ios devices but will work with other devices
 *  
 *  meant to be used with an arduino bluefruit m0
 *  
 *  code written by Jeroen Janmaat in 2013, 2014 was very helpful in creating this sketch
 *  
 *  
*/


#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_UART.h>
#include <Arduino.h>
#include <SPI.h>
#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#include "BluefruitConfig.h"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);



/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.7.6"
/*=========================================================================*/


float VBatPin = A7;
int BatPercent = 100; 




const byte ROWS = 8; //eight rows
const byte COLS = 8; //eight columns
//define the cymbols on the buttons of the keypads
/*const byte keys_tbl[8][8] = {
  {KEY_1,          KEY_Tab,    KEY_Left_Control,  KEY_Escape,        KEY_Space,          KEY_Left_GUI,      KEY_Q,       KEY_2},
  {KEY_3,          KEY_W,      KEY_A,             KEY_Left_Shift,    KEY_Z,              KEY_S,             KEY_E,       KEY_4},
  {KEY_5,          KEY_R,      KEY_D,             KEY_X,             KEY_C,              KEY_F,             KEY_T,       KEY_6},
  {KEY_7,          KEY_Y,      KEY_G,             KEY_V,             KEY_B,              KEY_H,             KEY_U,       KEY_8},
  {KEY_9,          KEY_I,      KEY_J,             KEY_N,             KEY_M,              KEY_K,             KEY_O,       KEY_0},
  {KEY_Equal,      KEY_P,      KEY_L,             KEY_Comma,         KEY_Period,         KEY_Semi_colon,    KEY_2,       KEY_Dash},
  {KEY_3,          KEY_8,      KEY_Semi_colon,    KEY_Forward_slash, KEY_Right_Shift,    KEY_Equal,         KEY_6,       KEY_Home},
  {KEY_Backspace,  KEY_Return, KEY_Right_arrow,   KEY_Down_arrow,    KEY_F1,             KEY_F3,            KEY_F5,      KEY_F7}};
  */
  
// Table containing keyboard assignments with no modifier keys pressed
const String keys_tbl[8][8] = {
  {"1E", "2B", "E0", "29", "2C", "E3", "14", "1F"},
  {"20", "1A", "04", "E1", "1D", "16", "08", "21"},
  {"22", "15", "07", "1B", "06", "09", "17", "23"},
  {"24", "1C", "0A", "19", "05", "0B", "18", "25"},
  {"26", "0C", "0D", "11", "10", "0E", "12", "27"},
  {"2E", "13", "0F", "36", "37", "33", "1F", "2D"},
  {"20", "25", "33", "38", "E5", "2E", "23", "50"},
  {"2A", "28", "4F", "51", "80", "00", "00", "01"}};
  
// Accompanying modifiers to map certain keys (such as "*" and "@") to US keyboard
const String keys_tbl_mod[8][8] = {
  {"00", "00", "00", "00", "00", "00", "00", "00"},
  {"00", "00", "00", "00", "00", "00", "00", "00"},
  {"00", "00", "00", "00", "00", "00", "00", "00"},
  {"00", "00", "00", "00", "00", "00", "00", "00"},
  {"00", "00", "00", "00", "00", "00", "00", "00"},
  {"02", "00", "00", "00", "00", "02", "02", "00"},
  {"04", "02", "00", "00", "00", "00", "02", "08"},
  {"00", "00", "00", "00", "00", "01", "02", "00"}};

// Table containing keyboard assignments with Right Shift modifier key pressed
const String keys_tbl_rshift[8][8] = {
  {"1E", "2B", "E0", "29", "2C", "E3", "14", "34"},
  {"20", "1A", "04", "E1", "1D", "16", "08", "21"},
  {"22", "15", "07", "1B", "06", "09", "17", "24"},
  {"35", "1C", "0A", "19", "05", "0B", "18", "26"},
  {"27", "0C", "0D", "11", "10", "0E", "12", "27"},
  {"2E", "13", "0F", "36", "37", "2F", "1F", "2D"},
  {"20", "35", "30", "31", "E5", "31", "13", "4F"},
  {"4C", "28", "50", "52", "40", "10", "20", "08"}};
  
// Accompanying modifiers to map home and left and up arrow to US keyboard
const String keys_tbl_rshift_mod[8][8] = {
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"00", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "02", "02", "02", "02", "00"},
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "00", "02", "02", "04", "08"},
  {"00", "02", "00", "00", "00", "00", "00", "00"}};

// Table containing keyboard assignments with Left Shift modifier key pressed
const String keys_tbl_lshift[8][8] = {
  {"1E", "2B", "E0", "29", "2C", "E3", "14", "34"},
  {"20", "1A", "04", "E1", "1D", "16", "08", "21"},
  {"22", "15", "07", "1B", "06", "09", "17", "24"},
  {"34", "1C", "0A", "19", "05", "0B", "18", "26"},
  {"27", "0C", "0D", "11", "10", "0E", "12", "27"},
  {"2E", "13", "0F", "36", "37", "2F", "1F", "2D"},
  {"20", "25", "30", "38", "E5", "31", "13", "50"},
  {"4C", "28", "4F", "51", "40", "10", "20", "08"}};
  
// Accompanying modifiers to map to US keyboard
const String keys_tbl_lshift_mod[8][8] = {
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "02", "02", "02", "02", "02"},
  {"00", "02", "02", "02", "02", "02", "02", "02"},
  {"02", "02", "02", "02", "02", "02", "02", "00"},
  {"02", "02", "02", "02", "02", "00", "02", "02"},
  {"02", "02", "00", "02", "02", "02", "04", "03"},
  {"00", "02", "02", "02", "00", "00", "00", "00"}};

// Table containing keyboard assignments with GUI modifier key pressed ("CBM")
const String keys_tbl_gui[8][8] = {
  {"1E", "2B", "E0", "29", "2C", "E3", "14", "1F"},
  {"20", "1A", "04", "E1", "1D", "16", "08", "21"},
  {"22", "15", "07", "1B", "06", "09", "17", "23"},
  {"24", "1C", "0A", "19", "05", "0B", "18", "25"},
  {"26", "0C", "0D", "11", "10", "0E", "12", "27"},
  {"2E", "13", "0F", "36", "37", "30", "1F", "2D"},
  {"20", "25", "2F", "38", "E5", "2E", "23", "4A"},
  {"2A", "28", "4F", "51", "3A", "3C", "3E", "40"}};
  
// Accompanying modifiers to map certain keys (such as "\" and "|") to US keyboard
const String keys_tbl_gui_mod[8][8] = {
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"}};

  const String keys_tbl_gui_mod_rshift[8][8] = {
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"},
  {"08", "08", "08", "08", "08", "0A", "08", "08"},
  {"08", "08", "08", "08", "08", "08", "08", "08"}};



const int rowLatchPin = 5; // to pin 12 on 74HC595
const int rowClockPin = 6; // to pin 11 on 74HC595
const int rowDataPin = 10; // to pin 14 on 74HC959

const byte rowOuts[ROWS] = {254, 253, 251, 247, 239, 223, 191, 127}; // turn off one row of the keypad OUTPUT through 74hc595 shift register

// bytes to send to 74HC595 that controls RGB led
const byte red = 0;
const byte blue = 1;
const byte green = 2;
const byte white = 3; 
const byte ledColors[4] = {128, 64, 32, 224};

int ledColor = 0;

/* on 74HC595
 *  pin 16 and 10 to 3.3V
 *  pin 8 and 13 to ground
 *  pin 15 to pin 13 on c64
 *  pin 1 to pin 14 on c64
 *  pin 2 to pin 15 on c64
 *  pin 3 to pin 16 on c64
 *  pin 4 to pin 17 on c64
 *  pin 5 to pin 18 on c64
 *  pin 6 to pin 19 on c64
 *  pin 7 to pin 20 on c64
 */

  

int colPin[COLS] = {11, 12, 13, A0, A1, A2, A3, A4}; //connect to the column pinouts of the keypad INPUT



/*  on bluefruit m0 feather
 *  pin 11 to pin 12 on c64
 *  pin 12 to pin 11 on c64
 *  pin 13 to pin 10 on c64
 *  pin A0 to pin 9 on c64
 *  pin A1 to pin 8 on c64
 *  pin A2 to pin 7 on c64
 *  pin A3 to pin 6 on c64
 *  pin A4 to pin 5 on c64
 *  pin A5 to pin 3 on c64
 *  
 *  pin 1 on c64 to ground
 */

int col;
int row;
int lowRow;
int restore_pin = A5;

int timer = 0; 
int numBTfails = 0; 

// Key pressed?
bool key_pressed[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};

bool restore_key_pressed = false;
bool lshift_key_pressed = false;
bool rshift_key_pressed = false;
bool control_key_pressed = false;
bool CBM_key_pressed = false;

// Key was pressed
bool key_was_pressed[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}};

bool restore_key_was_pressed = false;
bool lshift_key_was_pressed = false;
bool rshift_key_was_pressed = false;
bool control_key_was_pressed = false;
bool CBM_key_was_pressed = false;

void updateShiftRegister(byte rowToTurnLow) {
  digitalWrite(rowLatchPin, LOW);
  delayMicroseconds(20);
  shiftOut(rowDataPin, rowClockPin, LSBFIRST, ledColors[ledColor]);
  shiftOut(rowDataPin, rowClockPin, LSBFIRST, rowOuts[rowToTurnLow]);
  digitalWrite(rowLatchPin, HIGH);
} 



// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


void setup(){

  
  Serial.begin(115200);
  
  pinMode(rowLatchPin, OUTPUT);
  pinMode(rowClockPin, OUTPUT);
  pinMode(rowDataPin, OUTPUT);

  for (col=0; col<8; col++){
    pinMode(colPin[col], INPUT_PULLUP);
  }
  
  pinMode(restore_pin, INPUT_PULLUP);



  // Bluetooth set up
   /* Initialise the module */
  Serial.print("Initialising the Bluefruit LE module: ");

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Commodore 64 Keyboard': "));
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Commodore 64 Keyboard" )) ) {
    error(F("Could not set device name?"));
  }

  /* Enable HID Service */
  Serial.println(F("Enable HID Service (including Keyboard): "));
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if ( !ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ))) {
      error(F("Could not enable Keyboard"));
    }
  }else
  {
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      error(F("Could not enable Keyboard"));
    }
  }


  
}



  
void loop(){

  //set the color of the led: red- low battery; blue-connected; green- on not connected;
  if (timer == 0) {
    timer = 50; //higher the number the longer between checking to change LED status
    BatPercent = (analogRead(VBatPin)*2*3.3/1024-3.2)*100;  
    if (BatPercent < 29) {
    ledColor = red; 
    } else {
    if (ble.isConnected()) {
      ledColor = blue; 
      numBTfails = 0; 
    } else {
      if (numBTfails < 5) {
        numBTfails = 1 + numBTfails; 
        ledColor = blue; 
      } else {
        ledColor = green; 
      }
    }
  }
  }

  timer = timer - 1;  //timer keeps from checking if bluetooth is connected and battery level on every loop

  

  //check if restore key pressed
  restore_key_pressed = !(digitalRead(restore_pin));
  if (restore_key_pressed == true && restore_key_was_pressed == false) {
    restore_key_was_pressed = true;
  } 
  if (restore_key_pressed == false && restore_key_was_pressed == true) {
    restore_key_was_pressed = false;
  } 

  //turns LOW one row at a time 
  for (lowRow=0; lowRow<8; lowRow++) {
     updateShiftRegister(lowRow);
     delay(1);
    
     // read the columns
     for (col=0; col<8; col++){
      //delay(.1);
         if (digitalRead(colPin[col]) == LOW) {
          key_pressed[lowRow][col] = true;
         } else {
          key_pressed[lowRow][col] = false;
         }




         //set moidifier keys to pressed if needed
         if (key_pressed[lowRow][col] == true) {
           if (lowRow == 1 && col == 3 && lshift_key_pressed == false) {
            lshift_key_pressed = true;
            lshift_key_was_pressed = true;
            
           }
           if (lowRow == 6 && col == 4 && rshift_key_pressed == false) {
            rshift_key_pressed = true;
            rshift_key_was_pressed = true;
           }
           if (lowRow == 0 && col == 2 && control_key_pressed == false) {
            control_key_pressed = true;
            control_key_was_pressed = true;
           }
           if (lowRow == 0 && col == 5 && CBM_key_pressed == false) {
            CBM_key_pressed = true;
            CBM_key_was_pressed = true;
           }

           //take action of sending new key press bluetooth command if key was newly pressed
           if (key_was_pressed[lowRow][col] == false) {
            
            // action if no modifiers present
            if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
              // check if an f_key was pressed
              if (lowRow == 7 && col >3) {
                 switch (col) {
                   case 4:  ble.println(F("AT+BleHidControlKey=PLAYPAUSE")); break;
                           
                   case 5:  ble.println(F("AT+BleHidControlKey=VOLUME+")); break;

                   case 6:  ble.println(F("AT+BleHidControlKey=VOLUME-")); break;

                   case 7:  ble.println(F("AT+BleHidControlKey=MUTE")); break;

                   break; 
                 }    
              } else {
                //ble.println("AT+BleKeyboardCode=00-00-04");
                ble.print("AT+BleKeyboardCode="); ble.print(keys_tbl_mod[lowRow][col]); ble.print("-00-"); ble.println(keys_tbl[lowRow][col]);
              }
            }
            
            // action if left shift only pressed
            if (lshift_key_pressed == true && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
              // check if an f_key was pressed
              if (lowRow == 7 && col >3) {
                switch (col) {
                   case 4:  ble.println(F("AT+BleHidControlKey=MEDIANEXT")); break;
                           
                   case 5:  ble.println(F("AT+BleHidControlKey=MEDIAPREVIOUS")); break;

                   case 6:  ble.println(F("AT+BleHidControlKey=SEARCH")); break;

                   case 7:  ble.println(F("AT+BleHidControlKey=HOME")); break;

                   break; 
                }
              } else {
                ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_lshift_mod[lowRow][col]); ble.print(F("-00-")); ble.println(keys_tbl_lshift[lowRow][col]);
              }
            }

            // action if right shift only pressed
            if (lshift_key_pressed == false && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
              // check if an f_key was pressed
              if (lowRow == 7 && col >3) {
                 switch (col) {
                   case 4:  ble.println(F("AT+BleHidControlKey=BRIGHTNESS+")); break;
                           
                   case 5:  ble.println(F("AT+BleHidControlKey=BRIGHTNESS-")); break;

                   case 6:  ble.println(F("AT+BleHidControlKey=SEARCH")); break;

                   case 7:  ble.println(F("AT+BleHidControlKey=0x01AE")); break;  //toggle virtual onscreen keyboard

                   break; 
                }
                
                
              } else {
                ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_rshift_mod[lowRow][col]); ble.print(F("-00-")); ble.println(keys_tbl_rshift[lowRow][col]);
              }
            }

            // action if lshift and right shift pressed
            if (lshift_key_pressed == true && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
              ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_lshift_mod[lowRow][col]); ble.print(F("-00-")); ble.println(keys_tbl_rshift[lowRow][col]);
            }

             // action if cbd only pressed
            if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == true) {
              
              ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_gui_mod[lowRow][col]); ble.print(F("-00-")); ble.println(keys_tbl_gui[lowRow][col]);
            }
             
            // action if control only pressed
            if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == true && restore_key_pressed == false && CBM_key_pressed == false) {
              
              ble.print(F("AT+BleKeyboardCode=")); ble.print(F("01-00-")); ble.println(keys_tbl[lowRow][col]);
              
            }


            // action if restore only pressed
            if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == true && CBM_key_pressed == false) {

              ble.print(F("AT+BleKeyboardCode=")); ble.print(F("04-00-")); ble.println(keys_tbl[lowRow][col]);
              
            }


            // action if right shift and restore only pressed
            if (lshift_key_pressed == false && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == true && CBM_key_pressed == false) {

              ble.print(F("AT+BleKeyboardCode=")); ble.print(F("06-00-")); ble.println(keys_tbl[lowRow][col]);
              
            }


            // action if restore and cbd  pressed
            if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == true && CBM_key_pressed == true) {
              
              ble.print(F("AT+BleKeyboardCode=")); ble.print(F("0c-00-")); ble.println(keys_tbl_gui[lowRow][col]);
              
            }

            //action if left shift and cbd pressed
            if (lshift_key_pressed == true && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == true) {
              
              ble.print(F("AT+BleKeyboardCode=")); ble.print(F("0A-00-")); ble.println(keys_tbl_lshift[lowRow][col]);
              
            }

            //action is right shift and cbd pressed
            if (lshift_key_pressed == false && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == true) {
              
              ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_gui_mod_rshift[lowRow][col]); ble.print("-00-"); ble.println(keys_tbl_rshift[lowRow][col]);
              
            }

            //action is control and left shift pressed
            if (lshift_key_pressed == true && rshift_key_pressed == false && control_key_pressed == true && restore_key_pressed == false && CBM_key_pressed == false) {
              
              ble.print(F("AT+BleKeyboardCode=")); ble.print(F("03-00-")); ble.println(keys_tbl_lshift[lowRow][col]);
              
            }
            
            //change previous key state so that on next cycle it is known that the key was already pressed
            key_was_pressed[lowRow][col] = true;
            //Serial.print("a key was pressed");  Serial.print(lowRow); Serial.println(col);
            
            
           }
           
         }


         
         //update previous state of key if needed
         if ((key_pressed[lowRow][col] == false) && (key_was_pressed[lowRow][col] == true)) {
           key_was_pressed[lowRow][col] = false;

           
           //update modifier states
           if (lowRow == 1 && col == 3 && lshift_key_was_pressed == true) {
             lshift_key_pressed = false;
             lshift_key_was_pressed = false;
            }
            if (lowRow == 6 && col == 4 && rshift_key_pressed == true) {
             rshift_key_pressed = false;
             rshift_key_was_pressed = false;
            }
            if (lowRow == 0 && col == 2 && control_key_pressed == true) {
             control_key_pressed = false;
             control_key_was_pressed = false;
            }
            if (lowRow == 0 && col == 5 && CBM_key_pressed == true) {
             CBM_key_pressed = false;
             CBM_key_was_pressed = false;
           }
       
          //take care of key releases, leaving modifying keys pressed if they are still pressed
           if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
                 
               ble.println(F("AT+BleKeyboardCode=00-00"));
              } else {
              if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == true) {
              
               ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_gui_mod[lowRow][col]); ble.println(F("-00-00"));
               
              }  else {
              if (lshift_key_pressed == false && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
            
              ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_rshift_mod[lowRow][col]); ble.println(F("-00-00"));
              
              } else {
              if (lshift_key_pressed == true && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
            
              ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_lshift_mod[lowRow][col]); ble.println(F("-00-00"));
              
              }  else {
              if (lshift_key_pressed == true && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == false) {
              
              {ble.print(F("AT+BleKeyboardCode=")); ble.print(keys_tbl_lshift_mod[lowRow][col]); ble.println(F("-00-00"));}
              
              } else {
              if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == true && restore_key_pressed == false && CBM_key_pressed == false) {
              
              ble.println(F("AT+BleKeyboardCode=01-00-00")); 
              
              } else {
              if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == true && CBM_key_pressed == false) {

              ble.println(F("AT+BleKeyboardCode=04-00-00"));
              
              } else {
              if (lshift_key_pressed == false && rshift_key_pressed == true && control_key_pressed == false && restore_key_pressed == true && CBM_key_pressed == false) {

              ble.println(F("AT+BleKeyboardCode=06-00-00"));
              
              } else {
              if (lshift_key_pressed == false && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == true && CBM_key_pressed == true) {
              
              ble.println(F("AT+BleKeyboardCode=0C-00-00"));
              
              } else {
                
              if (lshift_key_pressed == true && rshift_key_pressed == false && control_key_pressed == false && restore_key_pressed == false && CBM_key_pressed == true) {
              
              ble.println(F("AT+BleKeyboardCode=0A-00-00")); 
                
              } else {

                ble.println(F("AT+BleKeyboardCode=00-00"));
              }}}}}}}}}}

           
         } 
         
     }
     
  }

  delay(1);
  
}


 

