#ifndef DRIVERS__IRTCC_H_
#define DRIVERS__IRTCC_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: RTCC.
//
// *******************************************************************************

//! \file
//! \brief RTCC interface class.
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
#include <chrono>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Drivers
{


using tTime = std::chrono::seconds;
using tDate = std::chrono::year_month_day;
using tWeekday = std::chrono::weekday;

class Alarm final
{
public:
    enum class eRate
    {
        OncePerSecond,
        SecondsMatch,
        MinutesSecondMatch,
        HoursMinutesSecondsMatch,
        DateHoursMinutesSecondsMatch,
        DayHoursMinutesSecondsMatch,
        OncePerMinute,
        MinutesMatch,
        HoursMinutesMatch,
        DateHoursMinutesMatch,
        DayHoursMinutesMatch
    };

    using ProcessAlarmFct = bool (*)(void*, Alarm*);

    void SetAlarm(tTime aTime, tDate aDate, Alarm::eRate aPeriod);
    void SetAlarm(tTime aTime, tWeekday aWeekday, Alarm::eRate aPeriod);
    void ClearAlarm();
    bool ProcessAlarm();

private:
    unsigned int mID;
    eRate mRate;
};


//! \brief RTCC interface.
class IRTCC
{
public:
    virtual ~IRTCC() = default;

    struct tTimeAndDate
    {
        tTime mTime{};
        tDate mDate{};
    };

    virtual void Init() = 0;
    virtual void SetInterrupt(bool aEnable) = 0;
    virtual bool ISR() = 0;

    virtual auto RdTimeAndDate() const -> tTimeAndDate = 0;

    virtual void WrTime(tTime aTime) = 0;
    virtual void WrDate(tDate aDate) = 0;
    virtual void WrTimeAndDate(tTime aTime, tDate aDate) = 0;

    virtual void SetAlarm(tTime aTime, tDate aDate, Alarm::eRate aPeriod, unsigned int aAlarmID) = 0;
    virtual void SetAlarm(tTime aTime, tWeekday aWeekday, Alarm::eRate aPeriod, unsigned int aAlarmID) = 0;
    virtual void ClearAlarm(unsigned int aAlarmID) = 0;
    virtual bool ProcessAlarms(const Alarm::ProcessAlarmFct& aFct, void* aParam) = 0;
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
#endif // DRIVERS__IRTCC_H_
