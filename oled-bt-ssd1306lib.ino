#include <SPI.h>
#include "Adafruit_BLE_UART.h"
#include <Wire.h>

// pin definitions
#define OLED_DC    7
#define OLED_CS   6
#define OLED_CLK  4
#define OLED_DATA 5

#include "SSD1306Ascii.h"
#include "SSD1306AsciiSoftSpi.h"

SSD1306AsciiSoftSpi oled;

// bluetooth constants

// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
//#define ADAFRUITBLE_REQ 10
//#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
//#define ADAFRUITBLE_RST 9
//Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(10, 2, 9);



// OLED

// If using software SPI (the default case):
//#define OLED_MOSI   8
//#define OLED_CLK   7
//#define OLED_DC    6
//#define OLED_CS    5
//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


// piezo element
int touchSensor = A0;

void setup() {
  Serial.begin(9600);
  oled.reset(8);	
  
  bootDisplay();
  
  oled.clear();
  oled.println("Hello world!");
  
  delay(2000);
  
  Serial.println("setup");
 
  BTLEserial.begin();
}

/**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/

aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop() {
 
  connectBluetooth();
  // put your main code here, to run repeatedly:
  // Tell the nRF8001 to do whatever it should be working on.
  int sensorValue = analogRead(A0);
  // print out the value you read:
  if(sensorValue > 20) {
    Serial.println(sensorValue);
  }
  
   if (laststatus == ACI_EVT_CONNECTED) {
    // Lets see if there's any data for us!
     while (BTLEserial.available()) {
       
        char c = BTLEserial.read();
        oled.print(c);

     }
   } 
  delay(1);
   
   
}

void bootDisplay() {
  oled.begin(&Adafruit128x64, OLED_CS, OLED_DC, OLED_CLK, OLED_DATA);
  oled.setFont(Arial14);
}


void connectBluetooth() {
  BTLEserial.pollACI();
  
  // Ask what is our current status
   aci_evt_opcode_t status = BTLEserial.getState();
   // If the status changed....
   if (status != laststatus) {
      // print it out!
      if (status == ACI_EVT_DEVICE_STARTED) {
        oled.reset(8);
        bootDisplay();
        oled.clear();
        oled.set1X();
        oled.println("BT data");
      }
    
      if (status == ACI_EVT_CONNECTED) {
        oled.clear();
        oled.setCol(0);
        oled.println(F("* iPhone"));
        oled.println(F("connected!"));
        oled.setCol(60);
        oled.setRow(20);
        oled.setFont(CalLite24);
      }
    
      if (status == ACI_EVT_DISCONNECTED) {
        oled.clear();
        oled.println(F("* Connection lost"));
        delay(1000);
        oled.reset(8);
        bootDisplay();
        oled.clear();
      }
    
      // OK set the last status change to this one
      laststatus = status;
   }
 
}
