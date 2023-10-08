// *****************************************************************************
//
// Project: ARM Cortex-M.
//
// Module: CoreLink Peripherals.
//
// *****************************************************************************

//! \file
//! \brief CoreLink peripheral SSI GPIO class definition.
//! \ingroup corelink_peripherals

// *****************************************************************************
//
//        Copyright (c) 2015-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This module.
#include "corelink/inc/SSIGPIO.h"

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

namespace CoreLink
{


void SSIGPIO::SetPins() const noexcept
{
    mClkPin.EnableSysCtlPeripheral();
    mRxPin.EnableSysCtlPeripheral();
    mTxPin.EnableSysCtlPeripheral();

    ROM_GPIOPinConfigure(mClkPinCfg);
    ROM_GPIOPinConfigure(mDat0PinCfg);
    ROM_GPIOPinConfigure(mDat1PinCfg);

    ROM_GPIOPinTypeSSI(mClkPin.mBaseAddr, mClkPin.mPin);
    ROM_GPIOPinTypeSSI(mRxPin.mBaseAddr, mRxPin.mPin);
    ROM_GPIOPinTypeSSI(mTxPin.mBaseAddr, mTxPin.mPin);

    // Set a weak pull-up on MISO pin for SD Card's proper operation.
    ROM_GPIOPadConfigSet(
        mRxPin.mBaseAddr,
        mRxPin.mPin,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    // Clk and Tx set for standard push-pull operation by GPIOPinTypeSSI.
    ROM_GPIOPadConfigSet(
        mClkPin.mBaseAddr,
        mClkPin.mPin,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD
    );

    ROM_GPIOPadConfigSet(
        mTxPin.mBaseAddr,
        mTxPin.mPin,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD
    );
}


} // namespace CoreLink

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
