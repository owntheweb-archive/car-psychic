/*
 * DataLogger.h - Log car fuel level for use in model predictions
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 // TODO: What will I do for live TensorFlow Lite model usage? Store an array of values here for model use? Think about that...

 #ifndef DataLogger_h
 #define DataLogger_h

 #include <Arduino.h>
 #include <SparkFun_Qwiic_OpenLog_Arduino_Library.h>

 #include "RtcUtils.h" // format dates for logs
 #include "Obd2.h" // Communicate with the car via SparkFun Car OBD-II UART
 
 class DataLogger {
  // define class variables
  RV1805& rtc; // reference shared RTC clock instance
  OpenLog& openLog; // reference shared OpenLog instance
  Obd2& obd2; // reference shared Obd2 instance
  RtcUtils rtcUtils; // create RTC utils instance
  unsigned long startTime; // milliseconds to start timing from (no RTC required, based on milliseconds since Arduino start)
  unsigned long curTime; // current milliseconds
  const static unsigned long idlePeriod = 15000; // one minute
  const static int logCount = 15; // logging 15 different readings from OBD-II UART
  int logIndex = 0; // only going to log one reading at a time per loop to prevent major lag
  String rtcDateTime; // last RTC date/time for log
  bool logBusy = false; // OpenLog will take about 15ms to write, moving along in the loop while waiting
  const static int logBusyPeriod = 30; // 30ms 
  unsigned long logBusyStartTime;
  // log each data point type separately
  const char logFiles[logCount][40];

  // public class methods
  public:
    // constructor
    DataLogger(RV1805 &rtc, OpenLog &openLog, Obd2 &obd2): 
      // member initializer list
      rtc(rtc),
      openLog(openLog),
      obd2(obd2),
      logFiles {
        "stfueltrimb1.txt",
        "ltfueltrimb1.txt",
        "stfueltrimb2.txt",
        "ltfueltrimb2.txt",
        "speed.txt",
        "intaketemp.txt",
        "runrimeenginestart.txt",
        "distancewithmil.txt",
        "warmupssincecleared.txt",
        "distancesincecleared.txt",
        "absbarampressure.txt",
        "absload.txt",
        "timerunwithmil.txt",
        "timesincecleared.txt",
        "absevapvaporpressure.txt"
      }
    {
    }

    // class setup
    void setup()
    {
      Serial.begin(9600);
      this->startTime = millis();
      this->curTime = millis();
    }

    // class loop
    void loop()
    {
      this->curTime = millis();
      if (this->curTime - this->startTime >= this->idlePeriod) {
        // skip this loop if OpenLog is still busy logging
        if (this->logBusy == true) {
          unsigned long logBusyCurTime = millis();
          if (logBusyCurTime - this->logBusyStartTime >= this->logBusyPeriod) {
            // TODO: It may help to check status of logger here (if possible), make it less time-only based
            this->logBusy = false;
          } else {
            return;
          }
        }
        
        // log data point for current logIndex
        // as it takes about 200ms to request data from the OBD-II UART and log, logging one piece
        // of data per loop will prevent freeze in OLED, also keep other tasks responsive
        this->logDataPoint();

        // move to the next data point or reset/pause for another minute before logging again
        this->logIndex += 1;
        if (this->logIndex >= this->logCount) {
          this->logIndex = 0;
          this->startTime = millis();
        }
      }
    }

  // private class methods
  private:
    // log car gas level with date/time
    void logDataPoint()
    {
      // get the YYYYMMDDHHMMSS timestamp
      String dateTime = this->rtcUtils.getDateTime(this->rtc);

      // log data point based on current logIndex
      int response;
      switch(this->logIndex)
      {
        case 0:
          response = this->obd2.getShortTermFuelTrimBank1();
          break;
        case 1:
          response = this->obd2.getLongTermFuelTrimBank1();
          break;
        case 2:
          response = this->obd2.getShortTermFuelTrimBank2();
          break;
        case 3:
          response = this->obd2.getLongTermFuelTrimBank2();
          break;
        case 4:
          response = this->obd2.getSpeed();
          break;
        case 5:
          response = this->obd2.getAirIntakeTemp();
          break;
        case 6:
          response = this->obd2.getRunTimeSinceEngineStart();
          break;
        case 7:
          response = this->obd2.getDistanceWithMilOn();
          break;
        case 8:
          response = this->obd2.getWarmupsSinceCodesCleared();
          break;
        case 9:
          response = this->obd2.getDistanceSinceCodesCleared();
          break;
        case 10:
          response = this->obd2.getAbsoluteBarametricPressure();
          break;
        case 11:
          response = this->obd2.getAbsoluteLoadValue();
          break;
        case 12:
          response = this->obd2.getTimeRunWithMilOn();
          break;
        case 13:
          response = this->obd2.getTimeSinceTroubleCodesCleared();
          break;
        case 14:
          response = this->obd2.getAbsoluteEvapSystemVaporPressure();
          break;
      }

      // write log
      if (dateTime != "" && response != -999) {
        this->writeLog(dateTime, response);
      } else {
        if (dateTime == "") {
          Serial.println("Unable to get date/time.");
        }
        if (response == -999) {
          Serial.println("Unable to get OBD-II response.");
        }
      }
    }

    // write date/time and mileCount to the log
    void writeLog(String dateTime, int response)
    {
      // give time for openLog to write file while not blocking loop()
      this->logBusy = true; 
      this->logBusyStartTime = millis();
      
      // write log
      String logLine = dateTime + "," + String(response);
      this->openLog.append(this->logFiles[this->logIndex]);
      this->openLog.println(logLine);
      this->openLog.syncFile();
      Serial.println("logged: " + logLine);
    }
};

#endif
