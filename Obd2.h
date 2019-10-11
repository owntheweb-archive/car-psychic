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
      Serial.begin(9600);

      // add a delay to give time for car wake up
      delay(2000);
      
      // Reset the OBD-II-UART
      Serial.println("ATZ");

      // add a delay to give time for car wake up
      delay(1000);

      // Delete any data that may be in the serial port before we begin.
      Serial.flush();
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
      Serial.flush();

      // Query the OBD-II-UART for the fuel tank level input.
      // 01 = mode 1 (current data)
      // 2F = PID for mode 1 -> fuel tank level input
      Serial.println("012F");
      
      // Get the response from the OBD-II-UART board
      // The first response is a repeat of the command sent* (if I recall correctly)
      // The second response is what we want.
      this->getObd2Response();
      this->getObd2Response();

      // TODO This part is specific to fuel level and int, the rest is not
      if (this->rxData[0] != NULL) {
        return ((strtol(&this->rxData[6],0,16)*256)+strtol(&this->rxData[9],0,16));
      } else {
        return -1;
      }
    }

    // This is pulled directly from:
    // https://github.com/sparkfun/OBD-II_UART/blob/master/Firmware/obdIIUartQuickstart.ino
    // The getObd2Response function collects incoming data from the UART into the rxData buffer
    // and only exits when a carriage return character is seen. Once the carriage return
    // string is detected, the rxData buffer is null terminated (so we can treat it as a string)
    // and the rxData index is reset to 0 so that the next string can be copied.
    char* getObd2Response(){
      char inChar = 0;
      int waitInt = 0;
      int waitMax = 2000;
      // Keep reading characters until we get a carriage return
      while (inChar != '\r' && waitInt <= waitMax) {
        // If a character comes in on the serial port, we need to act on it.
        if (Serial.available() > 0) {
          // Start by checking if we've received the end of message character ('\r').
          if (Serial.peek() == '\r') {
            // Clear the Serial buffer
            inChar = Serial.read();
            //Put the end of string character on our data string
            this->rxData[this->rxIndex] = '\0';
            //Reset the buffer index so that the next character goes back at the beginning of the string.
            this->rxIndex = 0;
          }
          //If we didn't get the end of message character, just add the new character to the string.
          else {
            //Get the new character from the Serial port.
            inChar = Serial.read();
            //Add the new character to the string, and increment the index variable.
            this->rxData[rxIndex++] = inChar;
          }
        } else {
          delay(1);
          waitInt += 1;
        }
      }
      if (waitInt >= waitMax) {
        this->rxData[20];
        this->rxIndex = 0;
      }
      return this->rxData;
    }
};

#endif
