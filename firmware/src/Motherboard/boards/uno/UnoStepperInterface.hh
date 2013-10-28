/*
 * StepperInterfaceImpl.h
 *
 *  Created on: Jan 19, 2012
 *      Author: smike
 */

#ifndef UNOSTEPPERINTERFACE_H_
#define UNOSTEPPERINTERFACE_H_

#include "Stepper.h"
#include "StepperInterface.hh"

class UnoStepperInterface : StepperInterface {
public:
  /// Set the direction for the stepper to move
      /// \param[in] forward True to move the stepper forward, false otherwise.
  void setDirection(bool forward);

  /// Set the value of the step line
      /// \param[in] value True to enable, false to disable. This should be toggled
      ///                  back and fourth to effect stepping.
  void step(bool value);

  /// Enable or disable the stepper motor on this axis
      /// \param[in] True to enable the motor
  void setEnabled(bool enabled);

  /// Check if the maximum endstop has been triggered for this axis.
      /// \return True if the axis has triggered its maximum endstop
  bool isAtMaximum();

  /// Check if the minimum endstop has been triggered for this axis.
      /// \return True if the axis has triggered its minimum endstop
  bool isAtMinimum();

private:
  UnoStepperInterface() : StepperInterface(), stepper(Stepper()), last_step_value(false) {}
  UnoStepperInterface(uint8_t pin1, uint8_t pin2);
  UnoStepperInterface(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);

  Stepper stepper;
  bool last_step_value;

  friend class Motherboard;
};

#endif /* UNOSTEPPERINTERFACE_H_ */
