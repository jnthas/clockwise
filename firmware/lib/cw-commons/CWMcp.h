#pragma once

// Clockwise MCP server — firmware adapter.
//
// Bridges the platform-independent MCP core (McpJsonRpc.h) to Clockwise
// hardware: NVS parameters (ClockwiseParams), Wi-Fi status, LDR sensor and
// restart. main.cpp registers two runtime hooks:
//   - time provider     -> CWDateTime::getFormattedTime()
//   - brightness applier -> dma_display->setBrightness8()
//
// HTTP plumbing: POST /mcp is dispatched here from ClockwiseWebServer.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <esp_system.h>
#include "CWPreferences.h"
#include "StatusController.h"
#include "McpJsonRpc.h"
#include "McpConfig.h"

#ifndef CW_FW_VERSION
  #define CW_FW_VERSION "0.0.0"
#endif
#ifndef CLOCKFACE_NAME
  #define CLOCKFACE_NAME "UNKNOWN"
#endif

// Runtime hooks registered by main.cpp (kept as plain function pointers so
// this header stays free of display/date-time dependencies).
typedef String (*McpTimeProvider)();
typedef void (*McpBrightnessHandler)(uint8_t level);
typedef void (*McpAutoBrightnessHandler)();

// Runtime hooks registered by main.cpp (kept as plain function pointers so
// this header stays free of display/date-time dependencies). Slots live in
// inline functions so all translation units share the same instance.
inline McpTimeProvider& mcpTimeProviderSlot() { static McpTimeProvider fn = nullptr; return fn; }
inline McpBrightnessHandler& mcpBrightnessHandlerSlot() { static McpBrightnessHandler fn = nullptr; return fn; }
inline void mcpSetTimeProvider(McpTimeProvider fn) { mcpTimeProviderSlot() = fn; }
inline void mcpSetBrightnessHandler(McpBrightnessHandler fn) { mcpBrightnessHandlerSlot() = fn; }
inline McpAutoBrightnessHandler& mcpAutoBrightnessHandlerSlot() { static McpAutoBrightnessHandler fn = nullptr; return fn; }
inline void mcpSetAutoBrightnessHandler(McpAutoBrightnessHandler fn) { mcpAutoBrightnessHandlerSlot() = fn; }

// Consumed by the HTTP server only after writing the tool response.
inline bool& mcpRestartRequestedSlot() { static bool requested = false; return requested; }
inline bool mcpTakeRestartRequest()
{
  bool requested = mcpRestartRequestedSlot();
  mcpRestartRequestedSlot() = false;
  return requested;
}

// ----- device adapter --------------------------------------------------------

class ClockwiseMcpDevice : public cw::mcp::McpDevice
{
public:
  ClockwiseMcpDevice() : config_(*ClockwiseParams::getInstance()) {}

  std::vector<std::string> paramNames() override
  {
    ClockwiseParams* p = ClockwiseParams::getInstance();
    return {
        p->PREF_DISPLAY_BRIGHT,
        p->PREF_DISPLAY_ABC_MIN,
        p->PREF_DISPLAY_ABC_MAX,
        p->PREF_LDR_PIN,
        p->PREF_TIME_ZONE,
        p->PREF_NTP_SERVER,
        p->PREF_USE_24H_FORMAT,
        p->PREF_SWAP_BLUE_GREEN,
        p->PREF_SWAP_BLUE_RED,
        p->PREF_DISPLAY_ROTATION,
        p->PREF_DRIVER,
        p->PREF_I2CSPEED,
        p->PREF_E_PIN,
        p->PREF_WIFI_SSID,
        p->PREF_WIFI_PASSWORD,
        p->PREF_CANVAS_FILE,
        p->PREF_CANVAS_SERVER,
        p->PREF_MANUAL_POSIX,
    };
  }

  bool getParam(const std::string& name, std::string& value) override
  {
    ClockwiseParams* p = ClockwiseParams::getInstance();
    p->load();

    if (name == p->PREF_DISPLAY_BRIGHT) value = std::to_string(p->displayBright);
    else if (name == p->PREF_DISPLAY_ABC_MIN) value = std::to_string(p->autoBrightMin);
    else if (name == p->PREF_DISPLAY_ABC_MAX) value = std::to_string(p->autoBrightMax);
    else if (name == p->PREF_LDR_PIN) value = std::to_string(p->ldrPin);
    else if (name == p->PREF_TIME_ZONE) value = p->timeZone.c_str();
    else if (name == p->PREF_NTP_SERVER) value = p->ntpServer.c_str();
    else if (name == p->PREF_USE_24H_FORMAT) value = p->use24hFormat ? "1" : "0";
    else if (name == p->PREF_SWAP_BLUE_GREEN) value = p->swapBlueGreen ? "1" : "0";
    else if (name == p->PREF_SWAP_BLUE_RED) value = p->swapBlueRed ? "1" : "0";
    else if (name == p->PREF_DISPLAY_ROTATION) value = std::to_string(p->displayRotation);
    else if (name == p->PREF_DRIVER) value = std::to_string(p->driver);
    else if (name == p->PREF_I2CSPEED) value = std::to_string((unsigned long)p->i2cSpeed);
    else if (name == p->PREF_E_PIN) value = std::to_string(p->E_pin);
    else if (name == p->PREF_WIFI_SSID) value = p->wifiSsid.c_str();
    else if (name == p->PREF_WIFI_PASSWORD) value = p->wifiPwd.length() > 0 ? "********" : "";
    else if (name == p->PREF_CANVAS_FILE) value = p->canvasFile.c_str();
    else if (name == p->PREF_CANVAS_SERVER) value = p->canvasServer.c_str();
    else if (name == p->PREF_MANUAL_POSIX) value = p->manualPosix.c_str();
    else return false;

    return true;
  }

  void beginParams() override { config_.begin(); }
  void cancelParams() override { config_.cancel(); }

  bool setParam(const std::string& name, const std::string& value) override
  {
    return config_.setParam(name, value);
  }

  bool saveParams() override
  {
    if (!config_.save())
      return false;
    if (mcpAutoBrightnessHandlerSlot())
      mcpAutoBrightnessHandlerSlot()();
    return true;
  }

  void applyBrightness(int level) override
  {
    if (mcpBrightnessHandlerSlot())
      mcpBrightnessHandlerSlot()((uint8_t)level);
  }

  int readLightSensor() override
  {
    return analogRead(ClockwiseParams::getInstance()->ldrPin);
  }

  void requestRestart() override
  {
    mcpRestartRequestedSlot() = true;
  }

  std::string statusJson() override
  {
    ClockwiseParams* p = ClockwiseParams::getInstance();
    p->load();

    DynamicJsonDocument doc(1024);
    doc["firmware"] = CW_FW_VERSION;
    doc["clockface"] = CLOCKFACE_NAME;
    doc["protocol"] = cw::mcp::kProtocolVersion;

    bool connected = (WiFi.status() == WL_CONNECTED);
    doc["wifiConnected"] = connected;
    if (connected)
    {
      doc["ip"] = WiFi.localIP().toString();
      doc["rssi"] = WiFi.RSSI();
      doc["ssid"] = WiFi.SSID();
    }
    doc["freeHeap"] = (long)ESP.getFreeHeap();
    doc["uptimeMs"] = (long)(millis() / 1000);
    doc["timezone"] = p->timeZone;
    doc["ntpServer"] = p->ntpServer;
    doc["localTime"] = currentTime();
    doc["lightSensor"] = readLightSensor();

    std::string out;
    serializeJson(doc, out);
    return out;
  }

  std::string currentTime() override
  {
    if (!mcpTimeProviderSlot())
      return "";
    String t = mcpTimeProviderSlot()();
    return std::string(t.c_str());
  }

  std::string updateFirmware(const std::string& url) override
  {
    HTTPClient http;
    WiFiClient plain;
    WiFiClientSecure secure;
    bool isHttps = url.find("https://") == 0;

    // No CA store on device: https images are accepted as-is. Images should
    // come from a trusted LAN host or release pipeline anyway.
    if (isHttps)
    {
      secure.setInsecure();
      http.begin(secure, url.c_str());
    }
    else
    {
      http.begin(plain, url.c_str());
    }
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(30000);

    int code = http.GET();
    if (code != 200)
    {
      http.end();
      return std::string("{\"error\":\"HTTP GET failed with code ") +
             std::to_string(code) + "\"}";
    }

    int total = http.getSize();
    if (total <= 0)
    {
      http.end();
      return "{\"error\":\"Missing Content-Length\"}";
    }

    Update.onProgress([](unsigned int progress, unsigned int size) {
      Serial.printf("[OTA] Flash progress: %u%%\r", (progress * 100) / size);
    });

    if (!Update.begin(total))
    {
      http.end();
      return "{\"error\":\"Not enough space for the update\"}";
    }

    WiFiClient* data = http.getStreamPtr();
    size_t written = Update.writeStream(*data);
    http.end();

    if (written != (size_t)total || !Update.end() || !Update.isFinished())
    {
      Update.abort();
      return std::string("{\"error\":\"Flash write failed (") +
             std::to_string(written) + "/" + std::to_string(total) + " bytes)\"}";
    }

    Serial.printf("[OTA] Flashed %u bytes, rebooting\n", (unsigned)written);
    // Let the JSON-RPC answer flush to the client before the reboot.
    delay(500);
    StatusController::getInstance()->forceRestart();
    return std::string("{\"updated\":true,\"bytes\":") +
           std::to_string(written) + "}";
  }
private:
  McpConfig<ClockwiseParams> config_;
};

// ----- server singleton & HTTP plumbing --------------------------------------

inline cw::mcp::McpServer& mcpGetServer()
{
  static ClockwiseMcpDevice device;
  static cw::mcp::McpServer server(device, CW_FW_VERSION);
  return server;
}

// Stable per-boot session id advertised in the Mcp-Session-Id header.
inline const char* mcpSessionId()
{
  static char sid[17];
  if (sid[0] == '\0')
  {
    snprintf(sid, sizeof(sid), "%08lx%08lx",
             (unsigned long)esp_random(), (unsigned long)esp_random());
  }
  return sid;
}

// POST /mcp entry point, invoked from ClockwiseWebServer. The HTTP status is
// decided first so headers are written in a single block:
//   200 + JSON body   -> request/response
//   202, empty body   -> notification
// keepAlive=true omits "Connection: close" so the MCP bridge can pipeline
// multiple JSON-RPC messages over one persistent TCP connection.
inline void mcpHandleHttpRequest(WiFiClient& client, const String& body, bool keepAlive)
{
  std::string response =
      mcpGetServer().handleMessage(std::string(body.c_str(), body.length()));

  if (response.empty())
  {
    client.println("HTTP/1.1 202 Accepted");
    client.println("Content-Length: 0");
  }
  else
  {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.printf("Content-Length: %u\r\n", (unsigned)response.size());
  }
  client.printf("Mcp-Session-Id: %s\r\n", mcpSessionId());
  if (!keepAlive)
    client.println("Connection: close");
  client.println();

  if (!response.empty())
    client.write(response.c_str(), response.size());
}
