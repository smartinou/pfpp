#ifndef CORELINK_SPIMASTERDEV_H_
#define CORELINK_SPIMASTERDEV_H_
// *******************************************************************************
//
// Project: ARM Cortex-M.
//
// Module: CoreLink Peripherals.
//
// *******************************************************************************

//! \file
//! \brief CoreLink peripheral SPI device class declaration.
//! \ingroup corelink_peripherals

// ******************************************************************************
//
//        Copyright (c) 2015-2022, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************************************************

#include "inc/CoreLinkPeripheralDev.h"
#include "inc/ISPIMasterDev.h"
#include "inc/SSIGPIO.h"


namespace CoreLink {

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

//! \class SPIMasterDev
//! \brief SPI master device
class SPIMasterDev final
    : public ISPIMasterDev
    , private PeripheralDev {

public:
    [[nodiscard]] explicit SPIMasterDev(
        uint32_t aBaseAddr,
        uint32_t aClkRate,
        SSIGPIO const &aSSIGPIO
    ) noexcept;

    // ISPIMasterDev interface.
    void RdData(
        SPISlaveCfg const &aSPICfg,
        std::span<std::byte> aData,
        std::optional<std::byte> aAddr = std::nullopt
    ) const noexcept final;

    void WrData(
        SPISlaveCfg const &aSPICfg,
        std::span<std::byte const> aData,
        std::optional<std::byte> aAddr = std::nullopt
    ) const noexcept final;

    [[maybe_unused]] auto PushPullByte(std::byte aByte) const noexcept -> std::byte final;
    [[maybe_unused]] auto PushPullByte(
        SPISlaveCfg const &aSPICfg,
        std::byte aByte
    ) const noexcept -> std::byte final;

//private:
    //void SetCfg(SPISlaveCfg const &aSPISlaveCfg) const noexcept;

    //static auto ToNativeProtocol(SPISlaveCfg::tProtocol aProtocol) noexcept -> uint32_t;
};


} // namespace CoreLink

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
#endif // CORELINK_SPIMASTERDEV_H_
