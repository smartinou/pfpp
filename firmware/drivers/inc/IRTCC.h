#ifndef DRIVERS_IRTCC_H_
#define DRIVERS_IRTCC_H_
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
#include <chrono>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

//! \brief RTCC interface.
class IRTCC {
public:
    virtual ~IRTCC() = default;

    using tTime = std::chrono::hh_mm_ss<std::chrono::seconds>;
    //using tTime = std::chrono::seconds;
    using tDate = std::chrono::year_month_day;
    using tWeekday = std::chrono::weekday;
    struct Entry_s {
        tTime mTime{};
        tDate mDate{};
    };
    using tTimeAndDate = struct Entry_s;

    virtual void Init() = 0;
    virtual void SetInterrupt(bool aEnable) = 0;
    virtual bool ISR() = 0;

    virtual auto RdTimeAndDate() const -> tTimeAndDate = 0;

    virtual void WrTime(tTime const &aTime) = 0;
    virtual void WrDate(tDate const &aDate) = 0;
    virtual void WrTimeAndDate(tTime const &aTime, tDate const &aDate) = 0;

    virtual void WrAlarm(tTime const &aTime, tDate const &aDate) = 0;
    virtual void WrAlarm(tTime const &aTime, tWeekday const &aWeekday) = 0;
    virtual void DisableAlarm() = 0;
};

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
#endif // DRIVERS_IRTCC_H_
