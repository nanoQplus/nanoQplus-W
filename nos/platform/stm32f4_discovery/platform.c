/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file platform.c
 * @brief STM32F4 Discovery platform initialization
 * @author Duk-Kyun Woo, Haeyong Kim (ETRI)
 * @date 2015. 10. 27.
 * @ingroup 
 * @copyright GNU General Public License v3
 */


#define NDEBUG
#include "kconf.h"
#include "nos_debug.h"

#include <stdio.h>
#include "nos_common.h"
#include "platform.h"
#include "nos_timer.h"
#include "nos_rtc.h"

#ifdef CFD_M
  extern int32_t  fd_init(void);
#endif

/**
 * The cortex allows this to be further divided into preemption and a "tie-breaker" sub-priority.
 * 7: 8bits for sub-priority
 * 6: 1bit for preemptive-priority (group), 7bits for sub-priority
 * 5: 2bit for preemptive-priority (group), 6bits for sub-priority
 * 4: 3bit for preemptive-priority (group), 5bits for sub-priority
 * 3: 4bit for preemptive-priority (group), 4bits for sub-priority
 * 2: 5bit for preemptive-priority (group), 3bits for sub-priority
 * 1: 6bit for preemptive-priority (group), 2bits for sub-priority
 * 0: 7bit for preemptive-priority (group), 1bits for sub-priority 
*/

//__NVIC_PRIO_BITS is 4 
#define NOS_NVIC_PRIO_BITS  4   // 3bits for preemptive priority, 1bit for sub-priority
static void nos_nvic_init(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

    /* Configure preemption priority groups. */
    /* only 3~7 is allowed for STM32 (4bits priority levels), 7:sub uses 4bits, 3:group uses 4bits */
    /* Configure preemption priority groups.*/
    NVIC_SetPriorityGrouping(NOS_NVIC_PRIO_BITS);

    /* Exceptions */
    /* Critical Error, Group 0 */
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 0, 0));
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 0, 0));
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 0, 0));
    /* Not used yet, Group 7 */
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 7, 1));
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 7, 1));
#ifdef KERNEL_M
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 1, 0)); // scheduling. must not be preempted.
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 6, 1));  // context-switch. must be the lowest proirity. 
#endif


    /* General ISRs */
    NVIC_SetPriority(RTC_WKUP_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 1, 0));
    NVIC_EnableIRQ(RTC_WKUP_IRQn);
    NVIC_SetPriority(RTC_Alarm_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 1, 1));
    NVIC_EnableIRQ(RTC_Alarm_IRQn);

#ifdef UART_M
#ifdef UART1
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 2, 0));
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_DisableIRQ(USART1_IRQn);
#endif
#ifdef UART2
    NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 2, 0));
    NVIC_ClearPendingIRQ(USART2_IRQn);
    NVIC_DisableIRQ(USART2_IRQn);
#endif
#endif


    /* Externel ISRs */
#if defined SLOT1_CC2520
    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 3, 0));
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);
#endif

#if defined SLOT1_CC2520
    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 3, 1));
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
#endif

#if (defined (SLOT2_CC2520)) || (defined (SLOT1_CC1120)) || (defined (SLOT1_ETHERNET_DEV))
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 3, 0));
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif

#if (defined (SLOT2_CC2520)) || (defined (SLOT2_CC1120)) || (defined (SLOT2_ETHERNET_DEV))
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 3, 1));
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif

#if (defined (BUTTON_M))
    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NOS_NVIC_PRIO_BITS, 3, 1));
    NVIC_ClearPendingIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);
#endif

#if 0
    /* Externel Interrupt Configurations */
    EXTI_InitTypeDef EXTI_InitStructure;

#ifdef SLOT1_ETHERNET_DEV
    //GPIO_EXTILineConfig(MG2420_EXTI_PORTSRC, MG2420_EXTI_PINSRC);
    //EXTI_InitStructure.EXTI_Line = MG2420_EXTI_PIN;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
#endif

#ifdef SLOT2_ETHERNET_DEV
    //GPIO_EXTILineConfig(MG2420_EXTI_PORTSRC, MG2420_EXTI_PINSRC);
    //EXTI_InitStructure.EXTI_Line = MG2420_EXTI_PIN;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
#endif
#endif
}

void nos_platform_init(void)
{
#ifdef UART_M
#ifdef UART1
    nos_uart_init(0);
#endif
#ifdef UART2
    //nos_uart_init(1);  
    nos_uart_init(2);
#endif
#endif

#ifdef LED_M
    nos_led_init();
#endif 

#ifdef BUTTON_M
    nos_button_init();
#endif

    // Initialize network interface 1.
#ifdef SLOT1_WPAN_DEV
    wpan_dev_init(0, NULL);
#elif defined SLOT1_ETHERNET_DEV
    //enc28j60_interface_init(0);
    //enc28j60_init(0);
#endif

    // Intialize network interface 2.
#ifdef SLOT2_WPAN_DEV
    wpan_dev_init(1, NULL);
#elif defined SLOT2_ETHERNET_DEV
    //enc28j60_interface_init(1);
    //enc28j60_init(1);
#endif

#ifdef LMC1623_M
    lmc1623_init();
    lmc1623_set_line(1, "on EToI platform");
#endif

#ifdef NAND_M
  FSMC_NANDDeInit(FSMC_Bank2_NAND);
  FSMC_NAND_Init();
#endif

#ifdef CFD_M
  fd_init();
#endif

    nos_nvic_init();
}

void nos_delay_us(uint32_t us) {
	while(us--) {
                // 150 times of nop has 1usec at STM32F407@168MHz
		
                // 100
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

		// 50
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

		// 0
		//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); __NOP();__NOP();__NOP();
	} // end while
} // end func

void nos_delay_ms(uint32_t ms) {
	while(ms--) {
		nos_delay_us(1000);
	} // end while
} // end func



