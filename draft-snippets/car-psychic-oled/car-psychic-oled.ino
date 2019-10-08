/******************************************************************************
 * drive.ino
 *
 * Distributed as-is; no warranty is given.
 ******************************************************************************/
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

// MicroOLED
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 9  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 1
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

// warp field
int starCount = 15;
// TODO: Can I not use STAR_COUNT HERE? e.g. int STARS[STAR_COUNT][3]
float stars[15][3];
float screenWidth = oled.getLCDWidth();
float screenHeight = oled.getLCDHeight();
float screenWidthDiv = screenWidth / 2;
float screenHeightDiv = screenHeight / 2;

// trouble codes
bool alert = false;
int animBlinkFrames = 10;
int animBlinkFramesInt = 0;
int animToggleAlert = true;
int troubleCodeFrames = 100;
int troubleCodeFramesInt = 0;
int troubleCodeInt = 0;
String troubleCodes[10]; // make room for up to 10 trouble codes for now
int troubleCodeCheckFrames = 300; // check OBD-II port every once in a while for trouble codes
int troubleCodeCheckFramesInt = 0;

// next oil change prediction
float nextOilChangeHours = 550;

void setup() {
  // OLED setup
  delay(100);
  Wire.begin();
  // increase clock speed to reduce OLED animation lag (check this: Is this ok for all Qwiic modules?)
  Wire.setClock(400000L);
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.

  // warp field background setup, because yes we need this
  randomSeed(analogRead(0)); // make stars more random-like
  createWarpField();

  // serial output
  Serial.begin(9600);
  Serial.println("Debugging has begun.");
}

//////////////////////////
// warp field animation //
//////////////////////////

// initialize the warp field with starting star positions
void createWarpField()
{
  for (int i=0; i<starCount; i++)
  {
    setStarPos(stars[i][0], stars[i][1], stars[i][2]);
  }
}

// place a star at a random location
void setStarPos(float & x, float & y, float & z)
{
  x = random(-screenWidthDiv, screenWidthDiv);
  y = random(-screenHeightDiv, screenHeightDiv);
  z = random(5, 10) * 0.01;
}

// animate the warp field and show it on the oled, run in loop()
void animateWarpField()
{
  if (alert == false) {
    for (int i=0; i<starCount; i++)
    {
      stars[i][2] += 0.001;
      stars[i][0] = stars[i][0] + (stars[i][0] * stars[i][2]);
      stars[i][1] = stars[i][1] + (stars[i][1] * stars[i][2]);
      
      oled.pixel(stars[i][0] + screenWidthDiv, stars[i][1] + screenHeightDiv);
      
      if (abs(stars[i][0]) >= screenWidthDiv || abs(stars[i][1]) >= screenHeightDiv || stars[i][2] >= 0.2) {
        setStarPos(stars[i][0], stars[i][1], stars[i][2]);
      }
    }
  }
}

/////////////////////////
// trouble code alerts //
/////////////////////////

// make the trouble code(s) as visible as possible
void animateAlerts()
{
  if (alert == true) {
    //toggle alert border
    if (animToggleAlert == true) {
      // draw outline
      oled.rectFill(0, 0, 5, oled.getLCDHeight());
      oled.rectFill(0, 0, oled.getLCDWidth(), 5);
      oled.rectFill(0, oled.getLCDHeight() - 5, oled.getLCDWidth(), oled.getLCDHeight());
      oled.rectFill(oled.getLCDWidth() - 5, 0, oled.getLCDWidth() - 5, oled.getLCDHeight());
    }
    animBlinkFramesInt += 1;
    if (animBlinkFramesInt > animBlinkFrames) {
      animBlinkFramesInt = 0;
      animToggleAlert = !animToggleAlert;
    }

    // show trouble codes
    oled.setFontType(1);
    oled.setCursor(11, 9);
    oled.print("UH OH");
    oled.setFontType(1);
    oled.setCursor(11, 24);
    oled.print(troubleCodes[troubleCodeInt]);
    troubleCodeFramesInt += 1;
    if (troubleCodeFramesInt > troubleCodeFrames) {
      troubleCodeFramesInt = 0;
      troubleCodeInt += 1;
      if (troubleCodes[troubleCodeInt] == NULL) {
        troubleCodeInt = 0;
      }
    }
  }
}

// clear trouble codes and alert status
void resetTroubleCodes()
{
  troubleCodes[10];
  alert = false;
}

// check OBD-II port to see if there are any trouble codes
void checkForTroubleCodes()
{
  troubleCodeCheckFramesInt += 1;
  if (troubleCodeCheckFramesInt > troubleCodeCheckFrames)
  {
    troubleCodeCheckFramesInt = 0;

    // TEMPORARY: Check the actual data soon...
    troubleCodes[0] = "P0171";
    troubleCodes[1] = "P0300";
    troubleCodes[2] = "C0031";
    alert = true;

    // if there were no trouble codes, reset codes/status to 'ok', showing oil change prediction...
    // resetTroubleCodes();
  }
}

///////////////////////////
// oil change prediction //
///////////////////////////

void predictNextOilChange()
{
  // oh goodness where to start...
}

// show oil change prediction that resulted from machine learning algorithm
void animateOilChangePrediction()
{
  if (alert == false) {
    oled.setFontType(0);
    oled.setCursor(11, 9);
    oled.print("Next Oil");
    oled.setCursor(14, 18);
    oled.print("Change:");

    if (nextOilChangeHours > 72) {
      // show in days
      float days = nextOilChangeHours / 24.0;
      String dayString = String(round(days));
      // attempt to center line of text
      if (days >= 100) {
        oled.setCursor(10, 32);
      } else {
        oled.setCursor(13, 32);
      }
      oled.setFontType(0);
      oled.print(dayString + " Days");
    } else {
      // show in hours
      String hourString = String(round(nextOilChangeHours));
      // attempt to center line of text
      if (nextOilChangeHours >= 20) {
        oled.setCursor(10, 32);
      } else {
        oled.setCursor(13, 32);
      }
      oled.setFontType(0);
      oled.print(hourString + " Hours");
    }
  }
}

//////////////
// the loop //
//////////////

void loop() {
  oled.clear(PAGE);  // Clear the OLED buffer
  checkForTroubleCodes();
  animateWarpField(); // make the stars move
  animateOilChangePrediction(); // reveal oil change date
  animateAlerts(); // animate trouble code alerts if present
  oled.display(); // Draw the OLED memory buffer
}
