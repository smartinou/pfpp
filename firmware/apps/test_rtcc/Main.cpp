// TI Library.
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
//#include <driverlib/rom.h>
//#include <driverlib/rom_map.h>
#include <driverlib/sysctl.h>

// CoreLink Library.
#include "inc/GPIO.h"
#include "inc/SPIMasterDev.h"

// Driver library.
#include "inc/DS3234.h"

// Standard Library.
#include <memory>


//static std::unique_ptr<CoreLink::SPIMasterDev> lSPIDev{};
static std::unique_ptr<IRTCC> sRTCCDev{};


static constexpr CoreLink::GPIO sRTCCInterruptPin{GPIOP_BASE, GPIO_PIN_3};


int main() {

    static constexpr auto sClkRate{120000000U};
    auto const lClkRate = SysCtlClockFreqSet(
        SYSCTL_XTAL_25MHZ
            | SYSCTL_OSC_MAIN
            | SYSCTL_USE_PLL
            | SYSCTL_CFG_VCO_480,
        sClkRate
    );

    if (lClkRate != sClkRate) {
        exit 1;
    }

    // Create pin configuration.
    // Initialize SPI Master.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
    static constexpr CoreLink::SSIGPIO lSSIGPIO{
        GPIO_PQ0_SSI3CLK,
        GPIO_PQ2_SSI3XDAT0,
        GPIO_PQ3_SSI3XDAT1,
        GPIOQ_BASE,
        GPIO_PIN_0, // Clk.
        GPIO_PIN_3, // Rx.
        GPIO_PIN_2  // Tx.
    };
    auto lSPIDev{
        std::make_unique<CoreLink::SPIMasterDev>(
            SSI3_BASE,
            lClkRate,
            lSSIGPIO
        );
    }

    static constexpr auto sBaseYear{2000};
    static constexpr auto sInterruptNumber{INT_GPIOP3};
    static constexpr CoreLink::GPIO sCSnPin{GPIOQ_BASE, GPIO_PIN_1};
    sRTCCDev = std::make_unique<DS3234>(
        sBaseYear,
        sInterruptNumber,
        sRTCCInterruptPin,
        lSPIDev,
        sCSnPin
    );

    // [MG] CHECK WHAT OF THIS IS ACTUALLY REQUIRED.

    // Assign all priority bits for preemption-prio. and none to sub-prio.
    //NVIC_SetPriorityGrouping(0U); from CMSIS
    MAP_IntPriorityGroupingSet(0U);
    MAP_IntPrioritySet(INT_GPIOA, 0x20); //GPIOPORTA_PRIO);
    MAP_IntPrioritySet(INT_GPIOB, 0x20); //GPIOPORTA_PRIO);
    MAP_IntPrioritySet(INT_GPIOC, 0x60); //GPIOPORTA_PRIO);
    MAP_IntPrioritySet(INT_GPIOD, 0x60); //GPIOPORTA_PRIO);
    MAP_IntPrioritySet(INT_GPIOE, 0x40); //GPIOPORTE_PRIO);
    //MAP_IntPrioritySet(INT_GPIOF, 0x60); //GPIOPORTF_PRIO);
    MAP_IntPrioritySet(INT_EMAC0, 0x60);

    //MAP_IntEnable(INT_EMAC0);

    // [MG] USE SEGGER_RTT.
#ifdef USE_RTT
    // Leave here until RTT has other use than QSPY.
    SEGGER_RTT_Init();
    static constexpr auto sRTTUpBufferName{"RTTUpBuffer"};
    static std::array<uint8_t, sRTTUpBufferSize> sRTTUpBuffer{0};
    int lResult{
        SEGGER_RTT_ConfigUpBuffer(
            sRTTBufferIndex,
            sRTTUpBufferName,
            sRTTUpBuffer.data(),
            sRTTUpBuffer.size(),
            SEGGER_RTT_MODE_NO_BLOCK_SKIP
        );
    }

    if (lResult < 0) {
        // Error while configuring buffer.
        return;
    }

    static constexpr auto sRTTDownBufferName{"RTTDownBuffer"};
    static constexpr size_t sRTTDownBufferSize{64};
    static std::array<uint8_t, sRTTDownBufferSize> sRTTDownBuffer{0};
    lResult = SEGGER_RTT_ConfigDownBuffer(
        sRTTBufferIndex,
        sRTTDownBufferName,
        sRTTDownBuffer.data(),
        sRTTDownBuffer.size(),
        SEGGER_RTT_MODE_NO_BLOCK_SKIP
    );

    if (lResult < 0) {
        // Error while configuring buffer.
        return;
    }

    //ITM_SendChar("Hello, World!\n");
    // TODO: HAS TO BE ENABLED FIRST.
    //ITM_SendChar('M');
#endif // USE_RTT

    return 0;
}


// GPIO port P interrupt handler.
extern "C" void GPIOPortP3_IRQHandler() {

    // Get the state of the GPIO and issue the corresponding event.
    static constexpr bool sIsMasked{true};
    static constexpr auto sPin{sRTCCInterruptPin.mPin};
    auto const lIntStatus{MAP_GPIOIntStatus(sRTCCInterruptPin.mPort, sIsMasked)};
    if (sPin & lIntStatus) {
        MAP_GPIOIntClear(sRTCCInterruptPin.mPort, sPin);

        // Do something.
        [[maybe_unused]] auto const lIsAlarmTripped{sRTCCDev->ISR()};
    }
}
