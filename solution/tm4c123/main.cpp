// *****************************************************************************
//
// Project: PFPP.
//
// Module: Application.
//
// *****************************************************************************

//! \file
//! \brief PFPP Application.
//! \ingroup app

// *****************************************************************************
//
//        Copyright (c) 2023, Martin Garon, All rights reserved.
//
// This source code is licensed under the GPL-3.0-style license found in the
// LICENSE file in the root directory of this source tree.
//
// *****************************************************************************

// *****************************************************************************
//                              INCLUDE FILES
// *****************************************************************************

// RTE.
#include <RTE_Components.h>
#ifdef RTE_UTILITY_QP_CPP_QS
#include <Pre_Include_Utility_QS.h>
#endif

#include <qpcpp.hpp>

// Standard Libraries.
#include <array>
#include <chrono>
#include <numeric>

// Firmware Libraries.
#include "inc/FeedCfg.h"
#include "drivers/inc/IMotorControl.h"

// CoreLink Library.
#include "corelink/inc/SPIMasterDev.h"

// QM codegen.
#include "qp_ao/codegen/PFPP_AOs.h"
#include "qp_ao/codegen/PFPP_Events.h"
#include "qp_ao/codegen/Signals.h"

// From CMSIS-Pack.
// the device specific header (TI)
#include "TM4C123GH6PM.h"

// This project.
// Borrowed from TI TivaWare libraries.
#include "corelink/tm4c/gpio.h"
#include "corelink/tm4c/pin_map.h"
#include "corelink/tm4c/rom.h"
#include "corelink/tm4c/sysctl.h"

// *****************************************************************************
//                      DEFINED CONSTANTS AND MACROS
// *****************************************************************************

static constexpr uint32_t LED_RED {0x1U << 1};
static constexpr uint32_t LED_GREEN {0x1U << 3};
static constexpr uint32_t LED_BLUE {0x1U << 2};

static constexpr uint32_t BTN_SW1 {0x1U << 4};
static constexpr uint32_t BTN_SW2 {0x1U << 0};

// *****************************************************************************
//                         TYPEDEFS AND STRUCTURES
// *****************************************************************************

class DummyMotorControl final
    : public Drivers::IMotorControl
{
    // IMotorControl interface.
    void TurnOnCW([[maybe_unused]] unsigned int aDutyCycle = 100) const
    {
        GPIOF->RESERVED[LED_RED] = 0xFFU;
    }

    void TurnOnCCW([[maybe_unused]] unsigned int aDutyCycle = 100) const
    {
        // Do nothing.
    }

    void TurnOff() const
    {
        GPIOF->RESERVED[LED_RED] = 0U;
    }
};

// *****************************************************************************
//                            FUNCTION PROTOTYPES
// *****************************************************************************

static void Init();
static void DebounceSwitches();

// *****************************************************************************
//                             GLOBAL VARIABLES
// *****************************************************************************

static constexpr auto sBSPTicksPerSecond {100};

#ifdef Q_SPY

static QP::QSTimeCtr QS_tickTime_ {};
static QP::QSTimeCtr QS_tickPeriod_ {};

// QSpy source IDs
static constexpr QP::QSpyId sSysTick_Handler {0U};
static constexpr QP::QSpyId sOnFlush {0U};

#endif // Q_SPY


// PB4: SSI0CLK
// PB6: SSI0RX (MISO)
// PB7: SSI0TX (MOSI)
static constexpr CoreLink::SSIGPIO sSSI2GPIO{
    .mClkPinCfg{GPIO_PB4_SSI2CLK},
    .mDat0PinCfg{GPIO_PB6_SSI2RX},
    .mDat1PinCfg{GPIO_PB7_SSI2TX},

    .mClkPin{GPIOB_BASE, GPIO_PIN_4},
    .mRxPin{GPIOB_BASE, GPIO_PIN_6},
    .mTxPin{GPIOB_BASE, GPIO_PIN_7}
};

static constexpr CoreLink::SPIMasterDev sSPIMasterDev{
    SSI2_BASE,
    50000000UL
};

static constexpr CoreLink::SPISlaveCfg sRTCCSPISlaveCfg{
    .mProtocol{CoreLink::SPISlaveCfg::tProtocol::MOTO_0},
    .mBitRate{4000000UL},
    .mDataWidth{8},
    .mCSn{GPIOA_BASE, GPIO_PIN_3}
};

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************
int main()
{
    // Initialize the framework and the underlying RT kernel.
    QP::QF::init();

    // Initialize event pool.
    static QF_MPOOL_EL(PFPP::Event::Feeder::ManualFeedCmd) sSmallPoolSto[20] {};
    QP::QF::poolInit(
        sSmallPoolSto,
        sizeof(sSmallPoolSto),
        sizeof(sSmallPoolSto[0])
    );

    // Init publish-subscribe.
    static std::array<QP::QSubscrList, QTY_SIG> lSubsribeSto {};
    QP::QF::psInit(lSubsribeSto.data(), lSubsribeSto.size());

    // Send object dictionaries for event pools...
    QS_OBJ_DICTIONARY(sSmallPoolSto);
    QS_FUN_DICTIONARY(&QP::QHsm::top);

    Init();

    using Ticks = std::chrono::duration<QP::QTimeEvtCtr, std::ratio<1, sBSPTicksPerSecond>>;
    auto lToTicksFct {
        [](const auto aDuration)
        {
            return std::chrono::duration_cast<Ticks>(aDuration).count();
        }
    };

    static constexpr auto sAlarmID {0};
    PFPP::AO::Mgr lPFPPAO {
        sAlarmID,
        std::make_unique<DummyMotorControl>(),
        lToTicksFct
    };

    static std::array<const QP::QEvt*, 10> sEventQSto {};
    lPFPPAO.start(
        1U,
        sEventQSto.data(),
        sEventQSto.size(),
        nullptr, 0U
    );

    return QP::QF::run();
}

// *****************************************************************************
//                              LOCAL FUNCTIONS
// *****************************************************************************

// BSP functions =============================================================
static void Init()
{
    // NOTE: SystemInit() already called from the startup code
    //  but SystemCoreClock needs to be updated
    //
    SystemCoreClockUpdate();

    // NOTE: The VFP (hardware Floating Point) unit is configured by QV

    // enable clock for to the peripherals used by this application...
    SYSCTL->RCGCGPIO |= (1U << 5); // enable Run mode for GPIOF

    // configure the LEDs and push buttons
    GPIOF->DIR |= (LED_RED | LED_GREEN | LED_BLUE); // set direction: output
    GPIOF->DEN |= (LED_RED | LED_GREEN | LED_BLUE); // digital enable
    GPIOF->RESERVED[LED_RED]   = 0U;  // turn the LED off
    GPIOF->RESERVED[LED_GREEN] = 0U;  // turn the LED off
    GPIOF->RESERVED[LED_BLUE]  = 0U;  // turn the LED off

    // configure the Buttons:
    // PF0 can be used as NMI, and requires unlocking Commit Register (CR)
    // before any write to PUR, PDR, PAFSEL, PDEN.
    GPIOF->LOCK = 0x4c4f434b;
    GPIOF->CR |= BTN_SW2;
    //GPIOF->PDEN |= BTN_SW2;

    // Set direction: input.
    GPIOF->DIR &= ~(BTN_SW1 | BTN_SW2);
    ROM_GPIOPadConfigSet(
        GPIOF_BASE,
        (BTN_SW1 | BTN_SW2),
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    sSSI2GPIO.SetPins();
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    // Call QS::onStartup().
    // Has to be setup early for dictionary entries to be set.
    QS_INIT(nullptr);
}


// QF callbacks ==============================================================
void QP::QF::onStartup()
{
    // set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate
    SysTick_Config(SystemCoreClock / sBSPTicksPerSecond);

    // assing all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE00
    //
    // !!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    // DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    //
    NVIC_SetPriority(SysTick_IRQn, QF_AWARE_ISR_CMSIS_PRI);
    // ...

    // enable IRQs...
}


//............................................................................
void QP::QF::onCleanup()
{
    QS_EXIT();
}


//............................................................................
void QP::QV::onIdle()
{ // CAUTION: called with interrupts DISABLED, NOTE01

    // toggle LED2 on and then off, see NOTE01
    GPIOF->RESERVED[LED_GREEN] = 0xFFU;
    GPIOF->RESERVED[LED_GREEN] = 0x00U;

#ifdef NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M3 MCU.
    //
    QV_CPU_SLEEP();  // atomically go to sleep and enable interrupts
#else
    QF_INT_ENABLE(); // just enable interrupts
#endif
}


//............................................................................
extern "C" Q_NORETURN Q_onAssert(
    [[maybe_unused]] const char* const module,
    [[maybe_unused]] const int loc
)
{
    //
    // NOTE: add here your application-specific error handling
    //
    QS_ASSERTION(module, loc, 10000U);
    NVIC_SystemReset();
}


// QS callbacks ==============================================================
#ifdef Q_SPY

//............................................................................
bool QP::QS::onStartup([[maybe_unused]] const void* const aArgs)
{
    // Buffer for Quantum Spy.
    static std::array<uint8_t, 2 * 1024> sQSTxBuf {};
    initBuf(sQSTxBuf.data(), sQSTxBuf.size());

    // Buffer for QS receive channel.
    static std::array<uint8_t, 100> sQSRxBuf {};
    rxInitBuf(sQSRxBuf.data(), sQSRxBuf.size());

    // To start the timestamp at zero.
    QS_tickPeriod_ = SystemCoreClock / sBSPTicksPerSecond;
    QS_tickTime_ = QS_tickPeriod_;

    // Setup the QS filters...
    QS_GLB_FILTER(QS_QEP_STATE_ENTRY);
    QS_GLB_FILTER(QS_QEP_STATE_EXIT);
    QS_GLB_FILTER(QS_QEP_STATE_INIT);
    QS_GLB_FILTER(QS_QEP_INIT_TRAN);
    QS_GLB_FILTER(QS_QEP_INTERN_TRAN);
    QS_GLB_FILTER(QS_QEP_TRAN);
    QS_GLB_FILTER(QS_QEP_IGNORED);
    QS_GLB_FILTER(QS_QEP_DISPATCH);
    QS_GLB_FILTER(QS_QEP_UNHANDLED);
    //QS_LOC_FILTER();

    return true;
}


//............................................................................
void QP::QS::onCleanup()
{
    // Used for QUTest only.
}


//............................................................................
// NOTE: invoked with interrupts DISABLED.
QP::QSTimeCtr QP::QS::onGetTime()
{
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {
        return QS_tickTime_ - static_cast<QSTimeCtr>(SysTick->VAL);
    }
    else {
        // The rollover occured, but the SysTick_ISR did not run yet.
        return QS_tickTime_ + QS_tickPeriod_
               - static_cast<QSTimeCtr>(SysTick->VAL);
    }
    return 0;
}


//............................................................................
void QP::QS::onFlush()
{
#if 1
    // At least one block of data ready to get out.
    // Create an event for each of them and publish to all subscribing sinks.
    // [MG] CREATE SINKS WITH lBlockSize BUFFERS.
    uint16_t lBlockSize {1024};
    QF_INT_DISABLE();
    while (const auto lBlock {QS::getBlock(&lBlockSize)}) {
        auto lQSPYBlockEvt {Q_NEW(BSP::Event::QSPYProcBlock, BSP_QSPY_PROC_BLOCK_SIG)};
        QF_INT_ENABLE();

        lQSPYBlockEvt->mBlock = lBlock;
        lQSPYBlockEvt->mSize = lBlockSize;

        // [MG] CONSIDER POSTING DIRECTLY TO SINKS.
        // [MG] GOOD WAY TO CONTROL THE SINKS.
        QP::QF::PUBLISH(lQSPYBlockEvt, &sOnFlush);
        QF_INT_DISABLE();
    }

    QF_INT_ENABLE();
#endif
}


//............................................................................
//! callback function to reset the target (to be implemented in the BSP)
void QP::QS::onReset()
{
    //NVIC_SystemReset();
}

#endif // Q_SPY


extern "C"
{

//............................................................................
void SysTick_Handler(void)
{
#ifdef Q_SPY
    // Clear SysTick_CTRL_COUNTFLAG.
    // Account for the clock rollover.
    [[maybe_unused]] const auto lTemp {SysTick->CTRL};
    QS_tickTime_ += QS_tickPeriod_;
#endif

    // Call QF Tick function.
    QP::QF::TICK_X(0U, &sSysTick_Handler);

    // Perform the debouncing of buttons. The algorithm for debouncing
    // adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    // and Michael Barr, page 71.
    DebounceSwitches();
    QV_ARM_ERRATUM_838869();
}

} // extern "C"


static void DebounceSwitches()
{
    // Read current pin state into array of pin states.
    static constexpr auto sStateDepth {5};
    using PinType = uint32_t;
    static std::array<PinType, sStateDepth> sPinsState {0};
    static PinType sPreviousDebounce {0};
    static decltype(sPinsState.size()) lStateIx {0};
    sPinsState.at(lStateIx) = ~GPIOF->RESERVED[BTN_SW1 | BTN_SW2];
    ++lStateIx;
    if (lStateIx >= sPinsState.size()) {
        lStateIx = 0;
    }

    // Bitwise-AND all last current pin states.
    const auto lCurrentDebounce {
        std::accumulate(
            sPinsState.cbegin(),
            sPinsState.cend(),
            (BTN_SW1 | BTN_SW2),
            std::bit_and<PinType>()
        )
    };

    // What changed now? Look for pressed states.
    if ((~sPreviousDebounce) & lCurrentDebounce) {

        if (lCurrentDebounce & BTN_SW1) {
            static const BSP::Event::ButtonEvt sOnEvt {BSP_MANUAL_FEED_BUTTON_EVT_SIG, 0U, 0U, true};
            QP::QF::PUBLISH(&sOnEvt, &sSysTick_Handler);
        }
        if (lCurrentDebounce & BTN_SW2) {
            static const BSP::Event::ButtonEvt sOnEvt {BSP_TIMED_FEED_BUTTON_EVT_SIG, 0U, 0U, true};
            QP::QF::PUBLISH(&sOnEvt, &sSysTick_Handler);
        }
    }

    // Look for released states.
    if (sPreviousDebounce & ~lCurrentDebounce) {
        if ((sPreviousDebounce) & BTN_SW1) {
            static const BSP::Event::ButtonEvt sOffEvt {BSP_MANUAL_FEED_BUTTON_EVT_SIG, 0U, 0U, false};
            QP::QF::PUBLISH(&sOffEvt, &sSysTick_Handler);
        }
    }

    sPreviousDebounce = lCurrentDebounce;
}

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
