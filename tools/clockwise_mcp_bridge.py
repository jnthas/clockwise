"""Clockwise MCP stdio bridge.

Exposes the Clockwise wall clock's embedded MCP endpoint (HTTP POST /mcp)
as a stdio MCP server, so desktop MCP clients (Claude Desktop, Cursor, ...)
can control the clock directly.

Usage (client config):
    python clockwise_mcp_bridge.py http://192.168.31.119/mcp
"""

import json
import sys
import http.client
from urllib.parse import urlparse

from mcp.server.fastmcp import FastMCP

# Clock URL: default to the known clock on the LAN (DHCP reservation
# recommended so 192.168.31.119 stays stable).
CLOCK_URL = sys.argv[1] if len(sys.argv) > 1 else "http://192.168.31.119/mcp"
_url = urlparse(CLOCK_URL)
_conn: http.client.HTTPConnection | None = None
_req_id = 0


def _ensure_conn() -> http.client.HTTPConnection:
    """One persistent TCP connection to the clock: ESP32 connection
    establishment is the flaky part, so we never reconnect per request."""
    global _conn
    if _conn is None:
        _conn = http.client.HTTPConnection(_url.hostname, _url.port or 80, timeout=15)
    return _conn


def rpc(method: str, params: dict | None = None) -> dict | None:
    """One JSON-RPC round-trip to the clock over the persistent connection.
    Returns the MCP result, or None for notifications (HTTP 202)."""
    global _req_id
    _req_id += 1
    payload = {"jsonrpc": "2.0", "id": _req_id, "method": method}
    if params is not None:
        payload["params"] = params
    data = json.dumps(payload)

    for attempt in (1, 2):
        try:
            conn = _ensure_conn()
            conn.request("POST", _url.path or "/mcp", body=data, headers={
                "Content-Type": "application/json",
                "Accept": "application/json, text/event-stream",
                "Connection": "keep-alive",
            })
            resp = conn.getresponse()
            body = resp.read()
            if resp.status == 202:
                return None
            if resp.status != 200:
                raise RuntimeError(f"Clock HTTP {resp.status}")
            result = json.loads(body.decode())
            if "error" in result:
                raise RuntimeError(f"Clock RPC error: {result['error']}")
            return result["result"]
        except (OSError, http.client.HTTPException):
            # Idle keep-alive connections expire server-side; transparently
            # reconnect once and retry.
            global _conn
            if _conn:
                _conn.close()
            _conn = None
            if attempt == 2:
                raise


def call_tool(name: str, arguments: dict) -> str:
    result = rpc("tools/call", {"name": name, "arguments": arguments})
    text = "\n".join(c["text"] for c in result["content"] if c["type"] == "text")
    if result.get("isError"):
        raise RuntimeError(text)
    return text


# Discover the clock's tool catalog at startup and mirror it 1:1.
init = rpc("initialize", {"protocolVersion": "2025-03-26", "capabilities": {}})
tools = rpc("tools/list", {})["tools"]

mcp = FastMCP("clockwise", instructions=f"Clockwise wall clock @ {CLOCK_URL}")

for tool in tools:
    props = tool["inputSchema"].get("properties", {})

    def make_handler(tool_name, prop_names):
        def handler(**kwargs) -> str:
            return call_tool(tool_name, {k: v for k, v in kwargs.items() if v is not None})
        # Explicit keyword-only signature (all optional) so FastMCP accepts
        # empty argument dicts instead of demanding a "kwargs" field.
        import inspect
        sig_params = [
            inspect.Parameter(n, inspect.Parameter.KEYWORD_ONLY, default=None, annotation=object)
            for n in prop_names
        ]
        handler.__signature__ = inspect.Signature(parameters=sig_params)
        return handler

    # Parameter schemas stay with the clock (see each tool's description);
    # provided kwargs are forwarded 1:1 to the firmware.
    mcp.add_tool(
        make_handler(tool["name"], list(props.keys())),
        name=tool["name"],
        description=tool["description"],
    )

if __name__ == "__main__":
    mcp.run()  # stdio transport
