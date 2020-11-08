#ifndef DateTimeCf_h
#define DateTimeCf_h

#include <Arduino.h>

#include <NTPClient.h>
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
    char* getSeconds(const char *format);
    int getHour();
    int getMinute();
    int getSecond();

};
#endif