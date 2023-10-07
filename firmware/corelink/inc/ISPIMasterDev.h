#ifndef CORELINK__ISPIMASTERDEV_H_
#define CORELINK__ISPIMASTERDEV_H_
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
// ******************************************************************************

// This module.
#include "corelink/inc/SPISlaveCfg.h"

// Standard libraries.
#include <cstddef>
#include <cstdint>
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


using SPIRd = void (&)(std::span<std::byte> aData, std::optional<std::byte> aAddr);
using SPIWr = void (&)(std::span<const std::byte> aData, std::optional<std::byte> aAddr);


//! \brief Interface for SPI devices.
class ISPIMasterDev
{
public:
    virtual ~ISPIMasterDev() = default;

    virtual void RdData(
        const SPISlaveCfg& aSPICfg,
        std::span<std::byte> aData,
        std::optional<std::byte> aAddr
    ) const noexcept = 0;

    virtual void WrData(
        const SPISlaveCfg& aSPICfg,
        std::span<const std::byte> aData,
        std::optional<std::byte> aAddr
    ) const noexcept = 0;

    virtual auto PushPullByte(std::byte aByte) const noexcept -> std::byte = 0;
    virtual auto PushPullByte(
        const SPISlaveCfg& aSPICfg,
        std::byte aByte
    ) const noexcept -> std::byte = 0;
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
#endif // CORELINK__ISPIMASTERDEV_H_
