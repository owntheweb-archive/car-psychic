/*
 * Obd2.h - Communicate with SparkFun OBD-II UART
 * Created by Christopher Stevens @ https://interactive.guru on 10/11/19
 */

 #ifndef Obd2_h
 #define Obd2_h

 #include <Arduino.h>

 class Obd2 {
  // This is a character buffer that will store the data from the serial port (OBD-II-UART):
  char rxData[20];
  char rxIndex = 0;

  // public class methods
  public:
    // constructor
    Obd2() 
    {
    }

    // class setup
    void setup()
    {
      Serial1.begin(9600);

      // add a delay to give time for car wake up
      delay(2000);
      
      // Reset the OBD-II-UART
      Serial1.println("ATZ");

      // add a delay to give time for car wake up
      delay(1000);

      // Delete any data that may be in the serial port before we begin.
      Serial1.flush();
    }

    // class loop
    void loop()
    {
    }

    // get fuel tank level from SparkFun SparkFun OBD-II UART
    // TODO Expand this to make different kinds of reuquests, keeping in mind not all will return an int
    int getFuelTankLevel()
    {
      // Delete any data that may be in the serial port before we begin.  
      Serial1.flush();

      // Query the OBD-II-UART for the fuel tank level input.
      // 01 = mode 1 (current data)
      // 2F = PID for mode 1 -> fuel tank level input
      Serial1.println("012F");
      
      // Get the response from the OBD-II-UART board
      this->getObd2Response();

      // TODO This part is specific to fuel level and int, the rest is not
      if (this->rxData[0] != NULL) {
        // thank you: https://www.hackster.io/frankzhao/iot4car-1b07f1
        int vFuel = strtol(&rxData[6], 0, 16); // in the scale of 255
        vFuel = 1.0 * vFuel / 255 * 100; // in the scale of 100
        
        return vFuel;
      } else {
        return -1;
      }
    }

    // thank you: https://www.hackster.io/frankzhao/iot4car-1b07f1
    void getObd2Response(){
      this->rxData[20];
      this->rxIndex = 0;
      char inChar = 0;
      while(Serial1.available() > 0) {
          // Start by checking if we've received the end of message character ('\r').
          if(Serial1.peek() == '\r'){
            // reach the end of the message, clear the Serial buffer
            inChar = Serial1.read();
            this->rxData[rxIndex] = '\0';
            // Reset the buffer index so that the next character goes back at the beginning of the string
            this->rxIndex = 0;  
          }
          // If we didnt get the end of the message character, just add the new character to the string
          else{
            // Get the new character from the Serial port:
            inChar = Serial1.read();
            // add the new character to the string, and increase the index variable:
            this->rxData[rxIndex++] = inChar;
          }  
      }
    }
};

#endif
