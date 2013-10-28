/*
 * StepperInterfaceImpl.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: smike
 */

#include "Configuration.hh"
#include "StepperInterface.hh"
#include "UnoStepperInterface.hh"

#include "Stepper.h"

UnoStepperInterface::UnoStepperInterface(uint8_t pin1, uint8_t pin2)
    : StepperInterface(), stepper(STEPS_PER_REV, pin1, pin2), last_step_value(false) {}

UnoStepperInterface::UnoStepperInterface(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4)
    : StepperInterface(), stepper(STEPS_PER_REV, pin1, pin2, pin3, pin4), last_step_value(false) {}

void UnoStepperInterface::setDirection(bool forward) {
  this->stepper.setDirection(forward);
}

void UnoStepperInterface::step(bool value) {
  if (value && !this->last_step_value) {
    this->stepper.stepNow();
  }

  this->last_step_value = value;
}

void UnoStepperInterface::setEnabled(bool enabled) {
  this->stepper.setHold(enabled);
}

bool UnoStepperInterface::isAtMaximum() {
  return false;
}

bool UnoStepperInterface::isAtMinimum() {
  return false;
}
