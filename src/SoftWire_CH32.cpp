/*
 *SoftWire_CH32.cpp - Software I2C library for CH32 microcontrollers
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

#include "SoftWire_CH32.h"

// Constructor
SoftWire_CH32::SoftWire_CH32() {
  _sda_pin = 0xFF;
  _scl_pin = 0xFF;
  _delay_us = 5;  // Default to ~100kHz
  _tx_buffer_index = 0;
  _tx_buffer_length = 0;
  _rx_buffer_index = 0;
  _rx_buffer_length = 0;
}

// Initialize I2C with specific pins
void SoftWire_CH32::begin(uint8_t sda_pin, uint8_t scl_pin) {
  _sda_pin = sda_pin;
  _scl_pin = scl_pin;

  // Initialize pins as inputs with pullups (idle state)
  pinMode(_sda_pin, INPUT_PULLUP);
  pinMode(_scl_pin, INPUT_PULLUP);
  i2c_delay();
}

// End I2C communication
void SoftWire_CH32::end() {
  // Release pins
  pinMode(_sda_pin, INPUT);
  pinMode(_scl_pin, INPUT);
}

// Set I2C clock speed (in Hz)
void SoftWire_CH32::setClock(uint32_t frequency) {
  if (frequency <= 0) return;

  // Calculate delay in microseconds
  // For I2C, we toggle the clock twice per bit (high + low)
  // So delay = 1000000 / (frequency * 2)
  _delay_us = 500000 / frequency;

  if (_delay_us < 1) _delay_us = 1;  // Minimum 1us delay
}

// Begin transmission to device
void SoftWire_CH32::beginTransmission(uint8_t address) {
  _tx_address = address;
  _tx_buffer_index = 0;
  _tx_buffer_length = 0;
}

// End transmission
uint8_t SoftWire_CH32::endTransmission(bool sendStop) {
  // Start condition
  i2c_start();

  // Send address (write mode)
  if (!i2c_write_byte((_tx_address << 1) | 0)) {
    if (sendStop) i2c_stop();
    return 2;  // NACK on address
  }

  // Send data
  for (uint8_t i = 0; i < _tx_buffer_length; i++) {
    if (!i2c_write_byte(_tx_buffer[i])) {
      if (sendStop) i2c_stop();
      return 3;  // NACK on data
    }
  }

  // Stop condition
  if (sendStop) {
    i2c_stop();
  }

  return 0;  // Success
}

// Request bytes from slave device
uint8_t SoftWire_CH32::requestFrom(uint8_t address, uint8_t quantity, bool sendStop) {
  if (quantity > SOFTWIRE_CH32_BUFFER_LENGTH) {
    quantity = SOFTWIRE_CH32_BUFFER_LENGTH;
  }

  // Start condition
  i2c_start();

  // Send address (read mode)
  if (!i2c_write_byte((address << 1) | 1)) {
    if (sendStop) i2c_stop();
    return 0;  // NACK on address
  }

  // Read data
  _rx_buffer_length = 0;
  for (uint8_t i = 0; i < quantity; i++) {
    _rx_buffer[i] = i2c_read_byte(i < quantity - 1);  // ACK all but last byte
    _rx_buffer_length++;
  }

  // Stop condition
  if (sendStop) {
    i2c_stop();
  }

  _rx_buffer_index = 0;
  return _rx_buffer_length;
}

// Write a byte
size_t SoftWire_CH32::write(uint8_t data) {
  if (_tx_buffer_length >= SOFTWIRE_CH32_BUFFER_LENGTH) {
    return 0;  // Buffer full
  }

  _tx_buffer[_tx_buffer_length++] = data;
  return 1;
}

// Write multiple bytes
size_t SoftWire_CH32::write(const uint8_t *data, size_t length) {
  size_t written = 0;
  for (size_t i = 0; i < length; i++) {
    if (write(data[i])) {
      written++;
    } else {
      break;  // Buffer full
    }
  }
  return written;
}

// Number of bytes available to read
int SoftWire_CH32::available() {
  return _rx_buffer_length - _rx_buffer_index;
}

// Read a byte
int SoftWire_CH32::read() {
  if (_rx_buffer_index < _rx_buffer_length) {
    return _rx_buffer[_rx_buffer_index++];
  }
  return -1;  // No data available
}

// Peek at next byte without removing it
int SoftWire_CH32::peek() {
  if (_rx_buffer_index < _rx_buffer_length) {
    return _rx_buffer[_rx_buffer_index];
  }
  return -1;  // No data available
}

// Check if device exists at address
bool SoftWire_CH32::exists(uint8_t address) {
  i2c_start();
  bool ack = i2c_write_byte((address << 1) | 0);
  i2c_stop();
  return ack;
}

// ========== Low-level I2C functions ==========

void SoftWire_CH32::i2c_delay() {
  delayMicroseconds(_delay_us);
}

void SoftWire_CH32::i2c_sda_high() {
  pinMode(_sda_pin, INPUT_PULLUP);  // Release SDA (open-drain)
}

void SoftWire_CH32::i2c_sda_low() {
  pinMode(_sda_pin, OUTPUT);
  digitalWrite(_sda_pin, LOW);
}

void SoftWire_CH32::i2c_scl_high() {
  pinMode(_scl_pin, INPUT_PULLUP);  // Release SCL (open-drain)
  i2c_delay();

  // Wait for clock stretching
  uint16_t timeout = 1000;
  while (digitalRead(_scl_pin) == LOW && timeout--) {
    delayMicroseconds(1);
  }
}

void SoftWire_CH32::i2c_scl_low() {
  pinMode(_scl_pin, OUTPUT);
  digitalWrite(_scl_pin, LOW);
  i2c_delay();
}

bool SoftWire_CH32::i2c_read_sda() {
  return digitalRead(_sda_pin);
}

bool SoftWire_CH32::i2c_read_scl() {
  return digitalRead(_scl_pin);
}

void SoftWire_CH32::i2c_start() {
  // Start condition: SDA goes low while SCL is high
  i2c_sda_high();
  i2c_scl_high();
  i2c_delay();
  i2c_sda_low();
  i2c_delay();
  i2c_scl_low();
}

void SoftWire_CH32::i2c_stop() {
  // Stop condition: SDA goes high while SCL is high
  i2c_sda_low();
  i2c_delay();
  i2c_scl_high();
  i2c_delay();
  i2c_sda_high();
  i2c_delay();
}

bool SoftWire_CH32::i2c_write_bit(bool bit) {
  if (bit) {
    i2c_sda_high();
  } else {
    i2c_sda_low();
  }
  i2c_delay();
  i2c_scl_high();
  i2c_scl_low();
  return true;
}

bool SoftWire_CH32::i2c_read_bit() {
  i2c_sda_high();  // Release SDA to read
  i2c_delay();
  i2c_scl_high();
  bool bit = i2c_read_sda();
  i2c_scl_low();
  return bit;
}

bool SoftWire_CH32::i2c_write_byte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    i2c_write_bit((byte & 0x80) != 0);
    byte <<= 1;
  }

  // Read ACK/NACK
  bool nack = i2c_read_bit();
  return !nack;  // Return true if ACK received
}

uint8_t SoftWire_CH32::i2c_read_byte(bool ack) {
  uint8_t byte = 0;
  for (int i = 0; i < 8; i++) {
    byte <<= 1;
    byte |= i2c_read_bit() ? 1 : 0;
  }

  // Send ACK or NACK
  i2c_write_bit(!ack);
  return byte;
}
