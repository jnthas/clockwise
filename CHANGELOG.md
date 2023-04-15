# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.1.1] - 2023-??-??

### Added

- Parameter to enable/disable LDR
- Read data from LDR and change the display bright according to it 

### Changed

- Avoid use extensive of String in CWWebServer.cpp
- Add a flag to restart the device instead of just restart it

### Fixed

- Clockface 0x05 (Pacman): fixed weekday was overlapping the boundaries


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
