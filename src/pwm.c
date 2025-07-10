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

#include "pwm.h"

#include <avr/io.h>

/**
 * Initialize Timer0 for 8-bit Fast PWM on OC0A (PB0).
 *
 * Steps:
 * 1. Set PB0 as output (OC0A pin).
 * 2. Configure Timer0 for Fast PWM (Mode 3): set WGM01 and WGM00.
 * 3. Select non-inverting output: set COM0A1, clear COM0A0.
 * 4. Choose prescaler = clk/1 by setting CS00 in TCCR0B.
 *    → PWM frequency = F_CPU / 256 (~31.3 kHz at 8 MHz)
 */
void pwm_init(void) {
  PWM_DDR |= (1 << PWM_PIN); /* PB0 as output */

  TCCR0A = (1 << WGM01) | (1 << WGM00) /* Fast PWM mode */
           | (1 << COM0A1);            /* Non-inverting on OC0A */

  /* Use TCCR0B for prescaler: clk/1 → ~31.3 kHz PWM at 8 MHz */
  TCCR0B = (1 << CS00);
}

/**
 * Set PWM duty cycle.
 * @param duty 0 → 0% (always low), 255 → ~100% (always high).
 */
void pwm_set(uint8_t duty) { OCR0A = duty; }

/**
 * Disable PWM and force the output pin low.
 *
 * Steps:
 * 1. Disconnect OC0A by clearing COM0A1 and COM0A0.
 * 2. Drive PB0 low via PORT register.
 */
void pwm_off(void) {
  TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0)); /* Disconnect OC0A */
  PWM_PORT &= ~(1 << PWM_PIN);                /* PB0 = 0 */
}
