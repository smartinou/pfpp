// *****************************************************************************
//
// Project: Component drivers.
//
// Module: TB6612.
//
// *******************************************************************************

//! \file
//! \brief TB6612Port class.
//! \ingroup ext_peripherals

// *****************************************************************************
//
//        Copyright (c) 2016-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This project.
#include "drivers/inc/TB6612.h"

// TI Library.
#include <corelink/tm4c/gpio.h>
#include <corelink/tm4c/rom.h>

// *****************************************************************************
//                      DEFINED CONSTANTS AND MACROS
// *****************************************************************************

// *****************************************************************************
//                         TYPEDEFS AND STRUCTURES
// *****************************************************************************

// *****************************************************************************
//                            FUNCTION PROTOTYPES
// *****************************************************************************

// *****************************************************************************
//                             GLOBAL VARIABLES
// *****************************************************************************

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

namespace Drivers
{


TB6612Port::TB6612Port(
    const CoreLink::GPIO& aIn1,
    const CoreLink::GPIO& aIn2,
    const CoreLink::GPIO& aPWM,
    const CoreLink::GPIO& aStby
) noexcept
    : mIn1{aIn1}
    , mIn2{aIn2}
    , mPWM{aPWM}
{
    // In1.
    mIn1.EnableSysCtlPeripheral();
    ROM_GPIOPinTypeGPIOOutput(mIn1.mBaseAddr, mIn1.mPin);
    ROM_GPIODirModeSet(mIn1.mBaseAddr, mIn1.mPin, GPIO_DIR_MODE_OUT);
    ROM_GPIOPadConfigSet(
        mIn1.mBaseAddr,
        mIn1.mPin,
        GPIO_STRENGTH_4MA,
        GPIO_PIN_TYPE_STD
    );

    // In2.
    mIn2.EnableSysCtlPeripheral();
    ROM_GPIOPinTypeGPIOOutput(mIn2.mBaseAddr, mIn2.mPin);
    ROM_GPIODirModeSet(mIn2.mBaseAddr, mIn2.mPin, GPIO_DIR_MODE_OUT);
    ROM_GPIOPadConfigSet(
        mIn2.mBaseAddr,
        mIn2.mPin,
        GPIO_STRENGTH_4MA,
        GPIO_PIN_TYPE_STD
    );

    // PWM.
    mPWM.EnableSysCtlPeripheral();
    ROM_GPIOPinTypeGPIOOutput(mPWM.mBaseAddr, mPWM.mPin);
    ROM_GPIODirModeSet(mPWM.mBaseAddr, mPWM.mPin, GPIO_DIR_MODE_OUT);
    ROM_GPIOPadConfigSet(
        mPWM.mBaseAddr,
        mPWM.mPin,
        GPIO_STRENGTH_4MA,
        GPIO_PIN_TYPE_STD
    );

    // Stby.
    mStby.EnableSysCtlPeripheral();
    ROM_GPIOPinTypeGPIOOutput(mStby.mBaseAddr, mStby.mPin);
    ROM_GPIODirModeSet(mStby.mBaseAddr, mStby.mPin, GPIO_DIR_MODE_OUT);
    ROM_GPIOPadConfigSet(
        mStby.mBaseAddr,
        mStby.mPin,
        GPIO_STRENGTH_4MA,
        GPIO_PIN_TYPE_STD
    );
    ROM_GPIOPinWrite(mStby.mBaseAddr, mStby.mPin, mStby.mPin);
}


void TB6612Port::TurnOnCW([[maybe_unused]] const unsigned int aDutyCycle) const noexcept
{
    // PWM: H
    // In1: H
    // In2: L
    // Stby: H
    //MAP_PWMGenEnable(PWM_BASE, PWM_GEN_1);
    ROM_GPIOPinWrite(mPWM.mBaseAddr, mPWM.mPin, mPWM.mPin);
    ROM_GPIOPinWrite(mIn1.mBaseAddr, mIn1.mPin, mIn1.mPin);
    ROM_GPIOPinWrite(mIn2.mBaseAddr, mIn2.mPin, 0);
}


void TB6612Port::TurnOnCCW([[maybe_unused]] const unsigned int aDutyCycle) const noexcept
{
    // PWM: H
    // In1: L
    // In2: H
    // Stby: H
    ROM_GPIOPinWrite(mPWM.mBaseAddr, mPWM.mPin, mPWM.mPin);
    //MAP_PWMGenEnable(PWM_BASE, PWM_GEN_1);
    ROM_GPIOPinWrite(mIn1.mBaseAddr, mIn1.mPin, 0);
    ROM_GPIOPinWrite(mIn2.mBaseAddr, mIn2.mPin, mIn2.mPin);
}


void TB6612Port::TurnOff() const noexcept
{
    // PWM: H
    // In1: L
    // In2: L
    // Stby: H
    //MAP_PWMGenDisable(PWM_BASE, PWM_GEN_1);
    ROM_GPIOPinWrite(mPWM.mBaseAddr, mPWM.mPin, mPWM.mPin);
    ROM_GPIOPinWrite(mIn1.mBaseAddr, mIn1.mPin, 0);
    ROM_GPIOPinWrite(mIn2.mBaseAddr, mIn2.mPin, 0);
}


} // namespace Drivers

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
