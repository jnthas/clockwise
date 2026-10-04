#include "CWDateTime.h"
#include "CWPreferences.h"

void CWDateTime::begin(const char *timeZone, bool use24format, const char *ntpServer = NTP_SERVER, const char *posixTZ = "")
{
  Serial.printf("[Time] NTP Server: %s, Timezone: %s\n", ntpServer, timeZone);
  ezt::setServer(String(ntpServer));

  if (strlen(posixTZ) > 1) {
    // An empty value still contains a null character so not empty is a value greater than 1.
    // Set to defined Posix TZ
    Serial.printf("[Time] Using manual user POSIX TZ: %s\n", posixTZ);
    myTZ.setPosix(posixTZ);
  } else {
    ClockwiseParams *params = ClockwiseParams::getInstance();
    bool hasValidCache = (params->cachedTz == timeZone && params->cachedPosix.length() > 0);

    // 1. Immediately apply cached POSIX string if available (Zero-latency boot)
    if (hasValidCache) {
      Serial.printf("[Time] Applying cached POSIX TZ for %s: %s\n", timeZone, params->cachedPosix.c_str());
      myTZ.setPosix(params->cachedPosix.c_str());
    }

    // 2. Attempt remote lookup with retries
    bool success = false;
    for (int attempt = 1; attempt <= 3; attempt++) {
      if (myTZ.setLocation(timeZone)) {
        success = true;
        break;
      }
      Serial.printf("[Time] Timezone lookup attempt %d failed, retrying...\n", attempt);
      delay(500);
    }

    // 3. If remote lookup succeeded, save/update cache in NVS
    if (success) {
      String resolved = myTZ.getPosix();
      Serial.printf("[Time] Resolved POSIX: %s -> %s\n", timeZone, resolved.c_str());
      if (resolved.length() > 0 && (params->cachedTz != timeZone || params->cachedPosix != resolved)) {
        params->cachedTz = String(timeZone);
        params->cachedPosix = resolved;
        params->save();
        Serial.println("[Time] Saved resolved POSIX to NVS cache.");
      }
    } else {
      if (hasValidCache) {
        myTZ.setPosix(params->cachedPosix.c_str());
        Serial.println("[Time] Remote lookup failed, but cached POSIX is active. Local time is preserved!");
      } else {
        Serial.println("[Time] WARNING: Timezone lookup failed and no cache available! Running in UTC.");
      }
    }
  }

  this->use24hFormat = use24format;
  ezt::updateNTP();
  waitForSync(10);
}

String CWDateTime::getFormattedTime()
{
  return myTZ.dateTime();
}

String CWDateTime::getFormattedTime(const char *format)
{
  return myTZ.dateTime(format);
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