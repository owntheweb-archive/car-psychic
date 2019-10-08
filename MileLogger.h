/*
 * OledOilChangePrediction.h - Show hours/days prediction to the next oil change on a SparkFun Micro OLED Qwiic
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 // TODO: What will I do for live TensorFlow Lite model usage? Store an array of values here for model use? Think about that...

 #ifndef MileLogger_h
 #define MileLogger_h

 #include <Arduino.h>
 #include <Wire.h>
 #include <SparkFun_Qwiic_OpenLog_Arduino_Library.h>

 #include "RtcUtils.h"; // format dates for logs

 class MileLogger {
  // define class variables
  RV1805& rtc; // reference shared RTC clock instance
  OpenLog& openLog; // reference shared OpenLog instance
  RtcUtils rtcUtils; // create RTC utils instance
  int logFrames = 26500; // time interval or number of "frames" between logs. 26500 = 15 mins-ish
  int logFramesInt = 0;
  String lastMiles = "";
  String lastDateTime = "";
  String logFileName = "miles.log";

  // public class methods
  public:
    // constructor
    MileLogger(RV1805 &rtc, OpenLog &openLog): 
      // member initializer list
      rtc(rtc),
      openLog(openLog)
    {
    }

    // class setup
    void setup()
    {
      Serial.begin(9600);
      
      // log miles at start, also setting currentMiles for use in display
      // add a delay to give time for car wake up
      // TODO: IS THIS NEEDED? CAN IT BE REDUCED?
      delay(5000);
      this->logMiles();
    }

    // class loop
    void loop()
    {
      logFramesInt += 1;
      if (logFramesInt >= logFrames) {
        logMiles();
        logFramesInt = 0;
      }
    }

    // return the last logged mile count
    String getLastMiles()
    {
      return this->lastMiles;
    }

    // return the last logged date/time
    String getLastDateTime()
    {
      return this->lastDateTime;
    }

  // private class methods
  private:
    // log car mile count with date/time
    void logMiles()
    {
      // get mile count
      this->lastMiles = this->getMileCount();

      // get the timestamp
      this->lastDateTime = this->rtcUtils.getDateTime(this->rtc);

      // write log
      if (this->lastMiles != "" && this->lastDateTime != "") {
        this->writeLog();
      } else {
        if (this->lastMiles == "") {
          Serial.println("Unable to get miles.");
        }
        if (this->lastDateTime == "") {
          Serial.println("Unable to get date/time.");
        }
      }
    }

    // get mile count from SparkFun SparkFun OBD-II UART
    String getMileCount()
    {
      // TEMPORARY
      return "100000";
    }

    // write date/time and mileCount to the log
    void writeLog()
    {
      String logLine = this->lastDateTime + " " + this->lastMiles;
      this->openLog.append(this->logFileName);
      this->openLog.println(logLine);
      this->openLog.syncFile();
      Serial.println("Miles logged: " + logLine);
    }
};

#endif
