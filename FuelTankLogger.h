/*
 * FuelTankLogger.h - Log car fuel level for use in model predictions
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 // TODO: What will I do for live TensorFlow Lite model usage? Store an array of values here for model use? Think about that...

 #ifndef FuelTankLogger_h
 #define FuelTankLogger_h

 #include <Arduino.h>
 #include <Wire.h>
 #include <SparkFun_Qwiic_OpenLog_Arduino_Library.h>

 #include "RtcUtils.h"; // format dates for logs
 #include "Obd2.h"; // Communicate with the car via SparkFun Car OBD-II UART
 
 class FuelTankLogger {
  // define class variables
  RV1805& rtc; // reference shared RTC clock instance
  OpenLog& openLog; // reference shared OpenLog instance
  Obd2& obd2; // reference shared Obd2 instance
  RtcUtils rtcUtils; // create RTC utils instance
  int logFrames = 1760; // time interval or number of "frames" between logs. 1760 = 1 min-ish
  int logFramesInt = 0;
  int lastFuelTankLevel;
  String lastDateTime;
  String logFileName = "fuelTankLevel.log";

  // public class methods
  public:
    // constructor
    FuelTankLogger(RV1805 &rtc, OpenLog &openLog, Obd2 &obd2): 
      // member initializer list
      rtc(rtc),
      openLog(openLog),
      obd2(obd2)
    {
    }

    // class setup
    void setup()
    {
      Serial.begin(9600);
      this->obd2.setup();
      this->logFuelTankLevel();
    }

    // class loop
    void loop()
    {
      logFramesInt += 1;
      if (logFramesInt >= logFrames) {
        logFuelTankLevel();
        logFramesInt = 0;
      }
    }

    // return the last logged mile count
    int getLastFuelTankLevel()
    {
      return this->lastFuelTankLevel;
    }

    // return the last logged date/time
    String getLastDateTime()
    {
      return this->lastDateTime;
    }

  // private class methods
  private:
    // log car gas level with date/time
    void logFuelTankLevel()
    {
      // get fuel tank level
      // this->lastFuelTankLevel = this->obd2.getFuelTankLevel();

      // get the timestamp
      this->lastDateTime = this->rtcUtils.getDateTime(this->rtc);

      // write log
      if (this->lastDateTime != "") {
        this->writeLog();
      } else {
        if (this->lastDateTime == "") {
          Serial.println("Unable to get date/time.");
        }
      }
    }

    // write date/time and mileCount to the log
    void writeLog()
    {
      String logLine = this->lastDateTime;
      this->openLog.append(this->logFileName);
      this->openLog.println(logLine);
      this->openLog.syncFile();
      Serial.println("FuelTankLevel logged: " + logLine);
    }
};

#endif
