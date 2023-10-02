#ifndef DRIVERS__INVMEMDEV_H_
#define DRIVERS__INVMEMDEV_H_
// *******************************************************************************
//
// Project: Drivers.
//
// Module: NVMem.
//
// *******************************************************************************

//! \file
//! \brief Non-volatile memory interface class.
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
#include <span>

// ******************************************************************************
//                       DEFINED CONSTANTS AND MACROS
// ******************************************************************************

// ******************************************************************************
//                         TYPEDEFS AND STRUCTURES
// ******************************************************************************

namespace Drivers
{


//! \brief Non-Volatile Memory interface.
class INVMem
{
public:
    virtual ~INVMem() = default;

    virtual std::size_t GetNVMemSize() const = 0;
    virtual void RdFromNVMem(
        std::span<std::byte> aData,
        std::size_t aOffset
    ) = 0;
    virtual void WrToNVMem(
        std::span<const std::byte> aData,
        std::size_t aOffset
    ) = 0;
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
#endif // DRIVERS__INVMEMDEV_H_
