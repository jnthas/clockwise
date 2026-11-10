#include "unity.h"
#include <map>
#include <string>
#include <vector>

// Included by relative path so the native env does not pull the whole
// cw-commons library (which depends on Arduino) into the host build.
#include "../../lib/cw-commons/McpJsonRpc.h"
#include "../../lib/cw-commons/McpConfig.h"
#include <ArduinoJson.h>

using cw::mcp::McpDevice;
using cw::mcp::McpServer;

namespace {

class MockDevice : public McpDevice
{
public:
  std::map<std::string, std::string> params = {
      {"displayBright", "255"},
      {"timeZone", "Asia/Shanghai"},
      {"ntpServer", "ntp.aliyun.com"},
      {"use24hFormat", "1"},
  };
  std::map<std::string, std::string> persisted;
  bool saved = false;
  int appliedBrightness = -1;
  int sensorValue = 777;
  bool restartRequested = false;

  // Fill the parameter set to the real firmware's 18 entries with realistic
  // value sizes, to reproduce the get_config serialization overflow.
  MockDevice()
  {
    params["autoBrightMin"] = "0";
    params["autoBrightMax"] = "0";
    params["ldrPin"] = "35";
    params["swapBlueGreen"] = "0";
    params["swapBlueRed"] = "0";
    params["displayRotation"] = "0";
    params["driver"] = "0";
    params["i2cSpeed"] = "8000000";
    params["E_pin"] = "18";
    params["wifiSsid"] = "system_客厅";
    params["wifiPwd"] = "";
    params["canvasFile"] = "";
    params["canvasServer"] = "raw.githubusercontent.com";
    params["manualPosix"] = "";
    persisted = params;
  }

  std::vector<std::string> paramNames() override
  {
    std::vector<std::string> names;
    for (const auto &kv : params)
      names.push_back(kv.first);
    return names;
  }

  bool getParam(const std::string &name, std::string &value) override
  {
    auto it = params.find(name);
    if (it == params.end())
      return false;
    value = it->second;
    return true;
  }

  bool setParam(const std::string &name, const std::string &value) override
  {
    if (name == "displayBright")
    {
      long v = strtol(value.c_str(), nullptr, 10);
      if (v < 0 || v > 255)
        return false;
    }
    if (params.count(name) == 0)
      return false;
    params[name] = value;
    return true;
  }

  void beginParams() override { params = persisted; }
  void cancelParams() override { params = persisted; }
  bool saveParams() override
  {
    long mn = std::stol(params["autoBrightMin"]);
    long mx = std::stol(params["autoBrightMax"]);
    if (mx > 0 && mx <= mn)
    {
      cancelParams();
      return false;
    }
    persisted = params;
    saved = true;
    return true;
  }
  void applyBrightness(int level) override { appliedBrightness = level; }
  int readLightSensor() override { return sensorValue; }
  void requestRestart() override { restartRequested = true; }
  std::string statusJson() override { return "{\"mock\":true}"; }
  std::string currentTime() override { return "21:59:00"; }
  std::string updateFirmware(const std::string &url) override
  {
    lastUpdateUrl = url;
    return updateResult;
  }
  std::string updateResult = "{\"updated\":true,\"bytes\":123456}";
  std::string lastUpdateUrl;
};

struct FakeConfigState
{
  bool swapBlueGreen = false, swapBlueRed = false, use24hFormat = true;
  uint8_t displayBright = 32, ldrPin = 35, displayRotation = 0, driver = 0, E_pin = 18;
  uint16_t autoBrightMin = 0, autoBrightMax = 0;
  uint32_t i2cSpeed = 8000000;
  std::string timeZone = "America/Sao_Paulo", ntpServer = "time.google.com";
  std::string wifiSsid, wifiPwd, canvasFile, canvasServer, manualPosix;
};

struct FakeConfigParams : FakeConfigState
{
    const char* const PREF_SWAP_BLUE_GREEN = "swapBlueGreen";
    const char* const PREF_SWAP_BLUE_RED = "swapBlueRed";
    const char* const PREF_USE_24H_FORMAT = "use24hFormat";
    const char* const PREF_DISPLAY_BRIGHT = "displayBright";
    const char* const PREF_DISPLAY_ABC_MIN = "autoBrightMin";
    const char* const PREF_DISPLAY_ABC_MAX = "autoBrightMax";
    const char* const PREF_LDR_PIN = "ldrPin";
    const char* const PREF_TIME_ZONE = "timeZone";
    const char* const PREF_WIFI_SSID = "wifiSsid";
    const char* const PREF_WIFI_PASSWORD = "wifiPwd";
    const char* const PREF_NTP_SERVER = "ntpServer";
    const char* const PREF_CANVAS_FILE = "canvasFile";
    const char* const PREF_CANVAS_SERVER = "canvasServer";
    const char* const PREF_MANUAL_POSIX = "manualPosix";
    const char* const PREF_CACHED_POSIX = "cachedPosix";
    const char* const PREF_CACHED_TZ = "cachedTz";
    const char* const PREF_DISPLAY_ROTATION = "displayRotation";
    const char* const PREF_DRIVER = "driver";
    const char* const PREF_I2CSPEED = "i2cSpeed";
    const char* const PREF_E_PIN = "E_pin";

  FakeConfigState flash;
  int loads = 0, saves = 0;
  void load() { ++loads; static_cast<FakeConfigState&>(*this) = flash; }
  void save() { ++saves; flash = static_cast<FakeConfigState&>(*this); }
};

// Uses the same transaction/parser as ClockwiseMcpDevice, including NVS-like
// reloads, so tool-level tests exercise the adapter's persistence contract.
class ConfigDevice : public MockDevice
{
public:
  FakeConfigParams config;
  McpConfig<FakeConfigParams> updates;
  bool autoBrightnessEnabled = false;
  ConfigDevice() : updates(config) {}
  void beginParams() override { updates.begin(); }
  void cancelParams() override { updates.cancel(); }
  bool setParam(const std::string& name, const std::string& value) override
  {
    return updates.setParam(name, value);
  }
  bool saveParams() override
  {
    if (!updates.save())
      return false;
    autoBrightnessEnabled = config.autoBrightMax > 0;
    return true;
  }
};

// Parses `body` and returns a heap-allocated document (caller deletes).
StaticJsonDocument<6144> *parse(const std::string &body)
{
  auto *doc = new StaticJsonDocument<6144>();
  DeserializationError err = deserializeJson(*doc, body);
  TEST_ASSERT_TRUE_MESSAGE(!err, err.c_str());
  return doc;
}

std::string extractText(StaticJsonDocument<6144> &doc)
{
  return doc["result"]["content"][0]["text"].as<std::string>();
}

} // namespace

void test_initialize_returns_server_info_and_protocol(void)
{
  MockDevice device;
  McpServer server(device, "9.9.9-test");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"initialize\",\"params\":{}}"));

  TEST_ASSERT_EQUAL(1, (*resp)["id"].as<long>());
  TEST_ASSERT_EQUAL_STRING("clockwise", (*resp)["result"]["serverInfo"]["name"]);
  TEST_ASSERT_EQUAL_STRING("9.9.9-test", (*resp)["result"]["serverInfo"]["version"]);
  TEST_ASSERT_TRUE((*resp)["result"]["capabilities"]["tools"].is<JsonObject>());

  delete resp;
}

void test_initialize_negotiates_requested_protocol_version(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"initialize\","
      "\"params\":{\"protocolVersion\":\"2025-03-26\"}}"));

  TEST_ASSERT_EQUAL_STRING("2025-03-26",
                           (*resp)["result"]["protocolVersion"].as<const char *>());

  delete resp;
}

void test_tools_list_contains_all_tools(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":2,\"method\":\"tools/list\"}"));

  JsonArray tools = (*resp)["result"]["tools"].as<JsonArray>();
  TEST_ASSERT_EQUAL(8, tools.size());

  std::vector<std::string> names;
  for (JsonObject tool : tools)
    names.push_back(tool["name"].as<std::string>());

  for (const char *expected : {"get_config", "set_config", "set_brightness",
                               "set_auto_brightness", "read_light_sensor",
                               "get_status", "restart_device",
                               "update_firmware"})
  {
    bool found = false;
    for (auto &n : names)
      found = found || n == expected;
    TEST_ASSERT_TRUE_MESSAGE(found, expected);
  }

  delete resp;
}

void test_get_config_returns_typed_values(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":3,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"get_config\",\"arguments\":{}}}"));

  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);

  StaticJsonDocument<1024> cfg;
  deserializeJson(cfg, extractText(*resp));
  TEST_ASSERT_EQUAL(18, cfg.size());
  TEST_ASSERT_EQUAL(255, cfg["displayBright"].as<int>());
  TEST_ASSERT_EQUAL_STRING("Asia/Shanghai", cfg["timeZone"].as<const char *>());
  TEST_ASSERT_EQUAL_STRING("raw.githubusercontent.com",
                           cfg["canvasServer"].as<const char *>());

  delete resp;
}

void test_set_config_updates_params_and_saves(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":4,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"set_config\",\"arguments\":{\"params\":{"
      "\"timeZone\":\"Europe/London\",\"use24hFormat\":\"0\"}}}}"));

  TEST_ASSERT_TRUE(device.saved);
  TEST_ASSERT_EQUAL_STRING("Europe/London", device.params["timeZone"].c_str());
  TEST_ASSERT_EQUAL_STRING("0", device.params["use24hFormat"].c_str());
  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);

  delete resp;
}

void test_set_config_rejects_unknown_param_with_is_error(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":5,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"set_config\",\"arguments\":{\"params\":{"
      "\"timeZone\":\"Europe/London\",\"bogusParam\":\"x\"}}}}"));

  TEST_ASSERT_TRUE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_FALSE(device.saved);
  TEST_ASSERT_EQUAL_STRING("Asia/Shanghai", device.params["timeZone"].c_str());

  delete resp;
}

void test_set_brightness_applies_immediately(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":6,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"set_brightness\",\"arguments\":{\"level\":60}}}"));

  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL(60, device.appliedBrightness);
  TEST_ASSERT_EQUAL_STRING("60", device.params["displayBright"].c_str());

  delete resp;
}

void test_set_brightness_out_of_range_is_error(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":7,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"set_brightness\",\"arguments\":{\"level\":300}}}"));

  TEST_ASSERT_TRUE((*resp)["result"]["isError"] | false);

  delete resp;
}

void test_read_light_sensor_returns_value(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":8,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"read_light_sensor\",\"arguments\":{}}}"));

  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  StaticJsonDocument<128> sensor;
  deserializeJson(sensor, extractText(*resp));
  TEST_ASSERT_EQUAL(777, sensor["ldrValue"].as<int>());

  delete resp;
}

void test_restart_device_requests_restart(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":9,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"restart_device\",\"arguments\":{}}}"));

  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_TRUE(device.restartRequested);

  delete resp;
}

void test_unknown_method_returns_minus_32601(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":10,\"method\":\"bogus/method\"}"));

  TEST_ASSERT_EQUAL(-32601, (*resp)["error"]["code"].as<int>());

  delete resp;
}

void test_unknown_tool_returns_minus_32602(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":11,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"bogus_tool\",\"arguments\":{}}}"));

  TEST_ASSERT_EQUAL(-32602, (*resp)["error"]["code"].as<int>());

  delete resp;
}

void test_malformed_json_returns_parse_error(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage("{not json"));

  TEST_ASSERT_EQUAL(-32700, (*resp)["error"]["code"].as<int>());

  delete resp;
}

void test_notification_returns_empty_body(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  TEST_ASSERT_EQUAL_STRING("",
                           server.handleMessage(
                               "{\"jsonrpc\":\"2.0\",\"method\":\"notifications/initialized\"}")
                               .c_str());
}

void test_ping_returns_empty_result(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":12,\"method\":\"ping\"}"));

  TEST_ASSERT_TRUE((*resp)["result"].is<JsonObject>());
  TEST_ASSERT_EQUAL(0, (*resp)["result"].size());

  delete resp;
}

void test_string_request_id_is_preserved(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");
  auto *resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":"req-1","method":"ping"})"));
  TEST_ASSERT_TRUE((*resp)["id"].is<const char *>());
  TEST_ASSERT_EQUAL_STRING("req-1", (*resp)["id"]);
  delete resp;
}

void test_large_integer_request_id_is_preserved(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");
  auto *resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":9007199254740991,"method":"ping"})"));
  TEST_ASSERT_TRUE((*resp)["id"].is<int64_t>());
  TEST_ASSERT_EQUAL_INT64(INT64_C(9007199254740991), (*resp)["id"].as<int64_t>());
  delete resp;
}

void test_configuration_batch_uses_one_load_and_persists_all_fields(void)
{
  ConfigDevice device;
  McpServer server(device, "1.0.0");
  auto *resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"set_config","arguments":{"params":{"timeZone":"Asia/Shanghai","manualPosix":"CST-8","use24hFormat":"0"}}}})"));
  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL(1, device.config.loads);
  TEST_ASSERT_EQUAL(1, device.config.saves);
  TEST_ASSERT_EQUAL_STRING("Asia/Shanghai", device.config.flash.timeZone.c_str());
  TEST_ASSERT_EQUAL_STRING("CST-8", device.config.flash.manualPosix.c_str());
  TEST_ASSERT_FALSE(device.config.flash.use24hFormat);
  delete resp;
}

void test_invalid_configuration_rolls_back_all_edits(void)
{
  ConfigDevice device;
  McpServer server(device, "1.0.0");
  auto *resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"set_config","arguments":{"params":{"timeZone":"Asia/Shanghai","displayBright":"999"}}}})"));
  TEST_ASSERT_TRUE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL(0, device.config.saves);
  TEST_ASSERT_EQUAL_STRING("America/Sao_Paulo", device.config.timeZone.c_str());
  TEST_ASSERT_EQUAL_STRING("America/Sao_Paulo", device.config.flash.timeZone.c_str());
  delete resp;
}

void test_auto_brightness_persists_both_endpoints_and_can_disable(void)
{
  ConfigDevice device;
  McpServer server(device, "1.0.0");
  auto *resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"set_auto_brightness","arguments":{"min":100,"max":2000}}})"));
  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL(100, device.config.flash.autoBrightMin);
  TEST_ASSERT_EQUAL(2000, device.config.flash.autoBrightMax);
  TEST_ASSERT_TRUE(device.autoBrightnessEnabled);
  delete resp;

  resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"set_auto_brightness","arguments":{"min":0,"max":0}}})"));
  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL(0, device.config.flash.autoBrightMin);
  TEST_ASSERT_EQUAL(0, device.config.flash.autoBrightMax);
  TEST_ASSERT_FALSE(device.autoBrightnessEnabled);
  delete resp;
}

void test_auto_brightness_rejects_equal_or_reversed_enabled_range(void)
{
  ConfigDevice device;
  McpServer server(device, "1.0.0");
  for (int maximum : {100, 99})
  {
    std::string request = R"({"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"set_auto_brightness","arguments":{"min":100,"max":)" + std::to_string(maximum) + "}}}";
    auto *resp = parse(server.handleMessage(request));
    TEST_ASSERT_TRUE((*resp)["result"]["isError"] | false);
    TEST_ASSERT_EQUAL(0, device.config.saves);
    delete resp;
  }
  // set_config must enforce the same invariant, including single-field edits.
  device.config.flash.autoBrightMax = 200;
  auto *resp = parse(server.handleMessage(
      R"({"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"set_config","arguments":{"params":{"autoBrightMin":"200"}}}})"));
  TEST_ASSERT_TRUE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL(0, device.config.saves);
  TEST_ASSERT_EQUAL(0, device.config.autoBrightMin);
  TEST_ASSERT_EQUAL(200, device.config.autoBrightMax);
  delete resp;
}

void test_rotation_uses_quarter_turns_without_truncation(void)
{
  FakeConfigParams params;
  McpConfig<FakeConfigParams> config(params);
  for (int turns : {0, 1, 2, 3})
  {
    config.begin();
    TEST_ASSERT_TRUE(config.setParam("displayRotation", std::to_string(turns)));
    TEST_ASSERT_TRUE(config.save());
    TEST_ASSERT_EQUAL(turns, params.flash.displayRotation);
  }
  for (const char *invalid : {"4", "90", "180", "270", "", "-1"})
  {
    config.begin();
    TEST_ASSERT_FALSE(config.setParam("displayRotation", invalid));
    config.cancel();
    TEST_ASSERT_EQUAL(3, params.displayRotation);
  }
}

void test_string_request_id_is_preserved_on_tool_and_error_responses(void)
{
  ConfigDevice device;
  McpServer server(device, "1.0.0");
  for (const char *method : {"tools/list", "missing-method"})
  {
    std::string request = R"({"jsonrpc":"2.0","id":"007","method":")" + std::string(method) + R"("})";
    auto *resp = parse(server.handleMessage(request));
    TEST_ASSERT_TRUE((*resp)["id"].is<const char *>());
    TEST_ASSERT_EQUAL_STRING("007", (*resp)["id"]);
    delete resp;
  }
}

void test_long_string_request_id_survives_response_allocation(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");
  std::string id(300, 'x');
  for (const char *method : {"ping", "missing-method"})
  {
    std::string request = R"({"jsonrpc":"2.0","id":")" + id + R"(","method":")" + method + R"("})";
    auto *resp = parse(server.handleMessage(request));
    TEST_ASSERT_EQUAL_STRING(id.c_str(), (*resp)["id"]);
    TEST_ASSERT_TRUE((*resp).containsKey(std::string(method) == "ping" ? "result" : "error"));
    delete resp;
  }
}

void test_update_firmware_returns_device_result(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":13,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"update_firmware\",\"arguments\":"
      "{\"url\":\"http://192.168.31.100/firmware.bin\"}}}"));

  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL_STRING("http://192.168.31.100/firmware.bin",
                           device.lastUpdateUrl.c_str());
  TEST_ASSERT_TRUE(strstr((*resp)["result"]["content"][0]["text"].as<std::string>().c_str(),
                          "\"updated\":true") != nullptr);

  delete resp;
}

void test_update_firmware_rejects_bad_url(void)
{
  MockDevice device;
  McpServer server(device, "1.0.0");

  auto *resp = parse(server.handleMessage(
      "{\"jsonrpc\":\"2.0\",\"id\":14,\"method\":\"tools/call\","
      "\"params\":{\"name\":\"update_firmware\",\"arguments\":"
      "{\"url\":\"ftp://example.com/firmware.bin\"}}}"));

  TEST_ASSERT_TRUE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_TRUE(device.lastUpdateUrl.empty());

  delete resp;
}

void run_mcp_json_rpc_tests(void)
{
  RUN_TEST(test_initialize_returns_server_info_and_protocol);
  RUN_TEST(test_initialize_negotiates_requested_protocol_version);
  RUN_TEST(test_tools_list_contains_all_tools);
  RUN_TEST(test_get_config_returns_typed_values);
  RUN_TEST(test_set_config_updates_params_and_saves);
  RUN_TEST(test_set_config_rejects_unknown_param_with_is_error);
  RUN_TEST(test_set_brightness_applies_immediately);
  RUN_TEST(test_set_brightness_out_of_range_is_error);
  RUN_TEST(test_read_light_sensor_returns_value);
  RUN_TEST(test_restart_device_requests_restart);
  RUN_TEST(test_unknown_method_returns_minus_32601);
  RUN_TEST(test_unknown_tool_returns_minus_32602);
  RUN_TEST(test_malformed_json_returns_parse_error);
  RUN_TEST(test_notification_returns_empty_body);
  RUN_TEST(test_ping_returns_empty_result);
  RUN_TEST(test_string_request_id_is_preserved);
  RUN_TEST(test_large_integer_request_id_is_preserved);
  RUN_TEST(test_configuration_batch_uses_one_load_and_persists_all_fields);
  RUN_TEST(test_invalid_configuration_rolls_back_all_edits);
  RUN_TEST(test_auto_brightness_persists_both_endpoints_and_can_disable);
  RUN_TEST(test_auto_brightness_rejects_equal_or_reversed_enabled_range);
  RUN_TEST(test_rotation_uses_quarter_turns_without_truncation);
  RUN_TEST(test_string_request_id_is_preserved_on_tool_and_error_responses);
  RUN_TEST(test_long_string_request_id_survives_response_allocation);
  RUN_TEST(test_update_firmware_returns_device_result);
  RUN_TEST(test_update_firmware_rejects_bad_url);
}
