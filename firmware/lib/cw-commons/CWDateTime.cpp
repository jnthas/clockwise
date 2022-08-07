#include "CWDateTime.h"

void CWDateTime::begin()
{
  myTZ.setCache(0);
  waitForSync();
}

void CWDateTime::setTimezone(const char *timeZone)
{
  myTZ.setCache(0);
  myTZ.setLocation(timeZone);
  waitForSync();
}

String CWDateTime::getTimezone()
{
  return myTZ.getTimezoneName(0);
}


void CWDateTime::update()
{
}

String CWDateTime::getFormattedTime()
{
  return myTZ.dateTime();
}

char *CWDateTime::getHour(const char *format)
{
  static char buffer[3] = {'\0'};
  strncpy(buffer, myTZ.dateTime("H").c_str(), sizeof(buffer));
  return buffer;
}

char *CWDateTime::getMinute(const char *format)
{
  static char buffer[3] = {'\0'};
  strncpy(buffer, myTZ.dateTime("i").c_str(), sizeof(buffer));
  return buffer;
}

int CWDateTime::getHour()
{
  return myTZ.dateTime("H").toInt();
}

int CWDateTime::getMinute()
{
  return myTZ.dateTime("i").toInt();
}

int CWDateTime::getSecond()
{
  return myTZ.dateTime("s").toInt();
}

int CWDateTime::getDay() 
{
  return myTZ.dateTime("d").toInt();
}
int CWDateTime::getMonth()
{
  return myTZ.dateTime("m").toInt();
}
int CWDateTime::getWeekday() 
{
  return myTZ.dateTime("w").toInt()-1;
}

long CWDateTime::getMilliseconds() 
{
  return myTZ.ms(TIME_NOW);
}


