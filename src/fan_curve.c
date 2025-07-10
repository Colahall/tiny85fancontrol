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

#include "fan_curve.h"

// Structure to define a point on the fan curve
typedef struct {
  int8_t temperature; // Temperature in Celsius (-85 to +125 C))
  uint8_t pwm_duty;   // PWM duty cycle (0-255)
} fan_curve_point_t;

// Define your fan curve points in a PROGMEM array to save RAM
// The points MUST be sorted by temperature in ascending order.
static const fan_curve_point_t fan_curve[] = {
    {25, 0},   // Example: Below 25C, fan is off
    {27, 200},  // At 27C, fan is quiet ~60% duty cycle
    {35, 210}, // At 35C, fan starts to ramp up
    {40, 220}, // At 40C, fan is more audible
    {50, 230}, // At 50C and above, fan is at near full speed
    {60, 255}  // Ensure full speed is maintained even at higher temps
};

#define NUM_FAN_CURVE_POINTS (sizeof(fan_curve) / sizeof(fan_curve_point_t))

/**
 * Compute the PWM duty cycle based on the current temperature.
 *
 * @param temperature The current temperature in Celsius.
 * @return The computed PWM duty cycle (0-255).
 */
uint8_t fan_curve_compute_pwm(int16_t current_temp) {
  // If current_temp is below the first point, return 0 duty
  if (current_temp < fan_curve[0].temperature) {
    return fan_curve[0].pwm_duty;
  }

  // Find the two points between which to interpolate
  for (uint8_t i = 0; i < NUM_FAN_CURVE_POINTS - 1; i++) {
    const fan_curve_point_t *p1 = &fan_curve[i];
    const fan_curve_point_t *p2 = &fan_curve[i + 1];

    if (current_temp >= p1->temperature && current_temp < p2->temperature) {
      // Perform linear interpolation (LERP)
      // current_pwm = p1.pwm_duty + (current_temp - p1.temperature) *
      // (p2.pwm_duty - p1.pwm_duty) / (p2.temperature - p1.temperature)

      // Use long for calculations to prevent overflow
      int32_t temp_range = p2->temperature - p1->temperature;
      int32_t pwm_range = p2->pwm_duty - p1->pwm_duty;
      int32_t temp_offset = current_temp - p1->temperature;

      // Avoid division by zero if two points have the same temperature
      if (temp_range == 0) {
        // (shouldn't happen if sorted uniquely)
        return p1->pwm_duty;
      }

      int32_t interpolated_pwm =
          (int32_t)p1->pwm_duty + (temp_offset * pwm_range) / temp_range;

      // Clamp the result to 0-255 just in case
      if (interpolated_pwm > 255)
        return 255;
      if (interpolated_pwm < 0)
        return 0;

      return (uint8_t)interpolated_pwm;
    }
  }

  // Handle temperatures equal to or above the last point
  return fan_curve[NUM_FAN_CURVE_POINTS - 1].pwm_duty;
}