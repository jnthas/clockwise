#include "unity.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // tear stuff down here
}

void test_function_should_be_true(void) {
  TEST_ASSERT_TRUE(true);
}

// Defined in WiFiSetupStateMachineTests.cpp
void run_wifi_setup_state_machine_tests(void);

// Defined in McpJsonRpcTests.cpp
void run_mcp_json_rpc_tests(void);

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_function_should_be_true);
  run_wifi_setup_state_machine_tests();
  run_mcp_json_rpc_tests();
  return UNITY_END();
}


int main() {
  runUnityTests();  
}
