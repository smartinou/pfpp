#ifndef CORELINK__SPISLAVECFG_H_
#define CORELINK__SPISLAVECFG_H_
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

// Corelink library.
#include "corelink/inc/GPIO.h"

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace CoreLink
{

struct CSnGPIO final
    : public GPIO
{
    enum class tCSPolarity
    {
        ActiveHigh,
        ActiveLow
    };

    void InitCSnGPIO() const noexcept;
    void AssertCSn() const noexcept;
    void DeassertCSn() const noexcept;

    tCSPolarity mCSPolarity{tCSPolarity::ActiveLow};
};


struct SPISlaveCfg final
{
    enum class tProtocol
    {
        MOTO_0,
        MOTO_1,
        MOTO_2,
        MOTO_3,
        TI,
        NMW
    };

    bool operator==(const SPISlaveCfg& aCfg) const noexcept
    {
        return ((this->mProtocol == aCfg.mProtocol)
            && (this->mBitRate == aCfg.mBitRate)
            && (this->mDataWidth == aCfg.mDataWidth));
    }

    bool operator!=(const SPISlaveCfg& aCfg) const noexcept
    {
        return !(*this == aCfg);
    }

    tProtocol mProtocol{tProtocol::MOTO_0};
    unsigned int mBitRate{0};

    static constexpr auto sDfltDataWidth{8};
    unsigned int mDataWidth{sDfltDataWidth};

    CSnGPIO mCSn{};
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
#endif // CORELINK__SPISLAVECFG_H_
