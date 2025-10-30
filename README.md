# SoftWire_CH32

A Wire-compatible software I2C (bit-banging) library for CH32 series microcontrollers.

<p align="center">
  <a href="https://www.paypal.com/paypalme/jobitjoseph">
    <img src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif" alt="Donate with PayPal" />
  </a>
</p>

## Overview

SoftWire_CH32 allows you to use any GPIO pins for I2C communication on CH32 microcontrollers. This is particularly useful when:
- Hardware I2C pins conflict with other peripherals (e.g., USB on CH32X035)
- You need multiple I2C buses
- Hardware I2C pins are not available on your package variant
- You want to use non-standard pins for I2C

## Features

- ✅ **Wire-compatible API** - Drop-in replacement for the standard Wire library
- ✅ **Any GPIO pins** - Use any available pins for SDA and SCL
- ✅ **Clock stretching support** - Compatible with devices that use clock stretching
- ✅ **Configurable speed** - Adjust I2C clock speed via `setClock()`
- ✅ **Arduino IDE & PlatformIO** - Works with both development environments
- ✅ **Multiple instances** - Create multiple software I2C buses

## Compatibility

- **Tested on**: CH32X035G8U6 (QFN28)
- **Should work on**: All CH32 series microcontrollers (CH32V, CH32X, etc.)

## Installation

### Arduino IDE

1. Download this repository as a ZIP file
2. In Arduino IDE: `Sketch` → `Include Library` → `Add .ZIP Library...`
3. Select the downloaded ZIP file
4. Restart Arduino IDE

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps =
    SoftWire_CH32
```

Or clone directly into your `lib` folder:

```bash
cd lib
git clone https://github.com/yourusername/SoftWire_CH32.git
```

## Usage

### Basic Example

```cpp
#include <SoftWire_CH32.h>

// Create SoftWire instance
SoftWire_CH32 Wire;

void setup() {
  // Initialize I2C on PB6 (SDA) and PB7 (SCL)
  Wire.begin(PB6, PB7);

  // Optional: Set clock speed (default is ~100kHz)
  Wire.setClock(100000);  // 100kHz
}

void loop() {
  // Use just like the standard Wire library!
  Wire.beginTransmission(0x50);
  Wire.write(0x00);
  Wire.write(0xAA);
  Wire.endTransmission();
}
```

### Scanning for Devices

```cpp
for (uint8_t address = 1; address < 127; address++) {
  if (Wire.exists(address)) {
    Serial.print("Device found at 0x");
    Serial.println(address, HEX);
  }
}
```

### Reading Data

```cpp
// Read 4 bytes from device at 0x50
Wire.beginTransmission(0x50);
Wire.write(0x00);  // Register address
Wire.endTransmission(false);  // Repeated start

uint8_t count = Wire.requestFrom(0x50, 4);
while (Wire.available()) {
  uint8_t data = Wire.read();
  // Process data
}
```

### Writing Data

```cpp
// Write to device at 0x50
Wire.beginTransmission(0x50);
Wire.write(0x00);  // Register address
Wire.write(0xAA);  // Data byte 1
Wire.write(0xBB);  // Data byte 2
uint8_t error = Wire.endTransmission();

if (error == 0) {
  Serial.println("Write successful!");
}
```

## API Reference

### Initialization

#### `begin(uint8_t sda_pin, uint8_t scl_pin)`
Initialize I2C with specified pins.

**Parameters:**
- `sda_pin`: GPIO pin for SDA
- `scl_pin`: GPIO pin for SCL

**Example:**
```cpp
Wire.begin(PB6, PB7);
```

#### `end()`
Release I2C pins.

#### `setClock(uint32_t frequency)`
Set I2C clock frequency in Hz.

**Parameters:**
- `frequency`: Target frequency in Hz (e.g., 100000 for 100kHz)

**Note:** Actual speed may vary due to software implementation.

**Example:**
```cpp
Wire.setClock(100000);  // 100kHz
Wire.setClock(400000);  // 400kHz (fast mode)
```

### Transmission

#### `beginTransmission(uint8_t address)`
Start a transmission to the I2C device.

**Parameters:**
- `address`: 7-bit I2C device address

#### `endTransmission(bool sendStop = true)`
End transmission and send stop condition.

**Parameters:**
- `sendStop`: Send stop condition (default: true)

**Returns:**
- `0`: Success
- `1`: Data too long
- `2`: NACK on address
- `3`: NACK on data
- `4`: Other error

#### `write(uint8_t data)`
Write a single byte.

**Returns:** Number of bytes written (1 or 0)

#### `write(const uint8_t *data, size_t length)`
Write multiple bytes.

**Returns:** Number of bytes written

### Reception

#### `requestFrom(uint8_t address, uint8_t quantity, bool sendStop = true)`
Request data from I2C device.

**Parameters:**
- `address`: 7-bit I2C device address
- `quantity`: Number of bytes to request
- `sendStop`: Send stop condition (default: true)

**Returns:** Number of bytes received

#### `available()`
Number of bytes available to read.

**Returns:** Byte count

#### `read()`
Read one byte.

**Returns:** Byte value or -1 if no data

#### `peek()`
Peek at next byte without removing it.

**Returns:** Byte value or -1 if no data

### Utility

#### `exists(uint8_t address)`
Check if device exists at address.

**Parameters:**
- `address`: 7-bit I2C device address

**Returns:** `true` if device responds, `false` otherwise

**Example:**
```cpp
if (Wire.exists(0x50)) {
  Serial.println("Device found!");
}
```

## Hardware Requirements

### Pull-up Resistors

I2C requires pull-up resistors on both SDA and SCL lines. Typical values:
- **4.7kΩ** - Standard for most applications
- **10kΩ** - For slower speeds or shorter cables
- **2.2kΩ** - For faster speeds or longer cables

Connect pull-up resistors between each I2C line and VCC (3.3V or 5V depending on your devices).

### Pin Selection for CH32X035G8U6 (QFN28)

**Recommended pins:** PB6 (SDA), PB7 (SCL)

**Avoid these pins:**
- PC16, PC17: USB D-, D+ (if using USB)
- PC18, PC19: SWDIO, SWCLK (debug pins)
- PC1, PC2, PC5, PC6: Not available on QFN28 package

**Other available pins:** PA0-PA15, PB0-PB15, PD0-PD7 (check your specific package)

## Performance

### Speed

Software I2C is slower than hardware I2C due to software overhead:
- **Standard mode**: ~100kHz (typical)
- **Fast mode**: Up to ~400kHz (may vary)

Actual speed depends on:
- CPU clock speed
- Compiler optimizations
- System load

### CPU Usage

Software I2C is blocking and uses 100% CPU during transmission. For high-speed or continuous I2C operations, consider using hardware I2C if available.

## Multiple I2C Buses

You can create multiple software I2C instances:

```cpp
#include <SoftWire_CH32.h>

SoftWire_CH32 Wire1;
SoftWire_CH32 Wire2;

void setup() {
  Wire1.begin(PB6, PB7);   // First bus
  Wire2.begin(PA1, PA2);   // Second bus
}
```

## Troubleshooting

### No Devices Found

1. **Check pull-up resistors**: Both SDA and SCL need pull-ups
2. **Verify wiring**: Ensure SDA/SCL are connected correctly
3. **Check device address**: Use I2C scanner to find devices
4. **Check power**: Ensure I2C device is powered
5. **Pin conflicts**: Make sure pins aren't used by other peripherals

### Communication Errors

1. **Lower clock speed**: Try `Wire.setClock(50000)` for 50kHz
2. **Check voltage levels**: Ensure voltage compatibility (3.3V vs 5V)
3. **Cable length**: Keep I2C cables short (< 1 meter)
4. **Interference**: Use shielded cables for noisy environments

### Garbled Data

1. **Pull-up resistor values**: Try different resistor values (2.2kΩ - 10kΩ)
2. **Capacitance**: Long cables add capacitance, lower the speed
3. **Clock stretching**: Some devices need more time

## Examples

See the `examples/` folder for complete examples:
- **I2C_Scanner**: Scan for I2C devices on the bus
- **I2C_ReadWrite**: Read and write data to an I2C EEPROM

## Limitations

- Master mode only (no slave support)
- Blocking operation (CPU waits during I2C transactions)
- Speed limited by software implementation
- No DMA support
- Higher CPU usage than hardware I2C

## License

This library is released under the GNU Lesser General Public License v2.1.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Credits

Developed for the CH32 microcontroller community.

## Support

For issues, questions, or suggestions, please open an issue on GitHub.

---

**Note:** This library is provided as-is without warranty. Always test thoroughly with your specific hardware configuration.
