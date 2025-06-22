# Clockwise Troubleshooting Guide

Welcome to the Clockwise troubleshooting guide! This document aims to help you solve common problems you might encounter while setting up or using the Clockwise smart clock and its clockfaces.

Whether you're new to embedded projects or this is your first ESP32 project, this guide is designed to be approachable and straightforward.

---

## Table of Contents

1. Hardware Setup
2. Software Installation and Building
3. Wi-Fi and Network Issues
4. Display and Clockface Problems
5. Time and Timezone Issues
6. Common Build Errors
7. Firmware and Features
8. Additional Resources and Help

---

## 1. Hardware Setup

- **Correct Wiring for HUB75 Panels:**
  Ensure the panel pins are connected as per your board and the HUB75 specs. Mixing pins can cause the display to overlay or not show correctly.

- **Compatibility:**
  Some LED matrix panels using certain driver chips (e.g., FM6126A) might not be supported fully. Check if your panel is supported before building.


## 2. Software Installation and Building

- **Build Errors Related to Missing Files:**
  - Error: `fatal error: Clockface.h: No such file or directory` means either the source files are incomplete or paths are incorrect.
  - Ensure you clone all submodules and dependencies correctly.

- **Undefined Reference Errors:**
  - Issues compiling with errors like linked undefined symbols (e.g., SSL references) often stem from missing dependencies or incompatible ESP-IDF versions.
  - Check that your development environment meets the required versions for ESP-IDF and PlatformIO.

- **PlatformIO vs Arduino IDE:**
  - The project mainly supports PlatformIO. Arduino IDE compatibility might be limited or require specific setup.


## 3. Wi-Fi and Network Issues

- **Connection Fails or Disconnects:**
  - Double-check Wi-Fi SSID and password.
  - Some enterprise Wi-Fi networks require special authentication (e.g., PEAP) which may not be supported.

- **NTP Service Abnormalities:**
  - If the system crashes during time synchronization (NTP), check your NTP server settings.
  - Avoid using unreachable or malformed NTP servers.


## 4. Display and Clockface Problems

- **Display Shows Incorrect Images or Partial Glitches:**
  - Check wiring again.
  - Ensure correct configuration for panel type and wiring in the configuration files.

- **Specific Clockface Issues:**
  - e.g., the Pacman clockface time runs slow; this might be a known issue documented separately.

- **Adding New Clockfaces:**
  - Follow the repository guidelines and ensure additional resources like PNG decoding libraries are included.


## 5. Time and Timezone Issues

- **Timezone Incorrect Despite Configuration:**
  - Verify the timezone string and NTP server settings.
  - Some regions may need specific settings or offsets.
  
  **Important Note:** Some users experience incorrect time even with correct NTP configuration due to issues with the timezoned.rop.nl service used by the ezTime library. More details at https://github.com/ropg/ezTime?tab=readme-ov-file#timezonedropnl

- **No Core Dump Partition Found:**
  - Add or disable core dump partitions in the partition table as applicable.

## 6. Common Build Errors

- **Task Watchdog Triggered:**
  - This occurs when certain tasks block for too long.
  - Suggest looking at logs to pinpoint blocking function and optimizing code.

- **Linking Failures:**
  - Often caused by missing sources or incorrect library links.


## 7. Firmware and Features

- **Web Interface Customizations:**
  - Changing themes or enabling features like OTA updates requires extra development.

- **Feature Requests:**
  - Integration with Home Assistant, audio visualization, and others were discussed but it will require extra development.


## 8. Additional Resources and Help

- Visit the [GitHub Issues page](https://github.com/jnthas/clockwise/issues) for the most up-to-date discussions.
- Take a look at [README.md](https://github.com/jnthas/clockwise/blob/main/README.md) which explains the wiring, parameters and building.
- Read the source code comments and examples to deepen understanding.

---

*Last update: June 2025*