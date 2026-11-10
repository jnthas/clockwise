"""Exercise the bridge over stdio against a local HTTP clock fixture.

Run with: python -m unittest discover -s tools -p 'test_*.py'
Install tools/requirements-mcp.txt into the test environment first.
"""

import asyncio
import json
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import sys
import subprocess
import threading
import unittest

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client


class ClockHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def log_message(self, *_args):
        pass

    def do_POST(self):
        request = json.loads(self.rfile.read(int(self.headers["Content-Length"])))
        self.server.requests.append(request)
        method = request["method"]
        if method == "initialize":
            result = {
                "protocolVersion": "2025-03-26",
                "capabilities": {"tools": {}},
                "serverInfo": {"name": "clock-fixture", "version": "1.0"},
            }
        elif method == "tools/list":
            result = {"tools": [
                {"name": "get_status", "description": "Read status", "inputSchema": {
                    "type": "object", "properties": {},
                }},
                {"name": "set_brightness", "description": "Set brightness", "inputSchema": {
                    "type": "object", "properties": {"level": {"type": "integer"}},
                    "required": ["level"],
                }},
            ]}
        elif method == "tools/call":
            arguments = request["params"]["arguments"]
            result = {"content": [{"type": "text", "text": json.dumps(arguments)}]}
        else:
            self.send_error(400)
            return
        body = json.dumps({"jsonrpc": "2.0", "id": request["id"], "result": result}).encode()
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


class BridgeTests(unittest.TestCase):
    def run_cli_without_network(self, *arguments):
        # Fail on any connect attempt, including ones to an accidental default
        # device, rather than relying on an unreachable address or a timeout.
        probe = """
import runpy, socket, sys
def reject_connect(*args, **kwargs):
    raise AssertionError('UNEXPECTED_NETWORK_CONNECTION')
socket.socket.connect = reject_connect
sys.argv = sys.argv[1:]
runpy.run_path(sys.argv[0], run_name='__main__')
"""
        return subprocess.run(
            [sys.executable, "-c", probe,
             str(Path(__file__).with_name("clockwise_mcp_bridge.py")), *arguments],
            capture_output=True, text=True, timeout=5,
        )

    def test_missing_clock_url_prints_usage_without_connecting(self):
        result = self.run_cli_without_network()
        self.assertEqual(result.returncode, 2)
        self.assertIn("usage:", result.stderr)
        self.assertIn("clock_url", result.stderr)
        self.assertNotIn("UNEXPECTED_NETWORK_CONNECTION", result.stderr)

    def test_help_succeeds_without_connecting(self):
        result = self.run_cli_without_network("--help")
        self.assertEqual(result.returncode, 0)
        self.assertIn("http://<clock-ip>/mcp", result.stdout)
        self.assertNotIn("UNEXPECTED_NETWORK_CONNECTION", result.stderr)

    def test_invalid_urls_are_rejected_before_connecting(self):
        for url in (
            "https://clock.local/mcp", "clock.local/mcp", "http:///mcp",
            "http://clock.local:abc/mcp", "http://clock.local:65536/mcp",
            "http://clock.local:0/mcp", "http://bad host/mcp",
            "http://user:password@clock.local/mcp", "http://clock.local/mcp?token=x",
            "http://clock.local/mcp#fragment",
        ):
            with self.subTest(url=url):
                result = self.run_cli_without_network(url)
                self.assertEqual(result.returncode, 2)
                self.assertIn("error:", result.stderr)
                self.assertNotIn("UNEXPECTED_NETWORK_CONNECTION", result.stderr)

    def test_supported_dependency_starts_and_forwards_tools_over_stdio(self):
        server = ThreadingHTTPServer(("127.0.0.1", 0), ClockHandler)
        server.requests = []
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            asyncio.run(asyncio.wait_for(self.exercise_bridge(server.server_port), timeout=20))
            calls = [r["params"] for r in server.requests if r["method"] == "tools/call"]
            self.assertEqual(calls, [
                {"name": "get_status", "arguments": {}},
                {"name": "set_brightness", "arguments": {"level": 120}},
            ])
        finally:
            server.shutdown()
            server.server_close()
            thread.join(timeout=5)

    async def exercise_bridge(self, port):
        params = StdioServerParameters(
            command=sys.executable,
            args=[str(Path(__file__).with_name("clockwise_mcp_bridge.py")),
                  f"http://127.0.0.1:{port}/mcp"],
        )
        async with stdio_client(params) as (read, write):
            async with ClientSession(read, write) as session:
                await session.initialize()
                catalog = await session.list_tools()
                self.assertEqual({t.name for t in catalog.tools}, {"get_status", "set_brightness"})
                status = await session.call_tool("get_status", {})
                self.assertFalse(status.isError)
                self.assertEqual(json.loads(status.content[0].text), {})
                brightness = await session.call_tool("set_brightness", {"level": 120})
                self.assertFalse(brightness.isError)
                self.assertEqual(json.loads(brightness.content[0].text), {"level": 120})


if __name__ == "__main__":
    unittest.main()
