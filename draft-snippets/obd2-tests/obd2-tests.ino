#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SparkFun MicroOLED library

// MicroOLED
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1
MicroOLED oled(PIN_RESET, DC_JUMPER); // I2C declaration

char rxData[200];
char rxIndex = 0;
int counter = 0;
char pids[][5] = {
  "0100", // Supported PIDs
  "0120", // Supported PIDs
  "0140", // Supported PIDs
  "0160", // Supported PIDs
  "0180", // Supported PIDs
  "010C", // Engine RPM
  "010D", // Vehicle speed
  "010F", // Intake air temperature
  "0111", // Throttle position
  "011F", // Run time since engine start
  "0121", // Distance traveled with malfunction indicator lamp (MIL) on
  "012F", // Fuel Tank Level Input
  "0130", // Warm-ups since codes cleared
  "0131", // Distance traveled since codes cleared
  "0133", // Absolute Barometric Pressure
  "0143", // Absolute load value
  "0145", // Relative throttle position
  "0146", // Ambient air temperature
  "014D", // Time run with MIL on
  "014E", // Time since trouble codes cleared
  "0153", // Absolute Evap system Vapor Pressure
  "0159", // Fuel rail absolute pressure
  "015C", // Engine oil temperature
  "015E", // Engine fuel rate
  "0161", // Driver's demand engine - percent torque
  "0162", // Actual engine - percent torque
  "0167", // Engine coolant temperature
  "016B", // Exhaust gas recirculation temperature
  "017F", // Engine run time
  "019D", // Engine Fuel Rate
  "019E", // Engine Exhaust Flow Rate
  "019F", // Fuel System Percentage Use
  "01A2", // Cylinder Fuel Rate
  "01A6" // Odometer
};

char pidLabels[][40] = {
  "Supported PIDs", // 0100
  "Supported PIDs", // 0120
  "Supported PIDs", // 0140
  "Supported PIDs", // 0160
  "Supported PIDs", // 0180
  "Engine RPM", // 010C
  "Vehicle speed", // 010D
  "Intake air temperature", // 010F
  "Throttle position", // 0111
  "Run time since engine start", // 011F
  "Distance traveled with MIL", // 0121
  "Fuel Tank Level Input", // 012F
  "Warm-ups since codes cleared", // 0130
  "Distance traveled since codes cleared", // 0131
  "Absolute Barometric Pressure", // 0133
  "Absolute load value", // 0143
  "Relative throttle position", // 0145
  "Ambient air temperature", // 0146
  "Time run with MIL on", // 014D
  "Time since trouble codes cleared", // 014E
  "Absolute Evap system Vapor Pressure", // 0153
  "Fuel rail absolute pressure", // 0159
  "Engine oil temperature", // 015C
  "Engine fuel rate", // 015E
  "Driver's demand engine - percent torque", // 0161
  "Actual engine - percent torque", // 0162
  "Engine coolant temperature", // 0167
  "Exhaust gas recirculation temperature", // 016B
  "Engine run time", // 017F
  "Engine Fuel Rate", // 019D
  "Engine Exhaust Flow Rate", // 019E
  "Fuel System Percentage Use", // 019F
  "Cylinder Fuel Rate", // 01A2
  "Odometer" // 01A6
};

// get everything setup
void setup() {
  Wire.begin();
  // serial
  Serial.begin(9600);
  Serial.println("Debugging has begun.");

  // oled
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(100);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.

  // odb2
  Serial1.begin(9600);
  Serial1.flush();
  //Reset the OBD-II-UART
  delay(2000);
  Serial1.println("ATZ");
  delay(2000);
  getObd2Response();
  // don't echo sent commands when getting responses
  Serial1.flush();
  Serial1.println("ATE0");
  delay(200);
  getObd2Response();
}

// cycle through PIDs and show results on the OLED screen
void loop() {
  // put your main code here, to run repeatedly:
  Serial1.flush();
  Serial1.println(pids[counter]);
  delay(200);
  getObd2Response();
  displayMessage();
  delay(3000);
  Serial1.flush();

  counter += 1;
  size_t n = sizeof(pids) / sizeof(pids[0]);
  if (counter >= n) {
    counter = 0;
  }
}

// show a formatted message on the screen
void displayMessage(void) {
  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0, 0);
  String pidLabel((char*)pidLabels[counter]);
  oled.print(pidLabel);
  oled.setCursor(0, 24);
  String message((char*)rxData);
  oled.print(message);
  oled.display();
}

// thanks: https://forum.sparkfun.com/viewtopic.php?t=35507
void getObd2Response(void)
{
  char c;
  if (Serial1.available() > 0) {
    do {
      c = Serial1.read();
      
      // cut off the response if it is too big for some reason
      if (rxIndex >= 199) {
        c = '>';
      }
      
      if((c!= '>') && (c!='\r') && (c!='\n')) {
        rxData[rxIndex++] = c; //Add whatever we receive to the buffer
      }
    } while(c != '>'); // The ELM327 ends its response with this char so when we get it we exit out.
    rxData[rxIndex++] = '\0';// Converts the array into a string
    rxIndex=0; // Set this to 0 so next time we call the read we get a "clean buffer"
  } else {
    // temp
    rxData[rxIndex++] = 'N';
    rxData[rxIndex++] = '\0'; // Converts the array into a string
    rxIndex=0; // Set this to 0 so next time we call the read we get a "clean buffer"
  }
}
