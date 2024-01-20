#ifndef DRIVERS__LS0123B7DEV_H_
#define DRIVERS__LS0123B7DEV_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: LS0123B7.
//
// *******************************************************************************

//! \file
//! \brief LCD driver class.
//! \ingroup ext_peripherals

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
#include <cstddef>
#include <cstdint>
#include <span>

// TivaWare Graphics Library.
#include "grlib.h"

#include "drivers/inc/ILCD.h"
#include "corelink/inc/Types.h"

// Standard library.
#include <array>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Drivers
{

using GPIOOnOff = void (*)() noexcept;


//! \brief Non-Volatile Memory interface.
class LS013B7 final
    : public tDisplay
    , public ILCD
{
public:
    explicit LS013B7(
        CoreLink::SPIAssert aSPIAssert,
        CoreLink::SPIAssert aSPIDeassert,
        CoreLink::SPIWr aSPIWr,
        CoreLink::PushPullByte aPushPullByte,
        GPIOOnOff aGPIODisplayOn,
        GPIOOnOff aGPIODisplayOff
    ) noexcept;

    // ILCD interface.
    void Init() noexcept override;
    void DisplayOn() const noexcept override;
    void DisplayOff() const noexcept override;

private:
    static constexpr auto sWidth{128};
    static constexpr auto sHeight{128};
    static constexpr auto sPixelsPerByte{8};
    using Line = std::array<std::byte, sWidth / sPixelsPerByte>;

    Line CreateRow(int32_t aX1, int32_t aX2, bool aIsActive) noexcept;

    void PixelDraw(int32_t i32X, int32_t i32Y, uint32_t ui32Value) noexcept;
    void PixelDrawMultiple(
        const int32_t aColumnIx, const int32_t aRowIx,
        const int32_t i32X0, const int32_t aPixelCount,
        const int32_t i32BPP,
        const uint8_t * const pui8Data,
        const uint8_t * const pui8Palette
    ) noexcept;
    void PixelDrawMultiple1BPP(
        Line& aRow,
        const uint32_t aByteIndex,
        uint32_t aBitIndex,
        uint32_t aSourceBitIndex,
        const int32_t aPixelCount,
        const uint8_t *pui8Data,
        const uint8_t *pui8Palette
    ) noexcept;
    void PixelDrawMultiple8BPP(
        Line& aRow,
        const uint32_t aByteIndex,
        uint32_t aBitIndex,
        uint32_t aSourceBitIndex,
        const int32_t aPixelCount,
        const uint8_t *pui8Data,
        const uint8_t *pui8Palette
    ) noexcept;

    void LineDrawH(int32_t i32X1, int32_t i32X2, int32_t i32Y, uint32_t ui32Value) noexcept;
    void LineDrawV(int32_t i32X, int32_t i32Y1, int32_t i32Y2, uint32_t ui32Value) noexcept;
    void RectFill(const tRectangle *psRect, uint32_t ui32Value) noexcept;
    auto ColorTranslate(uint32_t ui32Value) noexcept -> uint32_t;
    void Flush() noexcept;

    void SetDisplayMode() noexcept;
    void SetAllClrMode() noexcept;
    void SetDataUpdateMode(uint8_t aRow, std::span<const std::byte> aSpan) noexcept;
    void SetDataUpdateModeMultiple(uint8_t aStartRowIndex, uint8_t aEndRowIndex) noexcept;

    CoreLink::SPIAssert mSPIAssert;
    CoreLink::SPIAssert mSPIDeassert;
    CoreLink::SPIWr mSPIWr;
    CoreLink::PushPullByte mSPIPushPullByte;

    GPIOOnOff mGPIODisplayOn;
    GPIOOnOff mGPIODisplayOff;

    std::array<Line, sHeight> mImgBuf;
    std::array<bool, sHeight> mIsLineDirty;
};


} // namespace Drivers

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
#endif // DRIVERS__LS0123B7DEV_H_
