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

#include <avr/io.h>
#include <util/delay.h>

static volatile uint8_t TEMP_SENSOR_INIT = 0;

// Initialize ADC to read the internal temperature sensor
void temp_sensor_init(void) {
  // 1. Select internal 1.1 V reference (bandgap)
  //    REFS1=1, REFS0=0 → Vref = 1.1 V bandgap
  // 2. Right-adjust result (ADLAR=0)
  // 3. Select channel MUX[3:0] = 1111 for temperature sensor
  ADMUX = (1 << REFS1) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);

  // 4. Enable ADC, set prescaler to 128 for 125 kHz ADC clock
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  // 5. Dummy conversion to let reference settle
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC))
    ;

  TEMP_SENSOR_INIT = 1; // Mark sensor as initialized
}

// Read one raw 10-bit sample from the temperature sensor
uint16_t temp_sensor_read_raw(void) {
  if (!TEMP_SENSOR_INIT) {
    temp_sensor_init(); // Ensure sensor is initialized
  }

  // Start conversion
  ADCSRA |= (1 << ADSC);
  // Wait for completion
  while (ADCSRA & (1 << ADSC))
    ;
  // ADC is right-adjusted, so ADC returns the full 10-bit result
  return ADC;
}

int16_t temp_sensor_read_celsius(void) {
  // Read raw sample
  uint16_t raw_value = 0;

  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value += temp_sensor_read_raw();
  raw_value /= 8; // Average the 8 samples

  // Convert to Celsius
  return ((int16_t)raw_value) - 270; // Adjust for calibration offset
}