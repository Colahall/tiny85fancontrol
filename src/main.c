/*
 * Copyright (c) 2025 Colahall, LLC..
 *
 * This file is part of Tiny85FanControl 
 * (see https://colahall.io/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "temp_sensor.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

// Simple hello world

int main(void) {
  uart_init();        // Initialize UART
  temp_sensor_init(); // Initialize temperature sensor

  uart_print("UART Initialized\r\n"); // Print initialization message

  while (1) {
    int16_t temp = temp_sensor_read_celsius(); // Read temperature in Celsius
    uart_print("Current Temp: ");
    uart_print_dec16(temp); // Print the raw temperature value
    _delay_ms(1000);        // Delay to avoid busy-waiting
  }

  return 0; // This line will never be reached
}
