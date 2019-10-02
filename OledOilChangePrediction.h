/*
 * OledOilChangePrediction.h - Show hours/days prediction to the next oil change on a SparkFun Micro OLED Qwiic
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 #ifndef OledOilChangePrediction_h
 #define OledOilChangePrediction_h

 #include <Arduino.h>
 #include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

 class OledOilChangePrediction {
  // define class variables
  MicroOLED& oled; // the canvas we'll paint on ("&" for: "a reference member" that is shared between classes)
  byte animate; // to animate or not to animate
  float nextOilChangeHours; // next oil change prediction

  // public class methods
  public:
    // constructor
    OledOilChangePrediction(MicroOLED &oled): oled(oled)
    {
    }

    // set if animating or not
    void setAnimate(byte animate)
    {
      this->animate = animate;
    }

    // set oil change prediction hours to show
    void setOilChangeHours(float hours)
    {
      this->nextOilChangeHours = hours;
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
        this->animateOilChangePrediction();
      }
    }

  // private class methods
  private:
    void animateOilChangePrediction()
    {
      this->oled.setFontType(0);
      this->oled.setCursor(11, 9);
      this->oled.print("Next Oil");
      this->oled.setCursor(14, 18);
      this->oled.print("Change:");
    
      if (this->nextOilChangeHours > 72) {
        // show in days
        float days = this->nextOilChangeHours / 24.0;
        String dayString = String(round(days));
        // attempt to center line of text
        if (days >= 100) {
          this->oled.setCursor(10, 32);
        } else {
          this->oled.setCursor(13, 32);
        }
        this->oled.setFontType(0);
        this->oled.print(dayString + " Days");
      } else {
        // show in hours
        String hourString = String(round(this->nextOilChangeHours));
        // attempt to center line of text
        if (this->nextOilChangeHours >= 20) {
          this->oled.setCursor(10, 32);
        } else {
          this->oled.setCursor(13, 32);
        }
        this->oled.setFontType(0);
        this->oled.print(hourString + " Hours");
      }
    }
};

#endif
