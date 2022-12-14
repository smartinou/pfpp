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
//        Copyright (c) 2015-2022, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This module.
#include "inc/SSIGPIO.h"

// TI Library.
#include <driverlib/gpio.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

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

namespace CoreLink {


void SSIGPIO::SetPins() const noexcept {

    mClkPin.EnableSysCtlPeripheral();
    mRxPin.EnableSysCtlPeripheral();
    mTxPin.EnableSysCtlPeripheral();

    MAP_GPIOPinConfigure(mClkPinCfg);
    MAP_GPIOPinConfigure(mDat0PinCfg);
    MAP_GPIOPinConfigure(mDat1PinCfg);

    MAP_GPIOPinTypeSSI(mClkPin.mBaseAddr, mClkPin.mPin);
    MAP_GPIOPinTypeSSI(mRxPin.mBaseAddr, mRxPin.mPin);
    MAP_GPIOPinTypeSSI(mTxPin.mBaseAddr, mTxPin.mPin);

    // Set a weak pull-up on MISO pin for SD Card's proper operation.
    // Clk and Tx set for standard push-pull operation by GPIOPinTypeSSI.
    MAP_GPIOPadConfigSet(
        mRxPin.mBaseAddr,
        mRxPin.mPin,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );
}


} // namespace CoreLink

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
