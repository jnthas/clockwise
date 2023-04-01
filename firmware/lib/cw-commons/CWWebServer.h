#pragma once

#define HTTP_GET_SIZE 3
#define HTTP_POST_SIZE 4

#include <WiFi.h>
#include <CWPreferences.h>
#include "StatusController.h"
#include "SettingsWebPage.h"

WiFiServer server(80);

struct ClockwiseWebServer
{
  String httpBuffer;
 
  static ClockwiseWebServer *getInstance()
  {
    static ClockwiseWebServer base;
    return &base;
  }

  void startWebServer()
  {
    server.begin();
    StatusController::getInstance()->blink_led(100, 3);
  }

  void stopWebServer()
  {
    server.stop();
  }

  void handleHttpRequest()
  {
    WiFiClient client = server.available();
    if (client)
    {
      StatusController::getInstance()->blink_led(100, 1);

      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();
          httpBuffer.concat(c);

          if (c == '\n')
          {
            //Serial.println(httpBuffer);

            uint8_t method_pos = httpBuffer.indexOf(' ');
            uint8_t path_pos = httpBuffer.indexOf(' ', method_pos + 1);

            String method = httpBuffer.substring(0, method_pos);
            String path = httpBuffer.substring(method_pos + 1, path_pos);
            String key = "";
            String value = "";

            if (method == "POST")
            {
              key = path.substring(path.indexOf('?') + 1, path.indexOf('='));
              value = path.substring(path.indexOf('=') + 1, ' ');
              path = path.substring(0, path.indexOf('?'));
            }

            processRequest(client, method, path, key, value);
            httpBuffer = "";
            break;
          }
        }
      }
      delay(1);
      client.stop();
    }
  }

  void processRequest(WiFiClient client, String method, String path, String key, String value)
  {

    // Serial.println(method);
    // Serial.println(path);
    // Serial.println(key);
    // Serial.println(value);

    if (method == "GET" && path == "/") {
      client.println("HTTP/1.0 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println(SETTINGS_PAGE);
    } else if (method == "GET" && path == "/get") {
      getCurrentSettings(client);
    } else if (method == "POST" && path == "/set") {
      ClockwiseParams::getInstance()->load();

      //a baby seal has died due this ifs
      if (key == ClockwiseParams::getInstance()->PREF_DISPLAY_BRIGHT) {
        ClockwiseParams::getInstance()->displayBright = value.toInt();
      } else if (key == ClockwiseParams::getInstance()->PREF_SWAP_BLUE_GREEN) {
        ClockwiseParams::getInstance()->swapBlueGreen = (value == "1");
      } else if (key == ClockwiseParams::getInstance()->PREF_USE_24H_FORMAT) {
        ClockwiseParams::getInstance()->use24hFormat = (value == "1");
      } else if (key == ClockwiseParams::getInstance()->PREF_TIME_ZONE) {
        ClockwiseParams::getInstance()->timeZone = value;
      }
      ClockwiseParams::getInstance()->save();
      client.println("HTTP/1.0 204 No Content");
    }
  }


  void getCurrentSettings(WiFiClient client) {
    ClockwiseParams::getInstance()->load();

    char response[256];
    snprintf(response, sizeof(response), "{\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":\"%s\",\"%s\":\"%s\"}", \
      ClockwiseParams::getInstance()->PREF_DISPLAY_BRIGHT,
      ClockwiseParams::getInstance()->displayBright,
      ClockwiseParams::getInstance()->PREF_SWAP_BLUE_GREEN,
      ClockwiseParams::getInstance()->swapBlueGreen,
      ClockwiseParams::getInstance()->PREF_USE_24H_FORMAT,
      ClockwiseParams::getInstance()->use24hFormat,
      ClockwiseParams::getInstance()->PREF_TIME_ZONE,
      ClockwiseParams::getInstance()->timeZone.c_str(),
      ClockwiseParams::getInstance()->PREF_WIFI_SSID,
      ClockwiseParams::getInstance()->wifiSsid.c_str());
      
      client.println("HTTP/1.0 200 OK");
      client.println("Content-Type: application/json");
      client.println();
      client.println(response);
  }

};
