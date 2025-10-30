# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-10-27

### Added
- Initial release of SoftWire_CH32
- Wire-compatible API for software I2C
- Support for any GPIO pins as SDA/SCL
- Clock stretching support
- Configurable I2C speed via `setClock()`
- `exists()` method to check device presence
- Two example sketches:
  - I2C_Scanner: Scan for devices on the bus
  - I2C_ReadWrite: Read/write example with EEPROM
- Complete documentation (README.md, QUICK_START.md)
- Arduino IDE and PlatformIO compatibility
- Keywords.txt for syntax highlighting
- LGPL 2.1 license

### Features
- Master mode I2C communication
- 7-bit addressing
- Configurable delay for speed control
- Buffer size: 32 bytes (configurable)
- Clock stretching timeout protection
- Open-drain pin configuration with pull-ups

### Tested On
- CH32X035G8U6 (QFN28 package)
- Pins: PB6 (SDA), PB7 (SCL)
- Device: 24C32 EEPROM at 0x57

### Known Limitations
- Master mode only (no slave support)
- Blocking operation
- Software timing (CPU-dependent speed)
- Maximum buffer: 32 bytes

## [Unreleased]

### Planned Features
- Multi-master support
- 10-bit addressing
- Fast mode plus (1MHz) support
- Non-blocking operation option
- DMA support (if possible)
- Slave mode support

---

For more details, see the [README.md](README.md) file.
