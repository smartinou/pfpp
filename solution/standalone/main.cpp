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
#include "drivers/inc/DS3234.h"
#include "drivers/inc/LS013B7.h"
#include "drivers/inc/TB6612.h"

// CoreLink Library.
#include "corelink/inc/SPIMasterDev.h"
#include "corelink/inc/SSIGPIO.h"

// QM codegen.
#include "qp_ao/codegen/GUI_AOs.h"
#include "qp_ao/codegen/PFPP_AOs.h"
#include "qp_ao/codegen/PFPP_Events.h"
#include "qp_ao/codegen/RTCC_AOs.h"
#include "qp_ao/codegen/Signals.h"

// From CMSIS-Pack.
// the device specific header (TI)
#include "TM4C123GH6PM.h"

// This project.
// Borrowed from TI TivaWare libraries.
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
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

static void Init();
static void InitOutputGPIO(const CoreLink::GPIO& aGPIO) noexcept;

[[nodiscard]] static auto StartMgr() noexcept -> std::unique_ptr<PFPP::AO::Mgr>;
[[nodiscard]] static auto StartGUI() noexcept -> std::unique_ptr<GUI::AO::Mgr>;
[[nodiscard]] static auto StartRTCC() noexcept -> std::unique_ptr<RTCC::AO::Mgr>;

static void DebounceSwitches() noexcept;

// *****************************************************************************
//                             GLOBAL VARIABLES
// *****************************************************************************

static constexpr auto sBSPTicksPerSecond{100};

#ifdef Q_SPY

static QP::QSTimeCtr QS_tickTime_{};
static QP::QSTimeCtr QS_tickPeriod_{};

// QSpy source IDs
static constexpr QP::QSpyId sSysTick_Handler{0U};
static constexpr QP::QSpyId sOnFlush{0U};

#endif // Q_SPY

static constexpr CoreLink::GPIO sLEDRed{GPIOF_BASE, GPIO_PIN_1};
static constexpr CoreLink::GPIO sLEDGreen{GPIOF_BASE, GPIO_PIN_3};
static constexpr CoreLink::GPIO sLEDBlue{GPIOF_BASE, GPIO_PIN_2};

static constexpr auto sButton1Pin{GPIO_PIN_4};
static constexpr auto sButton2Pin{GPIO_PIN_0};

static constexpr CoreLink::GPIO sButton1{GPIOF_BASE, sButton1Pin};
static constexpr CoreLink::GPIO sButton2{GPIOF_BASE, sButton2Pin};

static constexpr CoreLink::GPIO sRTCCInt{GPIOA_BASE, GPIO_PIN_4};

static constexpr CoreLink::SPIMasterDev sSPIMasterDev{
    SSI2_BASE,
    50000000UL
};

// *****************************************************************************
//                            EXPORTED FUNCTIONS
// *****************************************************************************
int main()
{
    // Initialize the framework and the underlying RT kernel.
    QP::QF::init();

    // Initialize event pool.
    static QF_MPOOL_EL(PFPP::Event::Feeder::ManualFeedCmd) sSmallPoolSto[20]{};
    QP::QF::poolInit(
        sSmallPoolSto,
        sizeof(sSmallPoolSto),
        sizeof(sSmallPoolSto[0])
    );

    // Init publish-subscribe.
    static std::array<QP::QSubscrList, QTY_SIG> lSubsribeSto{};
    QP::QF::psInit(lSubsribeSto.data(), lSubsribeSto.size());

    Init();

    // Send object dictionaries for event pools...
    QS_OBJ_DICTIONARY(sSmallPoolSto);
    QS_FUN_DICTIONARY(&QP::QHsm::top);

    // Keep each objects alive until the end of the program.
    enum class ePrio
    {
        RTCC = 1,
        Mgr = 2,
        GUI = 3
    };

    auto lPFPPAO{StartMgr()};
    if (lPFPPAO) {
        static std::array<const QP::QEvt*, 10> sEventQSto{};
        lPFPPAO->start(
            static_cast<int>(ePrio::Mgr),
            sEventQSto.data(),
            sEventQSto.size(),
            nullptr, 0U
        );
    }

    auto lGUIAO{StartGUI()};
    if (lGUIAO) {
        static std::array<const QP::QEvt*, 10> sEventQSto{};
        lGUIAO->start(
            static_cast<int>(ePrio::GUI),
            sEventQSto.data(),
            sEventQSto.size(),
            nullptr, 0U
        );
    }

    auto lRTCCAO{StartRTCC()};
    if (lRTCCAO) {
        static std::array<const QP::QEvt*, 10> sEventQSto{};
        lRTCCAO->start(
            static_cast<int>(ePrio::RTCC),
            sEventQSto.data(),
            sEventQSto.size(),
            nullptr, 0U
        );
    }

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

    InitOutputGPIO(sLEDRed);
    InitOutputGPIO(sLEDGreen);
    InitOutputGPIO(sLEDBlue);

    ROM_GPIOPinWrite(sLEDRed.mBaseAddr, sLEDRed.mPin, 0);
    ROM_GPIOPinWrite(sLEDGreen.mBaseAddr, sLEDGreen.mPin, 0);
    ROM_GPIOPinWrite(sLEDBlue.mBaseAddr, sLEDBlue.mPin, 0);

    // Configure the Buttons:
    // PF0 can be used as NMI, and requires unlocking Commit Register (CR)
    // before any write to PUR, PDR, PAFSEL, PDEN.
    GPIOF->LOCK = 0x4c4f434b;
    GPIOF->CR |= sButton2.mPin;
    //GPIOF->PDEN |= sButton2.mPin;

    // Set direction: input.
    ROM_GPIODirModeSet(sButton1.mBaseAddr, (sButton1.mPin | sButton2.mPin), GPIO_DIR_MODE_IN);
    ROM_GPIOPadConfigSet(
        sButton1.mBaseAddr,
        (sButton1.mPin | sButton2.mPin),
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    // RTCC pins, SPI peripheral.
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

    sSSI2GPIO.SetPins();
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    // Call QS::onStartup().
    // Has to be setup early for dictionary entries to be set.
    QS_INIT(nullptr);
}


static void InitOutputGPIO(const CoreLink::GPIO& aGPIO) noexcept
{
    aGPIO.EnableSysCtlPeripheral();
    ROM_GPIOPinTypeGPIOOutput(aGPIO.mBaseAddr, aGPIO.mPin);
    ROM_GPIODirModeSet(aGPIO.mBaseAddr, aGPIO.mPin, GPIO_DIR_MODE_OUT);
    ROM_GPIOPadConfigSet(
        aGPIO.mBaseAddr,
        aGPIO.mPin,
        GPIO_STRENGTH_8MA,
        GPIO_PIN_TYPE_STD_WPU
    );
}


static auto StartMgr() noexcept -> std::unique_ptr<PFPP::AO::Mgr>
{
    // TB6612 Motor Controller pins.
#if 0
    static constexpr CoreLink::GPIO sAIn1{GPIOD_BASE, GPIO_PIN_7};
    static constexpr CoreLink::GPIO sAIn2{GPIOA_BASE, GPIO_PIN_2};
    static constexpr CoreLink::GPIO sPWMA{GPIOF_BASE, GPIO_PIN_3};

    sAIn1.EnableSysCtlPeripheral();
    GPIOD->LOCK = 0x4c4f434b;
    GPIOD->CR |= sAIn1.mPin;

    InitOutputGPIO(sAIn1);
    InitOutputGPIO(sAIn2);
    InitOutputGPIO(sPWMA);
#else
    static constexpr CoreLink::GPIO sBIn1{GPIOC_BASE, GPIO_PIN_7};
    static constexpr CoreLink::GPIO sBIn2{GPIOC_BASE, GPIO_PIN_6};
    static constexpr CoreLink::GPIO sPWMB{GPIOF_BASE, GPIO_PIN_2};

    InitOutputGPIO(sBIn1);
    InitOutputGPIO(sBIn2);
    InitOutputGPIO(sPWMB);
#endif

    static constexpr CoreLink::GPIO sSTBYn{GPIOD_BASE, GPIO_PIN_6};
    InitOutputGPIO(sSTBYn);

    static constexpr auto sAlarmID{0};
    auto lMotorControl{std::make_unique<Drivers::TB6612Port>(sBIn1, sBIn2, sPWMB, sSTBYn)};
    return std::make_unique<PFPP::AO::Mgr>(
        sAlarmID,
        std::move(lMotorControl),
        [](const auto aDuration)
        {
            // Converts duration to ticks.
            using Ticks = std::chrono::duration<QP::QTimeEvtCtr, std::ratio<1, sBSPTicksPerSecond>>;
            return std::chrono::duration_cast<Ticks>(aDuration).count();
        }
    );
}


static auto StartGUI() noexcept -> std::unique_ptr<GUI::AO::Mgr>
{
    [[maybe_unused]] static constexpr CoreLink::GPIO sLCDExtComIn{GPIOB_BASE, GPIO_PIN_2};

    // Default option to power the board.
    // See slau786.pdf, section 2.3.3 Customizable LCD Power.
    static constexpr CoreLink::GPIO sLCDPwr{GPIOB_BASE, GPIO_PIN_5};
    InitOutputGPIO(sLCDPwr);
    ROM_GPIOPinWrite(sLCDPwr.mBaseAddr, sLCDPwr.mPin, sLCDPwr.mPin);

    static constexpr CoreLink::GPIO sLCDDisp{GPIOE_BASE, GPIO_PIN_4};
    InitOutputGPIO(sLCDDisp);
    ROM_GPIOPinWrite(sLCDDisp.mBaseAddr, sLCDDisp.mPin, 0);

    static constexpr CoreLink::SPISlaveCfg sLCDSPISlaveCfg{
        .mProtocol{CoreLink::SPISlaveCfg::tProtocol::MOTO_0},
        .mBitRate{1000000UL},
        .mDataWidth{8},
        .mCSn{GPIOE_BASE, GPIO_PIN_5, CoreLink::CSnGPIO::tCSPolarity::ActiveHigh}
    };
    InitOutputGPIO(sLCDSPISlaveCfg.mCSn);
    sLCDSPISlaveCfg.mCSn.DeassertCSn();

    auto lLCD{
        std::make_shared<Drivers::LS013B7>(
            []() noexcept {sLCDSPISlaveCfg.mCSn.AssertCSn();},
            []() noexcept {sLCDSPISlaveCfg.mCSn.DeassertCSn();},
            [](std::span<const std::byte> aData, std::optional<std::byte> aAddr) noexcept
            {
                sSPIMasterDev.WrData(sLCDSPISlaveCfg, aData, aAddr);
            },
            [](const std::byte aByte) noexcept
            {
                return sSPIMasterDev.PushPullByte(sLCDSPISlaveCfg, aByte);
            },
            []() noexcept {ROM_GPIOPinWrite(sLCDDisp.mBaseAddr, sLCDDisp.mPin, sLCDDisp.mPin);},
            []() noexcept {ROM_GPIOPinWrite(sLCDDisp.mBaseAddr, sLCDDisp.mPin, 0);}
        )
    };

    lLCD->Init();
    return std::make_unique<GUI::AO::Mgr>(lLCD, lLCD);
}


static auto StartRTCC() noexcept -> std::unique_ptr<RTCC::AO::Mgr>
{
    // NOTE: Shared with the board function Blue LED. Do not use. May require patching RTCC board.
    [[maybe_unused]] static constexpr CoreLink::GPIO sRTCCRst{GPIOF_BASE, GPIO_PIN_4};

    static constexpr CoreLink::SPISlaveCfg sRTCCSPISlaveCfg{
        .mProtocol{CoreLink::SPISlaveCfg::tProtocol::MOTO_0},
        .mBitRate{4000000UL},
        .mDataWidth{8},
        .mCSn{GPIOA_BASE, GPIO_PIN_3}
    };

    auto lRTCC{
        std::make_unique< Drivers::DS3234>(
            [](std::span<std::byte> aData, std::optional<std::byte> aAddr) noexcept
            {
                sSPIMasterDev.RdData(sRTCCSPISlaveCfg, aData, aAddr);
            },
            [](std::span<const std::byte> aData, std::optional<std::byte> aAddr) noexcept
            {
                sSPIMasterDev.WrData(sRTCCSPISlaveCfg, aData, aAddr);
            }
        )
    };

    //return std::make_unique<RTCC::AO::Mgr>(std::move(lRTCC));
    return nullptr;
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
{
    // CAUTION: called with interrupts DISABLED, NOTE01
    // Toggle LED2 on and then off, see NOTE01.
    ROM_GPIOPinWrite(sLEDGreen.mBaseAddr, sLEDGreen.mPin, sLEDGreen.mPin);
    ROM_GPIOPinWrite(sLEDGreen.mBaseAddr, sLEDGreen.mPin, 0);

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
Q_NORETURN Q_onError(const char* const module, const int loc)
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
    static std::array<uint8_t, 2 * 1024> sQSTxBuf{};
    initBuf(sQSTxBuf.data(), sQSTxBuf.size());

    // Buffer for QS receive channel.
    static std::array<uint8_t, 100> sQSRxBuf{};
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
    // At least one block of data ready to get out.
    // Create an event for each of them and publish to all subscribing sinks.
    // [MG] CREATE SINKS WITH lBlockSize BUFFERS.
    uint16_t lBlockSize{1024};
    QF_INT_DISABLE();
    while (const auto lBlock{QS::getBlock(&lBlockSize)}) {
#if 0
        auto lQSPYBlockEvt{Q_NEW(BSP::Event::QSPYProcBlock, BSP_QSPY_PROC_BLOCK_SIG)};
        QF_INT_ENABLE();

        lQSPYBlockEvt->mBlock = lBlock;
        lQSPYBlockEvt->mSize = lBlockSize;

        // [MG] CONSIDER POSTING DIRECTLY TO SINKS.
        // [MG] GOOD WAY TO CONTROL THE SINKS.
        QP::QF::PUBLISH(lQSPYBlockEvt, &sOnFlush);
        QF_INT_DISABLE();
#endif
    }

    QF_INT_ENABLE();
}


//............................................................................
//! callback function to reset the target (to be implemented in the BSP)
void QP::QS::onReset()
{
    //NVIC_SystemReset();
}

void QP::QS::onCommand(uint8_t cmdId, uint32_t param1,
                   uint32_t param2, uint32_t param3)
{
    (void)cmdId;
    (void)param1;
    (void)param2;
    (void)param3;
#if 0
    // application-specific record
    QS_BEGIN_ID(DPP::COMMAND_STAT, 0U)
        QS_U8(2, cmdId);
        QS_U32(8, param1);
    QS_END()

    if (cmdId == 10U) {
        queryCurrObj(param1);
    }
    else if (cmdId == 11U) {
        assert_failed("QS_onCommand", 11);
    }
#endif
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
    [[maybe_unused]] const auto lTemp{SysTick->CTRL};
    QS_tickTime_ += QS_tickPeriod_;
#endif

    // Call QF Tick function.
    QP::QF::TICK_X(0U, &sSysTick_Handler);

    // Perform the debouncing of buttons.
    DebounceSwitches();
    QV_ARM_ERRATUM_838869();
}

} // extern "C"


// The algorithm for debouncing
// adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
// and Michael Barr, page 71.
static void DebounceSwitches() noexcept
{
    // Read current pin state into array of pin states.
    static constexpr auto sStateDepth{5};
    using PinType = uint32_t;
    static std::array<PinType, sStateDepth> sPinsState{0};
    static PinType sPreviousDebounce{0};
    static decltype(sPinsState.size()) lStateIx{0};
    sPinsState.at(lStateIx) = ~ROM_GPIOPinRead(sButton1.mBaseAddr, (sButton1.mPin | sButton2.mPin));
    ++lStateIx;
    if (lStateIx >= sPinsState.size()) {
        lStateIx = 0;
    }

    // Bitwise-AND all last current pin states.
    const auto lCurrentDebounce{
        std::accumulate(
            sPinsState.cbegin(),
            sPinsState.cend(),
            (sButton1.mPin | sButton2.mPin),
            std::bit_and<PinType>()
        )
    };

    // What changed now? Look for pressed states.
    if ((~sPreviousDebounce) & lCurrentDebounce) {

        if (lCurrentDebounce & sButton1.mPin) {
            static const BSP::Event::ButtonEvt sOnEvt{
                QP::QEvt{BSP_MANUAL_FEED_BUTTON_EVT_SIG},
                true
            };
            QP::QF::PUBLISH(&sOnEvt, &sSysTick_Handler);
        }
        if (lCurrentDebounce & sButton2.mPin) {
            static const BSP::Event::ButtonEvt sOnEvt{
                QP::QEvt{BSP_TIMED_FEED_BUTTON_EVT_SIG},
                true
            };
            QP::QF::PUBLISH(&sOnEvt, &sSysTick_Handler);
        }
    }

    // Look for released states.
    if (sPreviousDebounce & ~lCurrentDebounce) {
        if ((sPreviousDebounce) & sButton1.mPin) {
            static const BSP::Event::ButtonEvt sOffEvt{
                QP::QEvt{BSP_MANUAL_FEED_BUTTON_EVT_SIG},
                false
            };
            QP::QF::PUBLISH(&sOffEvt, &sSysTick_Handler);
        }
    }

    sPreviousDebounce = lCurrentDebounce;
}

// *****************************************************************************
//                                END OF FILE
// *****************************************************************************
