#ifndef DRIVERS_DS3234_H_
#define DRIVERS_DS3234_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: DS3234 RTCC.
//
// *******************************************************************************

//! \file
//! \brief DS3234 RTCC.
//! \ingroup ext_peripherals

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

// Standard Libraries.
#include <cstddef>
#include <memory>

#include "IRTCC.h"
#include "INVMem.h"
#include "ITemperature.h"
#include "inc/SPISlaveCfg.h"
#include "inc/GPIO.h"

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

// Forward declaration.
namespace CoreLink {
    class ISPIMasterDev;
}


//! \brief DS3234 Real-time clock.
//! Implements INVMem and ITemperature interfaces.
class DS3234 final
    : public IRTCC
    , public INVMem
    , public ITemperature {

 public:
    [[nodiscard]] explicit DS3234(
        unsigned long aInterruptNumber,
        CoreLink::GPIO const &aInterruptPin,
        std::shared_ptr<CoreLink::ISPIMasterDev> aSPIMasterDev,
        CoreLink::GPIO const &aCSnPin
    ) noexcept;

    // RTCC Interface.
    void Init() noexcept override;
    void SetInterrupt(bool aEnable = true) noexcept override;
    bool ISR() noexcept override;

    auto RdTimeAndDate() const noexcept -> tTimeAndDate override;

    void WrTime(tTime const &aTime) noexcept override;
    void WrDate(tDate const &aDate) noexcept override;
    void WrTimeAndDate(tTime const &aTime, tDate const &aDate) noexcept override;

    void WrAlarm(tTime const &aTime, tDate const &aDate) noexcept override;
    void WrAlarm(tTime const &aTime, tWeekday const &aWeekday) noexcept override;
    void DisableAlarm() noexcept override;

    // INVMem Interface.
    auto GetNVMemSize() const noexcept -> std::size_t override {return mNVMemSize;}
    void RdFromNVMem(
        std::span<std::byte> aData,
        std::size_t aOffset
    ) noexcept override;
    void WrToNVMem(
        std::span<std::byte const> aData,
        std::size_t aOffset
    ) noexcept override;

    // ITemperature Interface.
    auto GetTemperature() const noexcept -> float override;

private:
    enum class eAlarmID {
        ALARM_ID_1,
        ALARM_ID_2,
    };
    using tAlarmID = enum eAlarmID;

    enum class eAlarm1Mode {
        ONCE_PER_SEC,
        WHEN_SECS_MATCH,
        WHEN_MINS_SECS_MATCH,
        WHEN_HOURS_MINS_SECS_MATCH,
        WHEN_DAY_HOURS_MINS_SECS_MATCH,
        WHEN_DATE_HOURS_MINS_SECS_MATCH,
    };
    using tAlarm1Mode = enum eAlarm1Mode;

    enum class eAlarm2Mode {
        ONCE_PER_MINUTE,
        WHEN_MINS_MATCH,
        WHEN_HOURS_MINS_MATCH,
        WHEN_DAY_HOURS_MINS_MATCH,
        WHEN_DATE_HOURS_MINS_MATCH
    };
    using tAlarm2Mode = enum eAlarm2Mode;

    using tRTCCReg = std::byte;

    struct RTCCTimeDate_s {
        tRTCCReg mSeconds{};
        tRTCCReg mMinutes{};
        tRTCCReg mHours{};
        tRTCCReg mWeekday{};
        tRTCCReg mDate{};
        tRTCCReg mMonth{};
        tRTCCReg mYear{};
    };
    using tRTCCTimeDate = struct RTCCTimeDate_s;

    struct RTCCAlarm1_s {
        tRTCCReg mSeconds{};
        tRTCCReg mMinutes{};
        tRTCCReg mHours{};
        tRTCCReg mDayDate_n{};
    };
    using tRTCCAlarm1 = struct RTCCAlarm1_s;

    struct RTCCAlarm2_s {
        tRTCCReg mMinutes{};
        tRTCCReg mHours{};
        tRTCCReg mDayDate_n{};
    };
    using tRTCCAlarm2 = struct RTCCAlarm2_s;

    enum class eHoursFields : uint8_t {
        H12_24_n = (0x1 << 6),
        PM_AM_n  = (0x1 << 5),
    };

    enum class eMonthFields : uint8_t {CENTURY = (0x1 << 7)};

    enum class eDayDateFields : uint8_t {
        DAY_DATE_n = (0x1 << 6),
        AnMx       = (0x1 << 7)
    };

    enum eAddr : uint8_t {WR_BASE_ADDR = 0x80};

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
    enum class eCtrl : uint8_t {
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
    enum class eStatus : uint8_t {
        AF1    = (0x1 << 0),
        AF2    = (0x1 << 1),
        BSY    = (0x1 << 2),
        EN32K  = (0x1 << 3),
        CRATE0 = (0x1 << 4),
        CRATE1 = (0x1 << 5),
        BB32K  = (0x1 << 6),
        ESF    = (0x1 << 7)
    };

private:
    static void FillTimeStruct(IRTCC::tTime const &aTime, tRTCCTimeDate &aRTCCTimeDate) noexcept;
    static void FillDateStruct(IRTCC::tDate const &aDate, tRTCCTimeDate &aRTCCTimeDate) noexcept;

    void WrAlarm1(
        tTime const &aTime,
        tDate const &aDate,
        tAlarm1Mode const aAlarmMode
    ) noexcept;
    static auto SetAlarm1Mode(
        tRTCCAlarm1 const &aAlarmStruct,
        tAlarm1Mode const aAlarmMode
    ) noexcept -> tRTCCAlarm1;
    void TxAlarm1Struct(tRTCCAlarm1 const &aRTCCAlarm) noexcept;

    static auto FillAlarm2Struct(tTime const &aTime, tDate const &aDate, tAlarm2Mode const aAlarmMode) noexcept -> tRTCCAlarm2;
    static auto FillAlarm2Struct(tTime const &aTime, tWeekday const &aWeekday, tAlarm2Mode const aAlarmMode) noexcept -> tRTCCAlarm2;
    static auto SetAlarm2Mode(
        tRTCCAlarm2 const &aAlarmStruct,
        tAlarm2Mode aAlarmMode
    ) noexcept -> tRTCCAlarm2;
    void TxAlarm2Struct(tRTCCAlarm2 const &aRTCCAlarm) noexcept;

    void EnableAlarm(tAlarmID aAlarmID) noexcept;
    void ClrAlarmFlag(tAlarmID aAlarmID) noexcept;

    static constexpr auto mBaseYear{2000};

    std::shared_ptr<CoreLink::ISPIMasterDev> mSPIMasterDev;
    CoreLink::SPISlaveCfg mSPISlaveCfg;

    unsigned long mInterruptNumber;
    CoreLink::GPIO mInterruptGPIO;

    bool mIsImpure{true};

    static constexpr auto mNVMemSize{256};
    static constexpr std::byte sSecondsAddr{0x00};
    static constexpr std::byte sMinutesAddr{0x01};
    static constexpr std::byte sHoursAddr{0x02};
    static constexpr std::byte sDayAddr{0x03};
    static constexpr std::byte sDateAddr{0x04};
    static constexpr std::byte sMonthAddr{0x05};
    static constexpr std::byte sYearAddr{0x06};
    static constexpr std::byte sAlarm1Addr{0x07};
    static constexpr std::byte sAlarm2Addr{0x0B};
    static constexpr std::byte sCtrlAddr{0x0E};
    static constexpr std::byte sStatusAddr{0x0F};
    static constexpr std::byte sTemperatureMSB{0x11};
    static constexpr std::byte sTemperatureLSB{0x12};
    static constexpr std::byte sSRAMAddrAddr{0x18};
    static constexpr std::byte sSRAMDataAddr{0x19};

    [[nodiscard]] static auto UpdateTime(tRTCCTimeDate const &aRTCCTimeDate) noexcept -> tTime;
    [[nodiscard]] static auto UpdateDate(tRTCCTimeDate const &aRTCCTimeDate) noexcept -> tDate;

    bool mIsPolled{false};
    mutable tTime mTimeCache{};
    mutable tDate mDateCache{};
    //float mTemperature{-1000.0};

    mutable bool mIsCacheValid{false};
    constexpr std::byte ToWrAddr(std::byte const aAddr) {return aAddr | std::byte{0x80};}
};

// ******************************************************************************
//                            EXPORTED VARIABLES
// ******************************************************************************

namespace DS3234Helper {
constexpr std::byte BinaryToBCD(uint8_t const aByte) {
    auto const lLowNibble = aByte % 10;
    auto const lHighNibble = aByte / 10;

    return std::byte(lHighNibble << 4 | lLowNibble);
}

constexpr uint8_t BCDToBinary(std::byte const aBCD) {
    auto const lBCD = std::to_integer<uint8_t>(aBCD);
    auto const lUnits = lBCD & 0x0F;
    auto const lDecimals = (lBCD & 0xF0) * 10;

    return lDecimals + lUnits;
}


} // namespace DS3234Helper

// ******************************************************************************
//                                 EXTERNS
// ******************************************************************************

// ******************************************************************************
//                            EXPORTED FUNCTIONS
// ******************************************************************************

// ******************************************************************************
//                                END OF FILE
// ******************************************************************************
#endif // DRIVERS_DS3234_H_
