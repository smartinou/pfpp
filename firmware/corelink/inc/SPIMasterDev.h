#ifndef CORELINK__SPIMASTERDEV_H_
#define CORELINK__SPIMASTERDEV_H_
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
//        Copyright (c) 2015-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************

// CoreLink library.
#include "corelink/inc/CoreLinkPeripheralDev.h"
#include "corelink/inc/SPISlaveCfg.h"
#include "corelink/inc/SSIGPIO.h"

// Standard libraries.
#include <cstddef>
#include <optional>
#include <span>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace CoreLink
{


//! \class SPIMasterDev
//! \brief SPI master device
class SPIMasterDev final
    : private PeripheralBase
{
public:
    [[nodiscard]] explicit constexpr SPIMasterDev(
        const uint32_t aBaseAddr,
        const uint32_t aClkRate
    ) noexcept
        :  PeripheralBase{aBaseAddr, aClkRate}
    {/* Ctor body. */}

    // ISPIMasterDev interface.
    void RdData(
        const SPISlaveCfg& aSPICfg,
        std::span<std::byte> aData,
        std::optional<std::byte> aAddr = std::nullopt
    ) const noexcept;

    void WrData(
        const SPISlaveCfg& aSPICfg,
        std::span<const std::byte> aData,
        std::optional<std::byte> aAddr = std::nullopt
    ) const noexcept;

    [[maybe_unused]] auto PushPullByte(std::byte aByte) const noexcept
        -> std::byte;
    [[maybe_unused]] auto PushPullByte(
        const SPISlaveCfg& aSPICfg,
        std::byte aByte
    ) const noexcept -> std::byte;

private:
    void SetCfg(const SPISlaveCfg& aSPICfg) const noexcept;

    mutable SPISlaveCfg mCachedSPISlaveCfg;
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
#endif // CORELINK__SPIMASTERDEV_H_
