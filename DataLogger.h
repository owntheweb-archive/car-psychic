/*
 * DataLogger.h - Log car fuel level for use in model predictions
 * Created by Christopher Stevens @ https://interactive.guru on 10/01/19
 */

 // TODO: What will I do for live TensorFlow Lite model usage? Store an array of values here for model use? Think about that...
 // TODO: Need to clear codes with OBD-II if there are no codes and distance since code clear is maxed/near-maxed at 65,535 - 8,046 km (5,000 miles)
 // TODO: Need to clear codes with oil change (button!) as mechanic doesn't always clear codes after oil change if there were no codes already (confirm this)

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
  const static unsigned long idlePeriod = 600000; // 10 minutes
  const static int logCount = 15; // logging 15 different readings from OBD-II UART
  int logIndex = 0; // only going to log one reading at a time per loop to prevent major lag
  String rtcDateTime; // last RTC date/time for log
  bool logBusy = false; // OpenLog will take about 15ms to write, moving along in the loop while waiting
  const static int logBusyPeriod = 30; // 30ms 
  unsigned long logBusyStartTime;
  // log each data point type separately
  const char logFiles[logCount][40];

  // manage loop based on state of data retreival, skipping loops when waiting instead of using delay() to wait for data for a "faster" app
  const static int DATA_STATE_REQUESTING = 0;
  const static int DATA_STATE_WRITING = 1;
  const static int DATA_STATE_READY = 2;
  int dataState = DATA_STATE_READY;

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
      if (this->dataState == DATA_STATE_READY) {
        // There is no active request or log write happening: wait for next set of log readings to occur
        this->curTime = millis();
        if (this->curTime - this->startTime >= this->idlePeriod) {
          this->dataState = DATA_STATE_REQUESTING;
          this->requestDataPoint();
        }
      } else if (this->dataState == DATA_STATE_REQUESTING) {
        // skip this loop immediately if there's an pending OBD2 request that's busy
        if(this->obd2.isBusy()) {
          return;
        }

        // log requested data
        this->dataState = DATA_STATE_WRITING;
        this->logDataPoint();
      } else if (this->dataState == DATA_STATE_WRITING) {
        // skip this loop immediately if OpenLog is writing data from last request
        if (this->isLogBusy()) {
          return;
        }

        // get ready to request the next data point or wait for a period if all data points were requested/logged
        this->dataState = DATA_STATE_READY;
        this->logIndex += 1;
        if (this->logIndex >= this->logCount) {
          this->logIndex = 0;
          this->startTime = millis();
        }
      }
    }

  // private class methods
  private:
    // make a request to get data, collected later to prevent blocking the loop (takes some time)
    void requestDataPoint()
    {
      switch(this->logIndex)
      {
        case 0:
          this->obd2.makePidRequest(this->obd2.SHORT_TERM_FUEL_TRIM_BANK_1);
          break;
        case 1:
          this->obd2.makePidRequest(this->obd2.LONG_TERM_FUEL_TRIM_BANK_1);
          break;
        case 2:
          this->obd2.makePidRequest(this->obd2.SHORT_TERM_FUEL_TRIM_BANK_2);
          break;
        case 3:
          this->obd2.makePidRequest(this->obd2.LONG_TERM_FUEL_TRIM_BANK_2);
          break;
        case 4:
          this->obd2.makePidRequest(this->obd2.SPEED);
          break;
        case 5:
          this->obd2.makePidRequest(this->obd2.AIR_INTAKE_TEMP);
          break;
        case 6:
          this->obd2.makePidRequest(this->obd2.RUN_TIME_SINCE_ENGINE_START);
          break;
        case 7:
          this->obd2.makePidRequest(this->obd2.DISTANCE_WITH_MIL_ON);
          break;
        case 8:
          this->obd2.makePidRequest(this->obd2.WARMUPS_SINCE_CODES_CLEARED);
          break;
        case 9:
          this->obd2.makePidRequest(this->obd2.DISTANCE_SINCE_CODES_CLEARED);
          break;
        case 10:
          this->obd2.makePidRequest(this->obd2.ABSOLUTE_BARAMETRIC_PRESSURE);
          break;
        case 11:
          this->obd2.makePidRequest(this->obd2.ABSOLUTE_LOAD_VALUE);
          break;
        case 12:
          this->obd2.makePidRequest(this->obd2.TIME_RUN_WITH_MIL_ON);
          break;
        case 13:
          this->obd2.makePidRequest(this->obd2.TIME_SINCE_TROUBLE_CODES_CLEARED);
          break;
        case 14:
          this->obd2.makePidRequest(this->obd2.ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE);
          break;
      }
    }
    
    // log car gas level with date/time
    void logDataPoint()
    {
      // get requested data point that was based on current logIndex
      const int response = this->obd2.getRequestedData();
      
      // get the YYYYMMDDHHMMSS timestamp
      const String dateTime = this->rtcUtils.getDateTime(this->rtc);

      // write log
      if (dateTime != "" && response != -999 && this->obd2.lastRequestSucceeded()) {
        this->writeLog(dateTime, response);
      } else {
        if (dateTime == "") {
          Serial.println("Unable to get date/time.");
        }
        if (!this->obd2.lastRequestSucceeded() || response == -999) {
          Serial.println("Unable to get OBD-II response.");
        }
      }
    }

    // check if OpenLog is busy, updating status as needed
    bool isLogBusy()
    {
      if (this->logBusy == true) {
        unsigned long logBusyCurTime = millis();
        if (logBusyCurTime - this->logBusyStartTime >= this->logBusyPeriod) {
          // TODO: It may help to check status of logger here (if possible), make it less time-only based
          this->logBusy = false;
        }
      }
      return this->logBusy;
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
      this->openLog.syncFile(); // TODO: Check if this line is really needed, seems to work when not used
      Serial.println("logged: " + logLine);
    }
};

#endif
