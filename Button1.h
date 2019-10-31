/*
 * Button1.h - The one and only button: detects short and long press
 * Created by Christopher Stevens @ https://interactive.guru on 10/31/19
 */

 #ifndef Button1_h
 #define Button1_h

 #include <Arduino.h>
 #include <SparkFun_Qwiic_Button.h> // Include SparkFun Qwiic button library
 
 class Button1 {
  QwiicButton button;
  bool isShortClicked = false; // a short press and release has occured
  bool isShortPressed = false; // track when pressed starts to apply pulsating effect once when press starts
  bool isLongPressed = false; // the button has been pressed for a while
  int longPressTime = 5000; // 3 seconds
  int buttonBrightness = 250;
  int buttonPulsateTime = 1000;
  int buttonOffTime = 0;

  // public class methods
  public:
    // constructor
    Button1()
    {
    }

    // class setup
    void setup()
    {
      // Qwiic button setup
      if (button.begin() == false) {
        Serial.println("Device did not acknowledge! Freezing.");
      }
      this->button.LEDoff();  // start with the LED off
    }

    // class loop
    void loop()
    {
      if (this->button.isPressed()) {
        // start pulsating button with short press
        if (this->isShortPressed == false) {
          this->isShortPressed = true;
          this->button.LEDconfig(this->buttonBrightness, this->buttonPulsateTime, this->buttonOffTime);
        }
        
        // make the button light solidly when pressed for a while
        if (this->button.timeSinceLastPress() >= this->longPressTime) {
          if (this->isLongPressed == false) {
            this->isLongPressed = true;
            this->button.LEDon(255);
          }
        }
      } else if (this->button.isPressedQueueEmpty() == false) {
        // a "short click" has occured
        if (this->button.isClickedQueueEmpty() == false && this->button.timeSinceLastClick() < 200) {
          if (this->isShortClicked == false) {
            // full bright until acknowledged by parent class
            this->isShortClicked = true;
            this->button.LEDon(255);
          }
        }
      }
    }

    bool getIsShortClicked()
    {
      return this->isShortClicked;
    }

    bool getIsLongPressed()
    {
      return this->isLongPressed;
    }

    // reset button status after parent class acklowledges pressed values
    void resetButtonStatus()
    {
      this->isShortPressed = false;
      this->isShortClicked = false;
      this->isLongPressed = false;
      this->button.LEDoff();
    }
  private:
    //
};

#endif
