#ifndef CORELINK_ISPIMASTERDEV_H_
#define CORELINK_ISPIMASTERDEV_H_
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

// This module.
#include "SPISlaveCfg.h"

// Standard Libraries.
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

namespace CoreLink {


//! \brief Interface for SPI devices.
class ISPIMasterDev {
public:
    virtual ~ISPIMasterDev() = default;

    virtual void RdData(
        SPISlaveCfg const &aSPICfg,
        std::span<std::byte> aData,
        std::optional<std::byte> aAddr
    ) const = 0;

    virtual void WrData(
        SPISlaveCfg const &aSPICfg,
        std::span<std::byte const> aData,
        std::optional<std::byte> aAddr
    ) const = 0;

    [[maybe_unused]] virtual auto PushPullByte(std::byte aByte) const -> std::byte = 0;
    [[maybe_unused]] virtual auto PushPullByte(
        SPISlaveCfg const &aSPICfg,
        std::byte aByte
    ) const -> std::byte = 0;
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
#endif // CORELINK_ISPIMASTERDEV_H_
