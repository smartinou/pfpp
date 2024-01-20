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
#include <array>
#include <cstddef>
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

static constexpr std::array sSwappedNibbleLookup{
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
};

static constexpr auto BitSwap(const uint8_t aByte) noexcept
{
    return std::byte(
        static_cast<uint8_t>(
            sSwappedNibbleLookup[aByte & 0b1111] << 4 |
            sSwappedNibbleLookup[aByte >> 4]
        )
    );
}

static constexpr auto ToGateLine(const uint8_t aRowIndex) noexcept
{
    const auto lGateLineIndex{aRowIndex + 1};
    return BitSwap(lGateLineIndex);
}

template<const size_t aSize>
static constexpr auto FillGateLineLookup() noexcept
{
    std::array<std::byte, aSize> lGateLineLookup;
    for (auto i = decltype(aSize){0}; i < aSize; ++i) {
        lGateLineLookup[i] = ToGateLine(i);
    }

    return lGateLineLookup;
}

// *****************************************************************************
//                             GLOBAL VARIABLES
// *****************************************************************************

static constexpr auto sGateLineLookup{FillGateLineLookup<128>()};

static constexpr auto sSize{256};
static constexpr auto sBitSwapLookup{
    []() noexcept
    {
        std::array<std::byte, sSize> lBitSwapLookup;
        for (auto i{0}; i < sSize; ++i) {
            lBitSwapLookup[i] = ~BitSwap(i);
        }
        return lBitSwapLookup;
    } ()
};

// 6-5-1 ) Data update mode (1 line).
// 6-5-2 ) Data Update Mode (Multiple Lines)
// Updates data of only one specified line. (M0=”H”, M2＝”L”)
static constexpr std::byte sDataUpdateModeCmd{0x1 << 7};

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

namespace Drivers
{


LS013B7::LS013B7(
    const CoreLink::SPIAssert aSPIAssert,
    const CoreLink::SPIAssert aSPIDeassert,
    const CoreLink::SPIWr aSPIWr,
    const CoreLink::PushPullByte aSPIPushPullByte,
    GPIOOnOff aGPIODisplayOn,
    GPIOOnOff aGPIODisplayOff
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
    , mGPIODisplayOn{aGPIODisplayOn}
    , mGPIODisplayOff{aGPIODisplayOff}
    , mImgBuf{std::byte{0}}
    , mIsLineDirty{false}
{
    // Ctor body.
}


void LS013B7::Init() noexcept
{
    SetAllClrMode();
}


void LS013B7::DisplayOn() const noexcept
{
    mGPIODisplayOn();
}


void LS013B7::DisplayOff() const noexcept
{
    mGPIODisplayOff();
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

LS013B7::Line LS013B7::CreateRow(const int32_t aX1, const int32_t aX2, bool aIsActive) noexcept
{
    Line lRow{std::byte{0}};
    auto lBitIndex{aX1 % sPixelsPerByte};

    for (auto lX{aX1}; lX <= aX2; ++lX) {
        auto lByteIndex {lX / sPixelsPerByte};
        lRow[lByteIndex] |= std::byte{aIsActive} << lBitIndex;
        ++lBitIndex;
        if (lBitIndex >= sPixelsPerByte) {
            lBitIndex = 0;
        }
    }

    return lRow;
}


void LS013B7::PixelDraw(
    const int32_t aColumnIndex,
    const int32_t aRowIndex,
    const uint32_t aColor
) noexcept
{
    auto& lRow{mImgBuf[aRowIndex]};
    const auto lByteIndex{aColumnIndex / sPixelsPerByte};
    const auto lBitIndex{aColumnIndex % sPixelsPerByte};
    const auto lColorByte{std::byte{static_cast<uint8_t>(aColor ? 0x1 : 0x0)} << lBitIndex};
    lRow[lByteIndex] = (lRow[lByteIndex] & ~std::byte{static_cast<uint8_t>(0x1 << lBitIndex)}) | lColorByte;

    mIsLineDirty[aRowIndex] = true;
}


void LS013B7::PixelDrawMultiple(
    const int32_t aColumnIx, const int32_t aRowIx,
    const int32_t i32X0, const int32_t aPixelCount,
    const int32_t aBitsPerPixel,
    const uint8_t * const aSourceData,
    const uint8_t * const aColorPalette
) noexcept
{
    auto& lRow{mImgBuf[aRowIx]};
    const auto lByteIndex{aColumnIx / sPixelsPerByte};
    const auto lBitIndex{aColumnIx % sPixelsPerByte};

    switch (aBitsPerPixel & 0xFF)
    {
        case 1: PixelDrawMultiple1BPP(lRow, lByteIndex, lBitIndex, i32X0, aPixelCount, aSourceData, aColorPalette); break;
        case 4: break;
        case 8: PixelDrawMultiple8BPP(lRow, lByteIndex, lBitIndex, i32X0, aPixelCount, aSourceData, aColorPalette); break;
        default: // Invalid number of pixels per byte.
            break;
    }
}


void LS013B7::PixelDrawMultiple1BPP(
    Line& aRow,
    const uint32_t aByteIndex,
    const uint32_t aBitIndex,
    const uint32_t aSourceBitIndex,
    const int32_t aPixelCount,
    const uint8_t *aSourceData,
    const uint8_t *aColorPalette
) noexcept
{
    auto lByteIndex{aByteIndex};
    auto lBitIndex{static_cast<uint8_t>(aBitIndex)};
    auto lSourceBitIndex{static_cast<uint8_t>(aSourceBitIndex)};
    auto lPixelCount{aPixelCount};
    auto lSourceData{aSourceData};
    while (lPixelCount) {
        const std::byte lPixelByte{*lSourceData};

        for (; (lSourceBitIndex < 8) && lPixelCount; ++lSourceBitIndex, --lPixelCount) {

            const auto lColorByte{std::byte{static_cast<uint8_t>(1 << lBitIndex)}};
            const auto lColorIndex{std::to_integer<unsigned int>((lPixelByte >> (7 - lSourceBitIndex)) & std::byte{1})};
            const auto lColorValue{
                std::byte{static_cast<uint8_t>(((reinterpret_cast<const uint32_t*>(aColorPalette)[lColorIndex]) << lBitIndex))}
            };

            aRow[lByteIndex] = (aRow[lByteIndex] & ~lColorByte) | lColorValue;

            if (--lBitIndex == 0) {
                lBitIndex = 7;
                ++lSourceData;
            }
        }

        lSourceBitIndex = 0;
        ++lByteIndex;
    }
}



void LS013B7::PixelDrawMultiple8BPP(
    Line& aRow,
    const uint32_t aByteIndex,
    const uint32_t aBitIndex,
    const uint32_t aSourceBitIndex,
    const int32_t aPixelCount,
    const uint8_t *aSourceData,
    const uint8_t *aColorPalette
) noexcept
{
    auto lByteIndex{aByteIndex};
    auto lBitIndex{static_cast<uint8_t>(aBitIndex)};
    [[maybe_unused]] auto lSourceBitIndex{static_cast<uint8_t>(aSourceBitIndex)};
    auto lPixelCount{aPixelCount};
    auto lSourceData{aSourceData};

    while (lPixelCount) {

        // Get the next byte of pixel data and extract the corresponding entry from the palette.
        const auto lColorByte{std::byte{static_cast<uint8_t>(1 << lBitIndex)}};
        const auto lColorIndex{*lSourceData * 3};
        const auto lColorValue{*(reinterpret_cast<const uint32_t*>(aColorPalette + lColorIndex)) & 0x00FFFFFF};

        aRow[lByteIndex] = (aRow[lByteIndex] & ~lColorByte)
            | std::byte{static_cast<uint8_t>(ColorTranslate(lColorValue) << lBitIndex)};

        if (--lBitIndex == 0) {
            lBitIndex = 7;
            ++lSourceData;
        }

        --lPixelCount;
    }
}


void LS013B7::LineDrawH(
    const int32_t aX1,
    const int32_t aX2,
    const int32_t aRowIx,
    const uint32_t aColor
) noexcept
{
    const auto lNewRow{CreateRow(aX1, aX2, aColor)};
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
    const uint32_t aColor
) noexcept
{
    const auto lByteIx{aColumnIx / sPixelsPerByte};
    const auto lBitIx{aColumnIx % sPixelsPerByte};
    for (auto lRowIx{aY1}; lRowIx <= aY2; ++lRowIx) {
        auto& lRow{mImgBuf[lRowIx]};
        lRow[lByteIx] |= (aColor ? std::byte{0x1} : std::byte{0x0}) << lBitIx;
        mIsLineDirty[lRowIx] = true;
    }
}


void LS013B7::RectFill(const tRectangle * const aRectangle, const uint32_t aColor) noexcept
{
    // Fill all the horizontal lines.
    // Send all lines to display.
    const auto lNewRow{CreateRow(aRectangle->i16XMin, aRectangle->i16XMax, aColor)};
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


//*****************************************************************************
//
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
auto LS013B7::ColorTranslate(const uint32_t ui32Value) noexcept -> uint32_t
{
    return (
        ((((ui32Value & 0x00ff0000) >> 16) * 19661) +
        (((ui32Value & 0x0000ff00) >> 8) * 38666) +
        ((ui32Value & 0x000000ff) * 7209)) /
        (65536 * 128)
    );
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


void LS013B7::SetDisplayMode() noexcept
{
    // 6-5-3 Display Mode
    // Maintains memory internal data (maintains current display). (M0=”L”, M2＝”L”)
    static constexpr std::array sDisplayModeCmd{std::byte{0x0}, std::byte{0x0}};

    mSPIWr(std::span{sDisplayModeCmd}, std::nullopt);
}


void LS013B7::SetAllClrMode() noexcept
{
    // 6-5-4 All Clear Mode
    // Clears memory internal data and writes white on screen. (M0=”L”, M2＝”H”)
    static constexpr std::array sClrCmd{std::byte{0x1 << 5}, std::byte{}};

    mSPIWr(std::span{sClrCmd}, std::nullopt);
    DisplayOn();
    std::fill(mImgBuf.begin(), mImgBuf.end(), Line{});
}


void LS013B7::SetDataUpdateMode(const uint8_t aRow, std::span<const std::byte> aSpan) noexcept
{
    // cmd, gateline, data, dummy bytes(2).
    mSPIAssert();

    mSPIPushPullByte(sDataUpdateModeCmd);
    mSPIPushPullByte(sGateLineLookup[aRow]);
    for (const auto lByte : aSpan) {
        mSPIPushPullByte(sBitSwapLookup[std::to_integer<uint8_t>(lByte)]);
    }

    mSPIPushPullByte(std::byte{0xa5});
    mSPIPushPullByte(std::byte{0});

    mSPIDeassert();
}

void LS013B7::SetDataUpdateModeMultiple(const uint8_t aStartRowIndex, const uint8_t aEndRowIndex) noexcept
{
    // cmd, gateline, data,
    // dummy (1), gateline, data
    // ...
    // dummy (1), gateline, data, dummy(2).
    mSPIAssert();

    mSPIPushPullByte(sDataUpdateModeCmd);
    for (auto i{aStartRowIndex}; i <= aEndRowIndex; ++i) {
        mSPIPushPullByte(sGateLineLookup[i]);
        for (const auto lByte : mImgBuf[i]) {
            mSPIPushPullByte(sBitSwapLookup[std::to_integer<uint8_t>(lByte)]);
        }

        mSPIPushPullByte(std::byte{0x5a});
    }

    mSPIPushPullByte(std::byte{0x00});

    mSPIDeassert();
}


} // namespace Drivers

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
