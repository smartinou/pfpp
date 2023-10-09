// *****************************************************************************
//
// Project: Drivers.
//
// Module: DS3234 RTCC.
//
// *****************************************************************************

//! \file
//! \brief DS3234 RTCC.
//! \ingroup ext_peripherals

// *****************************************************************************
//
//        Copyright (c) 2015-2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This modules.
#include "drivers/inc/DS3234.h"

// Standard Libraries.
#include <cstdint>

using namespace std::chrono_literals;

// *****************************************************************************
//                      DEFINED CONSTANTS AND MACROS
// *****************************************************************************

namespace Drivers
{


constexpr std::byte ToWrAddr(const std::byte aAddr) noexcept {return aAddr | std::byte{0x80};}

// *****************************************************************************
//                         TYPEDEFS AND STRUCTURES
// *****************************************************************************

enum class eAlarm1Mode
{
    ONCE_PER_SEC,
    WHEN_SECS_MATCH,
    WHEN_MINS_SECS_MATCH,
    WHEN_HOURS_MINS_SECS_MATCH,
    WHEN_DAY_HOURS_MINS_SECS_MATCH,
    WHEN_DATE_HOURS_MINS_SECS_MATCH,
};
using tAlarm1Mode = enum eAlarm1Mode;

enum class eAlarm2Mode
{
    ONCE_PER_MINUTE,
    WHEN_MINS_MATCH,
    WHEN_HOURS_MINS_MATCH,
    WHEN_DAY_HOURS_MINS_MATCH,
    WHEN_DATE_HOURS_MINS_MATCH
};
using tAlarm2Mode = enum eAlarm2Mode;

enum eHoursFields : uint8_t
{
    H12_24_n = (0x1 << 6),
    PM_AM_n  = (0x1 << 5),
};

enum eDayDateFields : uint8_t
{
    DAY_DATE_n = (0x1 << 6),
    AnMx       = (0x1 << 7)
};

// public:
/* -------------------------------------------------------------------------
    Control Register : 0x0E/8Eh
    Name    Value       Description
    ----    ---------   -------------------------------------------------------
    EOSCn   x--- ----   Enable Oscillator.
    BBSQW   -x-- ----   Battery-Backed Square-Wave Enable.
    CONV    --x- ----   Convert Temperature.
    RS2     ---x ----   Rate selection bit 1.
    RS1     ---- x---   Rate selection bit 2.
    INTCn   ---- -x--   Interrupt Control.
    AEI2    ---- --x-   Alarm 2 Interrupt Enable.
    AEI1    ---- ---x   Alarm 1 Interrupt Enable.
    ------------------------------------------------------------------------- */
enum eCtrl : uint8_t
{
    AEI1  = (0x1 << 0),
    AEI2  = (0x1 << 1),
    INTCn = (0x1 << 2),
    RS1   = (0x1 << 3),
    RS2   = (0x1 << 4),
    CONV  = (0x1 << 5),
    BBSQW = (0x1 << 6),
    EOSCn = (0x1 << 7)
};

/* -------------------------------------------------------------------------
    Control/Status Register : 0x0F/8Fh
    Name    Value       Description
    ----    ---------   -------------------------------------------------------
    OSF     x--- ----   Oscillator Stop Flag.
    BB32K   -x-- ----   Battery-Backed 32KHz Output.
    CRATE1  --x- ----   Conversion Rate 1.
    CRATE0  ---x ----   Conversion Rate 2.
    EN32K   ---- x---   Enable 32KHz Output.
    BSY     ---- -x--   Busy.
    AF2     ---- --x-   Alarm 2 Flag.
    AF1     ---- ---x   Alarm 1 Flag.
    ------------------------------------------------------------------------- */
enum eStatus : uint8_t
{
    AF1    = (0x1 << 0),
    AF2    = (0x1 << 1),
    BSY    = (0x1 << 2),
    EN32K  = (0x1 << 3),
    CRATE0 = (0x1 << 4),
    CRATE1 = (0x1 << 5),
    BB32K  = (0x1 << 6),
    ESF    = (0x1 << 7)
};

// *****************************************************************************
//                            FUNCTION PROTOTYPES
// *****************************************************************************

// *****************************************************************************
//                             GLOBAL VARIABLES
// *****************************************************************************

//static constexpr auto sSPIClk{4000000};
//static constexpr auto sSPIBits{8};

static constexpr auto mNVMemSize{256};

static constexpr std::byte sSecondsAddr{0x00};
static constexpr std::byte sCtrlAddr{0x0E};
static constexpr std::byte sStatusAddr{0x0F};
static constexpr std::byte sSRAMAddrAddr{0x18};
static constexpr std::byte sSRAMDataAddr{0x19};

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

// Ctor.
DS3234::DS3234(
    const CoreLink::SPIRd aSPIRd,
    const CoreLink::SPIWr aSPIWr
) noexcept
    : mSPIRd{aSPIRd}
    , mSPIWr{aSPIWr}
{
    // Ctor body.
}


void DS3234::Init() noexcept
{
    // Configure the RTC to desired state:
    //   -Oscillator enabled.
    //   -Battery-backed square wave disabled.
    //   -Interrupt enabled (square wave disabled).
    //   -Alarm 'N' disabled.
    // Disable all and clear all flags in status register.
    //mRegMap.mCtrl = std::byte{eCtrl::INTCn};
    //mRegMap.mStatus = std::byte{0x00};
    static constexpr std::array<tRTCCReg, 2> sCtrlStatus{std::byte{eCtrl::INTCn}, std::byte{0x00}};
    mSPIWr(std::as_bytes(std::span{sCtrlStatus}), std::optional(ToWrAddr(sCtrlAddr)));

    // Force set time to 24H mode if set to 12H: R-M-W.
    // [MG] ESSAYER DE MERGER CA DANS WrTimeAndDate().
#if 0
    tRTCCTime lTime{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{&lTime, sizeof(tRTCCTime)}),
        sSecondsAddr
    );

    if (std::to_integer<bool>(lTime.mHours & std::byte{eHoursFields::H12_24_n})) {
        lTime.mHours &= ~std::byte{eHoursFields::H12_24_n};
        mSPIMasterDev->WrData(
            mSPISlaveCfg,
            std::as_bytes(std::span{&lTime, sizeof(tRTCCTime)}),
            ToWrAddr(sSecondsAddr)
        );
    }
#endif
}


void DS3234::SetInterrupt(const bool aEnable) noexcept
{
#if 0
    MAP_IntDisable(mInterruptNumber);
    if (aEnable) {
        MAP_GPIOPinTypeGPIOInput(mInterruptGPIO.mBaseAddr, mInterruptGPIO.mPin);
        MAP_GPIOIntTypeSet(
            mInterruptGPIO.mBaseAddr,
            mInterruptGPIO.mPin,
            GPIO_FALLING_EDGE
        );
        MAP_GPIOPadConfigSet(
            mInterruptGPIO.mBaseAddr,
            mInterruptGPIO.mPin,
            GPIO_STRENGTH_2MA,
            GPIO_PIN_TYPE_STD
        );

        MAP_GPIOIntEnable(mInterruptGPIO.mBaseAddr, mInterruptGPIO.mPin);
        // [MG] THIS CLEARS THE 1ST INTERRUPT.
        // IT DOESN'T COME UP UNTIL FLAGS ARE CLEARED.
        //MAP_GPIOPinIntClear(mInterrupt->GetPort(), lGPIOInitEvtPtr->mIRQGPIOPin);

        MAP_IntEnable(mInterruptNumber);
#if 0
        // Alarm1 is dedicated to periodic interrupt.
        // This is also where we can throttle down the frequency of clock updates.
        // Instead of using square wave, the alarm is triggered periodically.
        WrAlarm1(
            tTime{},
            tDate{},
            eAlarm1Mode::ONCE_PER_SEC
        );
#else
        // This should be set using signal.    
#endif
    }

#else
    static_cast<void>(aEnable);
#endif
}


bool DS3234::ISR() noexcept
{
    // Read Status registers.
    std::array<tRTCCReg, 1> lRTCCStatus{};
    mSPIRd(std::as_writable_bytes(std::span{lRTCCStatus}), std::optional(sStatusAddr));

    // Alarm1 interrupt?
    std::byte lFlagMask{};
    if (std::to_integer<bool>(lRTCCStatus[0] & std::byte{eStatus::AF1})) {
        lFlagMask |= std::byte{eStatus::AF1};
        mIsCacheValid = false;
    }

    // Alarm2 interrupt?
    if (std::to_integer<bool>(lRTCCStatus[0] & std::byte{eStatus::AF2})) {
        lFlagMask |= std::byte{eStatus::AF2};
        mIsCacheValid = false;
    }

    // Clear raised flags.
    if (std::to_integer<bool>(lFlagMask)) {
        lRTCCStatus[0] &= ~lFlagMask;
        mSPIWr(std::as_bytes(std::span{lRTCCStatus}), std::optional(ToWrAddr(sStatusAddr)));
    }

    const auto [lTime, lDate] {GetTimeAndDate()};
    mTimeCache = lTime;
    mDateCache = lDate;

    return std::to_integer<bool>((lFlagMask & std::byte{eStatus::AF1})
        | (lFlagMask & std::byte{eStatus::AF2}));
}


auto DS3234::GetTimeAndDate() const noexcept -> IRTCC::tTimeAndDate
{
    if (!mIsCacheValid) {
        // Read the whole RTC into register map structure.
        const auto lRTCCTimeDate {RdTimeAndDate()};
        const auto lTimeCache {UpdateTime(lRTCCTimeDate)};
        const auto lDateCache {UpdateDate(lRTCCTimeDate)};
        mIsCacheValid = true;//!mIsPolled;
        return {lTimeCache, lDateCache};
    }

    return {mTimeCache, mDateCache};
}


void DS3234::SetTime(const tTime aTime) noexcept
{
    auto lRTCCTimeAndDate {RdTimeAndDate()};

    // Fill time structure to write to RTC.
    FillTimeStruct(aTime, lRTCCTimeAndDate);
    WrTimeAndDate(lRTCCTimeAndDate);
}


void DS3234::SetDate(const tDate aDate) noexcept
{
    auto lRTCCTimeAndDate {RdTimeAndDate()};

    // Fill date structure to write to RTC.
    FillDateStruct(aDate, lRTCCTimeAndDate);
    WrTimeAndDate(lRTCCTimeAndDate);
}


void DS3234::SetTimeAndDate(const tTime aTime, const tDate aDate) noexcept
{
    // Fill structure with time to write to RTC.
    // Fill structure with date to write to RTC.
    // Send time and date portion of the structure to RTC.
    tRTCCTimeDate lRTCCTimeAndDate {};
    FillTimeStruct(aTime, lRTCCTimeAndDate);
    FillDateStruct(aDate, lRTCCTimeAndDate);

    WrTimeAndDate(lRTCCTimeAndDate);
}


void DS3234::SetAlarm(
    const tTime aTime,
    const tDate aDate,
    const Alarm::eRate aRate,
    const unsigned int aAlarmID
) noexcept
{
    switch (aRate) {
        case Alarm::eRate::OncePerSecond:
        case Alarm::eRate::SecondsMatch:
        case Alarm::eRate::MinutesSecondsMatch:
        case Alarm::eRate::HoursMinutesSecondsMatch:
        case Alarm::eRate::DateHoursMinutesSecondsMatch:
        case Alarm::eRate::DayHoursMinutesSecondsMatch: {
            // Set alarm1.
            const auto lAlarm1{FillAlarm1Struct(aTime, aDate, aRate)};
            WrAlarm1Struct(lAlarm1);
            EnableAlarm(eAlarmID::ALARM_ID_1);
            break;
        }

        case Alarm::eRate::OncePerMinute:
        case Alarm::eRate::MinutesMatch:
        case Alarm::eRate::HoursMinutesMatch:
        case Alarm::eRate::DateHoursMinutesMatch:
        case Alarm::eRate::DayHoursMinutesMatch: {
            // Set alarm2.
            // Fill alarm structure to write to RTC.
            const auto lAlarm2{FillAlarm2Struct(aTime, aDate, aRate)};
            WrAlarm2Struct(lAlarm2);
            EnableAlarm(eAlarmID::ALARM_ID_2);
            break;
        }
    }
}


void DS3234::SetAlarm(
    const tTime aTime,
    const tWeekday aWeekday,
    const Alarm::eRate aRate,
    const unsigned int aAlarmID
) noexcept
{
    switch (aRate) {
        case Alarm::eRate::OncePerSecond:
        case Alarm::eRate::SecondsMatch:
        case Alarm::eRate::MinutesSecondsMatch:
        case Alarm::eRate::HoursMinutesSecondsMatch:
        case Alarm::eRate::DateHoursMinutesSecondsMatch:
        case Alarm::eRate::DayHoursMinutesSecondsMatch: {
            // Set alarm1.
            const auto lAlarm1{FillAlarm1Struct(aTime, aWeekday, aRate)};
            WrAlarm1Struct(lAlarm1);
            EnableAlarm(eAlarmID::ALARM_ID_1);
            break;
        }

        case Alarm::eRate::OncePerMinute:
        case Alarm::eRate::MinutesMatch:
        case Alarm::eRate::HoursMinutesMatch:
        case Alarm::eRate::DateHoursMinutesMatch:
        case Alarm::eRate::DayHoursMinutesMatch: {
            // Set alarm2.
            // Fill alarm structure to write to RTC.
            const auto lAlarm2{FillAlarm2Struct(aTime, aWeekday, aRate)};
            WrAlarm2Struct(lAlarm2);
            EnableAlarm(eAlarmID::ALARM_ID_2);
            break;
        }
    }
}

#if 0
void DS3234::DisableAlarm() noexcept
{
    // Read-modify-write Control register.
    //std::array<tRTCCReg, 1> lRTCCCtrl{};
    const auto lRTCCCtrl {RdCtrl() & ~std::byte{eCtrl::AEI2}};
    WrCtrl(lRTCCCtrl);
}
#endif

void DS3234::RdFromNVMem(
    const std::span<std::byte> aData,
    const std::size_t aOffset
) noexcept
{
    // Write SRAM address register.
    // Cap size.
    // Loop into data register.
    static constexpr auto sSRAMAddrWrAddr {ToWrAddr(sSRAMAddrAddr)};
    mSPIWr(
        std::as_writable_bytes(std::span{reinterpret_cast<std::byte*>(aOffset), sizeof(tRTCCReg)}),
        std::optional(sSRAMAddrWrAddr)
    );

    mSPIRd(aData, std::optional(sSRAMDataAddr));
}


void DS3234::WrToNVMem(
    const std::span<const std::byte> aData,
    const std::size_t aOffset
) noexcept
{
    // Write SRAM address register.
    // Cap size.
    // Loop into data register.
    mSPIWr(
        std::as_bytes(std::span{reinterpret_cast<std::byte*>(aOffset), sizeof(tRTCCReg)}),
        std::optional(ToWrAddr(sSRAMAddrAddr))
    );

    static constexpr auto sSRAMDataWrAddr {ToWrAddr(sSRAMDataAddr)};
    mSPIWr(aData, std::optional(sSRAMDataWrAddr));
}


auto DS3234::GetTemperature() const noexcept -> float
{
    // Return temperature field.
    // Convert temperature MSB and LSB to float.
    static constexpr std::byte sTemperatureMSBAddr{0x11};
    std::array<std::byte, 2> lRTCCTemperature{};
    mSPIRd(lRTCCTemperature, std::optional(sTemperatureMSBAddr));

    const float lTempFloat{
        static_cast<float>(lRTCCTemperature[0])
        + (0.25F * static_cast<float>(std::to_integer<uint8_t>(lRTCCTemperature[1] >> 6)))
    };
    return lTempFloat;
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

auto DS3234::RdCtrl() const noexcept -> tRTCCReg
{
    std::array<tRTCCReg, 1> lRTCCCtrl{};
    mSPIRd(std::as_writable_bytes(std::span{lRTCCCtrl}), std::optional(sCtrlAddr));

    return lRTCCCtrl[0];
}


void DS3234::WrCtrl(const std::array<tRTCCReg, 1> aCtrl) noexcept
{
    static constexpr auto sCtrlWrAddr {ToWrAddr(sCtrlAddr)};
    mSPIWr(std::as_bytes(std::span{aCtrl}), std::optional(sCtrlWrAddr));
}


auto DS3234::RdStatus() const noexcept -> tRTCCReg
{
    std::array<tRTCCReg, 1> lRTCCStatus{};
    mSPIRd(std::as_writable_bytes(std::span{lRTCCStatus}), std::optional(sStatusAddr));

    return lRTCCStatus[0];
}


void DS3234::WrStatus(const std::array<tRTCCReg, 1> aStatus) noexcept
{
    static constexpr auto sStatusWrAddr {ToWrAddr(sStatusAddr)};
    mSPIWr(std::as_bytes(std::span{aStatus}), std::optional(sStatusWrAddr));
}


auto DS3234::RdTimeAndDate() const noexcept -> tRTCCTimeDate
{
    tRTCCTimeDate lRTCCTimeDate{};
    mSPIRd(
        std::as_writable_bytes(std::span{&lRTCCTimeDate, sizeof(lRTCCTimeDate)}),
        std::optional(sSecondsAddr)
    );

    return lRTCCTimeDate;
}


void DS3234::WrTimeAndDate(const tRTCCTimeDate& aTimeAndDate) noexcept
{
    mSPIWr(
        std::as_bytes(std::span{&aTimeAndDate, sizeof(aTimeAndDate)}),
        std::optional(ToWrAddr(sSecondsAddr))
    );
    mIsCacheValid = false;
}


void DS3234::FillTimeStruct(const tTime aTime, tRTCCTimeDate& aRTCCTimeDate) noexcept
{
    const std::chrono::hh_mm_ss lTime{aTime};
    aRTCCTimeDate.mSeconds = DS3234Helper::BinaryToBCD(lTime.seconds().count());
    aRTCCTimeDate.mMinutes = DS3234Helper::BinaryToBCD(lTime.minutes().count());
    // Time is always stored as 24H.
    aRTCCTimeDate.mHours = DS3234Helper::BinaryToBCD(lTime.hours().count());
}


void DS3234::FillDateStruct(const tDate aDate, tRTCCTimeDate& aRTCCTimeDate) noexcept
{
    const std::chrono::weekday lWeekday{std::chrono::sys_days{aDate}};
    const auto lYears{std::chrono::years(int(aDate.year()) - int(std::chrono::year{mBaseYear}))};

    aRTCCTimeDate.mWeekday = std::byte{static_cast<unsigned char>(lWeekday.iso_encoding())};
    aRTCCTimeDate.mDate = DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.day()));
    aRTCCTimeDate.mMonth = DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.month()));
    aRTCCTimeDate.mYear = DS3234Helper::BinaryToBCD(lYears.count());
}


auto DS3234::FillAlarm1Struct(
    const tTime aTime,
    const tWeekday aWeekday,
    const Drivers::Alarm::eRate aAlarmMode
) noexcept -> DS3234::tRTCCAlarm1
{
    const std::chrono::hh_mm_ss lTime{aTime};
    const auto lSeconds {DS3234Helper::BinaryToBCD(lTime.seconds().count())};
    const auto lMinutes {DS3234Helper::BinaryToBCD(lTime.minutes().count())};
    const auto lHours {DS3234Helper::BinaryToBCD(lTime.hours().count())};
    const auto lWeekday {std::byte{static_cast<unsigned char>(aWeekday.iso_encoding())}};

    const tRTCCAlarm1 lRTCCAlarm{
        lSeconds,
        lMinutes,
        lHours,
        lWeekday
    };
    return SetAlarm1Mode(lRTCCAlarm, aAlarmMode);
}


auto DS3234::FillAlarm1Struct(
    const tTime aTime,
    const tDate aDate,
    const Drivers::Alarm::eRate aAlarmMode
) noexcept -> DS3234::tRTCCAlarm1
{
    const std::chrono::hh_mm_ss lTime{aTime};
    const auto lSeconds {DS3234Helper::BinaryToBCD(lTime.seconds().count())};
    const auto lMinutes {DS3234Helper::BinaryToBCD(lTime.minutes().count())};
    const auto lHours {DS3234Helper::BinaryToBCD(lTime.hours().count())};
    const auto lDay {DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.day()))};

    const tRTCCAlarm1 lRTCCAlarm{
        lSeconds,
        lMinutes,
        lHours,
        lDay | std::byte{eDayDateFields::DAY_DATE_n}
    };
    return SetAlarm1Mode(lRTCCAlarm, aAlarmMode);
}


auto DS3234::FillAlarm2Struct(
    const tTime aTime,
    const tWeekday aWeekday,
    const Drivers::Alarm::eRate aAlarmMode
) noexcept -> DS3234::tRTCCAlarm2
{
    const std::chrono::hh_mm_ss lTime{aTime};
    const auto lMinutes {DS3234Helper::BinaryToBCD(lTime.minutes().count())};
    const auto lHours {DS3234Helper::BinaryToBCD(lTime.hours().count())};
    const auto lWeekday {std::byte{static_cast<unsigned char>(aWeekday.iso_encoding())}};

    const tRTCCAlarm2 lRTCCAlarm{
        lMinutes,
        lHours,
        lWeekday
    };
    return SetAlarm2Mode(lRTCCAlarm, aAlarmMode);
}


auto DS3234::FillAlarm2Struct(
    const tTime aTime,
    const tDate aDate,
    const Drivers::Alarm::eRate aAlarmMode
) noexcept -> DS3234::tRTCCAlarm2
{
    const std::chrono::hh_mm_ss lTime{aTime};
    const auto lMinutes {DS3234Helper::BinaryToBCD(lTime.minutes().count())};
    const auto lHours {DS3234Helper::BinaryToBCD(lTime.hours().count())};
    const auto lDay {DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.day()))};

    const tRTCCAlarm2 lRTCCAlarm{
        lMinutes,
        lHours,
        lDay | std::byte{eDayDateFields::DAY_DATE_n}
    };
    return SetAlarm2Mode(lRTCCAlarm, aAlarmMode);
}


auto DS3234::SetAlarm1Mode(
    const tRTCCAlarm1& aAlarmStruct,
    const Drivers::Alarm::eRate aRate
) noexcept -> DS3234::tRTCCAlarm1
{
    auto lAlarmStruct{aAlarmStruct};

    // Assumes the AxMy bit was cleared on previous operation.
    // This should be performed by BinaryToBCD().
    switch (aRate) {
    case Drivers::Alarm::eRate::OncePerSecond:
        lAlarmStruct.mSeconds |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case Drivers::Alarm::eRate::SecondsMatch:
        lAlarmStruct.mMinutes |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case Drivers::Alarm::eRate::MinutesSecondsMatch:
        lAlarmStruct.mHours |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case Drivers::Alarm::eRate::HoursMinutesSecondsMatch:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::AnMx};
        // Don't care about DY/DATEn bit.
        break;

    case Drivers::Alarm::eRate::DateHoursMinutesSecondsMatch:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::DAY_DATE_n};
        break;

    case Drivers::Alarm::eRate::DayHoursMinutesSecondsMatch:
    default:
        // Nothing to set.
        break;
    }

    return lAlarmStruct;
}


auto DS3234::SetAlarm2Mode(
    const tRTCCAlarm2& aAlarmStruct,
    const Drivers::Alarm::eRate aRate
) noexcept -> DS3234::tRTCCAlarm2
{
    auto lAlarmStruct{aAlarmStruct};

    // Assumes the AxMy bit was cleared on previous operation.
    // This should be performed by BinaryToBCD().
    switch (aRate) {
    case Drivers::Alarm::eRate::OncePerMinute:
        lAlarmStruct.mMinutes |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case Drivers::Alarm::eRate::MinutesMatch:
        lAlarmStruct.mHours |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case Drivers::Alarm::eRate::HoursMinutesMatch:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::AnMx};
        // Don't care about DY/DATEn bit.
        break;

    case Drivers::Alarm::eRate::DateHoursMinutesMatch:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::DAY_DATE_n};
        break;

    case Drivers::Alarm::eRate::DayHoursMinutesMatch:
    default:
        // Nothing to set.
        break;
    }

    return lAlarmStruct;
}


void DS3234::WrAlarm1Struct(const tRTCCAlarm1& aRTCCAlarm) noexcept
{
    static constexpr auto sAlarm1RdAddr{std::byte{0x07}};
    static constexpr auto sAlarm1WrAddr{sAlarm1RdAddr};
    mSPIWr(
        std::as_bytes(std::span{&aRTCCAlarm, sizeof(tRTCCAlarm1)}),
        std::optional(sAlarm1WrAddr)
    );

    // Send control 1.
}


void DS3234::WrAlarm2Struct(const tRTCCAlarm2& aRTCCAlarm) noexcept
{
    static constexpr auto sAlarm2RdAddr{std::byte{0x0B}};
    static constexpr auto sAlarm2WrAddr{sAlarm2RdAddr};
    mSPIWr(
        std::as_bytes(std::span{&aRTCCAlarm, sizeof(tRTCCAlarm2)}),
        std::optional(sAlarm2WrAddr)
    );

    // Send control 2.
}


void DS3234::EnableAlarm(const tAlarmID aAlarmID) noexcept
{
    std::array<tRTCCReg, 1> lRTCCCtrl{RdCtrl()};

    switch (aAlarmID) {
    case eAlarmID::ALARM_ID_1: lRTCCCtrl[0] |= std::byte{eCtrl::AEI1}; break;
    case eAlarmID::ALARM_ID_2: lRTCCCtrl[0] |= std::byte{eCtrl::AEI2}; break;
    default: return;
    }

    WrCtrl(lRTCCCtrl);
}

#if 0
void DS3234::ClrAlarmFlag(const tAlarmID aAlarmID) noexcept
{
    auto lRTCCStatus {RdStatus()};

    // Only clear interrupt.
    // We don't care if the Alarm fields are set,
    // as long as the interrupt is not generated.
    switch (aAlarmID) {
    case eAlarmID::ALARM_ID_1: lRTCCStatus[0] &= ~std::byte{eStatus::AF1}; break;
    case eAlarmID::ALARM_ID_2: lRTCCStatus[0] &= ~std::byte{eStatus::AF2}; break;
    default: return;
    }

    WrStatus(lRTCCStatus);
}
#endif

auto DS3234::UpdateTime(const tRTCCTimeDate& aRTCCTimeDate) const noexcept -> tTime
{
    auto lHours{0};
    if (std::to_integer<bool>(aRTCCTimeDate.mHours & std::byte{eHoursFields::H12_24_n})) {
        // Should not take this branch: time is stored in 24H mode.
        const auto lHoursReg {
            aRTCCTimeDate.mHours
            & ~(std::byte{eHoursFields::H12_24_n} | std::byte{eHoursFields::PM_AM_n})
        };
        lHours = DS3234Helper::BCDToBinary(lHoursReg);
        if (std::to_integer<bool>(aRTCCTimeDate.mHours & std::byte{eHoursFields::PM_AM_n})) {
            lHours += 12;
        }
    }
    else {
        lHours = DS3234Helper::BCDToBinary(aRTCCTimeDate.mHours);
    }

    const auto lSeconds {DS3234Helper::BCDToBinary(aRTCCTimeDate.mSeconds)};
    const auto lMinutes {DS3234Helper::BCDToBinary(aRTCCTimeDate.mMinutes)};
    return tTime{
        std::chrono::hours(lHours) 
        + std::chrono::minutes(lMinutes)
        + std::chrono::seconds(lSeconds)
    };
}


auto DS3234::UpdateDate(const Drivers::DS3234::tRTCCTimeDate& aRTCCTimeDate) const noexcept -> tDate
{
    //auto const lWeekday = BCDToBinary(mRegMap.mDate.mWeekday);
    const std::chrono::day lDay {DS3234Helper::BCDToBinary(aRTCCTimeDate.mDate)};
    static constexpr auto sMonthFieldsCentury {std::byte{0x1 << 7}};
    const std::chrono::month lMonth {DS3234Helper::BCDToBinary(aRTCCTimeDate.mMonth & ~sMonthFieldsCentury)};
    const std::chrono::year lYear {DS3234Helper::BCDToBinary(aRTCCTimeDate.mYear) + mBaseYear};

    // Should check for a valid date here. If not, construct one.
    static constexpr auto lDefaultDate{std::chrono::January/1/2000};
    const auto lDate {tDate{lYear, lMonth, lDay}};
    return lDate.ok() ? lDate : lDefaultDate;
}


} // namespace Drivers

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
