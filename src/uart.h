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

#ifndef TINY85FANCONTROL_SRC_UART_H_
#define TINY85FANCONTROL_SRC_UART_H_

#include <stdint.h>

/**
 * UART Specific Definitions
 */
#define UART_BAUD_RATE (9600UL)
#define UART_BIT_TIME (1000000UL / UART_BAUD_RATE)

/** PIN for Tx **/
#define UART_TX_PIN PB2
#define UART_TX_PORT PORTB
#define UART_TX_DDR DDRB


// API Functions
void uart_init(void);
void uart_print(const char *s);
void uart_print_dec16(int16_t num);
int16_t temp_sensor_read_celsius(void);

#endif /* TINY85FANCONTROL_SRC_UART_H_ */