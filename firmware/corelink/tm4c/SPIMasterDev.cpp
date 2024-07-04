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

// This module.
#include "corelink/inc/SPIMasterDev.h"

// TI Library.
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

namespace CoreLink
{

void SPIMasterDev::RdData(
    const SPISlaveCfg& aSPICfg,
    const std::span<std::byte> aData,
    std::optional<std::byte> aAddr
) const noexcept
{
    // Assert the assigned CSn pin.
    SetCfg(aSPICfg);
    aSPICfg.mCSn.AssertCSn();

    // -Send address if any.
    // -Push dummy data (0s) as many as requested to read.
    if (aAddr)
    {
        [[maybe_unused]] const auto lByte{PushPullByte(aAddr.value())};
    }

    for (auto& lByte : aData) {
        lByte = PushPullByte(std::byte{0});
    }

    // Deassert the assigned CSn pin.
    aSPICfg.mCSn.DeassertCSn();
}


void SPIMasterDev::WrData(
    const SPISlaveCfg& aSPICfg,
    const std::span<const std::byte> aData,
    std::optional<std::byte> aAddr
) const noexcept
{
    // Assert the assigned CSn pin.
    SetCfg(aSPICfg);
    aSPICfg.mCSn.AssertCSn();

    // -Send address if any.
    // -Push data as many as requested to write.
    // -Read dummy data to empty receive register.
    // -Wait for all bytes to transmit.
    if (aAddr)
    {
        [[maybe_unused]] const auto lByte{PushPullByte(aAddr.value())};
    }

    for (const auto aByte : aData) {
        [[maybe_unused]] const auto lByte{PushPullByte(aByte)};
    }

    // Deassert the assigned CSn pin.
    aSPICfg.mCSn.DeassertCSn();
}


auto SPIMasterDev::PushPullByte(const std::byte aByte) const noexcept -> std::byte
{
    uint32_t lRxData{0UL};
    MAP_SSIDataPut(mBaseAddr, std::to_integer<uint32_t>(aByte));
    MAP_SSIDataGet(mBaseAddr, &lRxData);

    return static_cast<std::byte>(lRxData);
}


auto SPIMasterDev::PushPullByte(
    const SPISlaveCfg& aSPICfg,
    const std::byte aByte
) const noexcept -> std::byte
{
    SetCfg(aSPICfg);
    return PushPullByte(aByte);
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

void SPIMasterDev::SetCfg(const SPISlaveCfg& aSPICfg) const noexcept
{
    // Test the specified config. Matches the last one used?
    if (aSPICfg != mCachedSPISlaveCfg) {
        MAP_SSIDisable(mBaseAddr);

        const auto lNativeProtocol{static_cast<uint32_t>(aSPICfg.mProtocol)};
        MAP_SSIConfigSetExpClk(
            mBaseAddr,
            mClkRate,
            lNativeProtocol,
            SSI_MODE_MASTER,
            aSPICfg.mBitRate,
            aSPICfg.mDataWidth
        );

        MAP_SSIEnable(mBaseAddr);
        mCachedSPISlaveCfg = aSPICfg;
    }
}


} // namespace CoreLink

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
