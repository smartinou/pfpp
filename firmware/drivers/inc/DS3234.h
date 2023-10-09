#ifndef DRIVERS__DS3234_H_
#define DRIVERS__DS3234_H_
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
#include <cstddef>
#include <span>

#include "drivers/inc/INVMem.h"
#include "drivers/inc/IRTCC.h"
#include "drivers/inc/ITemperature.h"

#include "corelink/inc/Types.h"

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Drivers
{


//! \brief DS3234 Real-time clock.
//! Implements INVMem and ITemperature interfaces.
class DS3234 final
    : public IRTCC
    , public INVMem
    , public ITemperature
{
public:
    [[nodiscard]] explicit DS3234(
        CoreLink::SPIRd aSPIRd,
        CoreLink::SPIWr aSPIWr
    ) noexcept;

    // RTCC Interface.
    void Init() noexcept override;
    void SetInterrupt(bool aEnable = true) noexcept override;
    [[nodiscard]] bool ISR() noexcept override;

    [[nodiscard]] auto GetTimeAndDate() const noexcept -> tTimeAndDate override;

    void SetTime(tTime aTime) noexcept override;
    void SetDate(tDate aDate) noexcept override;
    void SetTimeAndDate(tTime aTime, tDate aDate) noexcept override;

    void SetAlarm(tTime aTime, tDate aDate, Alarm::eRate aPeriod, unsigned int aAlarmID = 0) noexcept override;
    void SetAlarm(tTime aTime, tWeekday aWeekday, Alarm::eRate aPeriod, unsigned int aAlarmID = 0) noexcept override;
    void ClearAlarm(unsigned int aAlarmID = 0) noexcept override {}
    bool ProcessAlarms(const Alarm::ProcessAlarmFct& aFct, void* aParam) noexcept override {return false;}

    // INVMem interface.
    [[nodiscard]] auto GetNVMemSize() const noexcept -> std::size_t override {return 256;}
    void RdFromNVMem(std::span<std::byte> aData, std::size_t aOffset) noexcept override;
    void WrToNVMem(std::span<const std::byte> aData, std::size_t aOffset) noexcept override;

    // ITemperature interface.
    [[nodiscard]] auto GetTemperature() const noexcept -> float override;

private:
    using tRTCCReg = std::byte;

    [[nodiscard]] auto RdCtrl() const noexcept -> tRTCCReg;
    void WrCtrl(std::array<tRTCCReg, 1> aCtrl) noexcept;
    [[nodiscard]] auto RdStatus() const noexcept -> tRTCCReg;
    void WrStatus(const std::array<tRTCCReg, 1> aStatus) noexcept;

    struct RTCCTimeDate_s
    {
        tRTCCReg mSeconds{};
        tRTCCReg mMinutes{};
        tRTCCReg mHours{};
        tRTCCReg mWeekday{};
        tRTCCReg mDate{};
        tRTCCReg mMonth{};
        tRTCCReg mYear{};
    };
    using tRTCCTimeDate = struct RTCCTimeDate_s;

    [[nodiscard]] auto RdTimeAndDate() const noexcept -> tRTCCTimeDate;
    void WrTimeAndDate(const tRTCCTimeDate& aTimeAndDate) noexcept;
    void FillTimeStruct(tTime aTime, tRTCCTimeDate& aRTCCTimeDate) noexcept;
    void FillDateStruct(tDate aDate, tRTCCTimeDate& aRTCCTimeDate) noexcept;
    [[nodiscard]] auto UpdateTime(const tRTCCTimeDate& aRTCCTimeDate) const noexcept -> tTime;
    [[nodiscard]] auto UpdateDate(const tRTCCTimeDate& aRTCCTimeDate) const noexcept -> tDate;


    struct tRTCCAlarm1
    {
        tRTCCReg mSeconds{};
        tRTCCReg mMinutes{};
        tRTCCReg mHours{};
        tRTCCReg mDayDate_n{};
    };

    [[nodiscard]] auto FillAlarm1Struct(
        tTime aTime,
        tDate aDate,
        Drivers::Alarm::eRate aAlarmMode
    ) noexcept -> tRTCCAlarm1;
    [[nodiscard]] auto FillAlarm1Struct(
        tTime aTime,
        tWeekday aWeekday,
        Drivers::Alarm::eRate aAlarmMode
    ) noexcept -> tRTCCAlarm1;

    struct tRTCCAlarm2
    {
        tRTCCReg mMinutes{};
        tRTCCReg mHours{};
        tRTCCReg mDayDate_n{};
    };

    [[nodiscard]] auto FillAlarm2Struct(
        tTime aTime,
        tDate aDate,
        Drivers::Alarm::eRate aAlarmMode
    ) noexcept -> tRTCCAlarm2;
    [[nodiscard]] auto FillAlarm2Struct(
        tTime aTime,
        tWeekday aWeekday,
        Drivers::Alarm::eRate aAlarmMode
    ) noexcept -> tRTCCAlarm2;

    [[nodiscard]] auto SetAlarm1Mode(const tRTCCAlarm1& aAlarmStruct, Drivers::Alarm::eRate aRate) noexcept -> tRTCCAlarm1;
    [[nodiscard]] auto SetAlarm2Mode(const tRTCCAlarm2& aAlarmStruct, Drivers::Alarm::eRate aRate) noexcept -> tRTCCAlarm2;

    void WrAlarm1Struct(const tRTCCAlarm1& aRTCCAlarm) noexcept;
    void WrAlarm2Struct(const tRTCCAlarm2& aRTCCAlarm) noexcept;

    enum class eAlarmID
    {
        ALARM_ID_1,
        ALARM_ID_2,
    };
    using tAlarmID = enum eAlarmID;
    void EnableAlarm(tAlarmID aAlarmID) noexcept;

    static constexpr auto mBaseYear {2000};

    std::array<Alarm, 2> mAlarms {};

    CoreLink::SPIRd mSPIRd;
    CoreLink::SPIWr mSPIWr;

    tTime mTimeCache {};
    tDate mDateCache {};
    mutable bool mIsCacheValid {false};
};

// ******************************************************************************
//                            EXPORTED VARIABLES
// ******************************************************************************

namespace DS3234Helper
{


constexpr std::byte BinaryToBCD(const uint8_t aByte) noexcept
{
    const auto lLowNibble {aByte % 10};
    const auto lHighNibble {aByte / 10};

    return std::byte(lHighNibble << 4 | lLowNibble);
}

constexpr uint8_t BCDToBinary(const std::byte aBCD) noexcept
{
    const auto lBCD {std::to_integer<uint8_t>(aBCD)};
    const auto lUnits {lBCD & 0x0F};
    const auto lDecimals {(lBCD & 0xF0) * 10};

    return lDecimals + lUnits;
}


} // namespace DS3234Helper


} // namespace Drivers

// ******************************************************************************
//                                 EXTERNS
// ******************************************************************************

// ******************************************************************************
//                            EXPORTED FUNCTIONS
// ******************************************************************************

// ******************************************************************************
//                                END OF FILE
// ******************************************************************************
#endif // DRIVERS__DS3234_H_
