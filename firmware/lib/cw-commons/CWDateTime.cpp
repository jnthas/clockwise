#include "CWDateTime.h"

void CWDateTime::begin(const char *timeZone, bool use24format, string ntpServer)
{
  myTZ.setLocation(timeZone);
  myTZ.setServer(ntpServer);
  this->use24hFormat = use24format;
  this->ntpServer = ntpServer;
  waitForSync();
}

String CWDateTime::getFormattedTime()
{
  return myTZ.dateTime();
}

char *CWDateTime::getHour(const char *format)
{
  static char buffer[3] = {'\0'};
  strncpy(buffer, myTZ.dateTime((use24hFormat ? "H" : "h")).c_str(), sizeof(buffer));
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
  return myTZ.dateTime((use24hFormat ? "H" : "h")).toInt();
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

bool CWDateTime::isAM() 
{
  return myTZ.isAM();
}

bool CWDateTime::is24hFormat() 
{
  return this->use24hFormat;
}

void CWDateTime::setNtpServer(string ntpServer)
{
  this->ntpServer = ntpServer;
  myTZ.setServer(ntpServer);
  myTZ.updateNTP();
  waitForSync();
}

String CWDateTime::getNtpServer()
{
  return this->ntpServer;
}
