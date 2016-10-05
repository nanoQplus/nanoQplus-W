/*
 * Copyright (c) 2014
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
 * @file spinlock.h
 * @brief Spinlock library
 * @date 2014. 5. 19.
 * @author Haeyong Kim (ETRI)
 */




#ifndef NOS_SPINLOCK_H
#define NOS_SPINLOCK_H

#include "nos_common.h"

typedef volatile enum spinlock_state
{
    SPINLOCK_UNLOCKED = 0,
    SPINLOCK_LOCKED = 1,
}SPINLOCK_T;


void spin_lock(SPINLOCK_T *lock);
bool spin_trylock(SPINLOCK_T *lock); //nonblocking spinlock operation
void spin_unlock(SPINLOCK_T *lock);


void spin_lock_bh(SPINLOCK_T *lock);    //disables software interrupts (bottom_half)
bool spin_trylock_bh(SPINLOCK_T *lock);
void spin_unlock_bh(SPINLOCK_T *lock);

void spin_lock_irqsave(SPINLOCK_T *lock, uint32_t flags);   // Store NVIC INTERRUPT ENABLE register
void spin_unlock_irqrestore(SPINLOCK_T *lock, uint32_t flags);

void spin_lock_irq(SPINLOCK_T *lock);   // disables IRQ(top_half)
void spin_unlock_irq(SPINLOCK_T *lock);





#endif  /* !NOS_SPINLOCK_H */
