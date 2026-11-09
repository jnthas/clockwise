# Clockwise MCP Server

Clockwise 固件内置的 MCP（Model Context Protocol）服务器，让 AI 助手
（Claude Desktop、Kimi、Cursor 等支持 MCP 的客户端）可以直接控制时钟：
改亮度、切时区、读光照传感器、重启设备等。

实现为 MCP Streamable HTTP 传输的嵌入式简化版：

- 端点：`POST http://<clock-ip>/mcp`（JSON-RPC 2.0，响应 `application/json`）
- 通知返回 `202 Accepted`（空响应体）
- `GET /mcp` 返回 `405`——本实现没有服务端主动推送，不开 SSE 流
- 每次响应带 `Mcp-Session-Id` 头（每次启动生成一次）

## 代码结构

| 文件 | 说明 |
|---|---|
| `firmware/lib/cw-commons/McpJsonRpc.h` | 平台无关的 MCP 核心（JSON-RPC + tools），仅依赖 ArduinoJson，可原生单元测试 |
| `firmware/lib/cw-commons/CWMcp.h` | 固件适配层：NVS 参数、Wi-Fi 状态、LDR、重启；HTTP 响应封装 |
| `firmware/lib/cw-commons/CWWebServer.h` | 路由 `POST /mcp`；HTTP 读取逻辑扩展为完整请求头 + Body（原实现只读请求行） |
| `firmware/src/main.cpp` | 注册时间与亮度的运行时钩子 |

## 工具清单

| 工具 | 说明 |
|---|---|
| `get_config` | 读取全部配置参数（数字返回 JSON number，其余 string） |
| `set_config` | 批量更新参数并写入 Flash，非法参数返回 `isError` |
| `set_brightness` | 设置亮度 0-255 并立即生效（自动亮度开启时可能被 LDR 覆盖） |
| `set_auto_brightness` | 设置 LDR 自动亮度量程 0-4095，`max=0` 关闭自动亮度 |
| `read_light_sensor` | 读取 LDR 原始 ADC 值 |
| `get_status` | 固件版本、表盘、Wi-Fi/IP/信号、堆内存、运行时间、当前时间 |
| `restart_device` | 重启设备（旋转/驱动/RGB 交换等参数改后需要） |

## 手动测试（curl）

```bash
IP=192.168.1.42   # 换成时钟的 IP

# 初始化
curl -s -X POST http://$IP/mcp \
  -H 'Content-Type: application/json' \
  -H 'Accept: application/json, text/event-stream' \
  -d '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"curl","version":"1.0"}}}'

# 工具列表
curl -s -X POST http://$IP/mcp -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":2,"method":"tools/list"}'

# 设置时区
curl -s -X POST http://$IP/mcp -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"set_config","arguments":{"params":{"timeZone":"Asia/Shanghai","manualPosix":"CST-8"}}}}'

# 立即调亮度
curl -s -X POST http://$IP/mcp -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"set_brightness","arguments":{"level":120}}}'

# 设备状态
curl -s -X POST http://$IP/mcp -H 'Content-Type: application/json' \
  -d '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"get_status","arguments":{}}}'
```

## 传输细节（重要）

ESP32 的 TCP **连接建立**在高频请求下不稳定（SYN 可能被静默丢弃，ICMP 正常），
但已建立的连接非常可靠。因此：

- `POST /mcp` 支持 HTTP/1.1 **keep-alive**：带 `Connection: keep-alive` 时连接保持
  打开，空闲 15 秒超时；其余端点仍按请求关闭
- MCP 客户端应通过 `tools/clockwise_mcp_bridge.py`（stdio MCP 服务器）连接，
  桥接内部用单条持久 TCP 连接顺序发送 JSON-RPC，空闲断线自动重连

## 在 MCP 客户端中配置

桥接依赖 Python 3.10+ 与 `mcp` 包（`pip install mcp`）。以 Claude Desktop 为例
（`claude_desktop_config.json`）：

```json
{
  "mcpServers": {
    "clockwise": {
      "command": "python",
      "args": [
        "C:\\path\\to\\clockwise\\tools\\clockwise_mcp_bridge.py",
        "http://192.168.31.119/mcp"
      ]
    }
  }
}
```

连接时钟后可直接对话控制，例如："把时钟亮度调到 120"、"现在几点了"（走
`get_status`）、"切换到 24 小时制"（走 `set_config`）。

## 构建与测试

```bash
# 原生单元测试（26 个用例，含 15 个 MCP 用例）
pio test -d firmware/ -e native

# 固件编译（先链接表盘，以马里奥为例）
cd firmware/lib && ln -sfn ../clockfaces/cw-cf-0x01 cw-cf-0x01 && cd ../..
pio run -d firmware/ -e esp32dev
```

注意：截至 2026-11，最新 `espressif32` 平台（8.0.0 / arduino-esp32 3.x）与
`PNGdec@1.0.1` 存在 ESP-DSP 头文件不兼容问题，本地编译需固定
`platform = espressif32@6.7.0`（CI 环境同样受影响，与本 MCP 改动无关）。
`upload_speed = 115200` 是为了兼容容易接触不良的 USB 数据线。

## 已知限制

- 无鉴权：任何能访问局域网的人都能控制时钟，请勿暴露在不可信网络中
- 请求体上限 2048 字节；总请求上限 4096 字节（超限返回 413）
- keep-alive 仅在 `/mcp` 端点提供；并发多个 TCP 客户端在 ESP32 上不可靠
  （连接建立阶段 SYN 易丢），请通过桥接的单连接顺序访问
- 表盘切换是编译期决定的，MCP 只能配置 Canvas 表盘的 `canvasFile`/`canvasServer`
- `wifiPwd` 通过 `get_config` 读取时返回掩码 `********`
