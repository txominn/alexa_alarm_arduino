# Changelog
All notable changes to the **Alexa Alarm Virtual Remote** project will be documented in this file.

## [2.1] - 2026-03-08
### Added
- **GitHub Repository**: Project archiving and version control ([txominn/alexa_alarm_arduino](https://github.com/txominn/alexa_alarm_arduino)).
- **Connection Layout**: Created `layout.png` diagram for clear hardware wiring instructions.

## [2.0] - 2025-02-04
### Added
- **Cloud synchronization protection**: 20-second connection stabilization delay to prevent false triggers during startup.
- **Momentary switch emulation**: Automatic high-to-low pulse of 1000ms for all action pins.
- **Action cooldown**: 3-second safety window between consecutive commands.
- **Status LED feedback**: Visual connection status via `LED_BUILTIN` (blinking = disconnected, LOW = stable).
- **Conditional Debugging**: Detailed serial logging controlled via `#define DEBUG`.

## [1.1] - 2022-09-26
### Changed
- Reverted to single input for arm/disarm (1000ms duration).

## [1.0] - 2022-08-18
### Added
- Double input logic for arm/disarm (600ms).
- Connection status verification on startup.

## [0.1] - 2022-07-14
### Added
- Initial project release and core logic.

---
**Author:** txominn ([df.antonacci@gmail.com](mailto:df.antonacci@gmail.com))  
**License:** Public Domain