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
//        Copyright (c) 2015-2022, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This project.
#include "inc/SPISlaveCfg.h"

// TI Library.
#include <driverlib/gpio.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/ssi.h>

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


void SPISlaveCfg::InitCSnGPIO() const noexcept {

    // Enable and configures the GPIO pin used for CSn.
    // The proper GPIO peripheral must be enabled using
    // SysCtlPeripheralEnable() prior to the following calls,
    // otherwise CPU will rise a HW fault.
    // Set for standard push-pull operation.
    mCSnGPIO.EnableSysCtlPeripheral();
    MAP_GPIOPinTypeGPIOOutput(mCSnGPIO.mBaseAddr, mCSnGPIO.mPin);
    MAP_GPIOPadConfigSet(
        mCSnGPIO.mBaseAddr,
        mCSnGPIO.mPin,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD
    );

    DeassertCSn();
}


void SPISlaveCfg::AssertCSn() const noexcept {

    MAP_GPIOPinWrite(mCSnGPIO.mBaseAddr, mCSnGPIO.mPin, 0);
}


void SPISlaveCfg::DeassertCSn() const noexcept {

    MAP_GPIOPinWrite(mCSnGPIO.mBaseAddr, mCSnGPIO.mPin, mCSnGPIO.mPin);
}


void SPISlaveCfg::SetCfg(uint32_t const aBaseAddr, uint32_t const aClkRate) const noexcept {

    // [MG] ICI, PLUSSE POSSIBLE DE GATER SI LAST = THIS???
    // Reconfigure with the newly specified config.
    MAP_SSIDisable(aBaseAddr);

    // [MG] PRIORITY INVERSION: WHY IS IT NOT SPISlaveCfg THAT PERFORMS THIS?
    //SPISlaveCfg::tProtocol const lProtocol{aSPISlaveCfg.GetProtocol()};
    //uint32_t const lNativeProtocol{ToNativeProtocol(lProtocol)};
    auto const lNativeProtocol{static_cast<uint32_t>(mProtocol)};
    MAP_SSIConfigSetExpClk(
        aBaseAddr,
        aClkRate,
        lNativeProtocol,
        SSI_MODE_MASTER,
        mBitRate,
        mDataWidth
    );

    MAP_SSIEnable(aBaseAddr);
}




} // namespace CoreLink

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
