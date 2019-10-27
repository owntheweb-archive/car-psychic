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
    char rxData[20]; // character buffer to store the data from the serial port
    char rxIndex = 0; // character buffer index to write to
    bool obdBusy = false; // OBD-II requests will take about 200ms to respond (at least with vehicle tested), moving along in the loop while waiting
    const static int obdBusyPeriod = 200; // 200ms 
    unsigned long obdBusyStartTime;
    unsigned long obdBusyCurTime;

    // Service/PID constants for use in requests
    const String SHORT_TERM_FUEL_TRIM_BANK_1 = "0106";
    const String LONG_TERM_FUEL_TRIM_BANK_1 = "0107";
    const String SHORT_TERM_FUEL_TRIM_BANK_2 = "0108";
    const String LONG_TERM_FUEL_TRIM_BANK_2 = "0109";
    const String SPEED = "010D";
    const String AIR_INTAKE_TEMP = "010F";
    const String RUN_TIME_SINCE_ENGINE_START = "011F";
    const String DISTANCE_WITH_MIL_ON = "0121";
    const String WARMUPS_SINCE_CODES_CLEARED = "0130";
    const String DISTANCE_SINCE_CODES_CLEARED = "0131";
    const String ABSOLUTE_BARAMETRIC_PRESSURE = "0133";
    const String ABSOLUTE_LOAD_VALUE = "0143";
    const String TIME_RUN_WITH_MIL_ON = "014D";
    const String TIME_SINCE_TROUBLE_CODES_CLEARED = "014E";
    const String ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE = "0153";
    String lastRequestPid;
    bool lastRequestSuccess;
    
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
      if(this->obdBusy == true) {
        this->obdBusyCurTime = millis();
        if (this->obdBusyCurTime - this->obdBusyStartTime >= obdBusyPeriod) {
          // last OBD-II UART request is complete*
          this->obdBusy = false;
        }
      }
    }

    // let other classes check if OBD2 request is underway or not
    bool isBusy() {
      return this->obdBusy;
    }

    // query OBD-II UART with four character PID
    // Example: 014E
    // 01 = mode 1 (current data)
    // 4E = PID for mode 1 -> get current time since trouble codes cleared
    void makePidRequest(String pid)
    {
      // let other functionality know that OBD-II is busy for a few ms
      this->obdBusy = true;
      this->obdBusyStartTime = millis();

      // remember request PID for when request is finished
      this->lastRequestPid = pid;
      
      Serial1.flush();
      Serial1.println(pid);
      this->getObd2Response();
    }

    // Did the last request succeed?
    bool lastRequestSucceeded()
    {
      return this->lastRequestSuccess;
    }

    // get the requested data once it is ready
    // See table at: https://en.wikipedia.org/wiki/OBD-II_PIDs
    // Good to know: keep in mind that not all generic PIDs are supported by all cars. For example:
    // Toyota tested here doesn't produce data for the following (among others e.g. you usually wouldn't get NOx sensor corrected data from a Camry...):
    // 012F: fuel tank level input
    // 01A6: odometer (this one is very new I think)
    int getRequestedData()
    {
      if (this->lastRequestSuccess && !this->obdBusy) {
        if (this->lastRequestPid == SHORT_TERM_FUEL_TRIM_BANK_1) {
          // get short term fuel trim, bank 1: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
          // PID 0106
          return (100 / 128 * strtol(&rxData[6], 0, 16)) - 100;
        } else if (this->lastRequestPid == LONG_TERM_FUEL_TRIM_BANK_1) {
          // get long term fuel trim, bank 1: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
          // PID 0107
          return (100 / 128 * strtol(&rxData[6], 0, 16)) - 100;
        } else if (this->lastRequestPid == SHORT_TERM_FUEL_TRIM_BANK_2) {
          // get short term fuel trim, bank 2: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
          // PID 0108
          return (100 / 128 * strtol(&rxData[6], 0, 16)) - 100;
        } else if (this->lastRequestPid == LONG_TERM_FUEL_TRIM_BANK_2) {
          // get long term fuel trim, bank 2: -100 (reduce fuel, too rich) - 99.2 (add fuel, too lean)
          // PID 0109
          return (100 / 128 * strtol(&rxData[6], 0, 16)) - 100;
        } else if (this->lastRequestPid == SPEED) {
          // get speed: 0 - 255 km/h
          // PID 010D
          return strtol(&rxData[6], 0, 16);
        } else if (this->lastRequestPid == AIR_INTAKE_TEMP) {
          // get intake air temperature: -40 - 215 °C
          // PID 010F
          return strtol(&rxData[6], 0, 16);
        } else if (this->lastRequestPid == RUN_TIME_SINCE_ENGINE_START) {
          // get run time since engine start: 0 - 65,535 seconds
          // PID 011F
          return (strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16);
        } else if (DISTANCE_WITH_MIL_ON) {
          // get distance traveled with malfunction indicator lamp (MIL) on: 0 - 65,535 km
          // PID 0121
          return (strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16);
        } else if (this->lastRequestPid == WARMUPS_SINCE_CODES_CLEARED) {
          // get warm-ups since codes cleared: 0 - 256 count
          // PID 0130
          return strtol(&rxData[6], 0, 16);
        } else if (this->lastRequestPid == DISTANCE_SINCE_CODES_CLEARED) {
          // get distance traveled since codes cleared: 0 - 65,535 km
          // PID 0131
          return (strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16);
        } else if (this->lastRequestPid == ABSOLUTE_BARAMETRIC_PRESSURE) {
          // get absolute barometric pressure: 0 - 256 kPa
          // PID 0133
          return strtol(&rxData[6], 0, 16);
        } else if (this->lastRequestPid == ABSOLUTE_LOAD_VALUE) {
          // get absolute load value: 0 - 25,700 %
          // PID 0143
          return 100 / 255 * ((strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16));
        } else if (this->lastRequestPid == TIME_RUN_WITH_MIL_ON) {
          // get time run with MIL on: 0 - 65,535 minutes
          // PID 014D
          return (strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16);
        } else if (this->lastRequestPid == TIME_SINCE_TROUBLE_CODES_CLEARED) {
          // get time since trouble codes cleared: 0 - 65,535 minutes
          // PID 014E
          return (strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16);
        } else if (this->lastRequestPid == ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE) {
          // get time since trouble codes cleared: 0 - 65,535 minutes
          // PID 014E
          return ((strtol(&rxData[6],0,16) * 256) + strtol(&rxData[9],0,16)) / 200;
        } else {
          // no match
          return -999;
        }
      } else {
        // use -999 as "failed data request error" for now
        // TODO: Likely better way to do this (otherwise not used because lastRequestSucceeded() is being used)
        return -999;
      }
    }

  private:
    // get response from OBD-II UART
    // many thanks: https://forum.sparkfun.com/viewtopic.php?t=35507
    void getObd2Response()
    {
      char c; // currently read character

      // don't get data if unavailable
      if (Serial1.available() <= 0) {
        this->lastRequestSuccess = false;
        return;
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
      rxIndex = 0;
      this->lastRequestSuccess = true;
      return;
    }
};

#endif
