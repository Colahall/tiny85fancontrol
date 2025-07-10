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

#ifndef TINY85FANCONTROL_SRC_PWM_H_
#define TINY85FANCONTROL_SRC_PWM_H_

/**
 * PWM driver for ATtiny85 using Timer0 channel A (OC0A on PB0).
 *
 * - Uses 8-bit Fast PWM (Mode 3):
 *     • WGM01=1, WGM00=1 → Fast PWM with TOP=0xFF
 *     • COM0A1=1, COM0A0=0 → Non-inverting output: OC0A clears on compare match, sets at BOTTOM
 * - Default prescaler = clk/1 → PWM frequency = F_CPU / 256
 *   (~31.3 kHz at 8 MHz CPU clock)
 *
 * Functions:
 *  • pwm_init():  Configure Timer0 and PB0 for PWM output
 *  • pwm_set(d): Set duty cycle (0–255)
 *  • pwm_off():  Disable PWM and drive PB0 low
 */

#include <stdint.h>

#define PWM_DDR   DDRB    /* Data Direction Register for PWM pin */
#define PWM_PORT  PORTB   /* Port register for PWM pin */
#define PWM_PIN   PB0     /* Pin number for OC0A (PB0) */

void pwm_init(void);
void pwm_set(uint8_t duty);
void pwm_off(void);

#endif /* TINY85FANCONTROL_SRC_PWM_H_ */