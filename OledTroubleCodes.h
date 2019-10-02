/*
 * OledTroubleCodes.h - Cycle through trouble codes on the SparkFun Micro OLED Qwiic
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 #ifndef OledTroubleCodes_h
 #define OledTroubleCodes_h

 #include <Arduino.h>
 #include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

 class OledTroubleCodes {
  // define class variables
  MicroOLED& oled; // the canvas we'll paint on ("&" for: "a reference member" that is shared between classes)
  byte animate; // to animate or not to animate
  String troubleCodes[10]; // make room for up to 10 trouble codes for now
  int troubleCodeFrames = 100; // how many frames to show each trouble code
  int troubleCodeFramesInt = 0; // how many frames code has been shown so far
  int troubleCodeIndex = 0; // the current trouble code index to be shown
  int borderBlinkFrames = 10; // how many frames to show/hide blinking border
  int borderBlinkFramesInt = 0; // how many frames shown/hidden so far
  bool borderBlinkToggle = true; // true: show border, false: hide border

  // public class methods
  public:
    // constructor
    OledTroubleCodes(MicroOLED &oled): oled(oled)
    {
    }

    // set if animating or not
    void setAnimate(byte animate)
    {
      this->animate = animate;
    }

    // set trouble codes to show
    void setTroubleCodes(String codes[10])
    {
      // It seems to take effort to assign an array to an array in C++, pre C++11?
      // Is there a better way to do this?
      for (int i=0; i<10; i++)
      {
        this->troubleCodes[i] = codes[i];
      } 
    }

    // clear trouble codes
    void resetTroubleCodes()
    {
      this->troubleCodes[10];
    }

    // class setup
    void setup()
    {
      //
    }

    // class loop
    void loop()
    {
      if (this->animate == 1)
      {
        this->animateTroubleCodes();
      }
    }

  // private class methods
  private:
    void animateTroubleCodes()
    {
      //toggle alert border
      if (this->borderBlinkToggle == true)
      {
        // draw border
        this->oled.rectFill(0, 0, 5, this->oled.getLCDHeight());
        this->oled.rectFill(0, 0, this->oled.getLCDWidth(), 5);
        this->oled.rectFill(0, this->oled.getLCDHeight() - 5, this->oled.getLCDWidth(), this->oled.getLCDHeight());
        this->oled.rectFill(this->oled.getLCDWidth() - 5, 0, this->oled.getLCDWidth() - 5, this->oled.getLCDHeight());
      }
      this->borderBlinkFramesInt += 1;
      if (this->borderBlinkFramesInt > this->borderBlinkFrames) {
        this->borderBlinkFramesInt = 0;
        this->borderBlinkToggle = !this->borderBlinkToggle;
      }
  
      // show trouble codes
      this->oled.setFontType(1);
      this->oled.setCursor(11, 9);
      this->oled.print("UH OH");
      this->oled.setFontType(1);
      this->oled.setCursor(11, 24);
      this->oled.print(this->troubleCodes[this->troubleCodeIndex]);
      this->troubleCodeFramesInt += 1;
      if (this->troubleCodeFramesInt > this->troubleCodeFrames) {
        this->troubleCodeFramesInt = 0;
        this->troubleCodeIndex += 1;
        if (this->troubleCodes[this->troubleCodeIndex] == NULL) {
          this->troubleCodeIndex = 0;
        }
      }
    }
};

#endif
