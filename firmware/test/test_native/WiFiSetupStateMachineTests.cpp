#include "unity.h"
#include <stdint.h>

// Included by relative path so the native env does not pull the whole
// cw-commons library (which depends on Arduino) into the host build.
#include "../../lib/cw-commons/WiFiSetupStateMachine.h"

using cw::WiFiSetupAction;
using cw::WiFiSetupState;
using cw::WiFiSetupStateMachine;

static WiFiSetupStateMachine::Config testConfig()
{
  WiFiSetupStateMachine::Config cfg;
  cfg.staConnectTimeoutMs = 10000;
  cfg.improvGraceMs = 15000;
  cfg.improvSessionMs = 120000;
  cfg.portalTimeoutMs = 300000;
  return cfg;
}

// Regression for the web-installer bug: with no stored credentials the device
// must NOT jump straight into the AP portal; it has to listen for Improv first.
void test_no_credentials_waits_for_improv_instead_of_starting_portal(void)
{
  WiFiSetupStateMachine fsm(testConfig());

  TEST_ASSERT_EQUAL(WiFiSetupAction::EnterImprovWait, fsm.begin(false, 1000));
  TEST_ASSERT_EQUAL(WiFiSetupState::ImprovWait, fsm.state());

  // Just before the grace window ends: still listening, no portal.
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(1000 + 14999, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::ImprovWait, fsm.state());
}

void test_no_credentials_starts_portal_after_grace_window(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(false, 1000);

  TEST_ASSERT_EQUAL(WiFiSetupAction::StartApPortal, fsm.update(1000 + 15000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::ApPortal, fsm.state());
}

void test_stored_credentials_connect_emits_connected_once(void)
{
  WiFiSetupStateMachine fsm(testConfig());

  TEST_ASSERT_EQUAL(WiFiSetupAction::StartStationConnect, fsm.begin(true, 0));
  TEST_ASSERT_EQUAL(WiFiSetupState::Connecting, fsm.state());

  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(3000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::Connected, fsm.update(4000, true, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::Connected, fsm.state());

  // Later passes (even if the link flaps) never re-run the network-ready path.
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(5000, true, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(6000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(7000, true, false));
}

// Stale credentials must fall back to Improv (not straight to the AP portal).
void test_stored_credentials_timeout_falls_back_to_improv(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(true, 0);

  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(9999, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::EnterImprovWait, fsm.update(10000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::ImprovWait, fsm.state());

  // Grace window is measured from entering ImprovWait, not from boot.
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(10000 + 14999, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::StartApPortal, fsm.update(10000 + 15000, false, false));
}

// While a browser is talking Improv, keep waiting so the user can pick an SSID
// and type the password without the device switching to AP mode underneath.
void test_improv_activity_extends_wait_window(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(false, 0);

  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(2000, false, true));

  // Past the grace window, but within the session window after activity.
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(60000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(2000 + 119999, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::ImprovWait, fsm.state());

  // Session expires without further activity -> AP portal fallback.
  TEST_ASSERT_EQUAL(WiFiSetupAction::StartApPortal, fsm.update(2000 + 120000, false, false));
}

void test_improv_activity_during_connecting_carries_over(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(true, 0);

  // Browser probes while stored credentials are still being tried.
  fsm.update(5000, false, true);
  TEST_ASSERT_EQUAL(WiFiSetupAction::EnterImprovWait, fsm.update(10000, false, false));

  // Grace would end at 25000, but the session (5000 + 120000) keeps us waiting.
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(30000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::StartApPortal, fsm.update(125000, false, false));
}

void test_improv_provisioning_during_wait_connects(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(false, 0);

  TEST_ASSERT_EQUAL(WiFiSetupAction::Connected, fsm.update(8000, true, true));
  TEST_ASSERT_EQUAL(WiFiSetupState::Connected, fsm.state());
}

void test_connection_while_portal_active_connects(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(false, 0);
  fsm.update(15000, false, false); // -> ApPortal

  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(100000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::Connected, fsm.update(100001, true, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::Connected, fsm.state());
}

void test_portal_timeout_requests_restart(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  fsm.begin(false, 0);
  fsm.update(15000, false, false); // -> ApPortal

  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(15000 + 299999, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupAction::Restart, fsm.update(15000 + 300000, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::Restarting, fsm.state());

  // Restart is requested only once.
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(15000 + 300001, false, false));
}

// millis() wraps every ~49.7 days; timeouts must survive the overflow.
void test_timeouts_survive_millis_wraparound(void)
{
  WiFiSetupStateMachine fsm(testConfig());
  const uint32_t start = 0xFFFFF000u; // 4096 ms before wrap

  fsm.begin(true, start);
  TEST_ASSERT_EQUAL(WiFiSetupAction::None, fsm.update(start + 5000u, false, false));
  TEST_ASSERT_EQUAL(WiFiSetupState::Connecting, fsm.state());
  TEST_ASSERT_EQUAL(WiFiSetupAction::EnterImprovWait, fsm.update(start + 10000u, false, false));
}

void run_wifi_setup_state_machine_tests(void)
{
  RUN_TEST(test_no_credentials_waits_for_improv_instead_of_starting_portal);
  RUN_TEST(test_no_credentials_starts_portal_after_grace_window);
  RUN_TEST(test_stored_credentials_connect_emits_connected_once);
  RUN_TEST(test_stored_credentials_timeout_falls_back_to_improv);
  RUN_TEST(test_improv_activity_extends_wait_window);
  RUN_TEST(test_improv_activity_during_connecting_carries_over);
  RUN_TEST(test_improv_provisioning_during_wait_connects);
  RUN_TEST(test_connection_while_portal_active_connects);
  RUN_TEST(test_portal_timeout_requests_restart);
  RUN_TEST(test_timeouts_survive_millis_wraparound);
}
