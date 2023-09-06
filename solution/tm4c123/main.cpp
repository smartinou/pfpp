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

// Standard Libraries.
#include <array>
#include <chrono>
#include <numeric>

// Firmware Libraries.
#include "inc/FeedCfg.h"
#include "drivers/inc/IMotorControl.h"

// QM codegen.
#include "qp_ao/codegen/PFPP_AOs.h"
#include "qp_ao/codegen/PFPP_Events.h"
#include "qp_ao/codegen/Signals.h"

// From CMSIS-Pack.
#include "TM4C123GH6PM.h"        // the device specific header (TI)

// This project.
// Borrowed from TI TivaWare libraries.
#include "gpio.h"
#include "rom.h"
#include "sysctl.h"

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
        GPIOF->RESERVED[LED_GREEN] = 0U;
    }
    void TurnOnCCW([[maybe_unused]] unsigned int aDutyCycle = 100) const { /* Do nothing. */}
    void TurnOff() const
    {
        GPIOF->RESERVED[LED_GREEN] = 0xFFU;
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

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************

int main()
{
    Init();

    // Initialize the framework and the underlying RT kernel.
    QP::QF::init();

    // Initialize event pool.
    static QF_MPOOL_EL(PFPP::Event::Mgr::Init) sSmallPoolSto[20] {};
    QP::QF::poolInit(
        sSmallPoolSto,
        sizeof(sSmallPoolSto),
        sizeof(sSmallPoolSto[0])
    );

    // Init publish-subscribe.
    static std::array<QP::QSubscrList, QTY_SIG> lSubsribeSto {};
    QP::QF::psInit(lSubsribeSto.data(), Q_DIM(lSubsribeSto));

    // Send object dictionaries for event pools...
    QS_OBJ_DICTIONARY(sSmallPoolSto);
    QS_FUN_DICTIONARY(&QP::QHsm::top);

    static constexpr FeedCfg lFeedCfg {};

    static constexpr auto sBSPTicksPerSecond {100};
    using Ticks = std::chrono::duration<QP::QTimeEvtCtr, std::ratio<1, sBSPTicksPerSecond>>;
    auto lToTicksFct {
        [](const auto aDuration)
        {
            return std::chrono::duration_cast<Ticks>(aDuration).count();
        }
    };

    static QP::QEvt const *lEventQSto[10]; // Event queue storage for Blinky
    PFPP::AO::Mgr_AO lPFPPAO(
        std::make_unique<DummyMotorControl>(),
        lFeedCfg,
        lToTicksFct
    );

    lPFPPAO.start(
        1U,       // QF-priority/preemption-threshold
        lEventQSto, Q_DIM(lEventQSto), // event queue
        nullptr, 0U                // stack (unused)
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

    // configure the Buttons
    GPIOF->DIR &= ~(BTN_SW1 | BTN_SW2); //  set direction: input
    ROM_GPIOPadConfigSet(
        GPIOF_BASE,
        (BTN_SW1 | BTN_SW2),
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );
}


// QF callbacks ==============================================================
void QP::QF::onStartup()
{
}


void QP::QF::onCleanup()
{
}


//............................................................................
void QP::QV::onIdle()
{ // CAUTION: called with interrupts DISABLED, NOTE01

    // toggle LED2 on and then off, see NOTE01
    GPIOF->RESERVED[LED_BLUE] = 0xFFU;
    GPIOF->RESERVED[LED_BLUE] = 0x00U;

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

extern "C"
{

//............................................................................
void SysTick_Handler(void)
{
#ifdef Q_SPY
    // QSpy source IDs
    static const QP::QSpyId l_SysTick_Handler {0U};
#endif

    //QTimeEvt::TICK_X(0U, nullptr); // process time events for rate 0
        // Call QF Tick function.
    QP::QF::TICK_X(0U, nullptr);

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

#ifdef Q_SPY
    static QP::QSpyId const sSysTick_Handler{0U};
#endif // Q_SPY
    // What changed now? Look for pressed states.
    if ((!sPreviousDebounce) & lCurrentDebounce) {

        if (lCurrentDebounce & BTN_SW1) {
            static const PFPP::Event::Mgr::ManualFeedCmd sOnEvt {FEED_MGR_MANUAL_FEED_CMD_SIG, 0U, 0U, true};
            QP::QF::PUBLISH(&sOnEvt, &sSysTick_Handler);
        }
        if (lCurrentDebounce & BTN_SW2) {
            // FireTimedFeedEvt: either custom here, or from PFPP::AO::Mgr::FireTimedFeedEvt().
            static const PFPP::Event::Mgr::TimedFeedCmd sOnEvent {FEED_MGR_TIMED_FEED_CMD_SIG, 0U, 0U};
            QP::QF::PUBLISH(&sOnEvent, &sSysTick_Handler);
        }
    }

    // Look for released states.
    if (sPreviousDebounce & !lCurrentDebounce) {
        if ((!lCurrentDebounce) & BTN_SW1) {
            static const PFPP::Event::Mgr::ManualFeedCmd sEvt {FEED_MGR_MANUAL_FEED_CMD_SIG, 0U, 0U, false};
            QP::QF::PUBLISH(&sEvt, &sSysTick_Handler);
        }
    }

    sPreviousDebounce = lCurrentDebounce;
}

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
