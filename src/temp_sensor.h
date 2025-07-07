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

#ifndef TINY85_FAN_CONTROL_SRC_TEMP_SENSOR_H_
#define TINY85_FAN_CONTROL_SRC_TEMP_SENSOR_H_

#include <stdint.h>

void temp_sensor_init(void);
uint16_t temp_sensor_read_raw(void);
int16_t temp_sensor_read_celsius(void);

#endif // TINY85_FAN_CONTROL_SRC_UART_H_
