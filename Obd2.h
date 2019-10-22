/*
 * Obd2.h - Communicate with SparkFun OBD-II UART
 * Created by Christopher Stevens @ https://interactive.guru on 10/11/19
 */

 #ifndef Obd2_h
 #define Obd2_h

 #include <Arduino.h>

 class Obd2 {
  
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
      const char* response = this->makePidRequest("0106");
      // if response is not null, return hex-to-int value, otherwise return null
      // OBD-II UART returns something in this format: "41 0D 00" (with fewer or more hex value sets per reading, third set+ as value(s)), needs to be converted to something more "English", an integer a good start
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get long term fuel trim, bank 1: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0107
    int getLongTermFuelTrimBank1(void)
    {
      const char* response = this->makePidRequest("0107");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get short term fuel trim, bank 2: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0108
    int getShortTermFuelTrimBank2(void)
    {
      const char* response = this->makePidRequest("0108");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get long term fuel trim, bank 2: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
    // PID 0109
    int getLongTermFuelTrimBank2(void)
    {
      const char* response = this->makePidRequest("0109");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get speed: 0 - 255 km/h
    // PID 010D
    int getSpeed(void)
    {
      const char* response = this->makePidRequest("010D");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get intake air temperature: -40 - 215 °C
    // PID 010F
    int getAirIntakeTemp(void)
    {
      const char* response = this->makePidRequest("010F");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get run time since engine start: 0 - 65,535 seconds
    // PID 011F
    int getRunTimeSinceEngineStart(void)
    {
      const char* response = this->makePidRequest("011F");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // get distance traveled with malfunction indicator lamp (MIL) on: 0 - 65,535 km
    // PID 0121
    int getDistanceWithMilOn(void)
    {
      const char* response = this->makePidRequest("0121");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // get warm-ups since codes cleared: 0 - 255 count
    // PID 0130
    int getWarmupsSinceCodesCleared(void)
    {
      const char* response = this->makePidRequest("0130");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get distance traveled since codes cleared: 0 - 65,535 km
    // PID 0131
    int getDistanceSinceCodesCleared(void)
    {
      const char* response = this->makePidRequest("0131");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // get absolute barometric pressure: 0 - 255 kPa
    // PID 0133
    int getAbsoluteBarametricPressure(void)
    {
      const char* response = this->makePidRequest("0133");
      return response ? strtol(&response[6], 0, 16) : NULL;
    }

    // get absolute load value: 0 - 25,700 %
    // PID 0143
    int getAbsoluteLoadValue(void)
    {
      const char* response = this->makePidRequest("0143");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // get time run with MIL on: 0 - 65,535 minutes
    // PID 014D
    int getTimeRunWithMilOn(void)
    {
      const char* response = this->makePidRequest("014D");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // get time since trouble codes cleared: 0 - 65,535 minutes
    // PID 014E
    int getTimeSinceTroubleCodesCleared(void)
    {
      const char* response = this->makePidRequest("014E");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // get absolute evap system vapor pressure: 0 - 327.675 kPa
    // PID 0153
    int getAbsoluteEvapSystemVaporPressure(void)
    {
      const char* response = this->makePidRequest("0153");
      return response ? ((strtol(&response[6],0,16) * 256) + strtol(&response[9],0,16)) : NULL;
    }

    // too bad, so sad for my Toyota (no data, perhaps with hidden proprietary PIDs or newer than 2016 cars?):
    // 012F: fuel tank level input
    // 01A6: odometer

    // query OBD-II UART with four character PID
    // Example: 014E
    // 01 = mode 1 (current data)
    // 2F = PID for mode 1 -> get current time since trouble codes cleared
    char* makePidRequest(String pid)
    {
      Serial1.flush();
      Serial1.println(pid);
      // give it time to respond
      delay(200);
      return this->getObd2Response();
    }
    
    // get response from OBD-II UART
    // many thanks: https://forum.sparkfun.com/viewtopic.php?t=35507
    char* getObd2Response()
    {
      char c; // currently read character
      char rxData[20]; // character buffer to store the data from the serial port
      char rxIndex = 0; // character buffer index to write to

      // don't get data if unavailable
      if (Serial1.available() <= 0) {
        return NULL;
      }

      // do get data if available
      do {
        c = Serial1.read();
        
        // cut off the response if it is too big for some reason
        if (rxIndex >= 19) {
          c = '>';
        }
        
        if((c!= '>') && (c!='\r') && (c!='\n')) {
          rxData[rxIndex++] = c; // add whatever we receive to the buffer
        }
      } while(c != '>'); // the ELM327 ends its response with this char so when we get it we exit out.
      
      rxData[rxIndex++] = '\0'; // convert char array into a string
      return rxData;
    }
};

#endif
