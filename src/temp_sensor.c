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

#include "temp_sensor.h"

#include <avr/io.h>
#include <util/delay.h>

static volatile uint8_t TEMP_SENSOR_INIT = 0;

/**
 * @brief Initialize ADC to read the internal temperature sensor
 */
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

/**
 * @brief Read a single raw sample from the temperature sensor
 *
 * @return uint16_t Raw ADC value from the temperature sensor
 */
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

/**
 * @brief Read the average temperature in Celsius from the sensor
 *
 * This function reads 8 samples from the temperature sensor, averages them,
 * and converts the result to Celsius.
 *
 * @return int16_t Temperature in degrees Celsius
 */
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
  return ((int16_t)raw_value) - 273; // Adjust for calibration offset
}