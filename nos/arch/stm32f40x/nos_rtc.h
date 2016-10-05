// -*- c-basic-offset:4; indent-tabs-mode:nil; tab-width:4; -*-
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
 * Realtime clock header
 *
 * @author Haeyong Kim (ETRI)
 * @date 2014. 6. 19.
 */

#ifndef __NOS_RTC_H__
#define __NOS_RTC_H__

#include "kconf.h"
#include "nos_common.h"


#ifdef LSE_OSC
#define PRESCALER_SEC    32767 /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
#else
#define PRESCALER_SEC    39999
#endif

void nos_rtc_init(void);

// keep current time
void nos_rtc_set_time(uint32_t sec);
uint32_t nos_rtc_get_time(void);

// set periodic alarm
int nos_rtc_set_alarm(uint32_t sec_period, void (*func)(void*), void* args, bool oneshot);
void nos_rtc_release_alarm(void);

// IRQ every sec.
void nos_rtc_enable_sec_intr(bool en);



#endif //__NOS_RTC_H__
