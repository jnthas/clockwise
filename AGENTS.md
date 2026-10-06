# AGENTS.md — Agent Developer Guide for Clockwise

This document provides essential architectural context, operational rules, and commands for AI agents working in this repository.

---

## 1. Project Overview & Community Resources

* **Project**: **Clockwise** — Open-source smart wall clock firmware.
* **Hardware**: Espressif ESP32 (WROOM/WROVER/Trinity/WiseShield-32) driving a 64×64 RGB LED Matrix (HUB75/HUB75E) using I2S DMA.
* **Framework**: Arduino-ESP32 framework built with **PlatformIO** (primary) and **ESP-IDF** (advanced).
* **Language**: C++11 / C++14.
* **Main Repository**: [https://github.com/jnthas/clockwise](https://github.com/jnthas/clockwise)
* **Community Context**: Always consider and cross-reference the [Wiki](https://github.com/jnthas/clockwise/wiki), open/closed **Issues**, and community **Pull Requests** when diagnosing problems or implementing solutions.

---

## 2. Codebase Layout

```text
clockwise/
├── firmware/
│   ├── src/main.cpp            # Entry point: setup() and loop()
│   ├── platformio.ini          # PlatformIO environments (esp32dev, native)
│   ├── lib/
│   │   ├── cw-commons/         # Core system services (WiFi, Time, Preferences, WebServer)
│   │   └── cw-gfx-engine/      # 2D game/graphics engine (Locator, EventBus, Sprites)
│   ├── clockfaces/             # Clockface git submodules (cw-cf-0x01 through cw-cf-0x07)
│   └── test/                   # Unity unit tests (test_native, test_embedded)
├── main/                       # ESP-IDF CMake wrapper and Kconfig.projbuild
├── components/                 # Git submodules for external libraries (DMA panel, ezTime, etc.)
├── docs/ARCHITECTURE.md        # Comprehensive technical architecture & design reference
├── RELEASE.md                  # Release procedure and guide
└── get-platformio.py           # PlatformIO installation script
```

---

## 3. Mandatory Operational Rules for Agents

These rules are strictly mandatory and must be followed on every task:

1. **Verification Rule (MANDATORY)**:
   * Always run the tests and build the firmware after finishing any change.
   * Never consider a task done without verifying compilation and test execution.
2. **Git & VCS Discipline (MANDATORY)**:
   * **Never commit or push anything unless explicitly ordered by the developer.**
   * For all changes, leave modified files uncommitted in the working tree for developer review and validation first.
   * When authorized to commit, work must be placed on a **separate branch** (never directly on `main`), and a **Pull Request targeting `main`** should be created.
3. **GitHub Issue & PR Integration**:
   * If a task originates from a GitHub Issue, integrate the fix with the Issue reference (e.g. `Fixes #...`).
   * Ensure changes are on a dedicated branch with a PR targeting `main`.
   * Prepare the PR description and draft comments for the Issue, but **the developer must explicitly review and approve before any branch is pushed, PR is created, or comment is posted.**
4. **Release Process**:
   * Releases must strictly follow the guide defined in [`RELEASE.md`](RELEASE.md).

---

## 4. Testing & Quality Assurance Policies

Agents must adhere to the following testing standards:

* **Bug Fixes (Test-Driven Fixes)**:
  * Whenever feasible, write a reproducing unit test first that fails before the fix.
  * Apply the bug fix and verify that the test now passes.
* **New Features**:
  * Check if it is possible to add unit tests covering the new feature logic.
* **Refactoring & Improvements**:
  * When improving or refactoring existing code that lacks test coverage, assess and add unit tests to protect against regressions.

---

## 5. Core Architectural Patterns

* **Modular Clockfaces (`IClockface`)**: Every theme implements `setup(CWDateTime*)` and `update()`. Clockfaces are decoupled submodules injected by symlinking the target clockface into `firmware/lib/`.
* **Service Locator (`Locator`)**: Use `Locator::getDisplay()` for `Adafruit_GFX*` and `Locator::getEventBus()` for `EventBus*`.
* **Persistent Settings (`ClockwiseParams`)**: Singleton wrapping ESP32 NVS `Preferences` under namespace `"clockwise"`. Access via `ClockwiseParams::getInstance()->load()` and `save()`.
* **Networking & Time**:
  * Provisioning: Improv WiFi over Serial, stored NVS credentials, or fallback AP (`WiFiManager`).
  * Time: Managed via `CWDateTime` (wrapping `ezTime`). Resolved POSIX timezones are cached in NVS to ensure instant, reliable local time at boot without relying on external lookups.
* **Canvas Theme Engine (`cw-cf-0x07`)**: Runtime JSON interpreter that dynamically renders remote/local themes and PNG images via `PNGdec`.

---

## 6. Key Commands & Environment Setup

All commands must be executed from the workspace root (`/home/jonathas/projects/clockwise`):

### What to do if `pio` is not installed or not in PATH
1. Check the standard PlatformIO virtual environment path first:
   ```bash
   ~/.platformio/penv/bin/pio --version
   ```
2. If not found, install PlatformIO using the included bootstrap script:
   ```bash
   python3 get-platformio.py
   ```
3. Or install/upgrade via pip:
   ```bash
   python3 -m pip install --upgrade platformio
   ```

### Run Native Unit Tests
```bash
pio test -d firmware/ -e native
# or via virtualenv:
~/.platformio/penv/bin/pio test -d firmware/ -e native
```

### Build Firmware for a Clockface (e.g. `cw-cf-0x01`)
Because clockfaces are injected as libraries, symlink the target clockface into `firmware/lib/` before compiling:
```bash
ln -sf ../clockfaces/cw-cf-0x01 firmware/lib/cw-cf-0x01
pio run -d firmware/ -e esp32dev
rm -f firmware/lib/cw-cf-0x01
```

---

## 7. Embedded Guidelines & Best Practices

1. **Memory Discipline**: ESP32 SRAM (~320 KB usable) is shared between FreeRTOS, WiFi stack, DMA framebuffers, and app code.
   * Avoid large stack allocations; use static or pooled buffers where possible.
   * Avoid heap allocations (`new`, `malloc`, dynamic `String` concatenation) inside `loop()` or high-frequency render routines to prevent heap fragmentation.
2. **Preserve Abstractions**:
   * Do not hardcode display pinouts in clockfaces; use `Locator::getDisplay()`.
   * Add new configuration options to `ClockwiseParams` (`CWPreferences.h`) and expose them cleanly in `CWWebServer.h` and `SettingsWebPage.h`.
3. **Deep Dive**: Refer to [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) for full hardware pinouts, sequence diagrams, and lifecycle specifics.
