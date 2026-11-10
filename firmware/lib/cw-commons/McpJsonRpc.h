#pragma once

// Clockwise MCP (Model Context Protocol) server core.
//
// Platform-independent JSON-RPC 2.0 handler implementing the MCP "tools"
// capability over HTTP POST bodies with application/json responses
// (a simplified embedded-friendly subset of the MCP Streamable HTTP
// transport: no server-initiated messages, single JSON responses).
//
// Pure C++ (std::string + ArduinoJson) so it can be unit-tested on the host
// and compiled into the ESP32 firmware unchanged. Hardware access is
// abstracted behind cw::mcp::McpDevice; see CWMcp.h for the firmware adapter.

// ESP32 long is 32-bit; JSON-RPC integer IDs must survive a full round trip.
#ifndef ARDUINOJSON_USE_LONG_LONG
#define ARDUINOJSON_USE_LONG_LONG 1
#endif
#include <ArduinoJson.h>
#include <ctype.h>
#include <stdint.h>
#include <functional>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

namespace cw {
namespace mcp {

const char* const kProtocolVersion = "2025-03-26";

// Abstraction over the device the MCP server controls.
class McpDevice
{
public:
  virtual ~McpDevice() {}

  // Names of all user-facing configuration parameters.
  virtual std::vector<std::string> paramNames() = 0;

  // Reads a parameter as a string (numbers as decimal). Returns false if unknown.
  virtual bool getParam(const std::string& name, std::string& value) = 0;

  // Starts/aborts a batch of edits against the persisted configuration.
  virtual void beginParams() = 0;
  virtual void cancelParams() = 0;

  // Sets a parameter from its string form. Returns false if unknown or invalid.
  virtual bool setParam(const std::string& name, const std::string& value) = 0;

  // Validates the whole configuration and persists it, or rolls back on error.
  virtual bool saveParams() = 0;

  // Runtime-applies display brightness (may be a no-op when unsupported).
  virtual void applyBrightness(int level) = 0;

  // Reads the ambient light sensor; negative means not available.
  virtual int readLightSensor() = 0;

  // Schedules a device restart.
  virtual void requestRestart() = 0;

  // Device status as a serialized JSON object (wifi, heap, time, ...).
  virtual std::string statusJson() = 0;

  // Current local time "HH:MM:SS", empty when not synced.
  virtual std::string currentTime() = 0;

  // Downloads a firmware image from `url` and flashes it to the OTA partition.
  // Returns a serialized JSON object: {"updated":true,...} on success (a
  // restart follows), or {"error":"..."} on failure.
  virtual std::string updateFirmware(const std::string& url) = 0;
};

class McpServer
{
public:
  McpServer(McpDevice& device, const std::string& serverVersion)
      : device_(device), serverVersion_(serverVersion) {}

  // Handles one JSON-RPC message body.
  // Returns the response body to send back, or an empty string for
  // notifications / client responses (caller answers HTTP 202, no body).
  std::string handleMessage(const std::string& body)
  {
    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, body);
    if (err)
      return errorResponse(JsonVariantConst(), false, -32700, "Parse error");

    // A message carrying result/error is a client response; ignore it.
    if (doc.containsKey("result") || doc.containsKey("error"))
      return "";

    bool hasId = doc.containsKey("id") && !doc["id"].isNull();
    JsonVariantConst id = doc["id"];
    if (hasId && !id.is<const char*>() && !id.is<int64_t>() && !id.is<uint64_t>())
      return errorResponse(JsonVariantConst(), false, -32600, "Invalid request id");

    if (!doc.containsKey("method") || !doc["method"].is<const char*>())
      return errorResponse(id, hasId, -32600, "Invalid Request");

    const char* method = doc["method"].as<const char*>();

    // Notifications (no id) never get a response body.
    if (!hasId)
      return "";

    if (strcmp(method, "initialize") == 0)
      return handleInitialize(id, doc["params"]);
    if (strcmp(method, "ping") == 0)
      return respond(id, 128, [](JsonObject result) { (void)result; });
    if (strcmp(method, "tools/list") == 0)
      return handleToolsList(id);
    if (strcmp(method, "tools/call") == 0)
      return handleToolsCall(id, doc["params"]);

    return errorResponse(id, true, -32601, std::string("Method not found: ") + method);
  }

private:
  McpDevice& device_;
  std::string serverVersion_;

  // ----- response builders -------------------------------------------------

  static std::string errorResponse(JsonVariantConst id, bool hasId, int code, const std::string& message)
  {
    DynamicJsonDocument doc(256 + measureJson(id));
    doc["jsonrpc"] = "2.0";
    if (hasId)
      doc["id"].set(id);
    else
      doc["id"] = nullptr;
    JsonObject err = doc.createNestedObject("error");
    err["code"] = code;
    err["message"] = message;
    std::string out;
    serializeJson(doc, out);
    return out;
  }

  static std::string respond(JsonVariantConst id, size_t capacity, std::function<void(JsonObject result)> fill)
  {
    DynamicJsonDocument doc(capacity + measureJson(id));
    doc["jsonrpc"] = "2.0";
    doc["id"].set(id);
    JsonObject result = doc.createNestedObject("result");
    fill(result);
    std::string out;
    serializeJson(doc, out);
    return out;
  }

  static std::string toolResult(JsonVariantConst id, const std::string& text, bool isError)
  {
    return respond(id, 1024 + text.size(), [&](JsonObject result) {
      JsonArray content = result.createNestedArray("content");
      JsonObject item = content.createNestedObject();
      item["type"] = "text";
      item["text"] = text;
      if (isError)
        result["isError"] = true;
    });
  }

  // Stores numeric-looking strings as JSON numbers, everything else as strings.
  static void setTyped(JsonObject obj, const std::string& key, const std::string& value)
  {
    bool numeric = !value.empty();
    size_t start = (value[0] == '-') ? 1 : 0;
    if (start == value.size())
      numeric = false;
    for (size_t i = start; i < value.size() && numeric; i++)
      numeric = isdigit((unsigned char)value[i]) != 0;

    // Pass the key as std::string: ArduinoJson 6 stores a const char* key
    // zero-copy, which would dangle here (the caller's key buffer does not
    // outlive the JsonDocument). A std::string key forces a copy.
    if (numeric)
      obj[std::string(key)] = (long)atol(value.c_str());
    else
      obj[std::string(key)] = value;
  }

  static std::string join(const std::vector<std::string>& items)
  {
    std::string out;
    for (size_t i = 0; i < items.size(); i++)
    {
      if (i)
        out += ", ";
      out += items[i];
    }
    return out;
  }

  // ----- MCP methods -------------------------------------------------------

  std::string handleInitialize(JsonVariantConst id, JsonVariantConst params)
  {
    return respond(id, 512, [&](JsonObject result) {
      // Echo the client's protocol version when we recognise it.
      const char* requested = params["protocolVersion"] | kProtocolVersion;
      result["protocolVersion"] = requested;
      JsonObject caps = result.createNestedObject("capabilities");
      JsonObject tools = caps.createNestedObject("tools");
      tools["listChanged"] = false;
      JsonObject serverInfo = result.createNestedObject("serverInfo");
      serverInfo["name"] = "clockwise";
      serverInfo["version"] = serverVersion_;
    });
  }

  static void addTool(JsonArray tools, const char* name, const char* description,
                      std::function<void(JsonObject schema)> fillSchema)
  {
    JsonObject tool = tools.createNestedObject();
    tool["name"] = name;
    tool["description"] = description;
    JsonObject schema = tool.createNestedObject("inputSchema");
    schema["type"] = "object";
    fillSchema(schema);
  }

  std::string handleToolsList(JsonVariantConst id)
  {
    return respond(id, 6144, [&](JsonObject result) {
      JsonArray tools = result.createNestedArray("tools");

      addTool(tools, "get_config",
              "Read all Clockwise configuration parameters (brightness, timezone, "
              "NTP server, rotation, auto-brightness, Wi-Fi, canvas, ...).",
              [](JsonObject schema) { schema.createNestedObject("properties"); });

      addTool(tools, "set_config",
              "Update configuration parameters, persisted to flash. Keys: displayBright "
              "(0-255), timeZone (Olson, e.g. Asia/Shanghai), manualPosix (e.g. CST-8), "
              "ntpServer, use24hFormat, displayRotation (0/1/2/3 quarter-turns), autoBrightMin/"
              "autoBrightMax (0-4095; max>0 enables auto brightness), ldrPin, "
              "swapBlueGreen, swapBlueRed, wifiSsid, wifiPwd, canvasFile, canvasServer, "
              "driver (0-5), i2cSpeed (8000000/16000000/20000000), E_pin. Changes to "
              "rotation/driver/RGB swap require a restart to take effect.",
              [](JsonObject schema) {
                JsonObject props = schema.createNestedObject("properties");
                JsonObject params = props.createNestedObject("params");
                params["type"] = "object";
                params["description"] = "Map of parameter name to string value.";
                JsonObject additional = params.createNestedObject("additionalProperties");
                additional["type"] = "string";
                schema.createNestedArray("required").add("params");
              });

      addTool(tools, "set_brightness",
              "Set display brightness (0-255) and apply it immediately. If auto "
              "brightness is enabled (autoBrightMax > 0) the LDR loop may override it.",
              [](JsonObject schema) {
                JsonObject props = schema.createNestedObject("properties");
                JsonObject level = props.createNestedObject("level");
                level["type"] = "integer";
                level["minimum"] = 0;
                level["maximum"] = 255;
                schema.createNestedArray("required").add("level");
              });

      addTool(tools, "set_auto_brightness",
              "Configure auto brightness from the LDR sensor: min/max raw ADC range "
              "(0-4095; max must exceed min when enabled). Set max to 0 to disable auto brightness.",
              [](JsonObject schema) {
                JsonObject props = schema.createNestedObject("properties");
                JsonObject mn = props.createNestedObject("min");
                mn["type"] = "integer";
                mn["minimum"] = 0;
                mn["maximum"] = 4095;
                JsonObject mx = props.createNestedObject("max");
                mx["type"] = "integer";
                mx["minimum"] = 0;
                mx["maximum"] = 4095;
                JsonArray req = schema.createNestedArray("required");
                req.add("min");
                req.add("max");
              });

      addTool(tools, "read_light_sensor",
              "Read the LDR ambient light sensor raw ADC value (0-4095).",
              [](JsonObject schema) { schema.createNestedObject("properties"); });

      addTool(tools, "get_status",
              "Device status: firmware, clockface, Wi-Fi connection/IP/signal, free "
              "heap, uptime, timezone and current local time.",
              [](JsonObject schema) { schema.createNestedObject("properties"); });

      addTool(tools, "restart_device",
              "Reboot the clock. Required for rotation, RGB swap, driver, I2S speed "
              "and E-pin changes to take effect.",
              [](JsonObject schema) { schema.createNestedObject("properties"); });

      addTool(tools, "update_firmware",
              "Download a firmware .bin from a URL (http or https) and flash it "
              "to the OTA partition, then reboot into the new firmware. Serve the "
              "bin over the LAN (a local HTTP server or a release asset). The "
              "call blocks until the update finishes or fails.",
              [](JsonObject schema) {
                JsonObject props = schema.createNestedObject("properties");
                JsonObject url = props.createNestedObject("url");
                url["type"] = "string";
                url["description"] = "Firmware .bin download URL.";
                schema.createNestedArray("required").add("url");
              });
    });
  }

  std::string handleToolsCall(JsonVariantConst id, JsonVariantConst params)
  {
    if (!params.is<JsonObjectConst>() || !params["name"].is<const char*>())
      return errorResponse(id, true, -32602, "Invalid params: missing tool name");

    const std::string name = params["name"].as<const char*>();
    JsonVariantConst args = params["arguments"];

    if (name == "get_config")
      return callGetConfig(id);

    if (name == "set_config")
      return callSetConfig(id, args);

    if (name == "set_brightness")
      return callSetBrightness(id, args);

    if (name == "set_auto_brightness")
      return callSetAutoBrightness(id, args);

    if (name == "read_light_sensor")
      return callReadLightSensor(id);

    if (name == "get_status")
      return toolResult(id, device_.statusJson(), false);

    if (name == "restart_device")
    {
      device_.requestRestart();
      return toolResult(id, "Restarting Clockwise...", false);
    }

    if (name == "update_firmware")
      return callUpdateFirmware(id, args);

    return errorResponse(id, true, -32602, "Unknown tool: " + name);
  }

  // ----- individual tools --------------------------------------------------

  std::string callGetConfig(JsonVariantConst id)
  {
    DynamicJsonDocument cfg(1536);
    JsonObject root = cfg.to<JsonObject>();
    for (const std::string& key : device_.paramNames())
    {
      std::string value;
      if (device_.getParam(key, value))
        setTyped(root, key, value);
    }
    std::string text;
    serializeJson(cfg, text);
    return toolResult(id, text, false);
  }

  std::string callSetConfig(JsonVariantConst id, JsonVariantConst args)
  {
    if (!args.is<JsonObjectConst>() || !args["params"].is<JsonObjectConst>())
      return errorResponse(id, true, -32602, "Invalid params: expected {params: {...}}");

    device_.beginParams();
    std::vector<std::string> updated, failed;
    for (JsonPairConst kv : args["params"].as<JsonObjectConst>())
    {
      std::string value = kv.value().as<std::string>();
      if (device_.setParam(kv.key().c_str(), value))
        updated.push_back(kv.key().c_str());
      else
        failed.push_back(kv.key().c_str());
    }

    if (!failed.empty())
    {
      device_.cancelParams();
      return toolResult(id, "Unknown or invalid parameter(s): " + join(failed), true);
    }

    if (!device_.saveParams())
      return toolResult(id, "Invalid configuration: autoBrightMax must exceed autoBrightMin when enabled", true);
    std::string text = "{\"updated\":[";
    for (size_t i = 0; i < updated.size(); i++)
    {
      if (i)
        text += ",";
      text += "\"" + updated[i] + "\"";
    }
    text += "]}";
    return toolResult(id, text, false);
  }

  std::string callSetBrightness(JsonVariantConst id, JsonVariantConst args)
  {
    if (!args["level"].is<int>())
      return errorResponse(id, true, -32602, "Invalid params: expected {level: 0-255}");

    int level = args["level"].as<int>();
    if (level < 0 || level > 255)
      return toolResult(id, "level must be between 0 and 255", true);

    device_.beginParams();
    if (!device_.setParam("displayBright", std::to_string(level)))
    {
      device_.cancelParams();
      return toolResult(id, "Unable to set display brightness", true);
    }
    if (!device_.saveParams())
      return toolResult(id, "Invalid configuration: autoBrightMax must exceed autoBrightMin when enabled", true);
    device_.applyBrightness(level);
    return toolResult(id, std::string("{\"displayBright\":") + std::to_string(level) +
                               ",\"applied\":true}",
                      false);
  }

  std::string callSetAutoBrightness(JsonVariantConst id, JsonVariantConst args)
  {
    if (!args["min"].is<int>() || !args["max"].is<int>())
      return errorResponse(id, true, -32602, "Invalid params: expected {min, max}");

    int mn = args["min"].as<int>();
    int mx = args["max"].as<int>();
    if (mn < 0 || mn > 4095 || mx < 0 || mx > 4095)
      return toolResult(id, "min/max must be between 0 and 4095", true);

    if (mx > 0 && mx <= mn)
      return toolResult(id, "max must exceed min when auto brightness is enabled", true);

    device_.beginParams();
    if (!device_.setParam("autoBrightMin", std::to_string(mn)) ||
        !device_.setParam("autoBrightMax", std::to_string(mx)))
    {
      device_.cancelParams();
      return toolResult(id, "Unable to set auto brightness range", true);
    }
    if (!device_.saveParams())
      return toolResult(id, "Invalid auto brightness range", true);
    return toolResult(id, std::string("{\"autoBrightMin\":") + std::to_string(mn) +
                               ",\"autoBrightMax\":" + std::to_string(mx) +
                               ",\"enabled\":" + (mx > 0 ? "true" : "false") + "}",
                      false);
  }

  std::string callReadLightSensor(JsonVariantConst id)
  {
    int value = device_.readLightSensor();
    if (value < 0)
      return toolResult(id, "Light sensor not available", true);
    return toolResult(id, std::string("{\"ldrValue\":") + std::to_string(value) + "}", false);
  }

  std::string callUpdateFirmware(JsonVariantConst id, JsonVariantConst args)
  {
    if (!args["url"].is<const char*>())
      return errorResponse(id, true, -32602, "Invalid params: expected {url: \"...\"}");

    const std::string url = args["url"].as<const char*>();
    if (url.length() < 8 ||
        (url.find("http://") != 0 && url.find("https://") != 0))
      return toolResult(id, "url must start with http:// or https://", true);

    // The adapter blocks here for the whole download + flash, then the device
    // reboots into the new firmware shortly after we answer.
    std::string result = device_.updateFirmware(url);
    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, result);
    if (err || (doc.containsKey("error") && doc["error"].as<bool>()))
      return toolResult(id, err ? result : doc["error"].as<std::string>(), true);
    return toolResult(id, result, false);
  }
};

} // namespace mcp
} // namespace cw
