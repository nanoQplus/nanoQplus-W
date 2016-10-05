// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
/*
 * Copyright (c) 2013
 * Electronics and Telecommunications Research Institute (ETRI)
 * All Rights Reserved.
 *
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 *
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 *
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 *
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 *
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS," WITHOUT A WARRANTY OF ANY KIND. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 */

/**
 * Scheduler HAL
 *
 * @author Haeyong Kim (ETRI)
 * @author Jongsoo Jeong (ETRI)
 * @author Duk-Kyun Woo (ETRI)
 * @date 2015. 9. 24.
 */
#include "critical_section.h"
#include "hal_sched.h"
#include "platform.h"

UINT32* irq_ret_addr; // To return orignal stacking $pc register value.

#ifdef KERNEL_M

/*
 * If the platform supports LFXO, RTC alarm tick is used for scheduling.
 * Otherwise, SysTick is used. The platform that supports LFXO should define
 * PLATFORM_USES_LFXO in platform.h.
 */
#ifdef PLATFORM_USES_LFXO

void nos_sched_hal_init(void)
{
    /* Disable SysTick */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* Init PendSV */
    // Clear a pending interrupt.
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
	//inserted by phj. @160311
	// setting rtc_ISR() as RTI tick interrupt handler
	//os_set_handler(TIM2_IRQn, TIM2_IRQHandler);

	sched_callback = NULL;
	global_os_counter = 0;
}

void nos_sched_timer_start(void)
{
    //Nothing to do
}

#else

void nos_sched_hal_init(void)
{
    SysTick->LOAD = (((SystemCoreClock / 1000) * SCHED_TIMER_MS) &
                     SysTick_LOAD_RELOAD_Msk) - 1;
    SysTick->VAL = 0;
    
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;  /* Enable SysTick IRQ and SysTick Timer */

#if 0
    SysTick->CTRL = SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;	// means external clock : sys/8, 9MHz
#endif

    /* Init PendSV */
    // Clear a pending interrupt.
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
}

void nos_sched_timer_start(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

#endif //~PLATFORM_USES_LFXO

#endif // KERNEL_M
