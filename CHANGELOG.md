# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Changed

- Avoid use extensive of String in CWWebServer.cpp


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
