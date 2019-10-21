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
      Serial1.flush();
      // add a delay to give time for car wake up
      delay(2000);
      // reset the OBD-II-UART
      Serial1.println("ATZ");
      // give time to reset
      delay(2000);
      this->getObd2Response();
      // don't echo sent commands when getting responses
      Serial1.flush();
      Serial1.println("ATE0");
      delay(200);
      this->getObd2Response();
    }

    // class loop
    void loop()
    {
    }

    // get short term fuel trim, bank 1: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0106
    int getShortTermFuelTrimBank1(void)
    {
      this->makePidRequest("0106");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get long term fuel trim, bank 1: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0107
    int getLongTermFuelTrimBank1(void)
    {
      this->makePidRequest("0107");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get short term fuel trim, bank 2: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0108
    int getShortTermFuelTrimBank2(void)
    {
      this->makePidRequest("0108");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get long term fuel trim, bank 2: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0109
    int getLongTermFuelTrimBank2(void)
    {
      this->makePidRequest("0109");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get speed: 0 - 255 km/h
    // PID 010D
    int getSpeed(void)
    {
      this->makePidRequest("010D");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get intake air temperature: -40 - 215 °C
    // PID 010F
    int getAirIntakeTemp(void)
    {
      this->makePidRequest("010F");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get run time since engine start: 0 - 65,535 seconds
    // PID 011F
    int getRunTimeSinceEngineStart(void)
    {
      this->makePidRequest("011F");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // get distance traveled with malfunction indicator lamp (MIL) on: 0 - 65,535 km
    // PID 0121
    int getDistanceWithMilOn(void)
    {
      this->makePidRequest("0121");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // get warm-ups since codes cleared: 0 - 255 count
    // PID 0130
    int getWarmupsSinceCodesCleared(void)
    {
      this->makePidRequest("0130");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get distance traveled since codes cleared: 0 - 65,535 km
    // PID 0131
    int getDistanceSinceCodesCleared(void)
    {
      this->makePidRequest("0131");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // get absolute barometric pressure: 0 - 255 kPa
    // PID 0133
    int getAbsoluteBarametricPressure(void)
    {
      this->makePidRequest("0133");
      if (this->rxData == "N") {
        return NULL;
      }
      return strtol(&this->rxData[6], 0, 16);
    }

    // get absolute load value: 0 - 25,700 %
    // PID 0143
    int getAbsoluteLoadValue(void)
    {
      this->makePidRequest("0143");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // get time run with MIL on: 0 - 65,535 minutes
    // PID 014D
    int getTimeRunWithMilOn(void)
    {
      this->makePidRequest("014D");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // get time since trouble codes cleared: 0 - 65,535 minutes
    // PID 014E
    int getTimeSinceTroubleCodesCleared(void)
    {
      this->makePidRequest("014E");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // get absolute evap system vapor pressure: 0 - 327.675 kPa
    // PID 0153
    int getAbsoluteEvapSystemVaporPressure(void)
    {
      this->makePidRequest("0153");
      if (this->rxData == "N") {
        return NULL;
      }
      // TODO DRAFT: Check/alter this:
      return (strtol(&rxData[6],0,16) * 256) + strtol(&this->rxData[9],0,16);
    }

    // too bad, so sad for my Toyota (no data, perhaps with hidden proprietary PIDs or newer than 2016 cars?):
    // 012F: fuel tank level input
    // 01A6: odometer

    // query OBD-II UART with four character PID
    // Example: 014E
    // 01 = mode 1 (current data)
    // 2F = PID for mode 1 -> get current time since trouble codes cleared
    void makePidRequest(String pid)
    {
      Serial1.flush();
      Serial1.println(pid);
      // give it time to respond
      delay(200);
      this->getObd2Response();
      Serial1.flush();
    }
    

    // thanks: https://forum.sparkfun.com/viewtopic.php?t=35507
    void getObd2Response(void)
    {
      char c;
      if (Serial1.available() > 0) {
        do {
          c = Serial1.read();
          
          // cut off the response if it is too big for some reason
          if (this->rxIndex >= 19) {
            c = '>';
          }
          
          if((c!= '>') && (c!='\r') && (c!='\n')) {
            this->rxData[this->rxIndex++] = c; //Add whatever we receive to the buffer
          }
        } while(c != '>'); // The ELM327 ends its response with this char so when we get it we exit out.
        this->rxData[this->rxIndex++] = '\0';// Converts the array into a string
        this->rxIndex=0; // Set this to 0 so next time we call the read we get a "clean buffer"
      } else {
        // return "N" for "no data"
        this->rxData[this->rxIndex++] = 'N';
        this->rxData[this->rxIndex++] = '\0'; // Converts the array into a string
        this->rxIndex=0; // Set this to 0 so next time we call the read we get a "clean buffer"
      }
    }
};

#endif
