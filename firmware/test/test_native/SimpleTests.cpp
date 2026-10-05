#include "unity.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_function_should_be_true(void) {
  TEST_ASSERT_TRUE(true);
}

// Defined in WiFiSetupStateMachineTests.cpp
void run_wifi_setup_state_machine_tests(void);

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_function_should_be_true);
  run_wifi_setup_state_machine_tests();
  return UNITY_END();
}


int main() {
  runUnityTests();  
}

