"""Exercise the bridge over stdio against a local HTTP clock fixture.

Run with: python -m unittest discover -s tools -p 'test_*.py'
Install tools/requirements-mcp.txt into the test environment first.
"""

import asyncio
import json
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import sys
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
