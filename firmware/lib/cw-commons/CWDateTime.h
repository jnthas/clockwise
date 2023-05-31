#pragma once
#include <Arduino.h>

#include <ezTime.h>
#include <WiFi.h>

class CWDateTime
{
private:
  Timezone myTZ;
  bool use24hFormat = true;
  string ntpServer = "pool.ntp.org"

public:
  void begin(const char *timeZone, bool use24format, string ntpServer);
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

  bool isAM();
  bool is24hFormat();

  void setNtpServer(string ntpServer);
  String getNtpServer();
};
