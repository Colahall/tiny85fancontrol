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
#include "onewire.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// Implementation of 1-Wire protocol for ATtiny85 microcontroller
// For more information, see:
// https://www.infineon.com/dgdl/Infineon-OneWire_001-43362-Software+Module+Datasheets-v01_01-EN.pdf

#define BIT_SET(x, bit) ((x) |= (1 << (bit)))    // Set bit
#define BIT_CLEAR(x, bit) ((x) &= ~(1 << (bit))) // Clear bit
#define BIT_READ(x, bit) (((x) >> (bit)) & 1)    // Read bit

// Set pin as input
#define ONEWIRE_MODE_INPUT BIT_CLEAR(ONEWIRE_DDR, ONEWIRE_BIT)

// Set pin as output
#define ONEWIRE_MODE_OUTPUT BIT_SET(ONEWIRE_DDR, ONEWIRE_BIT)

// Read pin state
#define ONEWIRE_READ_STATE BIT_READ(ONEWIRE_PIN, ONEWIRE_BIT)

// Set pin low
#define ONEWIRE_SET_LOW BIT_CLEAR(ONEWIRE_PORT, ONEWIRE_BIT)

// Set pin high
#define ONEWIRE_SET_HIGH BIT_SET(ONEWIRE_PORT, ONEWIRE_BIT)

// Disable interrupts to ensure timing accuracy
#define CRITICAL_SECTION_START(sreg)                                           \
  do {                                                                         \
    sreg = SREG;                                                               \
    cli();                                                                     \
  } while (0)

// Restore previous state of interrupts
#define CRITICAL_SECTION_END(sreg)                                             \
  do {                                                                         \
    SREG = sreg;                                                               \
  } while (0)

/**
 * @brief Reset the 1-Wire bus and check for presence pulse
 * @return 1 if device is present, 0 otherwise, 2 on error
 */
uint8_t onewire_reset(void) {
  // Ensure the pin is configured as input before starting
  uint8_t retry_count = ONEWIRE_RETRY_COUNT;
  uint8_t sreg;

  // Initialize pin state
  ONEWIRE_SET_LOW;    // Ensure output is low when driving
  ONEWIRE_MODE_INPUT; // Start as input

  while (!ONEWIRE_READ_STATE) {
    if (retry_count == 0) {
      // Error: bus is stuck low, return error code
      return ONEWIRE_ERROR;
    }

    _delay_us(1);
    --retry_count;
  }

  // Start critical section for timing accuracy
  CRITICAL_SECTION_START(sreg);

  ONEWIRE_MODE_OUTPUT; // Drive the bus low for reset
  _delay_us(482);      // Hold low for 480 us, state H

  // Release bus and wait for presence pulse
  ONEWIRE_MODE_INPUT; // Release bus (pull-up will bring it high)
  _delay_us(70);      // Wait for presence pulse

  uint8_t presence = !ONEWIRE_READ_STATE; // Device pulls low for presence

  _delay_us(410); // Wait for end of presence pulse window

  CRITICAL_SECTION_END(sreg);

  return presence ? ONEWIRE_LOW : ONEWIRE_HIGH; // Return presence status
}

/**
 * @brief Performs a read-write operation on the OneWire bus
 *
 * This function writes a single bit to the OneWire bus and reads the state of
 * the bus
 *
 * @param bit The bit to write (0 or 1)
 *
 * @return uint8_t The bit read from the bus after writing
 */
static uint8_t onewire_rw_bit(uint8_t bit) {
  uint8_t sreg;
  CRITICAL_SECTION_START(sreg);

  ONEWIRE_MODE_OUTPUT; // Set pin as output, state A
  _delay_us(1);        // Wait for 2 us, state A

  if (bit) {
    ONEWIRE_MODE_INPUT; // Set pin as input, state B
  }

  // For write 0: keep driving low
  _delay_us(14); // Wait for 14 us, state B

  // Read the pin state
  uint8_t state = ONEWIRE_READ_STATE; // Read the pin state, state C

  _delay_us(45); // Complete the time slot (total ~60us)

  ONEWIRE_MODE_INPUT; // Set pin as input again, state D

  CRITICAL_SECTION_END(sreg);

  return state ? ONEWIRE_HIGH : ONEWIRE_LOW; // Return the read state
}

/**
 * @brief Reads and writes a byte on the OneWire bus
 *
 * @param data Data to write to the bus
 *
 * @return uint8_t Data read from the bus after writing
 */
static uint8_t onewire_rw_byte(uint8_t data) {
  for (uint8_t i = 0; i < 8; ++i) {
    // Write the LSB of the byte and read the bus state
    uint8_t bit_read = onewire_rw_bit(data & 1);
    data >>= 1;

    if (bit_read) {
      data |= 0x80;
    }
  }

  return data;
}

/**
 * @brief Read a byte from the OneWire bus
 *
 * @return uint8_t
 */
uint8_t onewire_read_byte(void) { return onewire_rw_byte(0xFF); }

/**
 * @brief Write a byte to the OneWire bus
 *
 * @param data The byte to write
 */
void onewire_write_byte(uint8_t data) { (void)onewire_rw_byte(data); }

/**
 * @brief Read the state of the OneWire bus
 */
bool onewire_read_bus(void) { return ONEWIRE_READ_STATE; }