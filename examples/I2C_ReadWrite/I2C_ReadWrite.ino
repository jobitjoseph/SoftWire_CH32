/*
  SoftWire_CH32.cpp - Software I2C library for CH32 microcontrollers
  Copyright (C) 2025 Jobit Joseph
  I2C Read/Write Example for SoftWire_CH32

  This sketch demonstrates how to read and write data to an I2C device
  using the SoftWire_CH32 library with a Wire-compatible API.

  Example uses a 24C32 EEPROM at address 0x57, but can be adapted
  for any I2C device.

  For CH32X035G8U6 (QFN28):
  - Default pins: PB6 (SDA), PB7 (SCL)
  - You can use any available GPIO pins
  - Uses USB CDC for communication

  Note: Make sure you have pull-up resistors on SDA and SCL lines
  (typically 4.7kΩ to 10kΩ)
*/

#include <SoftWire_CH32.h>
#include <CH32X035_USBSerial.h>

using namespace wch::usbcdc;

// Create SoftWire instance
SoftWire_CH32 Wire;

// I2C pins - change these to match your hardware
#define SDA_PIN  PB6
#define SCL_PIN  PB7

// I2C device address (change to your device's address)
#define DEVICE_ADDRESS  0x57

void setup() {
  // Initialize USB Serial
  USBSerial.begin();

  // Wait for host to connect (5 second timeout)
  USBSerial.waitForPC(5000);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);  // 100kHz

  USBSerial.println("\n=== SoftWire_CH32 I2C Read/Write ===");
  USBSerial.print("Device address: 0x");
  USBSerial.println(DEVICE_ADDRESS, HEX);

  // Check if device exists
  if (!Wire.exists(DEVICE_ADDRESS)) {
    USBSerial.println("ERROR: Device not found!");
    USBSerial.println("Please check:");
    USBSerial.println("  - I2C connections (SDA, SCL, GND, VCC)");
    USBSerial.println("  - Pull-up resistors (4.7kΩ)");
    USBSerial.println("  - Device address");
    while (1) {
      if (USBSerial.dtr()) {
        USBSerial.print("♥ Still waiting... ");
        USBSerial.print(millis() / 1000);
        USBSerial.println("s");
      }
      delay(1000);
    }
  }

  USBSerial.println("Device found! ✓");
  USBSerial.println();
}

void loop() {
  // Example 1: Write a single byte to register 0x00
  USBSerial.println("Writing 0xAA to register 0x0000...");
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x00);  // Register address (MSB for EEPROM)
  Wire.write(0x00);  // Register address (LSB for EEPROM)
  Wire.write(0xAA);  // Data to write
  uint8_t error = Wire.endTransmission();

  if (error == 0) {
    USBSerial.println("  Write successful! ✓");
  } else {
    USBSerial.print("  Write failed with error: ");
    USBSerial.println(error);
    USBSerial.println("  Error codes: 0=OK, 1=Too long, 2=NACK addr, 3=NACK data, 4=Other");
  }

  delay(10);  // Wait for EEPROM write cycle

  // Example 2: Read a single byte from register 0x00
  USBSerial.println("Reading from register 0x0000...");
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0x00);  // Register address (MSB)
  Wire.write(0x00);  // Register address (LSB)
  Wire.endTransmission(false);  // Repeated start

  uint8_t bytesRead = Wire.requestFrom(DEVICE_ADDRESS, (uint8_t)1);
  if (bytesRead > 0) {
    uint8_t data = Wire.read();
    USBSerial.print("  Read value: 0x");
    USBSerial.println(data, HEX);

    if (data == 0xAA) {
      USBSerial.println("  Read/Write test PASSED! ✓");
    } else {
      USBSerial.println("  Read/Write test FAILED!");
      USBSerial.print("  Expected 0xAA, got 0x");
      USBSerial.println(data, HEX);
    }
  } else {
    USBSerial.println("  Read failed!");
  }

  USBSerial.println("\n--- Test Complete ---");

  if (USBSerial.dtr()) {  // Only show heartbeat if host connected
    USBSerial.println("Running again in 5 seconds...");
    USBSerial.print("Uptime: ");
    USBSerial.print(millis() / 1000);
    USBSerial.println("s\n");
  }

  delay(5000);
}
