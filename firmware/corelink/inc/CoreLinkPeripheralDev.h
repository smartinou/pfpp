#pragma once
// *******************************************************************************
//
// Project: ARM Cortex-M.
//
// Module: CoreLink Peripherals.
//
// *******************************************************************************

//! \file
//! \brief Generic CoreLink peripheral device base class.
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

// Standard Libraries.
#include <array>
#include <cstdint>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace CoreLink
{


struct PeripheralBase
{
    uint32_t mBaseAddr {};
    uint32_t mClkRate {};
};


struct tPeripheralIDs
{
    uint32_t mPeripheralID {};
    uint32_t mPrimeCellID {};
};

[[nodiscard]] inline auto GetPeripheralID(const uint32_t aBaseAddr) noexcept -> tPeripheralIDs
{
    struct tIDRegMap
    {
        std::array<uint32_t, 4> mPeripheralID {};
        std::array<uint32_t, 4> mPrimeCellID {};
    };
    static constexpr auto sIDRegOffset {0x0FE};
    const auto lIDRegMap {reinterpret_cast<const tIDRegMap*>(aBaseAddr + sIDRegOffset)};
    const uint32_t lPeripheralID {
        (lIDRegMap->mPeripheralID[0] <<  0)
        | (lIDRegMap->mPeripheralID[1] <<  8)
        | (lIDRegMap->mPeripheralID[2] << 16)
        | (lIDRegMap->mPeripheralID[3] << 24)
    };
    const uint32_t lPrimeCellID {
        (lIDRegMap->mPrimeCellID[0] <<  0)
        | (lIDRegMap->mPrimeCellID[1] <<  8)
        | (lIDRegMap->mPrimeCellID[2] << 16)
        | (lIDRegMap->mPrimeCellID[3] << 24)
    };

    return { lPeripheralID, lPrimeCellID };
}


#if 0
//! \class PeripheralDev
//! \brief CoreLink Peripheral Device base class.
//!
//! This is the base class from which all CoreLink peripheral device drivers
//! are derived.
//!

// [MG] EN+ CA PEUT PAS ETRE CONSTEXPR A CAUSE DU reinterpret_cast?
// [MG] C'EST BIEN CUTE, MAIS C'EST TU BEN PRATIQUE.
// [MG] VAUT PEUT-ETRE LA PEINE DE SACRIFIER CALCUL PERIPHERAL ID POUR constexpr.
// [MG] WHO GIVES A SHIT?
class PeripheralDev
    : private PeripheralBase {
protected:
    [[nodiscard]] constexpr explicit PeripheralDev(
        uint32_t const aBaseAddr,
        uint32_t const aClkRate
    ) noexcept
        : PeripheralBase{.mBaseAddr = aBaseAddr}
        , mClkRate{aClkRate}
#if 0
        , mPeripheralID{
            [aBaseAddr]() -> uint32_t {
                auto const lIDRegMap =
                    reinterpret_cast<tIDRegMap const *>(aBaseAddr + 0x0FE0);
                uint32_t const lID{
                    (lIDRegMap->mPeripheralID[0] <<  0)
                    | (lIDRegMap->mPeripheralID[1] <<  8)
                    | (lIDRegMap->mPeripheralID[2] << 16)
                    | (lIDRegMap->mPeripheralID[3] << 24)
                };
                return lID;
            } ()
        }
        , mPrimeCellID{
            [aBaseAddr]() -> uint32_t {
                auto const lIDRegMap =
                    reinterpret_cast<tIDRegMap const *>(aBaseAddr + 0x0FE0);
                uint32_t const lID{
                    (lIDRegMap->mPrimeCellID[0] <<  0)
                    | (lIDRegMap->mPrimeCellID[1] <<  8)
                    | (lIDRegMap->mPrimeCellID[2] << 16)
                    | (lIDRegMap->mPrimeCellID[3] << 24)
                };
                return lID;
            } ()
        }
   #endif
        {}

protected:
    [[nodiscard]] constexpr auto GetBaseAddr() const noexcept -> uint32_t {return mBaseAddr;}
    [[nodiscard]] constexpr auto GetClkRate() const noexcept -> uint32_t {return mClkRate;}

    // [MG] MAKE THOSE 2 NON CONSTEXPR.
    [[nodiscard]] constexpr auto GetPeripheralID() const noexcept -> uint32_t {return mPeripheralID;}
    [[nodiscard]] constexpr auto GetPrimCellID() const noexcept -> uint32_t {return mPrimeCellID;}

    virtual ~PeripheralDev() = default;

private:
    struct tIDRegMap {
        std::array<uint32_t, 4> mPeripheralID{};
        std::array<uint32_t, 4> mPrimeCellID{};
    };

    uint32_t mClkRate{};
    uint32_t mPeripheralID{};
    uint32_t mPrimeCellID{};
};
#endif

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
