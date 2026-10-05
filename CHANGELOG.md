# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]


## [1.4.4] - 2026-10-05

### Fixed

- Non-blocking Wi-Fi provisioning architecture (`WiFiSetupStateMachine`) to resolve Improv Wi-Fi serial starvation during ESP-Web-Tools installation.
- Prevented premature fallback to Access Point mode on clean firmware installations.
- Fixed missing clockface initialization when provisioning over Improv Wi-Fi.



## [1.4.3] - 2026-10-04

### Added

- Web configuration for Matrix Shift Driver (`FM6124`, `FM6126A`, `ICN2038S`, `MBI5124`, `DP3246`). Thanks @aschoelzhorn!
- Web configuration for I2S clock speed (`8MHz`, `16MHz`, `20MHz`). Thanks @aschoelzhorn!
- Web configuration for E Address Line Pin (default: GPIO 18). Thanks @aschoelzhorn!
- Web configuration to swap Blue and Red pins for BGR panels. Thanks @aschoelzhorn!
- Persistent NVS caching for POSIX timezone rules (`cachedPosix` and `cachedTz`) to ensure reliable local time on boot.
- Retry loop with backoff for remote timezone lookups.
- Technical architecture documentation (`docs/ARCHITECTURE.md`).
- Agent developer guidelines and operational standards (`AGENTS.md`).

### Changed

- Automatic Brightness Control (ABC) upgraded to 10 slots with hysteresis to prevent flickering. Thanks @yuan910715!

### Fixed

- Intermittent UTC time fallback when querying `timezoned.rop.nl:2342` over lossy UDP paths.
- WiFi AP configuration portal failing to start when SSID was empty on newly flashed devices. Thanks @aschoelzhorn!
- ESP-IDF build errors and matrix panel library dependencies.


## [1.4.2] - 2024-04-21

### Added 

- New Display Rotation param. Thanks @Xefan.
- Added ntp sync in main loop. Thanks @vandalon
- Add api documentation
- Pacman clockface: Add library.json to import using platformio
- Canvas: Feature/add sprite loop and frame delay. Thanks @robegamesios


## [1.4.1] - 2023-08-27

### Added 

- New Manual Posix param to avoid the `timezoned.rop.nl` ezTime's timezone service. Thanks @JeffWDH!

### Changed

- Set `time.google.com` as a default NTP server - `pool.ntp.org` is a slug

### Fixed

- A bug in the Canvas clockface - Commit a216c29c4f15b1b3cadbd89805d150c2f551562b


## [1.4.0] - 2023-07-01

### Added 

- Canvas clockface
- Created a method to make use of the ezTime formating string 
- Possibility to change Wifi user/pwd via API (must be connected)
- A helper to make HTTP requests

### Changed

- RGB icons used in the startup, it was replaced by one-bit images that reduce used flash


## [1.3.0] - 2023-06-11

### Added
- Configure the NTP Server
- Firmware version displayed on settings page
- LDR GPIO configuration
- Added a link in Settings page to read any pin on ESP32 (located in LDR Pin card)

### Changed

- [ABC] It's possible to turnoff the display if the LDR reading < minBright


## [1.2.0] - 2023-05-14

### Added

- Automatic bright control using LDR 
- Restart if offline for 5 minutes

### Fixed

- Clockface 0x06 (Pokedex): show AM PM only when is not using 24h format
- Restart endpoint returns HTTP 204 before restarting 


## [1.1.0] - 2023-04-02

### Added

- Implements Improv protocol to configure wifi
- Create a Settings Page where user can set up:
  - swap blue/green pins
  - use 24h format or not
  - timezone
  - display bright

### Removed

- Unused variables in main.cpp
