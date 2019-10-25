/*
 * RtcUtils.h - Get formatted Real Time Clock data from SparkFun RTC
 * Created by Christopher Stevens @ https://interactive.guru on 10/05/19
 */

 #ifndef RtcUtils_h
 #define RtcUItils_h

 #include <Arduino.h>
 #include <SparkFun_RV1805.h>

 class RtcUtils {
  // public class methods
  public:
    // constructor
    RtcUtils()
    {
    }

    // get a YYYYMMDDHHMMSS string of current RTC date/time
    // TODO: This assumes rtc.set24Hour(); was set first, should I check every time here and toggle? Hmm...
    char* getDateTime(RV1805 &rtc)
    {
      if (rtc.updateTime() == true)
      {
        static char date[15]; //Max of yyyymmddhhmmss with \0 terminator
        sprintf(date, "20%02d%02d%02d%02d%02d%02d", rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
        return(date);
      }
      // else unable to connect/update rtc date
      return (char*) 0;
    }
};

#endif
