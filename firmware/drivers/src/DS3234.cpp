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
//        Copyright (c) 2015-2022, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// This modules.
#include "inc/SPIMasterDev.h"

#include "inc/DS3234.h"

// TI Library.
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>

// Standard Libraries.
#include <cstdint>
#include <cstdio>

using namespace std::chrono_literals;

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

static constexpr auto sSPIClk{4000000};
static constexpr auto sSPIBits{8};

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

// Ctor.
DS3234::DS3234(
    unsigned long const aInterruptNumber,
    CoreLink::GPIO const &aInterruptPin,
    std::shared_ptr<CoreLink::ISPIMasterDev> const aSPIMasterDev,
    CoreLink::GPIO const &aCSnPin
) noexcept
    : mSPIMasterDev{std::move(aSPIMasterDev)}
    , mSPISlaveCfg{
        CoreLink::GPIO{aCSnPin.mBaseAddr, aCSnPin.mPin},
        CoreLink::SPISlaveCfg::tProtocol::MOTO_1,
        sSPIClk,
        sSPIBits
    }
    , mInterruptNumber{aInterruptNumber}
    , mInterruptGPIO{aInterruptPin}
{
    mSPISlaveCfg.InitCSnGPIO();
}


void DS3234::Init() noexcept {

    // Configure the RTC to desired state:
    //   -Oscillator enabled.
    //   -Battery-backed square wave disabled.
    //   -Interrupt enabled (square wave disabled).
    //   -Alarm 'N' disabled.
    // Disable all and clear all flags in status register.
    //mRegMap.mCtrl = std::byte{eCtrl::INTCn};
    //mRegMap.mStatus = std::byte{0x00};
    static constexpr std::array<tRTCCReg, 2> sCtrlStatus{std::byte{eCtrl::INTCn}, std::byte{0x00}};
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{sCtrlStatus}),//std::as_bytes(std::span{&mRegMap.mCtrl, 2 * sizeof(tRTCCReg)}),
        ToWrAddr(sCtrlAddr)
    );

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


void DS3234::SetInterrupt(bool const aEnable) noexcept {

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

        // Alarm1 is dedicated to periodic interrupt.
        // This is also where we can throttle down the frequency of clock updates.
        // Instead of using square wave, the alarm is triggered periodically.
        WrAlarm1(
            tTime{},
            tDate{},
            eAlarm1Mode::ONCE_PER_SEC
        );
    }
}


bool DS3234::ISR() noexcept {

    // Read Status registers.
    std::array<tRTCCReg, 1> lRTCCStatus{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{lRTCCStatus}),
        sStatusAddr
    );

    // Periodic interrupt?
    std::byte lFlagMask{};
    if (std::to_integer<bool>(lRTCCStatus[0] & std::byte{eStatus::AF1})) {
        lFlagMask |= std::byte{eStatus::AF1};
        mIsCacheValid = false;
    }

    // Alarm interrupt?
    if (std::to_integer<bool>(lRTCCStatus[0] & std::byte{eStatus::AF2})) {
        lFlagMask |= std::byte{eStatus::AF2};
    }

    // Clear raised flags.
    if (std::to_integer<bool>(lFlagMask)) {
        lRTCCStatus[0] &= ~lFlagMask;
        mSPIMasterDev->WrData(
            mSPISlaveCfg,
            std::as_bytes(std::span{lRTCCStatus}),
            ToWrAddr(sStatusAddr)
        );
    }

    return std::to_integer<bool>(lFlagMask & std::byte{eStatus::AF2});
}


auto DS3234::RdTimeAndDate() const noexcept -> IRTCC::tTimeAndDate {

    if (mIsPolled || !mIsCacheValid) {
        // Read the whole RTC into register map structure.
        // Update time ref.
        // Update date ref.
        tRTCCTimeDate lRTCCTimeDate{};
        mSPIMasterDev->RdData(
            mSPISlaveCfg,
            std::as_writable_bytes(std::span{&lRTCCTimeDate, sizeof(lRTCCTimeDate)}),
            sSecondsAddr
        );

        mTimeCache = UpdateTime(lRTCCTimeDate);
        mDateCache = UpdateDate(lRTCCTimeDate);
        mIsCacheValid = !mIsPolled;
    }
    return {mTimeCache, mDateCache};
}


void DS3234::WrTime(IRTCC::tTime const &aTime) noexcept {

    tRTCCTimeDate lRTCCTimeAndDate{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{&lRTCCTimeAndDate, sizeof(lRTCCTimeAndDate)}),
        sSecondsAddr
    );

    // Fill time structure to write to RTC.
    FillTimeStruct(aTime, lRTCCTimeAndDate);
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{&lRTCCTimeAndDate, sizeof(tRTCCTimeDate)}),
        ToWrAddr(sSecondsAddr)
    );
    mIsCacheValid = false;
}


void DS3234::WrDate(IRTCC::tDate const &aDate) noexcept {

    tRTCCTimeDate lRTCCTimeAndDate{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{&lRTCCTimeAndDate, sizeof(lRTCCTimeAndDate)}),
        sSecondsAddr
    );

    // Fill date structure to write to RTC.
    // [MG] REPEATED CODE!!!
    FillDateStruct(aDate, lRTCCTimeAndDate);
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{&lRTCCTimeAndDate, sizeof(tRTCCTimeDate)}),
        ToWrAddr(sSecondsAddr)
    );
    mIsCacheValid = false;
}


void DS3234::WrTimeAndDate(IRTCC::tTime const &aTime, IRTCC::tDate const &aDate) noexcept {

    // Fill structure with time to write to RTC.
    // Fill structure with date to write to RTC.
    // Send time and date portion of the structure to RTC.
    tRTCCTimeDate lRTCCTimeAndDate{};
    FillTimeStruct(aTime, lRTCCTimeAndDate);
    FillDateStruct(aDate, lRTCCTimeAndDate);

    // [MG] REPEATED CODE!!!
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{&lRTCCTimeAndDate, sizeof(lRTCCTimeAndDate)}),
        ToWrAddr(sSecondsAddr)
    );
    mIsCacheValid = false;
}


auto DS3234::GetTemperature() const noexcept -> float {

    // Return temperature field.
    // Convert temperature MSB and LSB to float.
    std::array<std::byte, 2> lRTCCTemperature{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        lRTCCTemperature,
        sTemperatureMSB
    );

    float const lTempFloat{
        static_cast<float>(lRTCCTemperature[0])
        + (0.25F * static_cast<float>(std::to_integer<uint8_t>(lRTCCTemperature[1] >> 6)))
    };
    return lTempFloat;
}


void DS3234::WrAlarm(IRTCC::tTime const &aTime, IRTCC::tDate const &aDate) noexcept {

    // Use alarm2.
    // Fill alarm structure to write to RTC.
    tAlarm2Mode constexpr sAlarmMode{eAlarm2Mode::WHEN_DATE_HOURS_MINS_MATCH};
    auto const lAlarm2{FillAlarm2Struct(aTime, aDate, sAlarmMode)};

    // Send alarm and control portion of the structure to the RTC.
    TxAlarm2Struct(lAlarm2);
    EnableAlarm(eAlarmID::ALARM_ID_2);
}


void DS3234::WrAlarm(IRTCC::tTime const &aTime, DS3234::tWeekday const &aWeekday) noexcept {

    // Use alarm2.
    // Fill alarm structure to write to RTC.
    tAlarm2Mode constexpr sAlarmMode{eAlarm2Mode::WHEN_DAY_HOURS_MINS_MATCH};
    auto const lAlarm2{FillAlarm2Struct(aTime, aWeekday, sAlarmMode)};

    // Send alarm and control portion of the structure to the RTC.
    TxAlarm2Struct(lAlarm2);
    EnableAlarm(eAlarmID::ALARM_ID_2);
}


void DS3234::DisableAlarm() noexcept {

    // Read-modify-write Control register.
    std::array<tRTCCReg, 1> lRTCCCtrl{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{lRTCCCtrl}),
        sCtrlAddr
    );
    lRTCCCtrl[0] &= ~std::byte{eCtrl::AEI2};
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{lRTCCCtrl}),
        ToWrAddr(sCtrlAddr)
    );
}


void DS3234::RdFromNVMem(
    std::span<std::byte> const aData,
    std::size_t const aOffset
) noexcept {

    // Write SRAM address register.
    // Cap size.
    // Loop into data register.
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{reinterpret_cast<std::byte *>(aOffset), sizeof(tRTCCReg)}),
        ToWrAddr(sSRAMAddrAddr)
    );

    mSPIMasterDev->RdData(mSPISlaveCfg, aData, sSRAMDataAddr);
}


void DS3234::WrToNVMem(
    std::span<std::byte const> const aData,
    std::size_t const aOffset
) noexcept {

    // Write SRAM address register.
    // Cap size.
    // Loop into data register.
    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{reinterpret_cast<std::byte *>(aOffset), sizeof(tRTCCReg)}),
        ToWrAddr(sSRAMAddrAddr)
    );

    mSPIMasterDev->WrData(mSPISlaveCfg, aData, ToWrAddr(sSRAMDataAddr));
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

void DS3234::FillTimeStruct(IRTCC::tTime const &aTime, tRTCCTimeDate &aRTCCTimeDate) noexcept {

    aRTCCTimeDate.mSeconds = DS3234Helper::BinaryToBCD(aTime.seconds().count());
    aRTCCTimeDate.mMinutes = DS3234Helper::BinaryToBCD(aTime.minutes().count());
    // Time is always stored as 24H.
    aRTCCTimeDate.mHours = DS3234Helper::BinaryToBCD(aTime.hours().count());
}


void DS3234::FillDateStruct(IRTCC::tDate const &aDate, tRTCCTimeDate &aRTCCTimeDate) noexcept {

    std::chrono::weekday const lWeekday{std::chrono::sys_days{aDate}};
    auto const lYears{std::chrono::years(int(aDate.year()) - int(std::chrono::year{mBaseYear}))};

    aRTCCTimeDate.mWeekday = std::byte{static_cast<unsigned char>(lWeekday.iso_encoding())};
    aRTCCTimeDate.mDate = DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.day()));
    aRTCCTimeDate.mMonth = DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.month()));
    aRTCCTimeDate.mYear = DS3234Helper::BinaryToBCD(lYears.count());
}


auto DS3234::FillAlarm2Struct(
    IRTCC::tTime const &aTime,
    IRTCC::tDate const &aDate,
    tAlarm2Mode const aAlarmMode
) noexcept -> DS3234::tRTCCAlarm2
{
    auto const lHours = DS3234Helper::BinaryToBCD(aTime.hours().count());
    auto const lMinutes = DS3234Helper::BinaryToBCD(aTime.minutes().count());
    auto const lDay = DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.day()));

    tRTCCAlarm2 const lRTCCAlarm{
        lHours,
        lMinutes,
        lDay | std::byte{eDayDateFields::DAY_DATE_n}
    };
    return SetAlarm2Mode(lRTCCAlarm, aAlarmMode);
}


auto DS3234::FillAlarm2Struct(
    IRTCC::tTime const &aTime,
    DS3234::tWeekday const &aWeekday,
    tAlarm2Mode const aAlarmMode
) noexcept -> DS3234::tRTCCAlarm2
{
    auto const lHours = DS3234Helper::BinaryToBCD(aTime.hours().count());
    auto const lMinutes = DS3234Helper::BinaryToBCD(aTime.minutes().count());
    auto const lWeekday = std::byte{static_cast<unsigned char>(aWeekday.iso_encoding())};

    tRTCCAlarm2 const lRTCCAlarm{
        lHours,
        lMinutes,
        lWeekday
    };
    return SetAlarm2Mode(lRTCCAlarm, aAlarmMode);
}


auto DS3234::SetAlarm1Mode(
    tRTCCAlarm1 const &aAlarmStruct,
    tAlarm1Mode const aAlarmMode
) noexcept -> DS3234::tRTCCAlarm1
{
    tRTCCAlarm1 lAlarmStruct{aAlarmStruct};

    // Assumes the AxMy bit was cleared on previous operation.
    // This should be performed by BinaryToBCD().
    switch (aAlarmMode) {
    case eAlarm1Mode::ONCE_PER_SEC:
        lAlarmStruct.mSeconds |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case eAlarm1Mode::WHEN_SECS_MATCH:
        lAlarmStruct.mMinutes |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case eAlarm1Mode::WHEN_MINS_SECS_MATCH:
        lAlarmStruct.mHours |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case eAlarm1Mode::WHEN_HOURS_MINS_SECS_MATCH:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::AnMx};
        // Don't care about DY/DATEn bit.
        break;

    case eAlarm1Mode::WHEN_DAY_HOURS_MINS_SECS_MATCH:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::DAY_DATE_n};
        break;

    case eAlarm1Mode::WHEN_DATE_HOURS_MINS_SECS_MATCH:
    [[fallthrough]];
    default:
        // Nothing to set.
        break;
    }

    return lAlarmStruct;
}


auto DS3234::SetAlarm2Mode(
    tRTCCAlarm2 const &aAlarmStruct,
    tAlarm2Mode const aAlarmMode
) noexcept -> DS3234::tRTCCAlarm2
{
    tRTCCAlarm2 lAlarmStruct{aAlarmStruct};

    // Assumes the AxMy bit was cleared on previous operation.
    // This should be performed by BinaryToBCD().
    switch (aAlarmMode) {
    case eAlarm2Mode::ONCE_PER_MINUTE:
        lAlarmStruct.mMinutes |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case eAlarm2Mode::WHEN_MINS_MATCH:
        lAlarmStruct.mHours |= std::byte{eDayDateFields::AnMx};
        [[fallthrough]];

    case eAlarm2Mode::WHEN_HOURS_MINS_MATCH:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::AnMx};
        // Don't care about DY/DATEn bit.
        break;

    case eAlarm2Mode::WHEN_DAY_HOURS_MINS_MATCH:
        lAlarmStruct.mDayDate_n |= std::byte{eDayDateFields::DAY_DATE_n};
        break;

    case eAlarm2Mode::WHEN_DATE_HOURS_MINS_MATCH:
    [[fallthrough]];
    default:
        // Nothing to set.
        break;
    }

    return lAlarmStruct;
}


void DS3234::TxAlarm1Struct(tRTCCAlarm1 const &aRTCCAlarm) noexcept {

    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{&aRTCCAlarm, sizeof(tRTCCAlarm1)}),
        ToWrAddr(sAlarm1Addr)
    );

    // Send control 1.
}


void DS3234::TxAlarm2Struct(tRTCCAlarm2 const &aRTCCAlarm) noexcept {

    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{&aRTCCAlarm, sizeof(tRTCCAlarm2)}),
        ToWrAddr(sAlarm2Addr)
    );

    // Send control 2.
}


void DS3234::EnableAlarm(tAlarmID const aAlarmID) noexcept {

    std::array<tRTCCReg, 1> lRTCCCtrl{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{lRTCCCtrl}),
        sCtrlAddr
    );

    switch (aAlarmID) {
    case eAlarmID::ALARM_ID_1: lRTCCCtrl[0] |= std::byte{eCtrl::AEI1}; break;
    case eAlarmID::ALARM_ID_2: lRTCCCtrl[0] |= std::byte{eCtrl::AEI2}; break;
    default: return;
    }

    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{lRTCCCtrl}),
        ToWrAddr(sCtrlAddr)
    );
}


void DS3234::ClrAlarmFlag(tAlarmID const aAlarmID) noexcept {

    std::array<tRTCCReg, 1> lRTCCStatus{};
    mSPIMasterDev->RdData(
        mSPISlaveCfg,
        std::as_writable_bytes(std::span{lRTCCStatus}),
        sStatusAddr
    );

    // Only clear interrupt.
    // We don't care if the Alarm fields are set,
    // as long as the interrupt is not generated.
    switch (aAlarmID) {
    case eAlarmID::ALARM_ID_1: lRTCCStatus[0] &= ~std::byte{eStatus::AF1}; break;
    case eAlarmID::ALARM_ID_2: lRTCCStatus[0] &= ~std::byte{eStatus::AF2}; break;
    default: return;
    }

    mSPIMasterDev->WrData(
        mSPISlaveCfg,
        std::as_bytes(std::span{lRTCCStatus}),
        ToWrAddr(sStatusAddr)
    );
}


void DS3234::WrAlarm1(
    IRTCC::tTime const &aTime,
    IRTCC::tDate const &aDate,
    tAlarm1Mode const aAlarmMode
) noexcept {

    auto const lSeconds = DS3234Helper::BinaryToBCD(aTime.seconds().count());
    auto const lHours = DS3234Helper::BinaryToBCD(aTime.hours().count());
    auto const lMinutes = DS3234Helper::BinaryToBCD(aTime.minutes().count());
    auto const lDay = DS3234Helper::BinaryToBCD(static_cast<unsigned>(aDate.day()));

    // Send alarm and control portion of the structure to the RTC.
    tRTCCAlarm1 const lRTCCAlarm = SetAlarm1Mode(
        {lSeconds, lHours, lMinutes, lDay},
        aAlarmMode
    );
    TxAlarm1Struct(lRTCCAlarm);
    EnableAlarm(eAlarmID::ALARM_ID_1);
}


auto DS3234::UpdateTime(tRTCCTimeDate const &aRTCCTimeDate) noexcept -> IRTCC::tTime {

    auto lHours{0};
    if (std::to_integer<bool>(aRTCCTimeDate.mHours & std::byte{eHoursFields::H12_24_n})) {
        // Should not take this branch: time is stored in 24H mode.
        auto const lHoursReg = aRTCCTimeDate.mHours
            & ~(std::byte{eHoursFields::H12_24_n} | std::byte{eHoursFields::PM_AM_n});
        lHours = DS3234Helper::BCDToBinary(lHoursReg);
        if (std::to_integer<bool>(aRTCCTimeDate.mHours & std::byte{eHoursFields::PM_AM_n})) {
            lHours += 12;
        }
    } else {
        lHours = DS3234Helper::BCDToBinary(aRTCCTimeDate.mHours);
    }

    auto const lSeconds = DS3234Helper::BCDToBinary(aRTCCTimeDate.mSeconds);
    auto const lMinutes = DS3234Helper::BCDToBinary(aRTCCTimeDate.mMinutes);
    return tTime{
        std::chrono::hours(lHours) 
        + std::chrono::minutes(lMinutes)
        + std::chrono::seconds(lSeconds)
    };
}


auto DS3234::UpdateDate(tRTCCTimeDate const &aRTCCTimeDate) noexcept -> IRTCC::tDate {

    //auto const lWeekday = BCDToBinary(mRegMap.mDate.mWeekday);
    std::chrono::day const lDay {DS3234Helper::BCDToBinary(aRTCCTimeDate.mDate)};
    std::chrono::month const lMonth {DS3234Helper::BCDToBinary(aRTCCTimeDate.mMonth & ~std::byte{eMonthFields::CENTURY})};
    std::chrono::year const lYear {DS3234Helper::BCDToBinary(aRTCCTimeDate.mYear) + mBaseYear};

    // Should check for a valid date here. If not, construct one.
    auto const lDate{tDate{lYear, lMonth, lDay}};
    static constexpr auto lDefaultDate{std::chrono::January/1/2000};
    return lDate.ok() ? lDate : lDefaultDate;
}

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
