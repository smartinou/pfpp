// *****************************************************************************
//
// Project: ARM Cortex-M.
//
// Module: CoreLink Peripherals.
//
// *****************************************************************************

//! \file
//! \brief CoreLink peripheral SPI device class definition.
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

// This project.
#include "corelink/inc/SPISlaveCfg.h"

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

namespace CoreLink
{


void CSnGPIO::InitCSnGPIO() const noexcept
{
    // Enable and configures the GPIO pin used for CSn.
    // The proper GPIO peripheral must be enabled using
    // SysCtlPeripheralEnable() prior to the following calls,
    // otherwise CPU will rise a HW fault.
    // Set for standard push-pull operation.
    //mCSn.EnableSysCtlPeripheral();
    MAP_GPIOPinTypeGPIOOutput(mBaseAddr, mPin);
    MAP_GPIOPadConfigSet(
        mBaseAddr,
        mPin,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD
    );

    DeassertCSn();
}


void CSnGPIO::AssertCSn() const noexcept
{
    MAP_GPIOPinWrite(mBaseAddr, mPin, (mCSPolarity == tCSPolarity::ActiveLow) ? 0 : mPin);
}


void CSnGPIO::DeassertCSn() const noexcept
{
    MAP_GPIOPinWrite(mBaseAddr, mPin, (mCSPolarity == tCSPolarity::ActiveLow) ? mPin : 0);
}


} // namespace CoreLink

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
