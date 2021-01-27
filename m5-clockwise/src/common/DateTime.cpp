
#include "DateTime.h"

const char* weekDayWords[] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};


DateTime::DateTime() {
  this->ntp = new NTPClient(udp, "a.st1.ntp.br", -3 * 3600, 3600000);
  setTime(ntp->getEpochTime());
}

void DateTime::begin()
{
  ntp->begin();
  ntp->forceUpdate();
}

void DateTime::update()
{
  ntp->update();
}

String DateTime::getFormattedTime()
{
  return ntp->getFormattedTime();
}

char* DateTime::getHour(const char *format)
{
  static char buffer[3] = {'\0'};
  snprintf(buffer, sizeof(buffer), format, ntp->getHours());
  return buffer;
}

char* DateTime::getMinute(const char *format)
{
  static char buffer[3] = {'\0'};
  snprintf(buffer, sizeof(buffer), format, ntp->getMinutes());
  return buffer;
}

char* DateTime::getSecond(const char *format)
{
  static char buffer[3] = {'\0'};
  snprintf(buffer, sizeof(buffer), format, ntp->getSeconds());
  return buffer;
}

const char* DateTime::getWeekdayName() {
  return weekDayWords[weekday(ntp->getEpochTime())-1];
}

int DateTime::getHour() {
  return ntp->getHours();
}

int DateTime::getMinute() {
  return ntp->getMinutes();
}

int DateTime::getSecond() {
  return ntp->getSeconds();
}

int DateTime::getWeekday() {
  return weekday(ntp->getEpochTime())-1;
}

int DateTime::getDay() {  
  return day(ntp->getEpochTime());
}

int DateTime::getMonth() {
  return month(ntp->getEpochTime());
}