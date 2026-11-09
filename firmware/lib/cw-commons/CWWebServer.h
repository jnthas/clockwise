#pragma once

#include <WiFi.h>
#include <CWPreferences.h>
#include "StatusController.h"
#include "SettingsWebPage.h"
#include "CWMcp.h"

#ifndef CLOCKFACE_NAME
  #define CLOCKFACE_NAME "UNKNOWN"
#endif

WiFiServer server(80);

struct ClockwiseWebServer
{
  String httpBuffer;
  bool force_restart;
  int contentLength;
  size_t headerEnd;
  const char* HEADER_TEMPLATE_D = "X-%s: %d\r\n";
  const char* HEADER_TEMPLATE_S = "X-%s: %s\r\n";

#ifndef MCP_MAX_BODY
  #define MCP_MAX_BODY 2048
#endif
 
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

  // Reads the whole request: headers up to the blank line, then the body when
  // a Content-Length is present. Returns false on timeout/oversize (response
  // already sent in that case).
  bool readRequest(WiFiClient &client, unsigned long timeoutMs)
  {
    httpBuffer = "";
    const unsigned long timeout = millis() + timeoutMs;
    contentLength = 0;
    headerEnd = 0;

    while (client.connected() && millis() < timeout)
    {
      if (!client.available())
      {
        delay(1);
        continue;
      }

      char c = (char)client.read();
      httpBuffer.concat(c);

      if (httpBuffer.length() > 4096)
      {
        client.println("HTTP/1.1 413 Request Entity Too Large");
        client.println();
        return false;
      }

      // Header terminator may arrive together with the body bytes, so it
      // must be searched for, not matched at the end of the buffer.
      if (headerEnd == 0)
      {
        int idx = httpBuffer.indexOf("\r\n\r\n");
        if (idx >= 0)
        {
          headerEnd = (size_t)idx + 4;
          contentLength = parseContentLength(httpBuffer);
          if (contentLength > MCP_MAX_BODY)
          {
            client.println("HTTP/1.1 413 Request Entity Too Large");
            client.println();
            return false;
          }
        }
      }

      if (headerEnd != 0 &&
          (int)(httpBuffer.length() - headerEnd) >= contentLength)
      {
        return true;
      }
    }
    return false; // timeout
  }

  int parseContentLength(const String &headers)
  {
    int idx = headers.indexOf("Content-Length:");
    if (idx < 0)
      idx = headers.indexOf("content-length:");
    if (idx < 0)
      return 0;
    int start = headers.indexOf(':', idx) + 1;
    int end = headers.indexOf("\r\n", start);
    if (start <= 0 || end < 0)
      return 0;
    return headers.substring(start, end).toInt();
  }

  void handleHttpRequest()
  {
    if (force_restart)
      StatusController::getInstance()->forceRestart();

    WiFiClient client = server.available();
    if (!client)
      return;

    StatusController::getInstance()->blink_led(100, 1);

    // Keep-alive loop for the MCP endpoint: connection establishment towards
    // an ESP32 is the flaky part (SYNs get dropped under load), so the MCP
    // bridge holds one persistent connection and pipelines requests.
    bool keepAlive = true;
    bool firstRead = true;
    while (keepAlive && client.connected())
    {
      keepAlive = false;

      // First read on a fresh connection gets a short timeout; an idle
      // keep-alive connection gets a long one so the MCP client can "think"
      // between tool calls.
      unsigned long readTimeout = firstRead ? 1500UL : 15000UL;
      firstRead = false;
      if (!readRequest(client, readTimeout))
        break;

      int8_t method_pos = httpBuffer.indexOf(' ');
      int8_t path_pos = httpBuffer.indexOf(' ', method_pos + 1);
      if (method_pos < 0 || path_pos < 0)
        break;

      String method = httpBuffer.substring(0, method_pos);
      String path = httpBuffer.substring(method_pos + 1, path_pos);
      String key = "";
      String value = "";

      if (path.indexOf('?') > 0)
      {
        key = path.substring(path.indexOf('?') + 1, path.indexOf('='));
        value = path.substring(path.indexOf('=') + 1);
        path = path.substring(0, path.indexOf('?'));
      }

      String body = (contentLength > 0)
                        ? httpBuffer.substring(headerEnd, headerEnd + contentLength)
                        : "";

      // Keep-alive is honoured on the MCP endpoint only.
      String lowered = httpBuffer;
      lowered.toLowerCase();
      bool wantsKeepAlive =
          path == "/mcp" && lowered.indexOf("connection: keep-alive") >= 0;

      processRequest(client, method, path, key, value, body, wantsKeepAlive);
      httpBuffer = "";

      if (wantsKeepAlive)
      {
        keepAlive = true;
        continue;
      }

      // Let the client close first: the ESP side then performs a passive
      // close and avoids TIME_WAIT, whose ~60 s PCB occupation would
      // otherwise exhaust lwIP's small pool of TCP slots after a burst of
      // requests and stall the server (new SYNs stop being accepted).
      unsigned long closeDeadline = millis() + 300;
      while (client.connected() && millis() < closeDeadline)
        delay(1);
    }

    client.stop();
  }

  void processRequest(WiFiClient client, String method, String path, String key, String value, String body, bool keepAlive)
  {
    if (method == "GET" && path == "/") {
      client.println("HTTP/1.0 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println(SETTINGS_PAGE);
    } else if (method == "GET" && path == "/get") {
      getCurrentSettings(client);
    } else if (method == "GET" && path == "/read") {
      if (key == "pin") {
        readPin(client, key, value.toInt());
      }
    } else if (method == "POST" && path == "/mcp") {
      mcpHandleHttpRequest(client, body, keepAlive);
    } else if (method == "GET" && path == "/mcp") {
      // No server-initiated messages: the SSE stream is intentionally absent.
      client.println("HTTP/1.1 405 Method Not Allowed");
      client.println("Allow: POST");
      client.println();
    } else if (method == "POST" && path == "/restart") {
      client.println("HTTP/1.0 204 No Content");
      force_restart = true;
    } else if (method == "POST" && path == "/set") {
      ClockwiseParams::getInstance()->load();
      //a baby seal has died due this ifs
      if (key == ClockwiseParams::getInstance()->PREF_DISPLAY_BRIGHT) {
        ClockwiseParams::getInstance()->displayBright = value.toInt();
      } else if (key == ClockwiseParams::getInstance()->PREF_WIFI_SSID) {
        ClockwiseParams::getInstance()->wifiSsid = value;
      } else if (key == ClockwiseParams::getInstance()->PREF_WIFI_PASSWORD) {
        ClockwiseParams::getInstance()->wifiPwd = value;
      } else if (key == "autoBright") {   //autoBright=0010,0800
        ClockwiseParams::getInstance()->autoBrightMin = value.substring(0,4).toInt();
        ClockwiseParams::getInstance()->autoBrightMax = value.substring(5,9).toInt();
      } else if (key == ClockwiseParams::getInstance()->PREF_SWAP_BLUE_GREEN) {
        ClockwiseParams::getInstance()->swapBlueGreen = (value == "1");
      } else if (key == ClockwiseParams::getInstance()->PREF_SWAP_BLUE_RED) {
        ClockwiseParams::getInstance()->swapBlueRed = (value == "1");
      } else if (key == ClockwiseParams::getInstance()->PREF_USE_24H_FORMAT) {
        ClockwiseParams::getInstance()->use24hFormat = (value == "1");
      } else if (key == ClockwiseParams::getInstance()->PREF_LDR_PIN) {
        ClockwiseParams::getInstance()->ldrPin = value.toInt();
      } else if (key == ClockwiseParams::getInstance()->PREF_TIME_ZONE) {
        ClockwiseParams::getInstance()->timeZone = value;
      } else if (key == ClockwiseParams::getInstance()->PREF_NTP_SERVER) {
        ClockwiseParams::getInstance()->ntpServer = value;
      } else if (key == ClockwiseParams::getInstance()->PREF_CANVAS_FILE) {
        ClockwiseParams::getInstance()->canvasFile = value;
      } else if (key == ClockwiseParams::getInstance()->PREF_CANVAS_SERVER) {
        ClockwiseParams::getInstance()->canvasServer = value;
      } else if (key == ClockwiseParams::getInstance()->PREF_MANUAL_POSIX) {
        ClockwiseParams::getInstance()->manualPosix = value;
      } else if (key == ClockwiseParams::getInstance()->PREF_DISPLAY_ROTATION) {
        ClockwiseParams::getInstance()->displayRotation = value.toInt();
      } else if (key == ClockwiseParams::getInstance()->PREF_DRIVER) {
        ClockwiseParams::getInstance()->driver = value.toInt();
      }  else if (key == ClockwiseParams::getInstance()->PREF_I2CSPEED) {
        ClockwiseParams::getInstance()->i2cSpeed = value.toInt();
      }  else if (key == ClockwiseParams::getInstance()->PREF_E_PIN) {
        ClockwiseParams::getInstance()->E_pin = value.toInt();
      }
      ClockwiseParams::getInstance()->save();
      client.println("HTTP/1.0 204 No Content");
    }
  }



  void readPin(WiFiClient client, String key, uint16_t pin) {
    ClockwiseParams::getInstance()->load();

    client.println("HTTP/1.0 204 No Content");
    client.printf(HEADER_TEMPLATE_D, key, analogRead(pin));
    
    client.println();
  }


  void getCurrentSettings(WiFiClient client) {
    ClockwiseParams::getInstance()->load();

    client.println("HTTP/1.0 204 No Content");

    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_DISPLAY_BRIGHT, ClockwiseParams::getInstance()->displayBright);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_DISPLAY_ABC_MIN, ClockwiseParams::getInstance()->autoBrightMin);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_DISPLAY_ABC_MAX, ClockwiseParams::getInstance()->autoBrightMax);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_SWAP_BLUE_GREEN, ClockwiseParams::getInstance()->swapBlueGreen);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_SWAP_BLUE_RED, ClockwiseParams::getInstance()->swapBlueRed);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_USE_24H_FORMAT, ClockwiseParams::getInstance()->use24hFormat);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_LDR_PIN, ClockwiseParams::getInstance()->ldrPin);    
    client.printf(HEADER_TEMPLATE_S, ClockwiseParams::getInstance()->PREF_TIME_ZONE, ClockwiseParams::getInstance()->timeZone.c_str());
    client.printf(HEADER_TEMPLATE_S, ClockwiseParams::getInstance()->PREF_WIFI_SSID, ClockwiseParams::getInstance()->wifiSsid.c_str());
    client.printf(HEADER_TEMPLATE_S, ClockwiseParams::getInstance()->PREF_NTP_SERVER, ClockwiseParams::getInstance()->ntpServer.c_str());
    client.printf(HEADER_TEMPLATE_S, ClockwiseParams::getInstance()->PREF_CANVAS_FILE, ClockwiseParams::getInstance()->canvasFile.c_str());
    client.printf(HEADER_TEMPLATE_S, ClockwiseParams::getInstance()->PREF_CANVAS_SERVER, ClockwiseParams::getInstance()->canvasServer.c_str());
    client.printf(HEADER_TEMPLATE_S, ClockwiseParams::getInstance()->PREF_MANUAL_POSIX, ClockwiseParams::getInstance()->manualPosix.c_str());
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_DISPLAY_ROTATION, ClockwiseParams::getInstance()->displayRotation);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_DRIVER, ClockwiseParams::getInstance()->driver);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_I2CSPEED, ClockwiseParams::getInstance()->i2cSpeed);
    client.printf(HEADER_TEMPLATE_D, ClockwiseParams::getInstance()->PREF_E_PIN, ClockwiseParams::getInstance()->E_pin);

    client.printf(HEADER_TEMPLATE_S, "CW_FW_VERSION", CW_FW_VERSION);
    client.printf(HEADER_TEMPLATE_S, "CW_FW_NAME", CW_FW_NAME);
    client.printf(HEADER_TEMPLATE_S, "CLOCKFACE_NAME", CLOCKFACE_NAME);
    client.println();
  }
  
};
