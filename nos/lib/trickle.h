// -*- c-basic-offset:4; tab-width:4; indent-tabs-mode:nil; -*-
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
 * @file trickle.h
 * @brief Trickle timer
 *
 * @author Jongsoo Jeong (ETRI)
 * @date 2010. 7. 20.
 */

#ifndef TRICKLE_H
#define TRICKLE_H

#include "kconf.h"
#ifdef KERNEL_M
#include "nos_common.h"
#include "errorcodes.h"

/**
 * @defgroup libnos-trickle Trickle timer library
 * @brief Trickle timer library
 * @{
 */

// States
typedef UINT8 TRICKLE_STATE_T;
enum
{
    TRICKLE_STOPPED = 0,
    TRICKLE_BEFORE_EXP = 1,
    TRICKLE_AFTER_EXP = 2,
    TRICKLE_EXPIRED = 3,
    TRICKLE_STOP_SIGNALED = 4,
    TRICKLE_RESET_SIGNALED = 5,
};

struct _trickle_timer
{
    UINT32 next_t;
    UINT32 elapsed;
    void (*func)(void *);
    void *arg;
    UINT8 imin;
    UINT8 doublings;
    UINT8 interval;
    TRICKLE_STATE_T state;
    INT8 tid;
    UINT8 k;
    UINT8 counter;
};
typedef struct _trickle_timer TRICKLE;

ERROR_T trickle_create(TRICKLE *t,
                       UINT8 min,
                       UINT8 dbls,
                       UINT8 k,
                       void(*func)(void *),
                       void *arg);

ERROR_T trickle_increment(TRICKLE *t);
ERROR_T trickle_reset(TRICKLE *t);
ERROR_T trickle_stop(TRICKLE *t);
BOOL trickle_is_suppressed(TRICKLE *t);

/**
 * @}
 */
#endif
#endif //~TRICKLE_H
