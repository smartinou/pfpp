#include <array>

#include "inc/FeedCfg.h"
#include "qp_ao/codegen/PFPP_AOs.h"
#include "qp_ao/codegen/PFPP_Events.h"
#include "qp_ao/codegen/Signals.h"

#include "drivers/inc/IMotorControl.h"

// From CMSIS-Pack.
#include "TM4C123GH6PM.h"        // the device specific header (TI)


class DummyMotorControl final
    : public Drivers::IMotorControl
{
    // IMotorControl interface.
    void TurnOnCW([[maybe_unused]] unsigned int aDutyCycle = 100) const {/* Do nothing. */}
    void TurnOnCCW([[maybe_unused]] unsigned int aDutyCycle = 100) const { /* Do nothing. */}
    void TurnOff() const {/* Do nothing. */}
};


int main()
{
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
    static constexpr auto lBSPTicksPerSecond {100};

    PFPP::AO::Mgr_AO lPFPPAO(
        std::make_unique<DummyMotorControl>(),
        lFeedCfg,
        lBSPTicksPerSecond
    );

    return 0;
}


// BSP functions =============================================================
void BSP_init()
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
    GPIOF->DATA_Bits[LED_RED]   = 0U;  // turn the LED off
    GPIOF->DATA_Bits[LED_GREEN] = 0U;  // turn the LED off
    GPIOF->DATA_Bits[LED_BLUE]  = 0U;  // turn the LED off

    // configure the Buttons
    GPIOF->DIR &= ~(BTN_SW1 | BTN_SW2); //  set direction: input
    ROM_GPIOPadConfigSet(GPIOF_BASE, (BTN_SW1 | BTN_SW2),
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
//............................................................................
void BSP_ledOff()
{
    GPIOF->DATA_Bits[LED_GREEN] = 0U;
}
//............................................................................
void BSP_ledOn()
{
    GPIOF->DATA_Bits[LED_GREEN] = 0xFFU;
}



// QF callbacks ==============================================================
void QP::QF::onStartup()
{
}


void QP::QF::onCleanup()
{
}


//............................................................................
void QV::onIdle(void) { // CATION: called with interrupts DISABLED, NOTE01

    // toggle LED2 on and then off, see NOTE01
    GPIOF->DATA_Bits[LED_BLUE] = 0xFFU;
    GPIOF->DATA_Bits[LED_BLUE] = 0x00U;

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
extern "C" Q_NORETURN Q_onAssert(const char* const module, const int loc)
{
    //
    // NOTE: add here your application-specific error handling
    //
    (void)module;
    (void)loc;
    QS_ASSERTION(module, loc, 10000U);
    NVIC_SystemReset();
}

extern "C"
{

//............................................................................
static void SysTick_Handler(void); // prototype
void SysTick_Handler(void)
{
    //QTimeEvt::TICK_X(0U, nullptr); // process time events for rate 0
        // Call QF Tick function.
    QP::QF::TICK_X(0U, &SysTick_Handler);

}

} // extern "C"
