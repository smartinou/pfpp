#ifndef CORELINK_SPISLAVECFG_H_
#define CORELINK_SPISLAVECFG_H_
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

#include "inc/GPIO.h"

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace CoreLink {


class SPISlaveCfg final {
public:
    enum class Protocol_s {
        MOTO_0 = 0,
        MOTO_1,
        MOTO_2,
        MOTO_3,
        TI,
        NMW
    };

    using tProtocol = enum class Protocol_s;
    [[nodiscard]] constexpr explicit SPISlaveCfg(
        GPIO const &aGPIO,
        tProtocol const aProtocol,
        unsigned int const aBitRate,
        unsigned int const aDataWidth
    ) noexcept
        : mProtocol{aProtocol}
        , mBitRate{aBitRate}
        , mDataWidth{aDataWidth}
        , mCSnGPIO{aGPIO} {}

    //constexpr void SetBitRate(unsigned int const aBitRate) noexcept {mBitRate = aBitRate;}
    //[[nodiscard]] constexpr auto GetProtocol() const noexcept -> tProtocol {return mProtocol;}
    //[[nodiscard]] constexpr auto GetBitRate() const noexcept -> unsigned int {return mBitRate;}
    //[[nodiscard]] constexpr auto GetDataWidth() const noexcept -> unsigned int {return mDataWidth;}

    void InitCSnGPIO() const noexcept;

    void AssertCSn() const noexcept;
    void DeassertCSn() const noexcept;
    void SetCfg(uint32_t aBaseAddr, uint32_t aClkRate) const noexcept;

private:
    tProtocol mProtocol{tProtocol::MOTO_0};
    uint32_t mBitRate{0};

    static constexpr auto sDfltDataWidth{8};
    uint32_t mDataWidth{sDfltDataWidth};
    GPIO mCSnGPIO{};
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
#endif // CORELINK_SPISLAVECFG_H_
