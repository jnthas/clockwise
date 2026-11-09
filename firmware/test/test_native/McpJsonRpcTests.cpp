#include "unity.h"
#include <map>
#include <string>
#include <vector>

// Included by relative path so the native env does not pull the whole
// cw-commons library (which depends on Arduino) into the host build.
#include "../../lib/cw-commons/McpJsonRpc.h"
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

  void saveParams() override { saved = true; }
  void applyBrightness(int level) override { appliedBrightness = level; }
  int readLightSensor() override { return sensorValue; }
  void requestRestart() override { restartRequested = true; }
  std::string statusJson() override { return "{\"mock\":true}"; }
  std::string currentTime() override { return "21:59:00"; }
};

// Parses `body` and returns a heap-allocated document (caller deletes).
StaticJsonDocument<4096> *parse(const std::string &body)
{
  auto *doc = new StaticJsonDocument<4096>();
  DeserializationError err = deserializeJson(*doc, body);
  TEST_ASSERT_TRUE_MESSAGE(!err, err.c_str());
  return doc;
}

std::string extractText(StaticJsonDocument<4096> &doc)
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
  TEST_ASSERT_EQUAL(7, tools.size());

  std::vector<std::string> names;
  for (JsonObject tool : tools)
    names.push_back(tool["name"].as<std::string>());

  for (const char *expected : {"get_config", "set_config", "set_brightness",
                               "set_auto_brightness", "read_light_sensor",
                               "get_status", "restart_device"})
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

  TEST_ASSERT_FALSE((*resp)["result"]["isError"] | false);
  TEST_ASSERT_EQUAL_STRING("", device.params["timeZone"].c_str());

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
  TEST_ASSERT_EQUAL(777, (*resp)["result"]["content"][0]["text"].as<std::string>().find("777") != std::string::npos);

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

  delete resp;
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
}
