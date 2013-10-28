/*
 * Copyright 2010 by Adam Mayer   <adam@makerbot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef BOARDS_UNO_CONFIGURATION_HH_
#define BOARDS_UNO_CONFIGURATION_HH_

// This file details the pin assignments and features of the RepRap Motherboard
// version 1.2 for the ordinary use case.


// Interval for the stepper update in microseconds.  This interval is the minimum
// possible time between steps; in practical terms, your time between steps should
// be at least eight times this large.  Reducing the interval can cause resource
// starvation; leave this at 64uS or greater unless you know what you're doing.
#define INTERVAL_IN_MICROSECONDS 64

#define DEBUG_MSG_BUFFER_SIZE   32

#define PARANOID                1

// Define as 1 if and SD card slot is present; 0 if not.
#define HAS_SD                  0

// --- Slave UART configuration ---
// The slave UART is presumed to be an RS485 connection through a sn75176 chip.
// Define as 1 if the slave UART is present; 0 if not.
#define HAS_SLAVE_UART          0

// --- Host UART configuration ---
// The host UART is presumed to always be present on the RX/TX lines.

// --- Axis configuration ---
// Define the number of stepper axes supported by the board.  The axes are
// denoted by X, Y, Z, A and B.

// Ordinary G3 motherboards have three stepper terminals.
#define STEPPER_COUNT           3
#define STEPS_PER_REV           200

// --- Stepper and endstop configuration ---
// Pins should be defined for each axis present on the board.  They are denoted
// X, Y, Z, A and B respectively.

#define X_NUM_PINS               4
#define X_PIN_1                  8
#define X_PIN_2                  9
#define X_PIN_3                  10
#define X_PIN_4                  11

#define Y_NUM_PINS               4
#define Y_PIN_1                  2
#define Y_PIN_2                  3
#define Y_PIN_3                  4
#define Y_PIN_4                  5

#define Z_NUM_PINS               2
#define Z_PIN_1                  6
#define Z_PIN_2                  7

// --- Debugging configuration ---
// The pin which controls the debug LED (active high)
#define DEBUG_PIN               13

// By default, debugging packets should be honored; this is made
// configurable if we're short on cycles or EEPROM.
// Define as 1 if debugging packets are honored; 0 if not.
#define HONOR_DEBUG_PACKETS     1

#endif // BOARDS_UNO_CONFIGURATION_HH_
