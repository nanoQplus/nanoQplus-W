// -*- c-basic-offset:4; tab-width:4; indent-tabs-mode:nil; -*-
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
 * Realtime clock 
 *
 * @author Haeyong Kim (ETRI)
 * @date 2014. 6. 19.
 */

#include "kconf.h"
#include "nos_debug.h"

#include "stm32f4xx_conf.h"
#include "nos_rtc.h"
#include "platform.h"
#include "critical_section.h"


void (*nos_rtc_alarm_callback)(void*);
void *nos_rtc_alarm_args;
uint32_t nos_rtc_alarm_period;
bool nos_rtc_alarm_oneshot;


void nos_rtc_init(void)
{
#if 0

    /* EXTI line 17 is connected to the RTC Alarm event */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable PWR and BKPRAM clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_AHB1Periph_BKPSRAM, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    /* Reset Backup Domain */
    BKP_DeInit();

#ifdef LSE_OSC
    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#else
    RCC_LSICmd(ENABLE);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);    
    RTC_WaitForSynchro(); /* Wait for RTC registers synchronization */
    RTC_WaitForLastTask();    
    
    /* Set RTC prescaler: set RTC tick period to 1sec */
    RTC_SetPrescaler(PRESCALER_SEC); 
    RTC_WaitForLastTask();

    /* Set the current time */
    RTC_SetCounter(0);
    RTC_WaitForLastTask();

#ifdef NOS_STACK_DEBUG_PRINT_M
    nos_rtc_enable_sec_intr(TRUE);
#endif

#endif
}

void nos_rtc_set_time(uint32_t sec)
{
#if 0
    
    uint32_t get_time;
    RTC_WaitForLastTask();
    RTC_SetCounter(sec);
    RTC_WaitForLastTask();
    get_time = RTC_GetCounter();
    while (sec != get_time)
    {
        NOS_ERROR_PRINTF(("error: (Set%u != Get%u)", sec, get_time));
        RTC_WaitForLastTask();
        RTC_SetCounter(sec);
        RTC_WaitForLastTask();
        get_time = RTC_GetCounter();
    }

#endif
}

uint32_t nos_rtc_get_time(void)
{
#if 0
    return RTC_GetCounter();
#endif
    return 0;
}

/* Set the RTC Periodic Alarm */
int nos_rtc_set_alarm(uint32_t sec_period, void (*func)(void*), void* args, bool oneshot)
{
#if 0
    if (nos_rtc_alarm_callback != NULL)
        return EXIT_RESOURCE_BUSY;
    else if (sec_period == 0 || func == NULL)
        return EXIT_INVALID_ARGS;
    else
    {
        NOS_ENTER_CRITICAL_SECTION();
        nos_rtc_alarm_callback = func;
        nos_rtc_alarm_args = args;
        nos_rtc_alarm_oneshot = oneshot;
        nos_rtc_alarm_period = sec_period;

        RTC_SetAlarm(RTC_GetCounter()+nos_rtc_alarm_period+1);
        RTC_WaitForLastTask();

        RTC_ITConfig(RTC_IT_ALR, ENABLE);
        RTC_WaitForLastTask();
        NOS_EXIT_CRITICAL_SECTION();
        return EXIT_SUCCESS;
    }
#endif
    return 0;
}

void nos_rtc_release_alarm(void)
{
#if 0
    RTC_ITConfig(RTC_IT_ALR, DISABLE);
    nos_rtc_alarm_callback = NULL;
    RTC_WaitForLastTask();
#endif
}



/* Enable/Disable the RTC Second Interrupt */
void nos_rtc_enable_sec_intr(bool en)
{
#if 0
    RTC_ITConfig(RTC_IT_SEC, (FunctionalState)en);
    RTC_WaitForLastTask();
#endif
}



/*******************************************************************************
 * Function Name  : RTC_IRQHandler
 * Description    : This function handles RTC global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RTC_IRQHandler(void)
{
#if 0
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear interrupt pending bit*/
        RTC_ClearITPendingBit(RTC_IT_SEC);
        RTC_WaitForLastTask();
#ifdef NOS_STACK_DEBUG_M
        nos_debug_print_min_stack_ptr();
#endif
    }
    if(RTC_GetITStatus(RTC_IT_OW) != RESET)
    {
        /* Clear interrupt pending bit*/
        RTC_ClearITPendingBit(RTC_IT_OW);
        RTC_WaitForLastTask();
    }
#endif
}



/*******************************************************************************
 * Function Name  : RTCAlarm_IRQHandler
 * Description    : This function handles RTC Alarm interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
#if 0
    if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        /* Clear interrupt pending bit*/
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_WaitForLastTask();

        if (nos_rtc_alarm_callback != NULL)
            nos_rtc_alarm_callback(nos_rtc_alarm_args);
        if (nos_rtc_alarm_oneshot)
            nos_rtc_release_alarm();
        else
        {
            RTC_SetAlarm(RTC_GetCounter()+nos_rtc_alarm_period);
            RTC_WaitForLastTask();
        }
    }
#endif
}

