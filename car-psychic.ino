/******************************************************************************
 * car-psychic.ino
 *
 * Distributed as-is; no warranty is given.
 ******************************************************************************/
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SparkFun MicroOLED library
#include <SparkFun_RV1805.h> // Include SparkFun Real Time Clock (RTC) library
#include <SparkFun_Qwiic_OpenLog_Arduino_Library.h> // Include SparkFun OpenLog library

#include "OledWarpField.h" // A star warp field for SparkFun Micro OLED Qwiic
#include "OledOilChangePrediction.h" // Show hours/days prediction to the next oil change on a SparkFun Micro OLED Qwiic
#include "OledTroubleCodes.h" // Cycle through active trouble code alerts on a SparkFun Micro OLED Qwiic
#include "Obd2.h" // Communicate with the car via SparkFun Car OBD-II UART
#include "DataLogger.h" // Log car data with SparkFun OpenLog Qwiic, SparkFun Real Time Clock Module - RV-1805 (Qwiic) and SparkFun OBD-II UART

// set state of app, determining what will be displayed
const byte STATE_OIL_CHANGE_PREDICTION = 0;
const byte STATE_TROUBLE_CODES = 1;
byte state; // assigned in setup() and loop()

// TEMPORARY
int demoLoopFrames = 300;
int demoLoopFramesInt = 0;
int demoLoopFramesToggle = true;

// MicroOLED
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1
MicroOLED* oled;

// create instance of OledWarpField class
OledWarpField* oledWarpField;

// declare OledTroubleCodes class
String troubleCodes[10]; // make room for up to 10 trouble codes for now
OledTroubleCodes* oledTroubleCodes;

// declare RTC clock
// Note: init class later as Wire (I2C communication) is initialized at this level with higher speed set, also
// initialized in RV1805. If RV1805 Wire.begin() is called in before Wire.begin() here (car-psychic.ino), this sketch freezes
RV1805* rtc;

// declare OpenLog
const int ledPin = 13; //Status LED connected to digital pin 13
const byte OpenLogAddress = 42; //Default Qwiic OpenLog I2C address
OpenLog* openLog;

// declare Obd2 class
Obd2* obd2;

// declare FuelTankLogger class
DataLogger* dataLogger;

// next oil change prediction
float nextOilChangeHours;
OledOilChangePrediction* oledOilChangePrediction;

// setup() is a required starting point for Arduino sketches
void setup() {
  // serial output for troubleshooting (may want to consider removing in future?)
  setupSerial();

  // Qwiic/I2C communication setup
  setupWire();

  // SparkFun OpenLog setup
  setupOpenLog();
  
  // SparkFun OLED setup
  setupOled();

  // SparkFun RTC setup
  setupRtc();

  // OBD-II UART setup
  obd2 = new Obd2();
  obd2->setup();

  // data logger setup
  dataLogger = new DataLogger(*rtc, *openLog, *obd2);
  dataLogger->setup();

  // warp field background display setup
  oledWarpField = new OledWarpField(*oled, 15);
  oledWarpField->setup();

  // oil change prediction shown over warp field background setup
  oledOilChangePrediction = new OledOilChangePrediction(*oled);
  oledOilChangePrediction->setup();

  // trouble code display setup
  oledTroubleCodes = new OledTroubleCodes(*oled);
  oledTroubleCodes->setup();

  // TEMPORARY
  troubleCodes[0] = "P0171";
  troubleCodes[1] = "P0300";
  troubleCodes[2] = "C0031";
  oledTroubleCodes->setTroubleCodes(troubleCodes);
  // setState(STATE_TROUBLE_CODES);
  nextOilChangeHours = 550;
  oledOilChangePrediction->setOilChangeHours(nextOilChangeHours);
  setState(STATE_OIL_CHANGE_PREDICTION);
  // END TEMPORARY

  // set initial state
  //setState(STATE_OIL_CHANGE_PREDICTION);
}

// loop() is an Arduino required method that will start running after setup()
void loop() {
  oled->clear(PAGE);  // Clear the OLED buffer
  //checkForTroubleCodes();
  obd2->loop();
  dataLogger->loop();
  oledWarpField->loop();
  oledTroubleCodes->loop();
  oledOilChangePrediction->loop();

  // TEMPORARY DEMO STATE CHANGES
  if (demoLoopFramesToggle == true) {
    setState(STATE_OIL_CHANGE_PREDICTION);
  } else {
    setState(STATE_TROUBLE_CODES);
  }
  demoLoopFramesInt += 1;
  if (demoLoopFramesInt > demoLoopFrames) {
    demoLoopFramesInt = 0;
    demoLoopFramesToggle = !demoLoopFramesToggle;
  }
  oled->display(); // Draw the OLED memory buffer
}

// setup serial port output
void setupSerial()
{
  Serial.begin(9600);
  Serial.println("Debugging has begun.");
}

// Wire setup
void setupWire()
{
  delay(100);
  Wire.begin();
  // increase clock speed to reduce OLED animation lag (check this: Is this ok for all Qwiic modules?)
  Wire.setClock(400000L);
}

// OpenLog setup
void setupOpenLog()
{
  openLog = new OpenLog();
  openLog->begin();
}

// OLED setup
void setupOled()
{
  oled = new MicroOLED(PIN_RESET, DC_JUMPER);
  oled->begin();    // Initialize the OLED
  oled->clear(ALL); // Clear the display's internal memory
  oled->display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled->clear(PAGE); // Clear the buffer.
}

// real time clock setup
void setupRtc()
{
  rtc = new RV1805();
  if (rtc->begin() == false) {
    Serial.println("Something went wrong with RTC. Check wiring.");
  }
  rtc->set24Hour();
}

// set state of the app
void setState(byte newState) {
  switch (newState)
  {
    case STATE_OIL_CHANGE_PREDICTION:
      oledWarpField->setAnimate(1);
      oledOilChangePrediction->setAnimate(1);
      oledTroubleCodes->setAnimate(0);
      break;
    case STATE_TROUBLE_CODES:
      oledWarpField->setAnimate(0);
      oledOilChangePrediction->setAnimate(0);
      oledTroubleCodes->setAnimate(1);
      break;
    default:
      Serial.println("An unknown state attempted to be set.");
  }
  state = newState;
}
