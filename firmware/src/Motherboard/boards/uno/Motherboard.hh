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

#ifndef BOARDS_UNO_MOTHERBOARD_HH_
#define BOARDS_UNO_MOTHERBOARD_HH_

#include "Configuration.hh"
#include "StepperInterface.hh"
#include "UnoStepperInterface.hh"
#include "Types.hh"

class Motherboard {
private:
  const static int STEPPERS = STEPPER_COUNT;

  UnoStepperInterface stepper[STEPPERS];

  /// Microseconds since board initialization
  volatile micros_t micros;

  /// Number of times to blink the debug LED on each cycle
  volatile uint8_t error_code;

  volatile char debug_message[DEBUG_MSG_BUFFER_SIZE];
  volatile uint8_t debug_message_len;

  /// Private constructor; use the singleton
  Motherboard();

  static Motherboard motherboard;
public:
  /// Reset the motherboard to its initial state.
  /// This only resets the board, and does not send a reset
  /// to any attached toolheads.
  void reset();

  void runMotherboardSlice();

  /// Get the number of microseconds that have passed since
  /// the board was initialized.  This value will wrap after
  /// 2**16 microseconds; callers should compensate for this.
  micros_t getCurrentMicros();

  /// Write an error code to the debug pin.
  void indicateError(int errorCode);
  /// Get the current error being displayed.
  uint8_t getCurrentError();

  /// Perform the timer interrupt routine.
  void doInterrupt();

  /// Sets a debug message that will be stored for later (usually by the DEBUG_GET_DEBUG_BUFFER
  /// command. only the first DEBUG_MSG_BUFFER_SIZE characters will be used.
  void setDebugMessage(char* message, uint8_t len);
  /// Gets a debug message set by setDebugMessage. The data will be put into the message variable,
  /// and the length of the message will be returned. messages must have at at least
  /// DEBUG_MSG_BUFFER_SIZE characters allocated.
  uint8_t getDebugMessage(char* message);

  /// Count the number of steppers available on this board.
  const int getStepperCount() const { return STEPPERS; }

  /// Get the stepper interface for the nth stepper.
  StepperInterface& getStepperInterface(int n) { return stepper[n]; }

  static Motherboard& getBoard() { return motherboard; }
};

#endif // BOARDS_UNO_MOTHERBOARD_HH_
