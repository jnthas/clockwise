#pragma once

#include "ImprovWiFiLibrary.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include "CWWebServer.h"
#include "StatusController.h"
#include <WiFiManager.h>
#include "WiFiSetupStateMachine.h"

ImprovWiFi improvSerial(&Serial);

/**
 * Non-blocking Wi-Fi provisioning.
 *
 * begin() returns immediately and update() must be called on every loop().
 * Improv Serial is serviced on every pass in every state, so ESP-Web-Tools can
 * always talk to the device (including while the WiFiManager AP portal runs).
 * Timing decisions live in cw::WiFiSetupStateMachine (unit tested natively);
 * this struct only performs the side effects.
 */
struct WiFiController
{
  long elapsedTimeOffline = 0;
  bool connectionSucessfulOnce = false;

  // Upper bound of bytes parsed per loop() pass to keep rendering responsive.
  static const uint16_t IMPROV_MAX_BYTES_PER_PASS = 256;

  cw::WiFiSetupStateMachine setupFsm;

  static WiFiManager &wifiManager()
  {
    // Must outlive begin() because the portal now runs in non-blocking mode.
    static WiFiManager wm;
    return wm;
  }

  static bool &networkReady()
  {
    static bool ready = false;
    return ready;
  }

  static void persistCredentials(const char *ssid, const char *password)
  {
    ClockwiseParams *params = ClockwiseParams::getInstance();
    params->load();
    if (params->wifiSsid == ssid && params->wifiPwd == password)
      return; // avoid needless NVS writes
    params->wifiSsid = String(ssid);
    params->wifiPwd = String(password);
    params->save();
  }

  static void onImprovWiFiErrorCb(ImprovTypes::Error err)
  {
    ClockwiseWebServer::getInstance()->stopWebServer();
    StatusController::getInstance()->blink_led(2000, 3);
  }

  // Runs inside improvSerial.handleSerial() after a successful WIFI_SETTINGS.
  // First-time setup (web server, mDNS, clockface) is driven by update() once
  // the state machine reports the connection, so it never races the WiFiManager
  // portal for port 80. When re-provisioning an already running clock, the web
  // server is restarted here as before.
  static void onImprovWiFiConnectedCb(const char *ssid, const char *password)
  {
    persistCredentials(ssid, password);

    if (networkReady() && !wifiManager().getConfigPortalActive())
      ClockwiseWebServer::getInstance()->startWebServer();
  }

  bool isConnected()
  {
    if (improvSerial.isConnected()) {
      elapsedTimeOffline = 0;
      return true;
    } else {
      // While provisioning, the state machine owns the timeouts (portal -> restart).
      if (setupFsm.isSettingUp())
        return false;

      if (elapsedTimeOffline == 0 && !connectionSucessfulOnce)
        elapsedTimeOffline = millis();
      
      if ((millis() - elapsedTimeOffline) > 1000 * 60 * 5)  // restart if clockface is not showed and is 5min offline 
        StatusController::getInstance()->forceRestart();

      return false;
    }
  }

  // Returns true if any byte was received (used to detect an Improv session).
  static bool handleImprovWiFi()
  {
    bool activity = false;
    for (uint16_t i = 0; i < IMPROV_MAX_BYTES_PER_PASS && Serial.available() > 0; i++)
    {
      improvSerial.handleSerial();
      activity = true;
    }
    return activity;
  }

  void startApPortal()
  {
    StatusController::getInstance()->wifiConnectionFailed("Setup WiFi via AP");

    WiFiManager &wm = wifiManager();
    wm.setConfigPortalBlocking(false);
    wm.setConfigPortalTimeout(0); // timeout is owned by WiFiSetupStateMachine
    wm.startConfigPortal("Clockwise-Wifi");

    Serial.println("[WiFi] AP 'Clockwise-Wifi' started; Improv Serial still available");
  }

  void onConnected()
  {
    // Persist first: covers the portal path (Improv already saved its own).
    persistCredentials(WiFi.SSID().c_str(), WiFi.psk().c_str());

    WiFiManager &wm = wifiManager();
    if (wm.getConfigPortalActive())
      wm.stopConfigPortal(); // frees the AP and port 80

    connectionSucessfulOnce = true;
    networkReady() = true;

    ClockwiseWebServer::getInstance()->startWebServer();
    if (MDNS.begin("clockwise"))
    {
      MDNS.addService("http", "tcp", 80);
    }

    Serial.printf("[WiFi] Connected to %s, IP address %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  }

  // Non-blocking: starts the provisioning flow and returns immediately.
  void begin()
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    improvSerial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32, CW_FW_NAME, CW_FW_VERSION, "Clockwise");
    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);

    ClockwiseParams *params = ClockwiseParams::getInstance();
    params->load();

    if (setupFsm.begin(!params->wifiSsid.isEmpty(), millis()) == cw::WiFiSetupAction::StartStationConnect)
    {
      Serial.printf("[WiFi] Connecting to %s\n", params->wifiSsid.c_str());
      WiFi.begin(params->wifiSsid.c_str(), params->wifiPwd.c_str());
    }
    else
    {
      Serial.println("[WiFi] No stored credentials, waiting for Improv Serial");
    }
  }

  // Call on every loop(). Returns true exactly once, when the network is ready.
  bool update()
  {
    const bool improvActivity = handleImprovWiFi();

    WiFiManager &wm = wifiManager();
    if (wm.getConfigPortalActive())
      wm.process();

    switch (setupFsm.update(millis(), improvSerial.isConnected(), improvActivity))
    {
    case cw::WiFiSetupAction::EnterImprovWait:
      Serial.println("[WiFi] Stored credentials failed, waiting for Improv Serial");
      WiFi.disconnect(); // stop retrying stale credentials in the background
      break;

    case cw::WiFiSetupAction::StartApPortal:
      startApPortal();
      break;

    case cw::WiFiSetupAction::Connected:
      onConnected();
      return true;

    case cw::WiFiSetupAction::Restart:
      StatusController::getInstance()->wifiConnectionFailed("WiFi Failed");
      StatusController::getInstance()->forceRestart();
      break;

    default:
      break;
    }
    return false;
  }
};
