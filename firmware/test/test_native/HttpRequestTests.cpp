#include "unity.h"
#include <memory>
#include <string>
#include "../../lib/cw-commons/HttpRequestReader.h"

namespace {
struct FakeSocket
{
  std::string incoming;
  std::string outgoing;
  size_t consumed = 0;
  unsigned reads = 0;
  bool connected = true;
};

// Copies share the same socket, like WiFiClient.
struct FakeClient
{
  std::shared_ptr<FakeSocket> socket;
  FakeClient() : socket(new FakeSocket) {}
  int available() { return (int)(socket->incoming.size() - socket->consumed); }
  int read() { ++socket->reads; return available() ? (unsigned char)socket->incoming[socket->consumed++] : -1; }
  bool connected() { return socket->connected; }
  void stop() { socket->connected = false; }
  void print(const char* text) { socket->outgoing += text; }
};

const std::string ping = "POST /mcp HTTP/1.1\r\nContent-Length: 2\r\nConnection: keep-alive\r\n\r\n{}";

void test_keep_alive_idle_yields_to_clock_loop()
{
  FakeClient client;
  client.socket->incoming = ping;
  HttpRequestSession<FakeClient> session;
  session.accept(client, 0);
  unsigned responses = 0;
  unsigned clockTicks = 0;
  const auto respond = [&](FakeClient& peer, const HttpRequestReader& request) {
    TEST_ASSERT_EQUAL_STRING("{}", request.request().substr(request.headerEnd()).c_str());
    ++responses;
    peer.print("response");
    return true;
  };
  session.poll(0, respond);
  const unsigned readsAfterRequest = client.socket->reads;
  for (uint32_t now = 1; now < 15000; ++now)
  {
    session.poll(now, respond);
    ++clockTicks; // loop work continues while the same socket is idle.
  }
  TEST_ASSERT_EQUAL(14999, clockTicks);
  TEST_ASSERT_EQUAL(1, responses);
  TEST_ASSERT_EQUAL(readsAfterRequest, client.socket->reads);
  TEST_ASSERT_TRUE(session.active());
  session.poll(15000, respond);
  TEST_ASSERT_FALSE(session.active());
}

void test_segmented_body_dispatches_only_when_complete()
{
  FakeClient client;
  client.socket->incoming = "POST /mcp HTTP/1.1\r\ncOnTeNt-LeNgTh: 7\r\n\r\n{\"x\":";
  HttpRequestSession<FakeClient> session;
  session.accept(client, 10);
  unsigned responses = 0;
  const auto respond = [&](FakeClient&, const HttpRequestReader& request) {
    ++responses;
    TEST_ASSERT_EQUAL_STRING("{\"x\":1}", request.request().substr(request.headerEnd()).c_str());
    return false;
  };
  session.poll(10, respond);
  TEST_ASSERT_EQUAL(0, responses);
  client.socket->incoming += "1}";
  session.poll(11, respond);
  TEST_ASSERT_EQUAL(1, responses);
  session.poll(310, respond);
  TEST_ASSERT_TRUE(session.active());
  session.poll(311, respond);
  TEST_ASSERT_FALSE(session.active());
}

void test_pipelined_requests_dispatch_once_per_poll()
{
  FakeClient client;
  client.socket->incoming = ping + ping;
  HttpRequestSession<FakeClient> session;
  session.accept(client, 0);
  unsigned responses = 0;
  const auto respond = [&](FakeClient&, const HttpRequestReader&) { ++responses; return true; };
  session.poll(0, respond);
  TEST_ASSERT_EQUAL(1, responses);
  TEST_ASSERT_EQUAL(ping.size(), client.socket->consumed);
  session.poll(1, respond);
  TEST_ASSERT_EQUAL(2, responses);
}

void test_request_read_budget_bounds_work_per_poll()
{
  FakeClient client;
  client.socket->incoming = "POST /mcp HTTP/1.1\r\nContent-Length: 1000\r\n\r\n" + std::string(1000, 'x');
  HttpRequestSession<FakeClient> session;
  session.accept(client, 0);
  unsigned responses = 0;
  const auto respond = [&](FakeClient&, const HttpRequestReader&) { ++responses; return true; };
  session.poll(0, respond);
  TEST_ASSERT_EQUAL(256, client.socket->reads);
  TEST_ASSERT_EQUAL(0, responses);
  for (uint32_t now = 1; now < 5; ++now)
    session.poll(now, respond);
  TEST_ASSERT_EQUAL(1, responses);
}

void test_partial_request_timeout_survives_millis_wrap()
{
  FakeClient client;
  client.socket->incoming = "POST /mcp HTTP/1.1\r\n";
  HttpRequestSession<FakeClient> session;
  const uint32_t start = UINT32_MAX - 100;
  session.accept(client, start);
  const auto respond = [](FakeClient&, const HttpRequestReader&) { TEST_FAIL_MESSAGE("partial request dispatched"); return false; };
  session.poll(start, respond);
  session.poll(start + 1499U, respond);
  TEST_ASSERT_TRUE(session.active());
  session.poll(start + 1500U, respond);
  TEST_ASSERT_FALSE(session.active());
}

void test_oversized_body_rejected_before_allocation()
{
  FakeClient client;
  client.socket->incoming = "POST /mcp HTTP/1.1\r\nContent-Length: 2049\r\n\r\n" + std::string(2049, 'x');
  HttpRequestSession<FakeClient> session;
  session.accept(client, 0);
  session.poll(0, [](FakeClient&, const HttpRequestReader&) { TEST_FAIL_MESSAGE("oversized request dispatched"); return false; });
  TEST_ASSERT_NOT_EQUAL(std::string::npos, client.socket->outgoing.find("413"));
  TEST_ASSERT_LESS_THAN(100, client.socket->consumed);
}

void test_negative_length_is_bad_request()
{
  FakeClient client;
  client.socket->incoming = "POST /mcp HTTP/1.1\r\nContent-Length: -1\r\n\r\n";
  HttpRequestSession<FakeClient> session;
  session.accept(client, 0);
  session.poll(0, [](FakeClient&, const HttpRequestReader&) { TEST_FAIL_MESSAGE("negative length dispatched"); return false; });
  TEST_ASSERT_NOT_EQUAL(std::string::npos, client.socket->outgoing.find("400"));
}

void test_restart_waits_until_after_response_and_yields()
{
  FakeClient client;
  client.socket->incoming = ping + ping;
  HttpRequestSession<FakeClient> session;
  const uint32_t start = UINT32_MAX - 100;
  session.accept(client, start);
  unsigned responses = 0;
  session.poll(start, [&](FakeClient& peer, const HttpRequestReader&) {
    ++responses;
    TEST_ASSERT_FALSE(session.restarting());
    peer.print("HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\n{}");
    session.requestRestart(start); // The firmware consumes MCP's flag here.
    return true;
  });
  TEST_ASSERT_TRUE(session.restarting());
  TEST_ASSERT_NOT_EQUAL(std::string::npos, client.socket->outgoing.find("200 OK"));
  TEST_ASSERT_FALSE(session.restartReady(start + 249U));
  session.poll(start + 100U, [&](FakeClient&, const HttpRequestReader&) { ++responses; return true; });
  TEST_ASSERT_EQUAL(1, responses); // No retried/pipelined tools during reboot grace.
  TEST_ASSERT_TRUE(session.restartReady(start + 250U));
  TEST_ASSERT_TRUE(client.connected()); // Response can drain before close/reboot.
}
}

void run_http_request_tests()
{
  RUN_TEST(test_keep_alive_idle_yields_to_clock_loop);
  RUN_TEST(test_segmented_body_dispatches_only_when_complete);
  RUN_TEST(test_pipelined_requests_dispatch_once_per_poll);
  RUN_TEST(test_request_read_budget_bounds_work_per_poll);
  RUN_TEST(test_partial_request_timeout_survives_millis_wrap);
  RUN_TEST(test_oversized_body_rejected_before_allocation);
  RUN_TEST(test_negative_length_is_bad_request);
  RUN_TEST(test_restart_waits_until_after_response_and_yields);
}
