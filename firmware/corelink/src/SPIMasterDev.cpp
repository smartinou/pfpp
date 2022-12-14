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

// This module.
#include "inc/SPIMasterDev.h"

// TI Library.
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/ssi.h>

// Standard libraries.
#include <limits>

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


SPIMasterDev::SPIMasterDev(
    uint32_t const aBaseAddr,
    uint32_t const aClkRate,
    SSIGPIO const &aSSIGPIO
) noexcept
    : PeripheralDev{aBaseAddr, aClkRate}
{
    MAP_SSIDisable(mBaseAddr);
        aSSIGPIO.SetPins();
    MAP_SSIEnable(mBaseAddr);
}


void SPIMasterDev::RdData(
    SPISlaveCfg const &aSPICfg,
    std::span<std::byte> const aData,
    std::optional<std::byte> aAddr
) const noexcept {

    // Assert the assigned CSn pin.
    aSPICfg.SetCfg(mBaseAddr, mClkRate);
    aSPICfg.AssertCSn();

    // -Send address if any.
    // -Push dummy data (0s) as many as requested to read.
    if (aAddr) PushPullByte(aAddr.value());
    for (auto &lByte : aData) {
        lByte = PushPullByte(std::byte{0});
    }

    // Deassert the assigned CSn pin.
    aSPICfg.DeassertCSn();
}


void SPIMasterDev::WrData(
    SPISlaveCfg const &aSPICfg,
    std::span<std::byte const> const aData,
    std::optional<std::byte> aAddr
) const noexcept {

    // Assert the assigned CSn pin.
    aSPICfg.SetCfg(mBaseAddr, mClkRate);
    aSPICfg.AssertCSn();

    // -Send address if any.
    // -Push data as many as requested to write.
    // -Read dummy data to empty receive register.
    // -Wait for all bytes to transmit.
    if (aAddr) PushPullByte(aAddr.value());
    for (auto const aByte : aData) {
        PushPullByte(aByte);
    }

    // Deassert the assigned CSn pin.
    aSPICfg.DeassertCSn();
}


auto SPIMasterDev::PushPullByte(std::byte const aByte) const noexcept -> std::byte {

    uint32_t lRxData{0UL};
    MAP_SSIDataPut(mBaseAddr, std::to_integer<uint32_t>(aByte));
    MAP_SSIDataGet(mBaseAddr, &lRxData);

    return static_cast<std::byte>(lRxData);
}


auto SPIMasterDev::PushPullByte(
    SPISlaveCfg const &aSPICfg,
    std::byte const aByte
) const noexcept -> std::byte {

    //SetCfg(aSPICfg);
    aSPICfg.SetCfg(mBaseAddr, mClkRate);
    return PushPullByte(aByte);
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************
#if 0
void SPIMasterDev::SetCfg(SPISlaveCfg const &aSPISlaveCfg) const noexcept {

    // Reconfigure with the newly specified config.
    MAP_SSIDisable(mBaseAddr);

    // [MG] PRIORITY INVERSION: WHY IS IT NOT SPISlaveCfg THAT PERFORMS THIS?
    SPISlaveCfg::tProtocol const lProtocol{aSPISlaveCfg.GetProtocol()};
    uint32_t const lNativeProtocol{ToNativeProtocol(lProtocol)};
    MAP_SSIConfigSetExpClk(
        mBaseAddr,
        mClkRate,
        lNativeProtocol,
        SSI_MODE_MASTER,
        aSPISlaveCfg.GetBitRate(),
        aSPISlaveCfg.GetDataWidth()
    );

    MAP_SSIEnable(mBaseAddr);
}


auto SPIMasterDev::ToNativeProtocol(
    SPISlaveCfg::tProtocol const aProtocol
) noexcept -> uint32_t {

    switch (aProtocol) {
    case SPISlaveCfg::tProtocol::MOTO_0: return SSI_FRF_MOTO_MODE_0;
    case SPISlaveCfg::tProtocol::MOTO_1: return SSI_FRF_MOTO_MODE_1;
    case SPISlaveCfg::tProtocol::MOTO_2: return SSI_FRF_MOTO_MODE_2;
    case SPISlaveCfg::tProtocol::MOTO_3: return SSI_FRF_MOTO_MODE_3;
    case SPISlaveCfg::tProtocol::TI:     return SSI_FRF_TI;
    case SPISlaveCfg::tProtocol::NMW:    return SSI_FRF_NMW;
    default: break;
    }

    return std::numeric_limits<uint32_t>::max();
}
#endif

} // namespace CoreLink

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
