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

#ifndef __HAL_THREAD_H__
#define __HAL_THREAD_H__
#include "kconf.h"
#ifdef KERNEL_M

#ifdef __ICCARM__
#include <intrinsics.h>
#endif

#include "nos_common.h"
#include "arch.h"

#if 1
typedef MEMENTRY_T STACK_ENTRY; // The stack is 4 byte-contiguous array
typedef MEMENTRY_T *STACK_PTR;  // stack pointer (32 bit wide)
#else
typedef UINT32 	STACK_ENTRY;	// The stack is 4 byte-contiguous array
typedef UINT32	*STACK_PTR;	// stack pointer (32 bit wide)
#endif

#ifdef THREAD_M
struct arch_dep_states
{
    UINT32 delay_tick;
    UINT16 delay_timer_counter;
    BOOL delaying;
};

// thread control block


#define stack_bottom(thread)	(thread->stack_start + (thread->stack_size >> 2))
//@phj.
#define os_thread_context_init(context) \
({		\
	context->reg0 	= (UINT32 *) 0x00000000; \
	context->reg1 	= (UINT32 *) 0x00000000; \
	context->reg2 	= (UINT32 *) 0x00000000; \
	context->reg3 	= (UINT32 *) 0x00000000; \
	context->reg4 	= (UINT32 *) 0x00000000;	\
	context->reg5 	= (UINT32 *) 0x00000000;	\
	context->reg6 	= (UINT32 *) 0x00000000;	\
	context->reg7 	= (UINT32 *) 0x00000000;	\
	context->reg8 	= (UINT32 *) 0x00000000;	\
	context->reg9 	= (UINT32 *) 0x00000000;	\
	context->reg10 	= (UINT32 *) 0x00000000;	\
	context->reg11 	= (UINT32 *) 0x00000000;	\
	context->reg12 	= (UINT32 *) 0x00000000; \
	context->primask 	= (UINT32 *) 0x00000000;	\
	context->psr 	= (UINT32 *) 0x01000000; 	 \
	context->lr 		= (UINT32 *) 0xFFFFFFF9; \
	context->pc 		= (UINT32 *) thread_entry; \
				\
})

#define __SAVE_SP(stk_ptr)                      \
    {                                           \
        stk_ptr = (STACK_PTR)__get_MSP();       \
    }

#define __LOAD_SP(stk_ptr)                      \
    {                                           \
        __set_MSP((MEMADDR_T) stk_ptr);            \
    }



#endif // THREAD_M
#endif // KERNEL_M
#endif // !__HAL_THREAD_H__
