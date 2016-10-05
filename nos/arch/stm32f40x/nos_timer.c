// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
/*
 * Copyright (c) 2006-2014
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
 * d) Using the name, trademark or logo of ETRI or the names of contribu1tors in
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
 * @file nos_timer.c
 * @author Haeyong Kim (ETRI) @phj.
 * @date 2013. 5. 22.
 */

#define NDEBUG
#include "nos.h"
#include "kconf.h"
#include "nos_debug.h"


#include "nos_timer.h"
#include "stm32f4xx_tim.h"
#include "hal_sched.h"


extern __IO uint32_t MeasureTimer_CNT;
extern __IO uint32_t SysTick_CNT;
extern DQUEUE tick_q;

uint32_t nos_hal_timer_range_us[NOS_TIMER_NUM];
UINT32 Tick_Period;
UINT32 Remain_Tick_Value;
UINT32 SysTick_Reload_OverFlow = 0;

UINT32 __gcounter;
UINT32 __saved_alid;

// functions declarations
void nos_EnableTimerInt(int timer_channel);



// functions definitions
void nos_timer_init(void) {
	int i;

	// systick does not use external timers
	// Enable Timer1~4 clock and release reset
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM3| RCC_APB1Periph_TIM4,ENABLE);

	for (i = 0; i < NOS_TIMER_NUM; i++) {
		nos_hal_timer_range_us[i] = 0;
	} // end for

	TIM_DeInit(TIM1);
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);
	TIM_DeInit(TIM4);

	/*// Enable update interrupt
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);*/

	//nos_EnableTimerInt(1); //@phj.
	//nos_timer_config(1, 10000); //Use Timer2(420kHz Speed, 10ms)
	//nos_timer_start(1);

} // end for

bool nos_timer_is_set(int timer_channel) {
    if (nos_hal_timer_range_us[timer_channel] == 0)
        return FALSE;
    else
        return TRUE;
} // end func


int nos_timer_config(int timer_channel, uint32_t us) {
	int ret;
	uint32_t timer_val, pr_val;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	NOS_DEBUG_START;

	if (timer_channel > NOS_TIMER_NUM) {
		NOS_DEBUG_ERROR;
		return EXIT_FAIL;       
	} // end if

	// Check whether timer is being used or not    
	if (nos_timer_is_set(timer_channel) == TRUE) {
		NOS_DEBUG_ERROR;
		nos_timer_release(timer_channel);
		//return EXIT_FAIL;       
	} // end if        

	// Check whether us value is proper or not
	if (us >= NOS_TIMER_MAX_US) {
		NOS_DEBUG_NOTIFY;
		timer_val = 0xFFFFFFFF;
		ret = EXIT_INVALID_ARGS;
	} else {
		if (timer_channel != 1) {
			//timer_val = 1 tick's clock. refer 8.2 RCC-Clocks, "Figure 11. Clock tree"
			timer_val = us * (SYSCLK / 1000000);
			ret = EXIT_SUCCESS;
		} else {
			//APB1. TIM2. 42MHZ. 42 clocks per 1us. //Inserted by Phj.
			timer_val = us * (PCLK1 / 1000000);
			ret = EXIT_SUCCESS;
		} // end if
	} // end if

	if (timer_channel != 1) {
		// Calculate values for HW registers
		pr_val=0;

		while ( (timer_val&0x80000000) == 0) {
			timer_val = timer_val << 1;
			pr_val++;
		} // end while

		if (pr_val >= 16) {
			timer_val = timer_val >> pr_val;
			pr_val = 1;
		} else {
			timer_val = timer_val >> 16;
			pr_val= 1 << (16-pr_val);
		} // end if

		TIM_TimeBaseInitStruct.TIM_Prescaler = pr_val-1;
		TIM_TimeBaseInitStruct.TIM_Period = timer_val-1;
		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;	// only for timer 1
	} else {
		/* Use 32bit TIM2 Timer. @phj */
		TIM_TimeBaseInitStruct.TIM_Prescaler = 1;
		TIM_TimeBaseInitStruct.TIM_Period = 420000 - 1;
		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
		// TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;	// only for timer 1
	} // end if

#ifdef NOS_DEBUG_TIMER_M
	printf("\n\rCOUNT_val:%lu\n\rPRESCALER_val:%lu\n\r", timer_val, pr_val);
#endif

	// configure HW timer regs
	switch (timer_channel) {
		case 0: //HW timer1
			TIM_DeInit(TIM1);
			TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
			break;
		case 1: //HW timer2
			TIM_DeInit(TIM2);
			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
			break;
		case 2: //HW timer3
			TIM_DeInit(TIM3);
			TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
			break;
		case 3: //HW timer4
			TIM_DeInit(TIM4);
			TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
			break;
		default:
			break;
	} // end switch

	nos_hal_timer_range_us[timer_channel] = us;

	NOS_DEBUG_END;

	return ret;
} // end func

int nos_timer_start(int timer_channel) {

	NOS_DEBUG_START;

	// Starts timer ASAP (check error later.)
	switch (timer_channel) {
		case 0: //HW timer1
			NOS_DEBUG_NOTIFY;
			TIM1->CNT = 0;
			TIM_ClearFlag(TIM1, TIM_IT_Update);
			TIM_Cmd(TIM1, ENABLE);
			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
			break;
		case 1: //HW timer2
			NOS_DEBUG_NOTIFY;
			TIM2->CNT = 0;
			TIM_ClearFlag(TIM2, TIM_IT_Update);
			TIM_Cmd(TIM2, ENABLE);
			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //inserted by phj. @160311
			break;
		case 2: //HW timer3
			NOS_DEBUG_NOTIFY;
			TIM3->CNT = 0;
			TIM_ClearFlag(TIM3, TIM_IT_Update);
			TIM_Cmd(TIM3, ENABLE);
			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
			break;
		case 3: //HW timer4
			NOS_DEBUG_NOTIFY;
			TIM4->CNT = 0;
			TIM_ClearFlag(TIM4, TIM_IT_Update);
			TIM_Cmd(TIM4, ENABLE);
			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
			break;
		default:
			break;
	} // end switch

	if (timer_channel > NOS_TIMER_NUM) {
		NOS_DEBUG_ERROR;
		return EXIT_FAIL;       
	} // end if

	// not configured timer
	if (nos_hal_timer_range_us[timer_channel] == 0) {
		NOS_DEBUG_ERROR;
		return EXIT_FAIL;     
	} // end if

	NOS_DEBUG_END;

	return EXIT_SUCCESS;
} // end func

bool nos_timer_expired(int timer_channel) {

	NOS_DEBUG_START;

	switch(timer_channel) {
		case 0: //HW timer1
			if (TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET) {
				//TIM_Cmd(TIM1, DISABLE);
				TIM_ClearFlag(TIM1, TIM_IT_Update);
				NOS_DEBUG_END;
				return TRUE;
			} // end if
			break;
		case 1: //HW timer2
			if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET) {
				//TIM_Cmd(TIM2, DISABLE);
				TIM_ClearFlag(TIM2, TIM_IT_Update);
				NOS_DEBUG_END;
				return TRUE;
			} // end if
			break;
		case 2: //HW timer3
			if (TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET) {
				//TIM_Cmd(TIM3, DISABLE);
				TIM_ClearFlag(TIM3, TIM_IT_Update);
				NOS_DEBUG_END;
				return TRUE;
			} // end if
			break;
		case 3: //HW timer4
			if (TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) == SET) {
				//TIM_Cmd(TIM4, DISABLE);
				TIM_ClearFlag(TIM4, TIM_IT_Update);
				NOS_DEBUG_END;
				return TRUE;
			} // end if
			break;
	} // end if

	NOS_DEBUG_END;

	return FALSE;
} // end func

uint32_t nos_timer_get_time(int timer_channel) {

	uint16_t count = 0, prescaler = 0;
	
	switch (timer_channel) {
		case 0: //HW timer1
			count = TIM1->CNT;
			prescaler = TIM1->PSC;
			break;
		case 1: //HW timer2
			count = TIM2->CNT;
			prescaler = TIM2->PSC;
			break;
		case 2: //HW timer3
			count = TIM3->CNT;
			prescaler = TIM3->PSC;
			break;
		case 3: //HW timer4
			count = TIM4->CNT;
			prescaler = TIM4->PSC;
			break;
	} // end switch

	return ((uint32_t)(count) * (uint32_t)(prescaler + 1)) / (SYSCLK / 1000000);
} // end func


void nos_timer_release(int timer_channel) {

	NOS_DEBUG_START;
	switch (timer_channel) {
		case 0: //HW timer1
			NOS_DEBUG_NOTIFY;
			TIM_Cmd(TIM1, DISABLE);
			TIM_ClearFlag(TIM1, TIM_IT_Update);
			break;
		case 1: //HW timer2
			NOS_DEBUG_NOTIFY;
			TIM_Cmd(TIM2, DISABLE);
			TIM_ClearFlag(TIM2, TIM_IT_Update);
			break;
		case 2: //HW timer3
			NOS_DEBUG_NOTIFY;
			TIM_Cmd(TIM3, DISABLE);
			TIM_ClearFlag(TIM3, TIM_IT_Update);
			break;
		case 3: //HW timer4
			NOS_DEBUG_NOTIFY;
			TIM_Cmd(TIM4, DISABLE);
			TIM_ClearFlag(TIM4, TIM_IT_Update);
			break;
	} // end switch

	nos_hal_timer_range_us[timer_channel] = 0;

	NOS_DEBUG_END;
} // end func

//Inserted by phj. @160302
// Only use  32bit counter timer.

void os_timer_tick_set(UINT32 tick) {

	UINT32 SysTickCTRL;
	//uart_printf("os_timer_tick_set :: tick is %d\r\n", tick);
	//UINT32 Reload_Value = 0;

	//DNODE *dnode = tick_q.head;

	if (!tick) {
		// if tick is 0, systick_handler calls immediately.
		//SysTick->LOAD = 10 * SYSTICK_CALIBRATION_SCALE;
		SysTick->LOAD = 1;
		SysTick->VAL = 0;
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		return;
	} // end if

	tick = tick * 10 * SYSTICK_CALIBRATION_SCALE;

	// backup & disable systick
	SysTickCTRL = SysTick->CTRL;
	SysTick->CTRL = SysTickCTRL & ~SysTick_CTRL_ENABLE_Msk;

	// normaly, enable tick and set up tick
	if ((tick > 0) && (tick < MAX_SUPPRESSTICK)) {
		SysTick->LOAD = tick * TICK_UNIT;
		SysTick->VAL = 0;
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		return;
	} // end if

	// when systick has overflow, uses SysTick_CNT
	if (SysTick_Reload_OverFlow == 0) {
		SysTick_Reload_OverFlow = 1;
		Tick_Period = tick / MAX_SUPPRESSTICK; // tick / 99msec
		Remain_Tick_Value = ((tick % MAX_SUPPRESSTICK) / 10) * 10; // more than 10msec
		Tick_Period--;
	} // end if

	SysTick_CNT++;

	SysTick->LOAD = MAX_SYSTICK_VALUE;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
} // end func

void os_timer_tick_stop(void) {
	UINT32 SysTickCTRL;

	SysTickCTRL = SysTick->CTRL;
	SysTick->CTRL = SysTickCTRL & ~SysTick_CTRL_ENABLE_Msk;

} // end func

// FIXME: os_timer_tick_get returns count of TIM2, not tick value. tick must be a unit of 10msec.
UINT32 os_timer_tick_get(void) {
	return SysTick->VAL;
} // end func

void nos_EnableTimerInt(int timer_channel)
{	
	NVIC_InitTypeDef  NVIC_InitStructure;
	switch(timer_channel)
	{
		case 0: //HW timer1
			NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        

			break;
		case 1: //HW timer2
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			break;
		case 2: //HW timer3
			NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        

			break;
		case 3: //HW timer4
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			break;
	}	

	NVIC_Init(&NVIC_InitStructure);
}



/*Timer for measuring elaspsed time in code.  @phj*/

void init_MT(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* TIM5 Clock Enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Enable TIM5 Global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM5 Initialize */   
	TIM_TimeBaseStructure.TIM_Period=1;
	TIM_TimeBaseStructure.TIM_Prescaler=42000-1; //1ms
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Down;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);


}


void start_MT(void)
{
	TIM5->CNT = 0;
	TIM_ClearFlag(TIM5, TIM_IT_Update);
	TIM_Cmd(TIM5, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}

uint32_t MT_get_time(void)
{
	return MeasureTimer_CNT; // 1ms
}
void MT_stop(void)
{
	TIM_Cmd(TIM5, DISABLE);
}
void MT_Restart(void)
{
	TIM_Cmd(TIM5, ENABLE);
}

void MT_Reset(void){
	TIM5->CNT = 0;
	TIM_ClearFlag(TIM5, TIM_IT_Update);
	TIM_Cmd(TIM5, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	MeasureTimer_CNT=0;
}


uint32_t get_SysTick_time(void)
{
	return (SysTick_CNT*10); 
}




