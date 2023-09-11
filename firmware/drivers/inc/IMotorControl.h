#ifndef DRIVERS__IMOTORCTRL_H_
#define DRIVERS__IMOTORCTRL_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: Motor controller.
//
// *******************************************************************************

//! \file
//! \brief Motor controller interface class.
//! \ingroup ext_peripherals

// ******************************************************************************
//
//        Copyright (c) 2016-2022, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// ******************************************************************************

// ******************************************************************************
//                              INCLUDE FILES
// ******************************************************************************

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Drivers
{


//! \brief Motor Controller interface.
class IMotorControl
{
public:
    virtual ~IMotorControl() = default;

    // Sets/clears the entry for the specified time, rounded to quarter hour.
    virtual void TurnOnCW(unsigned int aDutyCycle = 100) const = 0;
    virtual void TurnOnCCW(unsigned int aDutyCycle = 100) const = 0;
    virtual void TurnOff() const = 0;
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
#endif // DRIVERS__IMOTORCTRL_H_
