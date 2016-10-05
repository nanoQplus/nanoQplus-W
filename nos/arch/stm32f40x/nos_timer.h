/* Copyright (c) 2006-2014
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
 * @file nos_timer.h
 * @author Haeyong Kim (ETRI)
 * @date 2014. 4. 30.
 */

#ifndef __NOS_TIMER_H__
#define __NOS_TIMER_H__	

#include "nos_common.h"
#include "platform.h"

/**
NOS Timer Usage  (including CRITICAL_SECTION or ISR) 

    if ( nos_timer_config(channel, time) ==EXIT_SUCCESS){
        nos_timer_start(channel);
        do {
            ...your own code....
        } while ( !nos_timer_expired(channel) );
        nos_timer_release(channel);
    }

....in this case, the 'time' parameter must be larger than the duration of 'your own code'...
    if ( nos_timer_config(channel, time) ==EXIT_SUCCESS){
        nos_timer_start(channel);
        ...your own code....
        time = nos_timer_get_time();
        nos_timer_release(channel);
    }
*/


#define NOS_TIMER_NUM	4
#define NOS_TIMER_MAX_US	(0xFFFFFFFF/(SYSCLK/1000000))	//about 1min. SYSCLK=72MHZ
#define MAX_TICK	13094412U  /* 0xFFFFFFFF/TICK_UNIT */

// The units below are when TIM2 is systimer
//#define TICK_UNIT	506024		/* 10ms, approx */
//#define TICK_UNIT	420000		/* 10ms */

// The units below are when systick is systimer
#define MAX_SUPPRESSTICK		99
#define MAX_SYSTICK_VALUE		0xFFFFFF
//#define SYSTICK_CALIBRATION_SCALE	1.214
#define SYSTICK_CALIBRATION_SCALE	1
#define TICK_UNIT			168000		// 1msec

void nos_timer_init(void);
bool nos_timer_is_set(int timer_channel);
int nos_timer_config(int timer_channel, uint32_t us);
int nos_timer_start(int timer_channel);
bool nos_timer_expired(int timer_channel);
uint32_t nos_timer_get_time(int timer_channel);
void nos_timer_release(int timer_channel);


//Inserted by phj. @160302
void os_timer_tick_set(UINT32 tick);
void os_timer_tick_stop(void);
UINT32 os_timer_tick_get(void);

void init_MT(void);
void start_MT(void);
uint32_t MT_get_time(void);
void MT_stop(void);
void MT_Restart(void);
void MT_Reset(void);
uint32_t get_SysTick_time(void);

#endif // __NOS_TIMER_H__


