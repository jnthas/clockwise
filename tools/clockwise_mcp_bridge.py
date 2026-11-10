"""Clockwise MCP stdio bridge.

Exposes the Clockwise wall clock's embedded MCP endpoint (HTTP POST /mcp)
as a stdio MCP server, so desktop MCP clients (Claude Desktop, Cursor, ...)
can control the clock directly.

Usage (client config):
    python clockwise_mcp_bridge.py "http://<clock-ip>/mcp"
"""

import argparse
import ipaddress
import json
import http.client
import re
from urllib.parse import urlparse


def clock_url(value: str):
    """Validate the HTTP endpoint before loading the SDK or opening a socket."""
    try:
        parsed = urlparse(value)
        host = parsed.hostname
        port = parsed.port
        if parsed.scheme != "http" or not host:
            raise ValueError("expected http://<clock-ip>/mcp")
        if port is not None and not 1 <= port <= 65535:
            raise ValueError("port must be between 1 and 65535")
        if parsed.username is not None or parsed.password is not None:
            raise ValueError("URL credentials are not supported")
        if parsed.query or parsed.fragment:
            raise ValueError("URL queries and fragments are not supported")
        try:
            ipaddress.ip_address(host)
        except ValueError:
            ascii_host = host.encode("idna").decode("ascii").rstrip(".")
            if len(ascii_host) > 253 or not all(
                re.fullmatch(r"[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?", label)
                for label in ascii_host.split(".")
            ):
                raise ValueError("invalid clock hostname")
        return parsed
    except (ValueError, UnicodeError) as error:
        raise argparse.ArgumentTypeError(str(error)) from error


parser = argparse.ArgumentParser(description="Connect a stdio MCP client to a Clockwise HTTP endpoint.")
parser.add_argument("clock_url", type=clock_url, metavar="clock_url",
                    help="clock endpoint, e.g. http://<clock-ip>/mcp (required; no default device)")
_url = parser.parse_args().clock_url
CLOCK_URL = _url.geturl()

from mcp.server.fastmcp import FastMCP

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
