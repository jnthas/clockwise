#pragma once

#include "ImprovWiFiLibrary.h"
#include <WiFi.h>
#include "CWWebServer.h"
#include "StatusController.h"
#include <WiFiManager.h>

ImprovWiFi improvSerial(&Serial);

struct WiFiController
{
  bool connectionSucessfulOnce;

  static void onImprovWiFiErrorCb(ImprovTypes::Error err)
  {
    ClockwiseWebServer::getInstance()->stopWebServer();
    StatusController::getInstance()->blink_led(2000, 3);
  }

  static void onImprovWiFiConnectedCb(const char *ssid, const char *password)
  {
    ClockwiseParams::getInstance()->load();
    ClockwiseParams::getInstance()->wifiSsid = String(ssid);
    ClockwiseParams::getInstance()->wifiPwd = String(password);
    ClockwiseParams::getInstance()->save();

    ClockwiseWebServer::getInstance()->startWebServer();
  }

  static bool isConnected()
  {
    return improvSerial.isConnected();
  }

  static void handleImprovWiFi()
  {
    improvSerial.handleSerial();
  }

  void saveWiFiCredentials()
  {
    ClockwiseParams::getInstance()->wifiSsid = String(WiFi.SSID());
    ClockwiseParams::getInstance()->wifiPwd = String(WiFi.psk());
    ClockwiseParams::getInstance()->save();
  }

  bool begin()
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    improvSerial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32, "CW-20230402", "1.1.0", "Clockwise");
    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);

    ClockwiseParams::getInstance()->load();

    if (!ClockwiseParams::getInstance()->wifiSsid.isEmpty())
    {
      if (improvSerial.tryConnectToWifi(ClockwiseParams::getInstance()->wifiSsid.c_str(), ClockwiseParams::getInstance()->wifiPwd.c_str()))
      {
        connectionSucessfulOnce = true;
        ClockwiseWebServer::getInstance()->startWebServer();
        return true;
      }
    }

    StatusController::getInstance()->wifiConnectionFailed();

    if (WiFi.status() != WL_CONNECTED)
    {
      WiFiManager wifiManager;
      wifiManager.startConfigPortal("Clockwise");
      if (WiFi.status() == WL_CONNECTED)
      {
        saveWiFiCredentials();
      }
    }
    return false;
  }
};
