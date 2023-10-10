#ifndef DRIVERS__TB6612_H_
#define DRIVERS__TB6612_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: TB6612.
//
// *******************************************************************************

//! \file
//! \brief TB6612 class.
//! \ingroup ext_peripherals

// ******************************************************************************
//
//        Copyright (c) 2016-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************************************************

#include "drivers/inc/IMotorControl.h"
#include "corelink/inc/GPIO.h"

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Drivers
{


//! \brief TB6612Port motor controller.
// Creates a single port (half) of a motor controller.
class TB6612Port final
    : public IMotorControl
{
public:
    [[nodiscard]] explicit TB6612Port(
        const CoreLink::GPIO& aIn1,
        const CoreLink::GPIO& aIn2,
        const CoreLink::GPIO& aPWM
    ) noexcept;

    // IMotorController interface.
    void TurnOnCW(unsigned int aDutyCycle = 100) const noexcept override;
    void TurnOnCCW(unsigned int aDutyCycle = 100) const noexcept override;
    void TurnOff() const noexcept override;

 private:
    CoreLink::GPIO mIn1{};
    CoreLink::GPIO mIn2{};
    CoreLink::GPIO mPWM{};
};


//! \brief TB6612 motor controller.
// Creates one motor controller instance.
// A physical TB6612 has two control instances (A & B).
struct TB6612 final
{
    TB6612Port mPortA;
    TB6612Port mPortB;
};


} // namespace Drivers

// ******************************************************************************
//                            EXPORTED VARIABLES
// ******************************************************************************

// ******************************************************************************
//                                 EXTERNS
// ******************************************************************************

// ******************************************************************************
//                            EXPORTED FUNCTIONS
// ******************************************************************************

// ******************************************************************************
//                                END OF FILE
// ******************************************************************************
#endif // DRIVERS__TB6612_H_
