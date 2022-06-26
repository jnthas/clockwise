#pragma once

#include <WiFiManager.h>
#include "ezTime.h"

WiFiManager wifiManager;
Timezone myTZ;

struct WiFiConnect 
{
    std::vector<const char *> _menu = {"wifi","exit"};
    char timezone[40]="0";
    bool shouldSaveConfig = false;

    void saveTimezone(const char* value) {
      Serial.print("Save: ");
      Serial.println(value);
      myTZ.setCache(0);
      myTZ.setLocation(value);
      shouldSaveConfig = false;
    }

    String loadTimezone() {
      myTZ.setCache(0);
      String tz = myTZ.getTimezoneName();
      Serial.print("Load: ");
      Serial.println(tz);
      return tz;
    }
    
    void connect() 
    {
      bool resp; 
      //wifiManager.resetSettings(); 
      wifiManager.setSaveConfigCallback([&](){ shouldSaveConfig = true; });
      
      sprintf(timezone, "%s", loadTimezone());
      WiFiManagerParameter timezoneParam("tz", "Inform your timezone (e.g. America/Lima)", timezone, 36);

      wifiManager.setTitle("Clockwise Wifi Setup");
      wifiManager.setMenu(_menu);
      wifiManager.addParameter(&timezoneParam);
      
      resp = wifiManager.autoConnect("Clockwise-Wifi", "12345678");
      
      if (!resp) {
        Serial.println("Failed to connect");
        delay(3000);
        ESP.restart();
      } else {
        Serial.println("connected!");
        if (shouldSaveConfig) {
          saveTimezone(timezoneParam.getValue());
        }
      }
    }
};
