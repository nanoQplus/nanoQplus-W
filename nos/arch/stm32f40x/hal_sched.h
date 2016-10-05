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
 * HAL for scheduler.
 *
 * @author Haeyong Kim (ETRI)
 * @date 2013. 1. 22.
 */

#ifndef __HAL_SCHED_H__
#define __HAL_SCHED_H__

#include "kconf.h"

#ifdef KERNEL_M

#include "stm32f4xx.h"

// Note that changing this value does not mean channging tick interrupt interval.
#ifdef SCHED_PERIOD_5
#define SCHED_TIMER_MS          5
#elif defined SCHED_PERIOD_10
#define SCHED_TIMER_MS          10
#elif defined SCHED_PERIOD_32
#define SCHED_TIMER_MS          32
#elif defined SCHED_PERIOD_100
#define SCHED_TIMER_MS          100
#else
#error "Unknown scheduling time slice"
#endif

#define KERNEL_DEFERRED_CTX_SW 1
#define NOS_CTX_SW_PENDING_SET() \
    do { SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; } while (0)
#define NOS_CTX_SW_PENDING_CLEAR() \
    do { SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk; } while (0)

void nos_sched_hal_init(void);
void nos_sched_timer_start(void);
void (*sched_callback)(void);	// this varable indicates the scheduler is working or not.
void os_irq_exit(void);
void TIM2_IRQHandler(void);

//void TIM2_IRQHandler(void) __attribute__ ((naked));


#if 0
#define SEC(x)	(x*1000/SCHED_TIMER_MS)  // 420000 =10ms, 42000000 =1s
#define MSEC(x)	((x-1+SCHED_TIMER_MS)/SCHED_TIMER_MS)
#endif

//@phj.
#define SEC(x)	((x)*(100))  // 420000 =10ms, 42000000 =1s. *Use Tim2 to 42MHz speed.
//#define MSEC(x)	((x)*(42000))
#define MSEC(x)	((x+9)/10)


#endif // KERNEL_M
#endif // __!HAL_SCHED_H__
