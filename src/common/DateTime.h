#ifndef DateTimeCf_h
#define DateTimeCf_h

#include <Arduino.h>

#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFi.h>

class DateTime
{
  private:    
    WiFiUDP udp;
    NTPClient* ntp;

  public:
    DateTime();
    void begin();
    void update();
    String getFormattedTime();

    char* getHour(const char *format);
    char* getMinute(const char *format);
    char* getSecond(const char *format);
    const char* getWeekdayName();
    int getHour();
    int getMinute();
    int getSecond();
    int getWeekday();
    int getDay();
    int getMonth();

};
#endif