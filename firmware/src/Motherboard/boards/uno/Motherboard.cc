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

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "Arduino.h"
#include "Motherboard.hh"
#include "Configuration.hh"
#include "Steppers.hh"
#include "UnoStepperInterface.hh"
#include "Command.hh"
#include "Eeprom.hh"
#include "EepromMap.hh"
#include "UART.hh"

Motherboard Motherboard::motherboard;

/// Create Motherboard object
Motherboard::Motherboard() {
  /// Set up the stepper pins on board creation
#if STEPPER_COUNT > 0
  stepper[0] = UnoStepperInterface(X_PIN_1, X_PIN_2
  #if X_NUM_PINS == 4
      , X_PIN_3, X_PIN_4
  #endif
  );
#endif
#if STEPPER_COUNT > 1
  stepper[1] = UnoStepperInterface(Y_PIN_1, Y_PIN_2
  #if Y_NUM_PINS == 4
      , Y_PIN_3, Y_PIN_4
  #endif
  );
#endif
#if STEPPER_COUNT > 2
  stepper[2] = UnoStepperInterface(Z_PIN_1, Z_PIN_2
  #if Z_NUM_PINS == 4
      , Z_PIN_3, Z_PIN_4
  #endif
  );
#endif
}

/// Reset the Motherboard to its initial state.
/// This only resets the board, and does not send a reset
/// to any attached toolheads.
void Motherboard::reset() {
  setDebugMessage((char *)NULL, 0);
  indicateError(0);

  // Init steppers
  uint8_t axis_invert = eeprom::getEeprom8(eeprom::AXIS_INVERSION, 0);
  // Z holding indicates that when the Z axis is not in
  // motion, the machine should continue to power the stepper
  // coil to ensure that the Z stage does not shift.
  // Bit 7 of the AXIS_INVERSION eeprom setting
  // indicates whether or not to use z holding;
  // the bit is active low. (0 means use z holding,
  // 1 means turn it off.)
  bool hold_z = (axis_invert & (1<<7)) == 0;
  steppers::setHoldZ(hold_z);

  // Initialize the host and slave UARTs
  UART::getHostUART().enable(true);
  UART::getHostUART().in.reset();

  // TODO: These aren't done on other platforms, are they necessary?
  UART::getHostUART().reset();
  UART::getHostUART().out.reset();

  // Reset and configure timer 1, the microsecond and stepper
  // interrupt timer.
  TCCR1A = 0x00;
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A = INTERVAL_IN_MICROSECONDS * 16;
  TIMSK1 = 0x02; // turn on OCR1A match interrupt

  // Reset and configure timer 2, the debug LED flasher timer.
  TCCR2A = 0x00;
  TCCR2B = 0x07; // prescaler at 1/1024
  TIMSK2 = 0x01; // OVF flag on
  // Configure the debug pin.
  pinMode(DEBUG_PIN, OUTPUT);
}

void Motherboard::runMotherboardSlice() {
}

/// Run the Motherboard interrupt
void Motherboard::doInterrupt() {
  micros += INTERVAL_IN_MICROSECONDS;
  // Do not move steppers if the board is in a paused state
  if (command::isPaused()) return;
  steppers::doInterrupt();
}

/// Timer one comparator match interrupt
ISR(TIMER1_COMPA_vect) {
  Motherboard::getBoard().doInterrupt();
}

micros_t Motherboard::getCurrentMicros() {
  micros_t micros_snapshot;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    micros_snapshot = micros;
  }
  return micros_snapshot;
}

void Motherboard::setDebugMessage(char* message, uint8_t len) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    for (uint8_t i = 0; i < DEBUG_MSG_BUFFER_SIZE; i++) {
      if (i < len) {
        debug_message[i] = message[i];
      } else {
        debug_message[i] = 0;
      }
    }

    debug_message_len = len;
  }
}

uint8_t Motherboard::getDebugMessage(char* message) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    for (uint8_t i = 0; i < DEBUG_MSG_BUFFER_SIZE; i++) {
      if (i < debug_message_len) {
        message[i] = debug_message[i];
      } else {
        message[i] = 0;
      }
    }
    return debug_message_len;
  }
}

/// The current state of the debug LED
enum {
  BLINK_NONE,
  BLINK_ON,
  BLINK_OFF,
  BLINK_PAUSE
} blink_state = BLINK_NONE;

/// Write an error code to the debug pin.
void Motherboard::indicateError(int error_code) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    if (error_code == 0) {
      blink_state = BLINK_NONE;
      this->error_code = 0;
      digitalWrite(DEBUG_PIN, LOW);
    }
    else if (this->error_code != error_code) {
      blink_state = BLINK_OFF;
    }
    this->error_code = error_code;
  }
}

uint8_t Motherboard::getCurrentError() {
  return error_code;
}

/// Timer2 overflow cycles that the LED remains on while blinking
#define OVFS_ON 18
/// Timer2 overflow cycles that the LED remains off while blinking
#define OVFS_OFF 18
/// Timer2 overflow cycles between flash cycles
#define OVFS_PAUSE 80

/// Number of overflows remaining on the current blink cycle
int blink_ovfs_remaining = 0;
/// Number of blinks performed in the current cycle
int blinked_so_far = 0;

/// Timer 2 overflow interrupt
ISR(TIMER2_OVF_vect) {
  if (blink_ovfs_remaining > 0) {
    blink_ovfs_remaining--;
  } else {
    if (blink_state == BLINK_ON) {
      blinked_so_far++;
      blink_state = BLINK_OFF;
      blink_ovfs_remaining = OVFS_OFF;
      digitalWrite(DEBUG_PIN, LOW);
    } else if (blink_state == BLINK_OFF) {
      if (blinked_so_far >= Motherboard::getBoard().getCurrentError()) {
        blink_state = BLINK_PAUSE;
        blink_ovfs_remaining = OVFS_PAUSE;
      } else {
        blink_state = BLINK_ON;
        blink_ovfs_remaining = OVFS_ON;
        digitalWrite(DEBUG_PIN, HIGH);
      }
    } else if (blink_state == BLINK_PAUSE) {
      blinked_so_far = 0;
      blink_state = BLINK_ON;
      blink_ovfs_remaining = OVFS_ON;
      digitalWrite(DEBUG_PIN, HIGH);
    }
  }
}
