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
#ifndef TINY85FANCONTROL_ONEWIRE_H_
#define TINY85FANCONTROL_ONEWIRE_H_

#include <stdint.h>
#include <stdbool.h>

#define ONEWIRE_PORT PORTB
#define ONEWIRE_PIN PINB
#define ONEWIRE_BIT PB1
#define ONEWIRE_DDR DDRB

#define ONEWIRE_RETRY_COUNT (128)

enum {
    ONEWIRE_LOW = 0,
    ONEWIRE_HIGH = 1,
    ONEWIRE_ERROR = 2,
};

uint8_t onewire_reset(void);
uint8_t onewire_read_byte(void);
bool onewire_read_bus(void);
void onewire_write_byte(uint8_t data);

#endif // TINY85FANCONTROL_ONEWIRE_H_