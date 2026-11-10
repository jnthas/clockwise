#pragma once

// Shared configuration transaction used by the firmware adapter and native
// tests. Reload exactly once per transaction; discard all edits on failure.
#include <stdint.h>
#include <stdlib.h>
#include <string>

// ----- parsing helpers -------------------------------------------------------

inline bool mcpParseBool(const std::string& v, bool& out)
{
  if (v == "1" || v == "true" || v == "on") { out = true; return true; }
  if (v == "0" || v == "false" || v == "off") { out = false; return true; }
  char* end = nullptr;
  long l = strtol(v.c_str(), &end, 10);
  if (end != v.c_str() && *end == '\0') { out = (l != 0); return true; }
  return false;
}

inline bool mcpParseUIntRange(const std::string& v, long min, long max, long& out)
{
  char* end = nullptr;
  long l = strtol(v.c_str(), &end, 10);
  if (end == v.c_str() || *end != '\0' || l < min || l > max)
    return false;
  out = l;
  return true;
}

template <typename Params>
class McpConfig
{
public:
  explicit McpConfig(Params& params) : params_(params) {}

  void begin() { params_.load(); }
  void cancel() { params_.load(); }

  bool save()
  {
    // max=0 is the explicit off state, regardless of the stored minimum.
    if (params_.autoBrightMax > 0 && params_.autoBrightMax <= params_.autoBrightMin)
    {
      cancel();
      return false;
    }
    params_.save();
    return true;
  }

  bool setParam(const std::string& name, const std::string& value)
  {
    Params* p = &params_;
    long num = 0;
    bool flag = false;

    if (name == p->PREF_DISPLAY_BRIGHT)
    {
      if (!mcpParseUIntRange(value, 0, 255, num)) return false;
      p->displayBright = (uint8_t)num;
    }
    else if (name == p->PREF_DISPLAY_ABC_MIN)
    {
      if (!mcpParseUIntRange(value, 0, 4095, num)) return false;
      p->autoBrightMin = (uint16_t)num;
    }
    else if (name == p->PREF_DISPLAY_ABC_MAX)
    {
      if (!mcpParseUIntRange(value, 0, 4095, num)) return false;
      p->autoBrightMax = (uint16_t)num;
    }
    else if (name == p->PREF_LDR_PIN)
    {
      if (!mcpParseUIntRange(value, 0, 39, num)) return false;
      p->ldrPin = (uint8_t)num;
    }
    else if (name == p->PREF_TIME_ZONE)
    {
      if (value.empty() || value.length() > 64) return false;
      p->timeZone = value.c_str();
    }
    else if (name == p->PREF_NTP_SERVER)
    {
      if (value.length() > 64) return false;
      p->ntpServer = value.c_str();
    }
    else if (name == p->PREF_USE_24H_FORMAT)
    {
      if (!mcpParseBool(value, flag)) return false;
      p->use24hFormat = flag;
    }
    else if (name == p->PREF_SWAP_BLUE_GREEN)
    {
      if (!mcpParseBool(value, flag)) return false;
      p->swapBlueGreen = flag;
    }
    else if (name == p->PREF_SWAP_BLUE_RED)
    {
      if (!mcpParseBool(value, flag)) return false;
      p->swapBlueRed = flag;
    }
    else if (name == p->PREF_DISPLAY_ROTATION)
    {
      if (!mcpParseUIntRange(value, 0, 3, num)) return false;
      p->displayRotation = (uint8_t)num;
    }
    else if (name == p->PREF_DRIVER)
    {
      if (!mcpParseUIntRange(value, 0, 5, num)) return false;
      p->driver = (uint8_t)num;
    }
    else if (name == p->PREF_I2CSPEED)
    {
      if (!mcpParseUIntRange(value, 0, 20000000, num)) return false;
      if (num != 8000000 && num != 16000000 && num != 20000000) return false;
      p->i2cSpeed = (uint32_t)num;
    }
    else if (name == p->PREF_E_PIN)
    {
      if (!mcpParseUIntRange(value, 0, 39, num)) return false;
      p->E_pin = (uint8_t)num;
    }
    else if (name == p->PREF_WIFI_SSID)
    {
      if (value.length() > 32) return false;
      p->wifiSsid = value.c_str();
    }
    else if (name == p->PREF_WIFI_PASSWORD)
    {
      if (value.length() > 64) return false;
      p->wifiPwd = value.c_str();
    }
    else if (name == p->PREF_CANVAS_FILE)
    {
      if (value.length() > 128) return false;
      p->canvasFile = value.c_str();
    }
    else if (name == p->PREF_CANVAS_SERVER)
    {
      if (value.length() > 64) return false;
      p->canvasServer = value.c_str();
    }
    else if (name == p->PREF_MANUAL_POSIX)
    {
      if (value.length() > 64) return false;
      p->manualPosix = value.c_str();
    }
    else
    {
      return false;
    }

    return true;
  }

private:
  Params& params_;
};
