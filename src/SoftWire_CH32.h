/*
 *SoftWire_CH32.h - Software I2C library for CH32 microcontrollers
 * Copyright (C) 2025 Jobit Joseph
 * Author: Jobit Joseph
 * Project: SoftWire_CH32 Library
 *
 * Licensed under the MIT License
 * You may not use this file except in compliance with the License.
 * 
 * You may obtain a copy of the License at:
 * https://opensource.org/license/mit/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software (the "Software") and associated documentation files, to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, subject to the following additional conditions:

 * 1. All copies or substantial portions must retain:  
 *    - The original copyright notice  
 *    - A prominent statement crediting the original author/creator  

 * 2. Modified versions must:  
 *    - Clearly mark the changes as their own  
 *    - Preserve all original credit notices
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SOFTWIRE_CH32_H
#define SOFTWIRE_CH32_H

#include <Arduino.h>

#define SOFTWIRE_CH32_BUFFER_LENGTH 32

class SoftWire_CH32 {
public:
  // Constructor
  SoftWire_CH32();

  // Initialize I2C with specific pins
  void begin(uint8_t sda_pin, uint8_t scl_pin);

  // Initialize I2C (for slave mode - not implemented)
  void begin(uint8_t address) { (void)address; }

  // End I2C communication
  void end();

  // Set I2C clock speed (in Hz)
  // Note: Software I2C speed is approximate
  void setClock(uint32_t frequency);

  // Begin transmission to device
  void beginTransmission(uint8_t address);

  // End transmission
  // Returns:
  //   0: success
  //   1: data too long to fit in transmit buffer
  //   2: received NACK on transmit of address
  //   3: received NACK on transmit of data
  //   4: other error
  uint8_t endTransmission(bool sendStop = true);

  // Request bytes from slave device
  uint8_t requestFrom(uint8_t address, uint8_t quantity, bool sendStop = true);

  // Write a byte
  size_t write(uint8_t data);

  // Write multiple bytes
  size_t write(const uint8_t *data, size_t length);

  // Number of bytes available to read
  int available();

  // Read a byte
  int read();

  // Peek at next byte without removing it
  int peek();

  // Flush (no-op for I2C)
  void flush() {}

  // Check if device exists at address
  bool exists(uint8_t address);

private:
  uint8_t _sda_pin;
  uint8_t _scl_pin;
  uint32_t _delay_us;

  // Transmission state
  uint8_t _tx_address;
  uint8_t _tx_buffer[SOFTWIRE_CH32_BUFFER_LENGTH];
  uint8_t _tx_buffer_index;
  uint8_t _tx_buffer_length;

  // Receive state
  uint8_t _rx_buffer[SOFTWIRE_CH32_BUFFER_LENGTH];
  uint8_t _rx_buffer_index;
  uint8_t _rx_buffer_length;

  // Low-level I2C functions
  void i2c_delay();
  void i2c_sda_high();
  void i2c_sda_low();
  void i2c_scl_high();
  void i2c_scl_low();
  bool i2c_read_sda();
  bool i2c_read_scl();

  void i2c_start();
  void i2c_stop();
  bool i2c_write_bit(bool bit);
  bool i2c_read_bit();
  bool i2c_write_byte(uint8_t byte);
  uint8_t i2c_read_byte(bool ack);
};

#endif // SOFTWIRE_CH32_H
