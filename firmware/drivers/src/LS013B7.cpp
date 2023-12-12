// *****************************************************************************
//
// Project: Component drivers.
//
// Module: LS013B7.
//
// *******************************************************************************

//! \file
//! \brief LCD driver class.
//! \ingroup ext_peripherals

// *****************************************************************************
//
//        Copyright (c) 2016-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This project.
#include "drivers/inc/LS013B7.h"

// STL.
#include <functional>

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

namespace Drivers
{


LS013B7::LS013B7(
    const CoreLink::SPIAssert aSPIAssert,
    const CoreLink::SPIAssert aSPIDeassert,
    const CoreLink::SPIWr aSPIWr,
    const CoreLink::PushPullByte aSPIPushPullByte
) noexcept
    : tDisplay{
        .i32Size{sizeof(LS013B7)}
        , .pvDisplayData{static_cast<void*>(this)}
        , .ui16Width{sWidth}
        , .ui16Height{sHeight}
        , .pfnPixelDraw{
            [](void * const pvDisplayData, const int32_t i32X, const int32_t i32Y, const uint32_t ui32Value) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                lThis->PixelDraw(i32X, i32Y, ui32Value);
            }
        }
        , .pfnPixelDrawMultiple{
            [](
                void * const pvDisplayData,
                const int32_t i32X, const int32_t i32Y,
                const int32_t i32X0, const int32_t i32Count,
                const int32_t i32BPP,
                const uint8_t * const pui8Data,
                const uint8_t * const pui8Palette
            ) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                lThis->PixelDrawMultiple(i32X, i32Y, i32X0, i32Count, i32BPP, pui8Data, pui8Palette);    
            }
        }
        , .pfnLineDrawH{
            [](
                void * const pvDisplayData,
                const int32_t i32X1,
                const int32_t i32X2,
                const int32_t i32Y,
                const uint32_t ui32Value
            ) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                lThis->LineDrawH(i32X1, i32X2, i32Y, ui32Value);
            }
        }
        , .pfnLineDrawV{
            [](
                void * const pvDisplayData,
                const int32_t i32X,
                const int32_t i32Y1,
                const int32_t i32Y2,
                const uint32_t ui32Value
            ) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                lThis->LineDrawV(i32X, i32Y1, i32Y2, ui32Value);
            }
        }
        , .pfnRectFill{
            [](void * const pvDisplayData, const tRectangle * const psRect, uint32_t ui32Value) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                lThis->RectFill(psRect, ui32Value);
            }
        }
        , .pfnColorTranslate{
            [](void * const pvDisplayData, const uint32_t ui32Value) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                return lThis->ColorTranslate(ui32Value);
            }
        }
        , .pfnFlush{
            [](void * const pvDisplayData) noexcept
            {
                const auto lThis{static_cast<LS013B7*>(pvDisplayData)};
                lThis->Flush();
            }
        }
    }
    , mSPIAssert{aSPIAssert}
    , mSPIDeassert{aSPIDeassert}
    , mSPIWr{aSPIWr}
    , mSPIPushPullByte{aSPIPushPullByte}
    , mImgBuf{std::byte{0}}
    , mIsLineDirty{false}
{
    // Ctor body.
}


// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

LS013B7::Line LS013B7::CreateRow(const int32_t aX1, const int32_t aX2) noexcept
{
    Line lRow{std::byte{0}};
    auto lBitIndex{aX1 % sPixelsPerByte};

    for (auto lX{aX1}; lX <= aX2; ++lX) {
        auto lByteIndex {lX / sPixelsPerByte};
        lRow[lByteIndex] |= std::byte{0x1} << lBitIndex;
        ++lBitIndex;
        if (lBitIndex >= sPixelsPerByte) {
            lBitIndex = 0;
        }
    }

    return lRow;
}


void LS013B7::PixelDraw(
    const int32_t aColumnIx,
    const int32_t aRowIx,
    [[maybe_unused]] const uint32_t aColor
) noexcept
{
    auto& lRow{mImgBuf[aRowIx]};
    const auto lByteIndex{aColumnIx / sPixelsPerByte};
    const auto lBitIndex{aColumnIx % sPixelsPerByte};
    lRow[lByteIndex] |= std::byte{0x1} << lBitIndex;

    mIsLineDirty[aRowIx] = true;
}


void LS013B7::PixelDrawMultiple(
    [[maybe_unused]] const int32_t i32X, [[maybe_unused]] const int32_t i32Y,
    [[maybe_unused]] const int32_t i32X0, [[maybe_unused]] const int32_t i32Count,
    [[maybe_unused]] const int32_t i32BPP,
    [[maybe_unused]] const uint8_t * const pui8Data,
    [[maybe_unused]] const uint8_t * const pui8Palette
) noexcept
{
    // TODO: COMPLETE.
}


void LS013B7::LineDrawH(
    const int32_t aX1,
    const int32_t aX2,
    const int32_t aRowIx,
    [[maybe_unused]] const uint32_t aColor
) noexcept
{
    const auto lNewRow{CreateRow(aX1, aX2)};
    auto& lRow{mImgBuf[aRowIx]};
    std::transform(
        lNewRow.cbegin(), lNewRow.cend(),
        lRow.cbegin(), lRow.begin(),
        std::bit_or<>{}
    );

    mIsLineDirty[aRowIx] = true;
}


void LS013B7::LineDrawV(
    const int32_t aColumnIx,
    const int32_t aY1,
    const int32_t aY2,
    [[maybe_unused]] const uint32_t ui32Value
) noexcept
{
    const auto lByteIx{aColumnIx / sPixelsPerByte};
    const auto lBitIx{aColumnIx % sPixelsPerByte};
    for (auto lRowIx{aY1}; lRowIx <= aY2; ++lRowIx) {
        auto& lRow{mImgBuf[lRowIx]};
        lRow[lByteIx] |= std::byte{0x1} << lBitIx;
        mIsLineDirty[lRowIx] = true;
    }
}


void LS013B7::RectFill(const tRectangle * const aRectangle, [[maybe_unused]] const uint32_t ui32Value) noexcept
{
    // Fill all the horizontal lines.
    // Send all lines to display.
    const auto lNewRow{CreateRow(aRectangle->i16XMin, aRectangle->i16XMax)};
    for (auto lRowIx{aRectangle->i16YMin}; lRowIx <= aRectangle->i16YMax; ++lRowIx) {
        auto& lRow{mImgBuf[lRowIx]};
        std::transform(
            lNewRow.cbegin(), lNewRow.cend(),
            lRow.cbegin(), lRow.begin(),
            std::bit_or<>{}
        );
        mIsLineDirty[lRowIx] = true;
    }
}


auto LS013B7::ColorTranslate([[maybe_unused]] const uint32_t ui32Value) noexcept -> uint32_t
{
    return 0;
}


void LS013B7::Flush() noexcept
{
    auto lRowIx{0};
    for (auto& lIsDirty : mIsLineDirty) {
        if (lIsDirty) {
            SetDataUpdateMode(lRowIx, mImgBuf[lRowIx]);
            lIsDirty = false;
        }
        ++lRowIx;
    }
    SetDisplayMode();
}


void LS013B7::DisplayOn() noexcept
{
    // Set GPIO.
}


void LS013B7::DisplayOff() noexcept
{
    // Set GPIO.
}


void LS013B7::SetDisplayMode() noexcept
{
    // 6-5-3 Display Mode
    // Maintains memory internal data (maintains current display). (M0=”L”, M2＝”L”)
    static constexpr std::array sDisplayModeCmd{std::byte{0x0}, std::byte{0x0}};

    mSPIAssert();
    mSPIWr(std::span{sDisplayModeCmd}, std::nullopt);
    mSPIDeassert();
}


void LS013B7::SetAllClrMode() noexcept
{
    // 6-5-4 All Clear Mode
    // Clears memory internal data and writes white on screen. (M0=”L”, M2＝”H”)
    static constexpr std::array sClrCmd{std::byte{0x1 << 2}, std::byte{}};

    mSPIAssert();
    mSPIWr(std::span{sClrCmd}, std::nullopt);
    mSPIDeassert();

    std::fill(mImgBuf.begin(), mImgBuf.end(), Line{});
}


void LS013B7::SetDataUpdateMode(const uint8_t aRow, std::span<const std::byte> aSpan) noexcept
{
    // 6-5-1 Data update mode (1 line).
    // Updates data of only one specified line. (M0=”H”, M2＝”L”)
    static constexpr std::array sDataUpdateModeCmd{std::byte{0x1}};

    // cmd, gateline, data, dummy bytes(2).
    mSPIAssert();

    mSPIPushPullByte(sDataUpdateModeCmd[0]);
    mSPIPushPullByte(sDataUpdateModeCmd[1]);
    mSPIPushPullByte(std::byte{static_cast<uint8_t>(aRow + 1)});
    mSPIPushPullByte(std::byte{0x00});
    mSPIWr(aSpan, std::nullopt);

    mSPIDeassert();
}

// TODO: COMPLETE.
void LS013B7::SetDataUpdateModeMultiple(const uint8_t aStartGateLineAddress) noexcept
{
    // 6-5-2 ) Data Update Mode (Multiple Lines)
    // Updates arbitrary multiple lines data. (M0=”H”, M2＝”L”)
    static constexpr std::array sDataUpdateModeCmd{std::byte{0x1}};

    // cmd, gateline, data,
    // dummy (1), gateline, data
    // ...
    // dummy (1), gateline, data, dummy(2).
    mSPIAssert();

    mSPIPushPullByte(sDataUpdateModeCmd[0]);
    mSPIPushPullByte(sDataUpdateModeCmd[1]);
    mSPIPushPullByte(std::byte{static_cast<uint8_t>(aStartGateLineAddress + 1)});
    mSPIPushPullByte(std::byte{0x00});

    mSPIDeassert();
}


} // namespace Drivers

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
