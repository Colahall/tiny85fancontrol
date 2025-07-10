/*
 * Copyright (c) 2025 Colahall, LLC.
 *
 * This File is part of Tiny85FanControl (see https://colahall.io/).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * “Software”), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "ds18b20.h"
#include "onewire.h"

#include <util/delay.h>

// Error reading temperature, return -273°C in 0.01°C units
#define DS18B20_ERROR (-(273 << 4))

/**
 * Compute Dallas/Maxim OneWire CRC8 (poly = x^8 + x^5 + x^4 + 1, reflect=1)
 * @param data  pointer to bytes
 * @param len   number of bytes
 * @return      8-bit CRC
 */
static uint8_t crc8(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t in = *data++;
    for (uint8_t i = 0; i < 8; i++) {
      uint8_t mix = (crc ^ in) & 0x01;
      crc >>= 1;
      if (mix) {
        crc ^= 0x8C; // 0x8C = reversed 0x31 polynomial
      }
      in >>= 1;
    }
  }
  return crc;
}

int16_t ds18b20_read_raw(void) {
  uint8_t scratchpad[9];

  if (onewire_reset() == ONEWIRE_ERROR) {
    return DS18B20_ERROR; // Error reading temperature
  }

  onewire_write_byte(0xCC); // to all devices on the bus
  onewire_write_byte(0x44); // send DS18B20 command, "CONVERT T"

  while (!onewire_read_bus()) {
    // Wait for the bus to be released
    _delay_ms(16); // Short delay to avoid busy-waiting
  }

  _delay_ms(256); // Wait for conversion to complete

  onewire_reset();          // Reset the bus again
  onewire_write_byte(0xCC); // to all devices on the bus
  onewire_write_byte(0xBE); // send DS18B20 command, "READ SCRATCHPAD"

  // Read all 9 bytes of scratchpad
  for (uint8_t i = 0; i < 9; i++) {
    scratchpad[i] = onewire_read_byte();
  }

  onewire_reset(); // Reset the bus again

  // Verify CRC
  if (crc8(scratchpad, 8) != scratchpad[8]) {
    return DS18B20_ERROR; // CRC error, return error value
  }

  // Calculate temperature from scratchpad data
  uint8_t hi = scratchpad[1]; // High byte
  uint8_t lo = scratchpad[0]; // Low byte

  // Combine low and high byte
  int16_t raw_temp = ((int16_t)(hi << 8)) | lo;

  return raw_temp;
}

int16_t ds18b20_read_celsius(void) {
  int16_t t = ds18b20_read_raw();

  // Convert to Celsius: each bit = 0.0625 °C
  // Return temperature in 1 °C units

  return (t + 0x0F) >> 4;
}
