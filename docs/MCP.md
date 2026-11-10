# Clockwise MCP Server

Clockwise exposes configuration, brightness controls, device status, and restart
through a small Model Context Protocol (MCP) server. Desktop MCP clients can use
the supplied Python stdio bridge to communicate with the clock over local HTTP.

This is an embedded implementation of a limited MCP tools interface, not a claim
of full MCP or Streamable HTTP conformance. The endpoint is included in the
firmware in this branch and is available whenever the existing web server runs;
there is currently no separate build flag or setting to disable MCP.

## Connect a desktop client

Use Python 3.10 or newer. From the repository root, create a virtual environment
and install the bridge's tested dependency version:

```bash
python3 -m venv .venv-mcp
.venv-mcp/bin/python -m pip install -r tools/requirements-mcp.txt
```

On Windows, use `.venv-mcp\Scripts\python.exe` instead. The requirements file pins
`mcp==1.30.0` because the bridge uses the v1 `FastMCP` API.

The clock URL is required; the bridge does not assume a device address. Replace
`<clock-ip>` with the address shown by your clock or router:

```bash
.venv-mcp/bin/python tools/clockwise_mcp_bridge.py 'http://<clock-ip>/mcp'
```

The bridge uses stdin and stdout for MCP messages. Normally, a desktop client
launches it rather than a person interacting with it in a terminal.

For example, add this entry to a client that accepts `mcpServers` configuration,
such as Claude Desktop. Replace both filesystem paths with absolute paths for
your checkout and the Python environment used above, and replace `<clock-ip>`:

```json
{
  "mcpServers": {
    "clockwise": {
      "command": "/absolute/path/to/clockwise/.venv-mcp/bin/python",
      "args": [
        "/absolute/path/to/clockwise/tools/clockwise_mcp_bridge.py",
        "http://<clock-ip>/mcp"
      ]
    }
  }
}
```

On Windows, use the corresponding absolute paths with escaped backslashes in
JSON. The client must use the same Python environment in which the requirements
were installed. The bridge currently accepts plain `http://` URLs only.

Once connected, try requests such as “set the clock brightness to 120” or “read
the light sensor.” Configuration changes that require a reboot should be followed
by `restart_device`, as described below.

## Tools

Tool results contain text, usually a serialized JSON object. Invalid tool
arguments can produce a JSON-RPC error or a tool result with `isError: true`.

| Tool | Arguments and behavior |
|---|---|
| `get_config` | Reads the user-facing configuration. Numeric-looking values are returned as JSON numbers, and other values as strings. A configured `wifiPwd` is returned as `********`; an empty password remains empty. |
| `set_config` | Takes `params`, an object whose values are strings. Validates the batch before saving it to flash. Unknown keys, invalid values, or an invalid automatic-brightness range reject the batch and restore the persisted configuration. |
| `set_brightness` | Takes integer `level` from `0` to `255`, saves it, and applies it immediately. An enabled automatic-brightness loop can override the displayed level. |
| `set_auto_brightness` | Takes integer `min` and `max` from `0` to `4095`. An enabled range requires `max > min`. Setting `max` to `0` disables automatic brightness and restores the configured manual brightness. Changes take effect without rebooting. |
| `read_light_sensor` | Reads the LDR pin's raw ADC value. Requires a correctly connected and configured sensor. |
| `get_status` | Reads firmware and clockface identifiers, Wi-Fi status, network details when connected, free heap, local time, and sensor data. |
| `restart_device` | Writes the success response first, then schedules a restart after a 250 ms response-drain grace period. |

`set_config` accepts these keys:

- Brightness and sensor: `displayBright`, `autoBrightMin`, `autoBrightMax`, `ldrPin`.
- Time: `timeZone`, `manualPosix`, `ntpServer`, `use24hFormat`.
- Display hardware: `displayRotation`, `swapBlueGreen`, `swapBlueRed`, `driver`,
  `i2cSpeed`, `E_pin`.
- Network and Canvas: `wifiSsid`, `wifiPwd`, `canvasFile`, `canvasServer`.

`displayRotation` uses the existing firmware values `"0"`, `"1"`, `"2"`, and
`"3"`, corresponding to 0, 90, 180, and 270 degrees. Boolean settings can use
`"0"` or `"1"`. For example:

```json
{
  "params": {
    "displayRotation": "1",
    "use24hFormat": "1"
  }
}
```

Saving a setting is distinct from applying it to an initialized subsystem.
Restart after changing time zone, manual POSIX rules, NTP server, time format,
Wi-Fi credentials, rotation, RGB mapping, driver, I2S speed, or E-pin. The current
MCP adapter refreshes brightness and automatic-brightness state at runtime; it
does not reinitialize the time, network, or display subsystems for other settings.
Clockface selection remains a firmware build choice. `canvasFile` and
`canvasServer` configure the Canvas clockface, not the compiled clockface type.

## Direct HTTP examples

The stdio bridge is the recommended desktop-client path. For debugging, send
individual JSON-RPC requests to the clock. Replace the placeholder before running
these commands:

```bash
CLOCK_URL='http://<clock-ip>/mcp'

# Initialize.
curl -s -X POST "$CLOCK_URL" \
  -H 'Content-Type: application/json' \
  -H 'Accept: application/json, text/event-stream' \
  -d '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"curl","version":"1.0"}}}'

# List available tools.
curl -s -X POST "$CLOCK_URL" -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":2,"method":"tools/list"}'

# Save time-zone settings. Restart the clock to apply them.
curl -s -X POST "$CLOCK_URL" -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"set_config","arguments":{"params":{"timeZone":"Asia/Shanghai","manualPosix":"CST-8"}}}}'

# Apply manual brightness immediately.
curl -s -X POST "$CLOCK_URL" -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"set_brightness","arguments":{"level":120}}}'

# Read device status.
curl -s -X POST "$CLOCK_URL" -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"get_status","arguments":{}}}'
```

## Transport and limits

- Endpoint: `POST http://<clock-ip>/mcp`, using JSON-RPC 2.0 request bodies and
  `application/json` responses. The bridge initializes with protocol version
  `2025-03-26`.
- Notifications receive `202 Accepted` with an empty body. `GET /mcp` returns
  `405 Method Not Allowed`; there is no SSE stream or server-initiated messaging.
- Responses include a per-boot `Mcp-Session-Id` header. This value is informational;
  incoming session IDs are not used to enforce session isolation.
- The handler accepts one JSON-RPC message per HTTP request. JSON-RPC batch
  arrays, chunked request bodies, resources, and prompts are not implemented.
- `Connection: keep-alive` is honored for `POST /mcp`. The bridge reuses one
  connection and reconnects after an idle disconnect. Other endpoints close
  after their response.
- The web server services one active TCP connection at a time. Other clients may
  have to wait until that connection closes or expires.
- Idle MCP connections expire after 15 seconds. A new connection or a request
  that has started arriving has a 1.5-second read timeout. Timeout handling uses
  elapsed-time arithmetic that supports `millis()` wraparound.
- Each main-loop pass reads at most 256 bytes and dispatches at most one request.
  Waiting for input or connection closure does not hold the main loop, so clock
  rendering and automatic brightness can continue.
- The default body limit is 2048 bytes and the total request limit is 4096 bytes.
  Oversized requests receive HTTP `413`; invalid framing receives HTTP `400`.
  Requests that time out are closed.

## Security and resource considerations

The firmware endpoint has no authentication or TLS. Anyone who can reach the
clock's web server can read configuration, change settings, or restart it. Use
it only on a trusted local network and do not expose it directly to the internet.
Masking the Wi-Fi password in `get_config` does not provide access control or
protect HTTP traffic in transit.

Configuration updates write ESP32 NVS flash. Use them for settings changes,
not for high-frequency animation or continuous sensor-control loops. Request
limits bound accepted HTTP input, but parsing and response serialization still
require heap memory. The implementation also disables Wi-Fi modem sleep after
the network becomes ready; power consumption and network behavior should be
checked on the intended hardware.

## Implementation layout

| File | Responsibility |
|---|---|
| `firmware/lib/cw-commons/McpJsonRpc.h` | Platform-independent JSON-RPC dispatcher, tool schemas, and tool results; depends on ArduinoJson. |
| `firmware/lib/cw-commons/McpConfig.h` | Configuration parsing, batch updates, rollback, and range validation. |
| `firmware/lib/cw-commons/CWMcp.h` | ESP32 device adapter, runtime hooks, restart flag, and HTTP response writer. |
| `firmware/lib/cw-commons/HttpRequestReader.h` | Incremental HTTP request framing and connection lifecycle. |
| `firmware/lib/cw-commons/CWWebServer.h` | Routes requests to the existing settings API or MCP handler. |
| `firmware/src/main.cpp` | Registers time and brightness hooks and services HTTP alongside the clockface. |
| `tools/clockwise_mcp_bridge.py` | Discovers the clock's tools and exposes them over stdio through the Python MCP SDK. |

## Build and test

Run these commands from the repository root. Install PlatformIO using the
project's normal setup instructions.

```bash
# Native regression tests: JSON-RPC, configuration batches, HTTP, and Wi-Fi setup.
pio test -d firmware/ -e native

# Bridge integration tests: a local HTTP fixture and a real stdio MCP client.
.venv-mcp/bin/python -m unittest discover -s tools -p 'test_*.py' -v

# Initialize the Mario clockface used for this build example.
git submodule update --init firmware/clockfaces/cw-cf-0x01

# Create a temporary library link without replacing an existing file or link.
(
  set -eu
  CLOCKFACE_LINK=firmware/lib/cw-cf-0x01
  if [ -e "$CLOCKFACE_LINK" ] || [ -L "$CLOCKFACE_LINK" ]; then
    echo "Choose a clean clockface library path before building." >&2
    exit 1
  fi
  ln -s ../clockfaces/cw-cf-0x01 "$CLOCKFACE_LINK"
  trap 'rm -f "$CLOCKFACE_LINK"' EXIT
  pio run -d firmware/ -e esp32dev
)
```

The current PlatformIO configuration pins `espressif32@6.7.0` and sets the upload
speed to `115200`. These are build/upload configuration choices in this branch,
not MCP protocol requirements.

The pre-submission validation so far includes 43 native tests, bridge integration
tests, and an `esp32dev` build with the Mario clockface. This does not establish
compatibility with every clockface or board. Device validation is still pending:
check continuous rendering while connected, brightness controls and the LDR,
idle reconnection, and receipt of the restart response before rebooting.
