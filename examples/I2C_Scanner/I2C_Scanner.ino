/*
  SoftWire_CH32.cpp - Software I2C library for CH32 microcontrollers
  Copyright (C) 2025 Jobit Joseph
  I2C Scanner Example for SoftWire_CH32

  This sketch scans the I2C bus for connected devices and prints
  their addresses to the USB CDC Serial monitor.

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

void setup() {
  // Initialize USB Serial
  USBSerial.begin();

  // Wait for host to connect (5 second timeout)
  USBSerial.waitForPC(5000);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);  // 100kHz

  USBSerial.println("\n=== SoftWire_CH32 I2C Scanner ===");
  USBSerial.print("SDA: PB");
  USBSerial.print(SDA_PIN - PB0);
  USBSerial.print(", SCL: PB");
  USBSerial.println(SCL_PIN - PB0);
  USBSerial.println("Scanning I2C bus...\n");
}

void loop() {
  int deviceCount = 0;

  for (uint8_t address = 1; address < 127; address++) {
    if (Wire.exists(address)) {
      USBSerial.print("I2C device found at 0x");
      if (address < 16) {
        USBSerial.print("0");
      }
      USBSerial.println(address, HEX);
      deviceCount++;
    }
    delay(1);
  }

  if (deviceCount == 0) {
    USBSerial.println("No I2C devices found");
  } else {
    USBSerial.print("Found ");
    USBSerial.print(deviceCount);
    USBSerial.println(" device(s)");
  }

  USBSerial.println("\n--- Scan Complete ---");

  if (USBSerial.dtr()) {  // Only show heartbeat if host connected
    USBSerial.println("Scanning again in 5 seconds...");
    USBSerial.print("Uptime: ");
    USBSerial.print(millis() / 1000);
    USBSerial.println("s\n");
  }

  delay(5000);
}
