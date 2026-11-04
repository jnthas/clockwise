#pragma once
#include <Arduino.h>

#include <ezTime.h>
#include <WiFi.h>

class CWDateTime
{
private:
  Timezone myTZ;
  bool use24hFormat = true;
  String retryTimeZone;
  uint32_t nextTimezoneLookupMillis = 0;
  bool timezoneLookupPending = false;

public:
  void begin(const char *timeZone, bool use24format, const char *ntpServer, const char *posixTZ);
  void update();
  String getFormattedTime();
  String getFormattedTime(const char* format);

  char *getHour(const char *format);
  char *getMinute(const char *format);
  int getHour();
  int getMinute();
  int getSecond();
  long getMilliseconds();

  int getDay();
  int getMonth();
  int getWeekday();

  bool isAM();
  bool is24hFormat();
};
