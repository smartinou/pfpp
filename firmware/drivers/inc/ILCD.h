#ifndef DRIVERS__ILCD_H_
#define DRIVERS__ILCD_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: LCD driver.
//
// *******************************************************************************

//! \file
//! \brief Motor controller interface class.
//! \ingroup ext_peripherals

// ******************************************************************************
//
//        Copyright (c) 2016-2023, Martin Garon, All rights reserved.
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


//! \brief LCD interface.
class ILCD
{
public:
    virtual ~ILCD() = default;

    virtual void Init() = 0;
    virtual void DisplayOn() const = 0;
    virtual void DisplayOff() const = 0;
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
#endif // DRIVERS__ILCD_H_
