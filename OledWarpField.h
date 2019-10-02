/*
 * OledWarpField.h - A star warp field for SparkFun Micro OLED Qwiic
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 #ifndef OledWarpField_h
 #define OledWarpField_h

 #include <Arduino.h>
 #include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

 class OledWarpField {
  // define class variables
  MicroOLED& oled; // the canvas we'll paint on ("&" for: "a reference member" that is shared between classes)
  byte starCount; // star count set in constructor
  float *stars; // this will be an array
  float screenWidthDivBy2; // oled screen width divided by two
  float screenHeightDivBy2; // oled screen height divided by two
  byte animate; // to animate or not to animate

  // public class methods
  public:
    // constructor
    OledWarpField(MicroOLED &oled, byte starCount):
      // member initializer list
      oled(oled),
      starCount(starCount),
      animate(animate)
    {
      // We're allocating one memory block for n sets of x,y,z values instead of heavier multidimensional array.
      // See: https://stackoverflow.com/questions/936687/how-do-i-declare-a-2d-array-in-c-using-new
      this->stars = new float[starCount * 3];
      this->screenWidthDivBy2 = oled.getLCDWidth() / 2;
      this->screenHeightDivBy2 = oled.getLCDHeight() / 2;
    }

    // set if animating star warp or not
    void setAnimate(byte animate)
    {
      this->animate = animate;
    }

    // class setup
    void setup()
    {
      // make stars more random-like
      randomSeed(analogRead(0));
      this->createWarpField();
    }

    // class loop
    void loop()
    {
      if (this->animate == 1) {
        this->animateWarpField();
      }
    }

  // private class methods
  private:
    // initialize the warp field with starting star positions
    void createWarpField()
    {
      for (int i=0; i<this->starCount; i++)
      {
        this->setStarPos(this->stars[i*3+0], this->stars[i*3+1], this->stars[i*3+2]);
      }
    }
    
    // place a star at a random location on the screen
    void setStarPos(float &x, float &y, float &z)
    {
      x = random(-this->screenWidthDivBy2, this->screenWidthDivBy2);
      y = random(-this->screenWidthDivBy2, this->screenWidthDivBy2);
      z = random(2, 5) * 0.01;
    }

    // animate the warp field and show it on the oled
    // note: this writes to oled memory buffer, however oled.display() will be called elsewhere to save trips over IC
    // animate the warp field and show it on the oled, run in loop()
    void animateWarpField()
    {
      for (byte i=0; i<this->starCount; i++)
      {
        this->stars[i*3+2] += 0.001;
        this->stars[i*3+0] = this->stars[i*3+0] + (this->stars[i*3+0] * this->stars[i*3+2]);
        this->stars[i*3+1] = this->stars[i*3+1] + (this->stars[i*3+1] * this->stars[i*3+2]);
        
        this->oled.pixel(this->stars[i*3+0] + this->screenWidthDivBy2, this->stars[i*3+1] + this->screenWidthDivBy2);
        
        if (abs(this->stars[i*3+0]) >= this->screenWidthDivBy2 || abs(this->stars[i*3+1]) >= this->screenWidthDivBy2 || this->stars[i*3+2] >= 0.3) {
          this->setStarPos(this->stars[i*3+0], this->stars[i*3+1], this->stars[i*3+2]);
        }
      }
    }
};

#endif
