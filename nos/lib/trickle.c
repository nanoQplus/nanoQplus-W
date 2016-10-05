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
 * Trickle timer implementation
 *
 * @author Jongsoo Jeong (ETRI)
 * @date 2010. 7. 20.
 */

#include "kconf.h"
#ifdef KERNEL_M
#include "trickle.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "heap.h"
#include "user_timer.h"

//#define TRICKLE_DEBUG

void trickle_on_timer(void *arg);
ERROR_T trickle_set_timer(TRICKLE *t);

UINT32 trickle_get_random_t(UINT8 i)
{
    UINT32 max = 1;

    while (i-- > 1)
    {
        max <<= 1; // I/2
    }

    //return (random() / (RANDOM_MAX / max + 1)) + max;
    return ((rand() + rand()) % max) + max; // return a random value between (I/2, I)
}

BOOL trickle_is_suppressed(TRICKLE *t)
{
    return (t->counter >= t->k);
}

void trickle_on_timer(void *arg)
{
    TRICKLE *t;
    t = (TRICKLE *) arg;

#ifdef TRICKLE_DEBUG
    printf("%s()-st:%d, interval:%u, func:%p\n",
           __FUNCTION__, t->state, t->interval, t->func);
#endif

        if (t->next_t > t->elapsed)
        {
            // An interval has not expired yet.
            trickle_set_timer(t);
        }
        else
        {
            if (t->state == TRICKLE_BEFORE_EXP)
            {
                t->state = TRICKLE_EXPIRED;
                if (!trickle_is_suppressed(t) && (t->func != NULL))
                {
                    t->func(t->arg);
                }

                if (t->state != TRICKLE_STOPPED)
                {
                    UINT32 i = 1;
                    UINT8 interval = t->interval;

                    while (interval-- > 0)
                        i <<= 1;
                    //i = pow(2, t->interval);
                    t->next_t = i - t->next_t;
                    t->elapsed = 0;
                    if (trickle_set_timer(t) == ERROR_SUCCESS)
                        t->state = TRICKLE_AFTER_EXP;
                }
            }
            else if (t->state == TRICKLE_AFTER_EXP)
            {
                if (t->interval < t->imin + t->doublings)
                    t->interval++; // Set new interval value up to the maximum value.
                else
                    t->interval = t->imin + t->doublings;
                t->next_t = trickle_get_random_t(t->interval);
                t->elapsed = 0;
                t->counter = 0;
                if (trickle_set_timer(t) == ERROR_SUCCESS)
                    t->state = TRICKLE_BEFORE_EXP;
            }
        }
}

ERROR_T trickle_set_timer(TRICKLE *t)
{
    UINT32 remain = t->next_t - t->elapsed;
    UINT32 max_sec = nos_user_timer_get_max_sec();

    if (remain > max_sec * 1000)
    {
        t->tid = nos_user_timer_create_sec(trickle_on_timer,
                                           (void *) t,
                                           max_sec,
                                           NOS_USER_TIMER_ONE_SHOT);
        t->elapsed += max_sec * 1000;
    }
    else if (remain > 65535)
    {
        UINT16 sec = remain / 1000;
        t->tid = nos_user_timer_create_sec(trickle_on_timer,
                                           (void *) t,
                                           sec,
                                           NOS_USER_TIMER_ONE_SHOT);
        t->elapsed += (UINT32) sec * 1000;
    }
    else
    {
        t->tid = nos_user_timer_create_ms(trickle_on_timer,
                                          (void *) t,
                                          remain,
                                          NOS_USER_TIMER_ONE_SHOT);
        t->elapsed += remain;
    }

#ifdef TRICKLE_DEBUG
    printf("%s()-remain:%lu, new tid:%d\n",
           __FUNCTION__, remain, t->tid);
#endif

    if (t->tid < 0)
    {
        t->state = TRICKLE_STOPPED;
        return ERROR_GENERAL_FAILURE;
    }
    else
    {
        return ERROR_SUCCESS;
    }
}

ERROR_T trickle_init(TRICKLE *t)
{
    t->counter = 0;
    t->state = TRICKLE_BEFORE_EXP;

    t->interval = t->imin;
    t->next_t = trickle_get_random_t(t->imin);
    t->elapsed = 0;

    if (trickle_set_timer(t) < 0)
    {
        return ERROR_GENERAL_FAILURE;
    }
    else
    {
        return ERROR_SUCCESS;
    }
}

/**
 *
 * @note This Trickle implementation can support maximum interval time up to 2^31
 *       msec.
 */
ERROR_T trickle_create(TRICKLE *t,
                       UINT8 min,
                       UINT8 dbls,
                       UINT8 k,
                       void(*func)(void *),
                       void *arg)
{
    ERROR_T ret;

#ifdef TRICKLE_DEBUG
    printf("%s()-\n", __FUNCTION__);
#endif

    if ((min + dbls > 31) || (!t))
    {
        ret = ERROR_INVALID_ARGS;
    }
    else
    {
        t->state = TRICKLE_STOPPED;
        t->imin = min;
        t->doublings = dbls;
        t->func = func;
        t->arg = arg;
        t->k = k;

        ret = trickle_reset(t);
    }
    return ret;
}

ERROR_T trickle_increment(TRICKLE *t)
{
#ifdef TRICKLE_DEBUG
    printf("%s()-\n", __FUNCTION__);
#endif

    if (!t)
        return ERROR_INVALID_ARGS;

    if (t->counter < 255)
        t->counter++;

    return ERROR_SUCCESS;
}

ERROR_T trickle_reset(TRICKLE *t)
{
    ERROR_T ret;
#ifdef TRICKLE_DEBUG
    printf("%s()-\n", __FUNCTION__);
#endif

    if (t)
    {
        if (t->state == TRICKLE_AFTER_EXP ||
            t->state == TRICKLE_BEFORE_EXP ||
            t->state == TRICKLE_EXPIRED)
        {
            nos_user_timer_destroy(t->tid);
            ret = trickle_init(t);
        }
        else if (t->state == TRICKLE_STOPPED)
        {
            ret = trickle_init(t);
        }
        else
        {
            ret = ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        ret = ERROR_INVALID_ARGS;
    }
    return ret;
}

ERROR_T trickle_stop(TRICKLE *t)
{
    ERROR_T ret;

    if (t)
    {
#ifdef TRICKLE_DEBUG
        printf("%s()-f:%p, state:%d, tid:%d\n",
               __FUNCTION__, (void *) t->func, t->state, t->tid);
#endif
        if (t->state == TRICKLE_STOPPED)
        {
            t->tid = NOS_USER_TIMER_CREATE_ERROR;
            ret = SUCCESS_NOTHING_HAPPENED;
        }
        else if (t->state == TRICKLE_AFTER_EXP ||
                 t->state == TRICKLE_BEFORE_EXP ||
                 t->state == TRICKLE_EXPIRED)
        {
            nos_user_timer_destroy(t->tid);
            t->tid = NOS_USER_TIMER_CREATE_ERROR;
            t->state = TRICKLE_STOPPED;
            ret = ERROR_SUCCESS;
        }
        else
        {
            ret = ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        ret = ERROR_INVALID_ARGS;
    }
    return ret;
}

#endif

