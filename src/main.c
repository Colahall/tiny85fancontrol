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
#include "pwm.h"
#include "fan_curve.h"
#include "temp_sensor.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

#define BUILD_VERSION "1.0.0"

int main(void) {
  pwm_init();         // Initialize PWM
  uart_init();        // Initialize UART
  temp_sensor_init(); // Initialize temperature sensor

  uart_print("Tiny85 Fan Control (Table LERP)\r\n");
  uart_print("Build Version: " BUILD_VERSION "\r\n");
  uart_print("System Initialized\r\n");

  uint8_t current_pwm_duty = 0;
  int16_t case_temp = 0;

  // Set the PWM to max duty cycle initially until fan ramps up
  pwm_set(255);
  _delay_ms(9999);

  for (;;) {
    case_temp = ds18b20_read_celsius();

    uart_print("Current Temp = ");
    uart_print_dec16(case_temp);
    uart_print(" C, ");

    current_pwm_duty = fan_curve_compute_pwm(case_temp);

    pwm_set(current_pwm_duty);
    uart_print("PWM Duty Cycle = ");
    uart_print_dec16(current_pwm_duty);
    uart_print("\r\n");

    _delay_ms(2000); // Delay for a longer period (e.g., 2 seconds)
  }

  return 0; // This line will never be reached
}
