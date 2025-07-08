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

#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

/**
 * UART Specific Definitions
 */
#define UART_BAUD_RATE (9600UL)
#define UART_BIT_TIME (1000000UL / UART_BAUD_RATE)

/** PIN for Tx **/
#define TX_PIN PB0
#define TX_PORT PORTB
#define TX_DDR DDRB

static volatile uint8_t UART_INIT = 0;

// Local function prototypes
static void uart_send_byte(uint8_t c);

/**
 * Initialize UART for transmission
 */
void uart_init(void) {
  if (UART_INIT)
    return; // Prevent re-initialization
  UART_INIT = 1;
  TX_DDR |= (1 << TX_PIN);  // Set TX_PIN as output
  TX_PORT |= (1 << TX_PIN); // Set TX high (idle state)
}

/**
 * @brief Print a string to the UART
 *
 * @param s String to print
 */
void uart_print(const char *s) {
  if (!s) {
    return; // Handle null pointer gracefully
  }

  if (!UART_INIT) {
    uart_init(); // Ensure UART is initialized
  }

  while (*s) {
    uart_send_byte((uint8_t)*s);
    s++;
  }
}

/**
 * @brief Print a signed 16-bit decimal number to the UART
 *
 * @param num A signed 16-bit integer to print
 */
void uart_print_dec16(int16_t num) {
  int i = 8;       // Number of digits to print
  char negate = 0; // Flag for negative numbers
  char buffer[9];  // Buffer to hold digits and newline

  buffer[i--] = '\0'; // Null-terminate string
  buffer[i--] = '\n'; // Add newline for readability
  buffer[i--] = '\r'; // Add carriage return for readability

  if (num == 0) {
    buffer[i--] = '0'; // Special case for zero
    goto end;          // Skip further processing
  }

  if (num < 0) {
    negate = 1; // Set flag for negative numbers
    num = -num; // Make it positive for printing
  }

  while (num > 0 && i >= 0) {
    buffer[i--] = (num % 10) + '0'; // Convert digit to character
    num /= 10;
  }

  if (negate && i >= 0) {
    buffer[i--] = '-'; // Add minus sign for negative numbers
  }

  ++i; // Adjust index to point to the first character

end:
  uart_print(buffer + i);
}

/**
 * @brief Send one bit (0 or 1), then wait one bit-time
 * @param b: the bit to send (0 or 1)
 */
static inline void uart_tx_bit(_Bool b) {
  if (b)
    TX_PORT |= (1 << TX_PIN); // Set TX high for 1
  else
    TX_PORT &= ~(1 << TX_PIN); // Set TX low for 0

  _delay_us(UART_BIT_TIME);
}

/**
 * @brief Send a byte: start bit, 8 data bits LSB first, stop bit
 *
 * @param c: the byte to send
 */
static void uart_send_byte(uint8_t c) {
  cli();          // Stop interrupts to keep timing accurate
  uart_tx_bit(0); // start bit
  // Send 8 bits, LSB first
  uart_tx_bit(c & 0b00000001); // BIT 0
  uart_tx_bit(c & 0b00000010); // BIT 1
  uart_tx_bit(c & 0b00000100); // BIT 2
  uart_tx_bit(c & 0b00001000); // BIT 3
  uart_tx_bit(c & 0b00010000); // BIT 4
  uart_tx_bit(c & 0b00100000); // BIT 5
  uart_tx_bit(c & 0b01000000); // BIT 6
  uart_tx_bit(c & 0b10000000); // BIT 7
  uart_tx_bit(1);              // stop bit
  sei();                       // Re-enable interrupts
}