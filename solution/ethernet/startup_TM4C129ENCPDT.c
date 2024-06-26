/* File: startup_TM4C123GH6PM.c
 * Purpose: startup file for TM4C123GH6PM Cortex-M4 device.
 *          Should be used with GCC 'GNU Tools ARM Embedded'
 * Version: CMSIS 5.0.1
 * Date: 2017-09-13
 *
 * Created from the CMSIS template for the specified device
 * Quantum Leaps, www.state-machine.com
 *
 * NOTE:
 * The function assert_failed defined at the end of this file defines
 * the error/assertion handling policy for the application and might
 * need to be customized for each project. This function is defined in
 * assembly to re-set the stack pointer, in case it is corrupted by the
 * time assert_failed is called.
 */


// Start and end of stack defined in the linker script.
//extern int __stack_start__;
extern int __stack_end__;

// Weak prototypes for error handlers.
///
// \note
// The function assert_failed defined at the end of this file defines
// the error/assertion handling policy for the application and might
// need to be customized for each project. This function is defined in
// assembly to avoid accessing the stack, which might be corrupted by
// the time assert_failed is called.
void assert_failed(char const *aModule, int const aLoc);

// Function prototypes -----------------------------------------------------*/
void Default_Handler();  // Default empty handler.
void Reset_Handler();    // Reset Handler.
void SystemInit();       // CMSIS system initialization.
//void Q_onAssert(char const *module, int loc); // QP assertion handler.

//----------------------------------------------------------------------------
// weak aliases for each Exception handler to the Default_Handler.
// Any function with the same name will override these definitions.
// Cortex-M Processor fault exceptions...
void NMI_Handler           () __attribute__ ((weak));
void HardFault_Handler     () __attribute__ ((weak));
void MemManage_Handler     () __attribute__ ((weak));
void BusFault_Handler      () __attribute__ ((weak));
void UsageFault_Handler    () __attribute__ ((weak));

// Cortex-M Processor non-fault exceptions...
void SVC_Handler           () __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler      () __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler        () __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler       () __attribute__ ((weak, alias("Default_Handler")));

// external interrupts...
void GPIOPortA_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortB_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortC_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortD_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortE_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void SSI0_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void PWMFault_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
void PWMGen0_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void PWMGen1_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void PWMGen2_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void QEI0_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void ADCSeq0_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void ADCSeq1_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void ADCSeq2_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void ADCSeq3_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Watchdog_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
void Timer0A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer0B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer1A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer1B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer2A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer2B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Comp0_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void Comp1_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void Comp2_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void SysCtrl_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void FlashCtrl_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortF_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortG_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortH_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void UART2_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void SSI1_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void Timer3A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer3B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
//void QEI1_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void CAN0_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void CAN1_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
//void CAN2_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void EMAC0_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void Hibernate_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void USB0_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void PWMGen3_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void uDMAST_IRQHandler     () __attribute__ ((weak, alias("Default_Handler")));
void uDMAError_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void ADC1Seq0_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
void ADC1Seq1_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
void ADC1Seq2_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
void ADC1Seq3_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
void EPI0_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortJ_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortK_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortL_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void SSI2_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void SSI3_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void UART3_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void UART6_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void UART7_IRQHandler      () __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void I2C3_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void Timer4A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer4B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer5A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer5B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer0A_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer0B_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer1A_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer1B_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer2A_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer2B_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer3A_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer3B_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer4A_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer4B_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer5A_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
//void WideTimer5B_IRQHandler() __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler        () __attribute__ ((weak, alias("Default_Handler")));
void I2C4_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void I2C5_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortM_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortN_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
//void QEI2_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void Tamper_IRQHandler     () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP0_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP1_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP2_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP3_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP4_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP5_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP6_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortP7_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ0_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ1_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ2_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ3_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ4_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ5_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ6_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
void GPIOPortQ7_IRQHandler () __attribute__ ((weak, alias("Default_Handler")));
//void GPIOPortR_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
//void GPIOPortS_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
//void PWM1Gen0_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
//void PWM1Gen1_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
//void PWM1Gen2_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
//void PWM1Gen3_IRQHandler   () __attribute__ ((weak, alias("Default_Handler")));
//void PWM1Fault_IRQHandler  () __attribute__ ((weak, alias("Default_Handler")));
void SHA_IRQHandler        () __attribute__ ((weak, alias("Default_Handler")));
void AES_IRQHandler        () __attribute__ ((weak, alias("Default_Handler")));
void DES_IRQHandler        () __attribute__ ((weak, alias("Default_Handler")));
void Timer6A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer6B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer7A_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void Timer7B_IRQHandler    () __attribute__ ((weak, alias("Default_Handler")));
void I2C6_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void I2C7_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void I2C8_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));
void I2C9_IRQHandler       () __attribute__ ((weak, alias("Default_Handler")));


__attribute__ ((section(".isr_vector")))
static int const g_pfnVectors[] = {
    (int)&__stack_end__,          // Top of Stack
    (int)&Reset_Handler,          // Reset Handler
    (int)&NMI_Handler,            // NMI Handler
    (int)&HardFault_Handler,      // Hard Fault Handler
    (int)&MemManage_Handler,      // The MPU fault handler
    (int)&BusFault_Handler,       // The bus fault handler
    (int)&UsageFault_Handler,     // The usage fault handler
    0,                            // Reserved
    0,                            // Reserved
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    (int)&SVC_Handler,            /* SVCall handler                  */
    (int)&DebugMon_Handler,       /* Debug monitor handler           */
    0,                            /* Reserved                        */
    (int)&PendSV_Handler,         /* The PendSV handler              */
    (int)&SysTick_Handler,        /* The SysTick handler             */

    // IRQ handlers...
    (int)&GPIOPortA_IRQHandler,   /* GPIO Port A                     */
    (int)&GPIOPortB_IRQHandler,   /* GPIO Port B                     */
    (int)&GPIOPortC_IRQHandler,   /* GPIO Port C                     */
    (int)&GPIOPortD_IRQHandler,   /* GPIO Port D                     */
    (int)&GPIOPortE_IRQHandler,   /* GPIO Port E                     */
    (int)&UART0_IRQHandler,       /* UART0 Rx and Tx                 */
    (int)&UART1_IRQHandler,       /* UART1 Rx and Tx                 */
    (int)&SSI0_IRQHandler,        /* SSI0 Rx and Tx                  */
    (int)&I2C0_IRQHandler,        /* I2C0 Master and Slave           */
    (int)&PWMFault_IRQHandler,    /* PWM Fault                       */
    (int)&PWMGen0_IRQHandler,     /* PWM Generator 0                 */
    (int)&PWMGen1_IRQHandler,     /* PWM Generator 1                 */
    (int)&PWMGen2_IRQHandler,     /* PWM Generator 2                 */
    (int)&QEI0_IRQHandler,        /* Quadrature Encoder 0            */
    (int)&ADCSeq0_IRQHandler,     /* ADC Sequence 0                  */
    (int)&ADCSeq1_IRQHandler,     /* ADC Sequence 1                  */
    (int)&ADCSeq2_IRQHandler,     /* ADC Sequence 2                  */
    (int)&ADCSeq3_IRQHandler,     /* ADC Sequence 3                  */
    (int)&Watchdog_IRQHandler,    /* Watchdog timer                  */
    (int)&Timer0A_IRQHandler,     /* Timer 0 subtimer A              */
    (int)&Timer0B_IRQHandler,     /* Timer 0 subtimer B              */
    (int)&Timer1A_IRQHandler,     /* Timer 1 subtimer A              */
    (int)&Timer1B_IRQHandler,     /* Timer 1 subtimer B              */
    (int)&Timer2A_IRQHandler,     /* Timer 2 subtimer A              */
    (int)&Timer2B_IRQHandler,     /* Timer 2 subtimer B              */
    (int)&Comp0_IRQHandler,       /* Analog Comparator 0             */
    (int)&Comp1_IRQHandler,       /* Analog Comparator 1             */
    (int)&Comp2_IRQHandler,       /* Analog Comparator 2             */
    (int)&SysCtrl_IRQHandler,     /* System Control (PLL, OSC, BO)   */
    (int)&FlashCtrl_IRQHandler,   /* FLASH Control                   */
    (int)&GPIOPortF_IRQHandler,   /* GPIO Port F                     */
    (int)&GPIOPortG_IRQHandler,   /* GPIO Port G                     */
    (int)&GPIOPortH_IRQHandler,   /* GPIO Port H                     */
    (int)&UART2_IRQHandler,       /* UART2 Rx and Tx                 */
    (int)&SSI1_IRQHandler,        /* SSI1 Rx and Tx                  */
    (int)&Timer3A_IRQHandler,     /* Timer 3 subtimer A              */
    (int)&Timer3B_IRQHandler,     /* Timer 3 subtimer B              */
    (int)&I2C1_IRQHandler,        /* I2C1 Master and Slave           */
    //(int)&QEI1_IRQHandler,        /* Quadrature Encoder 1            */
    (int)&CAN0_IRQHandler,        /* CAN0                            */
    (int)&CAN1_IRQHandler,        /* CAN1                            */
    //(int)&CAN2_IRQHandler,        /* CAN2                            */
    //0,                            /* Reserved                        */
    (int)&EMAC0_IRQHandler, // [MG] TO ADD...
    (int)&Hibernate_IRQHandler,   /* Hibernate                       */
    (int)&USB0_IRQHandler,        /* USB0                            */
    (int)&PWMGen3_IRQHandler,     /* PWM Generator 3                 */
    (int)&uDMAST_IRQHandler,      /* uDMA Software Transfer          */
    (int)&uDMAError_IRQHandler,   /* uDMA Error                      */
    (int)&ADC1Seq0_IRQHandler,    /* ADC1 Sequence 0                 */
    (int)&ADC1Seq1_IRQHandler,    /* ADC1 Sequence 1                 */
    (int)&ADC1Seq2_IRQHandler,    /* ADC1 Sequence 2                 */
    (int)&ADC1Seq3_IRQHandler,    /* ADC1 Sequence 3                 */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    (int)&EPI0_IRQHandler, // [MG] TO ADD...
    (int)&GPIOPortJ_IRQHandler,   /* GPIO Port J                     */
    (int)&GPIOPortK_IRQHandler,   /* GPIO Port K                     */
    (int)&GPIOPortL_IRQHandler,   /* GPIO Port L                     */
    (int)&SSI2_IRQHandler,        /* SSI2 Rx and Tx                  */
    (int)&SSI3_IRQHandler,        /* SSI3 Rx and Tx                  */
    (int)&UART3_IRQHandler,       /* UART3 Rx and Tx                 */
    (int)&UART4_IRQHandler,       /* UART4 Rx and Tx                 */
    (int)&UART5_IRQHandler,       /* UART5 Rx and Tx                 */
    (int)&UART6_IRQHandler,       /* UART6 Rx and Tx                 */
    (int)&UART7_IRQHandler,       /* UART7 Rx and Tx                 */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    (int)&I2C2_IRQHandler,        /* I2C2 Master and Slave           */
    (int)&I2C3_IRQHandler,        /* I2C3 Master and Slave           */
    (int)&Timer4A_IRQHandler,     /* Timer 4 subtimer A              */
    (int)&Timer4B_IRQHandler,     /* Timer 4 subtimer B              */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    //0,                            /* Reserved                        */
    (int)&Timer5A_IRQHandler,     /* Timer 5 subtimer A              */
    (int)&Timer5B_IRQHandler,     /* Timer 5 subtimer B              */
    //(int)&WideTimer0A_IRQHandler, /* Wide Timer 0 subtimer A         */
    //(int)&WideTimer0B_IRQHandler, /* Wide Timer 0 subtimer B         */
    //(int)&WideTimer1A_IRQHandler, /* Wide Timer 1 subtimer A         */
    //(int)&WideTimer1B_IRQHandler, /* Wide Timer 1 subtimer B         */
    //(int)&WideTimer2A_IRQHandler, /* Wide Timer 2 subtimer A         */
    //(int)&WideTimer2B_IRQHandler, /* Wide Timer 2 subtimer B         */
    //(int)&WideTimer3A_IRQHandler, /* Wide Timer 3 subtimer A         */
    //(int)&WideTimer3B_IRQHandler, /* Wide Timer 3 subtimer B         */
    //(int)&WideTimer4A_IRQHandler, /* Wide Timer 4 subtimer A         */
    //(int)&WideTimer4B_IRQHandler, /* Wide Timer 4 subtimer B         */
    //(int)&WideTimer5A_IRQHandler, /* Wide Timer 5 subtimer A         */
    //(int)&WideTimer5B_IRQHandler, /* Wide Timer 5 subtimer B         */
    (int)&FPU_IRQHandler,         /* FPU                             */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    (int)&I2C4_IRQHandler,        /* I2C4 Master and Slave           */
    (int)&I2C5_IRQHandler,        /* I2C5 Master and Slave           */
    (int)&GPIOPortM_IRQHandler,   /* GPIO Port M                     */
    (int)&GPIOPortN_IRQHandler,   /* GPIO Port N                     */
    //(int)&QEI2_IRQHandler,        /* Quadrature Encoder 2            */
    //0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    (int)&Tamper_IRQHandler, // [MG] TO ADD...
    (int)&GPIOPortP0_IRQHandler,  /* GPIO Port P (Summary or P0)     */
    (int)&GPIOPortP1_IRQHandler,  /* GPIO Port P1                    */
    (int)&GPIOPortP2_IRQHandler,  /* GPIO Port P2                    */
    (int)&GPIOPortP3_IRQHandler,  /* GPIO Port P3                    */
    (int)&GPIOPortP4_IRQHandler,  /* GPIO Port P4                    */
    (int)&GPIOPortP5_IRQHandler,  /* GPIO Port P5                    */
    (int)&GPIOPortP6_IRQHandler,  /* GPIO Port P6                    */
    (int)&GPIOPortP7_IRQHandler,  /* GPIO Port P7                    */
    (int)&GPIOPortQ0_IRQHandler,  /* GPIO Port Q (Summary or Q0)     */
    (int)&GPIOPortQ1_IRQHandler,  /* GPIO Port Q1                    */
    (int)&GPIOPortQ2_IRQHandler,  /* GPIO Port Q2                    */
    (int)&GPIOPortQ3_IRQHandler,  /* GPIO Port Q3                    */
    (int)&GPIOPortQ4_IRQHandler,  /* GPIO Port Q4                    */
    (int)&GPIOPortQ5_IRQHandler,  /* GPIO Port Q5                    */
    (int)&GPIOPortQ6_IRQHandler,  /* GPIO Port Q6                    */
    (int)&GPIOPortQ7_IRQHandler,  /* GPIO Port Q7                    */
    //(int)&GPIOPortR_IRQHandler,   /* GPIO Port R                     */
    //(int)&GPIOPortS_IRQHandler,   /* GPIO Port S                     */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    (int)&SHA_IRQHandler,
    (int)&AES_IRQHandler,
    (int)&DES_IRQHandler,
    0,                            /* Reserved                        */
    //(int)&PWM1Gen0_IRQHandler,    /* PWM 1 Generator 0               */
    //(int)&PWM1Gen1_IRQHandler,    /* PWM 1 Generator 1               */
    //(int)&PWM1Gen2_IRQHandler,    /* PWM 1 Generator 2               */
    //(int)&PWM1Gen3_IRQHandler,    /* PWM 1 Generator 3               */
    //(int)&PWM1Fault_IRQHandler,   /* PWM 1 Fault                     */
    (int)&Timer6A_IRQHandler,     /* Timer 5 subtimer A              */
    (int)&Timer6B_IRQHandler,     /* Timer 5 subtimer B              */
    (int)&Timer7A_IRQHandler,     /* Timer 5 subtimer A              */
    (int)&Timer7B_IRQHandler,     /* Timer 5 subtimer B              */
    (int)&I2C6_IRQHandler,        /* I2C6 Master and Slave           */
    (int)&I2C7_IRQHandler,        /* I2C7 Master and Slave           */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    (int)&I2C8_IRQHandler,        /* I2C8 Master and Slave           */
    (int)&I2C9_IRQHandler,        /* I2C9 Master and Slave           */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
};


// reset handler.
void Reset_Handler() {
    extern int main();
    extern int __libc_init_array();
    extern int __libc_fini_array();
    extern unsigned __data_start;  // start of .data in the linker script.
    extern unsigned __data_end__;  // end of .data in the linker script.
    extern unsigned const __data_load; // initialization values for .data.
    extern unsigned __bss_start__; // start of .bss in the linker script.
    extern unsigned __bss_end__;   // end of .bss in the linker script.
    extern void software_init_hook() __attribute__((weak));

    (void)g_pfnVectors;
    // CMSIS system initialization.
    SystemInit();

    // Copy the data segment initializers from flash to RAM...
    unsigned const *src = &__data_load;
    for (unsigned *dst = (unsigned *)&__data_start; dst < (unsigned *)&__data_end__; ++dst, ++src) {
        *dst = *src;
    }

    // Zero fill the .bss segment in RAM...
    for (unsigned *dst = (unsigned *)&__bss_start__; dst < (unsigned *)&__bss_end__; ++dst) {
        *dst = 0;
    }

    // Init hook provided?
    if (&software_init_hook != (void (*)())(0)) {
        // Give control to the RTOS.
        // this will also call __libc_init_array.
        software_init_hook();
    } else {
        // Call all static constructors in C++ (comment out in C programs).
        __libc_init_array();
        // Application's entry point; should never return!
        [[maybe_unused]] int lResult = main();
        __libc_fini_array();
    }

    // The previous code should not return, but assert just in case...
    assert_failed("Reset_Handler", __LINE__);
}

void _close(void)
{
}
void _lseek(void)
{

}
void _read(void)
{
}
void _write(void)
{
}

// fault exception handlers
__attribute__((naked)) void NMI_Handler();
void NMI_Handler() {
    __asm volatile (
        "    ldr r0,=str_nmi\n\t"
        "    mov r1,#1\n\t"
        "    b assert_failed\n\t"
        "str_nmi: .asciz \"NMI\"\n\t"
        "  .align 2\n\t"
    );
}


__attribute__((naked)) void MemManage_Handler();
void MemManage_Handler() {
    __asm volatile (
        "    ldr r0,=str_mem\n\t"
        "    mov r1,#1\n\t"
        "    b assert_failed\n\t"
        "str_mem: .asciz \"MemManage\"\n\t"
        "  .align 2\n\t"
    );
}


__attribute__((naked)) void HardFault_Handler();
void HardFault_Handler() {
    __asm volatile (
        "    ldr r0,=str_hrd\n\t"
        "    mov r1,#1\n\t"
        "    b assert_failed\n\t"
        "str_hrd: .asciz \"HardFault\"\n\t"
        "  .align 2\n\t"
    );
}


__attribute__((naked)) void BusFault_Handler();
void BusFault_Handler() {
    __asm volatile (
        "    ldr r0,=str_bus\n\t"
        "    mov r1,#1\n\t"
        "    b assert_failed\n\t"
        "str_bus: .asciz \"BusFault\"\n\t"
        "  .align 2\n\t"
    );
}


__attribute__((naked)) void UsageFault_Handler();
void UsageFault_Handler() {
    __asm volatile (
        "    ldr r0,=str_usage\n\t"
        "    mov r1,#1\n\t"
        "    b assert_failed\n\t"
        "str_usage: .asciz \"UsageFault\"\n\t"
        "  .align 2\n\t"
    );
}


__attribute__((naked)) void Default_Handler();
void Default_Handler() {
    __asm volatile (
        "    ldr r0,=str_dflt\n\t"
        "    mov r1,#1\n\t"
        "    b assert_failed\n\t"
        "str_dflt: .asciz \"Default\"\n\t"
        "  .align 2\n\t"
    );
}


//****************************************************************************
// The function assert_failed defines the error/assertion handling policy
// for the application. After making sure that the stack is OK, this function
// calls Q_onAssert, which should NOT return (typically reset the CPU).
//
// NOTE: the function Q_onAssert should NOT return.
//****************************************************************************
__attribute__ ((naked))
void assert_failed([[maybe_unused]] char const *module, [[maybe_unused]] int loc) {
    /* re-set the SP in case of stack overflow */
    __asm volatile (
        "  MOV sp,%0\n\t"
        : : "r" (&__stack_end__));

    //Q_onAssert(module, loc); /* call the application-specific QP handler */

    while (1) {
        // Should not be reached, but just in case loop forever...
    }
}

// End Of File
