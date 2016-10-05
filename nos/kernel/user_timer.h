// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
/*
 * Copyright (c) 2006-2012
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
 * @author Sangcheol Kim (ETRI)
 * @author Haeyong Kim (ETRI)
 * @author Jongsoo Jeong (ETRI)
 */

#ifndef USER_TIMER_H
#define USER_TIMER_H

#include "nos_common.h"

enum
{
    NOS_USER_TIMER_ONE_SHOT = 0,
    NOS_USER_TIMER_PERIODIC = 1,
};

#define NOS_USER_TIMER_MAX_SEC   ((UINT16) (655 * SCHED_TIMER_MS)/10)
#define NOS_USER_TIMER_MAX_MS   ((UINT32) (65535 * SCHED_TIMER_MS))

enum
{
    NOS_MAX_NUM_USER_TIMER = 16,
};

enum
{
    NOS_USER_TIMER_CREATE_ERROR	= -1,
    NOS_USER_TIMER_NO_EMPTY_SLOT_ERROR = -2,
};

typedef struct _nos_user_timer
{
    void (*func)(void *);               // user timer function pointer
    void *arg;                          // user timer function argument pointer
    UINT16 periodic_ticks;              // user timer period 
    volatile UINT16 next_sched_tick;    // next nos_shced_tick for expiration
} NOS_USER_TIMER;

void nos_user_timer_init(void);

void nos_user_timer_check_next_expire_tick(void);

UINT16 nos_user_timer_get_max_sec(void);

UINT32 nos_user_timer_get_max_ms(void);

INT8 nos_user_timer_create(void (*func)(void *), 
                           void *arg, 
                           UINT16 ticks, 
                           BOOL is_periodic);

INT8 nos_user_timer_create_ms(void (*func)(void *),
                              void *arg,
                              UINT16 msec,
                              BOOL periodic);

INT8 nos_user_timer_create_sec(void (*func)(void *),
                               void *arg,
                               UINT16 sec,
                               BOOL periodic);

BOOL nos_user_timer_activate(UINT8 id);

BOOL nos_user_timer_deactivate(UINT8 id);

BOOL nos_user_timer_destroy(UINT8 id);

BOOL nos_user_timer_reschedule(UINT8 id, UINT16 ticks);

UINT32 nos_get_sched_tick_ms(void);
void nos_user_timer_destroy_by_arg(void (*func)(void *),
                                   void *arg);

#endif // ~USER_TIMER_H
