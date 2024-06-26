// *****************************************************************************
//
// Project: ARM Cortex-M.
//
// Module: CoreLink Peripherals.
//
// *****************************************************************************

//! \file
//! \brief CoreLink peripheral GPIO class definition.
//! \ingroup corelink_peripherals

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

// This project.
#include "corelink/inc/GPIO.h"

// TI Library.
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

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

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

namespace CoreLink
{


void GPIO::EnableSysCtlPeripheral() const noexcept
{
    switch (mBaseAddr) {
    case GPIO_PORTA_AHB_BASE:
    case GPIO_PORTA_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); break;
    case GPIO_PORTB_AHB_BASE:
    case GPIO_PORTB_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); break;
    case GPIO_PORTC_AHB_BASE:
    case GPIO_PORTC_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); break;
    case GPIO_PORTD_AHB_BASE:
    case GPIO_PORTD_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); break;
    case GPIO_PORTE_AHB_BASE:
    case GPIO_PORTE_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); break;
    case GPIO_PORTF_AHB_BASE:
    case GPIO_PORTF_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); break;
    case GPIO_PORTG_AHB_BASE:
    case GPIO_PORTG_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG); break;
    case GPIO_PORTH_AHB_BASE:
    case GPIO_PORTH_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH); break;
    case GPIO_PORTJ_AHB_BASE:
    case GPIO_PORTJ_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); break;
    case GPIO_PORTK_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK); break;
    case GPIO_PORTL_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL); break;
    case GPIO_PORTM_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM); break;
    case GPIO_PORTN_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); break;
    case GPIO_PORTP_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP); break;
    case GPIO_PORTQ_BASE: ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ); break;
    default: /* Do nothing. */ break;
    }
}


} // namespace CoreLink

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
