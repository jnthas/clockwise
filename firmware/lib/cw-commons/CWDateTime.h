#pragma once
#include <Arduino.h>

#include <ezTime.h>
#include <WiFi.h>

class CWDateTime
{
private:
  Timezone myTZ;
  bool use24hFormat = true;

public:
  void begin(const char *timeZone, bool use24format);
  String getFormattedTime();

  char *getHour(const char *format);
  char *getMinute(const char *format);
  int getHour();
  int getMinute();
  int getSecond();
  long getMilliseconds();

  int getDay();
  int getMonth();
  int getWeekday();


};
