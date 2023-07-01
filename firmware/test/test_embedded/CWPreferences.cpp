#define CW_PREF_DB_NAME "clockwise_test"

#include "unity.h"
#include "Arduino.h"
#include "CWPreferences.h"
#include <Update.h>
#include <WebServer.h>
#include <DNSServer.h>


void setUp(void) {
}

void tearDown(void) {
  // clean stuff up here
}

void test_function_should_get_default_values(void) {
  ClockwiseParams::getInstance()->load();
  TEST_ASSERT_EQUAL_STRING("raw.githubusercontent.com", ClockwiseParams::getInstance()->canvasServer.c_str());
  TEST_ASSERT_EQUAL_STRING("America/Sao_Paulo", ClockwiseParams::getInstance()->timeZone.c_str());
  TEST_ASSERT_EQUAL_STRING(NTP_SERVER, ClockwiseParams::getInstance()->ntpServer.c_str());
  TEST_ASSERT_EQUAL(32, ClockwiseParams::getInstance()->displayBright);
  TEST_ASSERT_EQUAL(true, ClockwiseParams::getInstance()->use24hFormat);
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_function_should_get_default_values);
  return UNITY_END();
}

void setup() 
{  
  delay(2000);
  runUnityTests();  
}
void loop() {}

