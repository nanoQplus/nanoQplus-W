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
 * @brief HAL for threads.
 * @author Haeyong Kim (ETRI)
 * @date 2013. 1. 22.
 */

#include "hal_thread.h"
#ifdef THREAD_M

#include "sched.h"
#include "thread.h"

#include <stdio.h>

/**
 * @brief Initialize a stack.
 *
 * This is necessary because a context should be placed in the stack when
 * context switching. After context switching call, the thread with this stack
 * will be executed starting from the function pointer (func).
 */
#if 0
STACK_PTR nos_tcb_stack_init(void (*func)(void), STACK_PTR fos, UINT16 stack_size)
{
    STACK_PTR sptr; // stack pointer

    /*
     * Stack pointer indicates the top of stack + 1.
     * ARM's stack pointer indicates last filled entity.
     */
    sptr = (STACK_PTR) ((MEMADDR_T) fos + stack_size);
    // double word alignment of the stack pointer.
    sptr = (STACK_PTR) (((MEMADDR_T)sptr) & 0xFFFFFFF8);

    // Stored by HW when it enters ISR
    *(--sptr) = (STACK_ENTRY) 0x01000000;   //xPSR. T bit[24] must be always 1.
    *(--sptr) = (STACK_ENTRY) func;         //return PC
    *(--sptr) = (STACK_ENTRY) func;         //LR
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R12
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R3
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R2
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R1
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R0

    // Stored by SW (context switch)
    /*
     * LR for EXC_RETURN: 0xFFFFFFF0 + (1,9 or 0xD)
      * where [3]: 1(thread)/0(handler),
     *       [2]: 0(main stack)/1(process stack)
     *       [1:0]: 01.
     */
    *(--sptr) = (STACK_ENTRY) 0xFFFFFFF9;   //LR for EXC_RETURN. Thread mode . use MSP
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //PRIMASK
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R7
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R6
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R5
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R4
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R11
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R10
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R9
    *(--sptr) = (STACK_ENTRY) 0x00000000;   //R8

    return ((STACK_PTR) sptr);              //EOS
}

#if defined __GNUC__ || defined __ICCARM__

void nos_context_switch_core(void)
{
    // Save current thread state
    NOS_THREAD_SAVE_STATE();
    __SAVE_SP(_tcb[_rtid]->sptr);

    // switch stack pointer
    _rtid = _next_rtid;
    tcb[_rtid]->state = RUNNING_STATE;

    __LOAD_SP(tcb[_rtid]->sptr);
    NOS_THREAD_LOAD_STATE();
}

/**
 * @brief just for 'EXIT_STATE' thread
 */
void nos_context_load_core(void)
{
    tcb[_rtid]->state = RUNNING_STATE;
    __LOAD_SP(tcb[_rtid]->sptr);
    NOS_THREAD_LOAD_STATE();
}

#elif defined __CC_ARM
void change_running_thread(void)
{
    __SAVE_SP(tcb[_rtid]->sptr);
    _rtid = _next_rtid;
    tcb[_rtid]->state = RUNNING_STATE;
    __LOAD_SP(tcb[_rtid]->sptr);
}

__asm void nos_context_switch_core(void)
{
    preserve8

    // NOS_THREAD_SAVE_STATE()
    push { lr }
    mrs r0, primask
    push { r0 }
    mov r0, r8
    mov r1, r9
    mov r2, r10
    mov r3, r11
    push { r0-r7 }

    bl __cpp(change_running_thread)
    
    // NOS_THREAD_LOAD_STATE()
    pop { r0-r7 }
    mov r8, r0
    mov r9, r1
    mov r10, r2
    mov r11, r3
    pop { r0 }
    msr primask,r0
    pop { pc }
}

void load_thread(void)
{
    tcb[_rtid]->state = RUNNING_STATE;
    __LOAD_SP(tcb[_rtid]->sptr);
}

__asm void nos_context_load_core(void)
{
    preserve8
    
    bl __cpp(load_thread)
    
    // NOS_THREAD_LOAD_STATE()
    pop { r0-r7 }
    mov r8, r0
    mov r9, r1
    mov r10, r2
    mov r11, r3
    pop { r0 }
    msr primask,r0
    pop { pc }        
}

#endif
#endif
#endif
