//===================================================================
//
// alarm.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "alarm.h"

#include "sched.h"
#include "heap.h"
#include "critical_section.h"
#include "tick.h"
#include "queue_delta.h"
#include "error.h"

UINT32 task_wt; //task working time. @phj.
//extern __IO uint32_t SysTick_CNT;
extern __IO uint32_t TIM2_CNT;
extern DQUEUE	tick_q;

extern UINT32 SysTick_Reload_OverFlow;
extern UINT32 Tick_Period;
extern UINT32 Remain_Tick_Value;



static void os_alarm_exe(UINT32 alid);
//modified by @phj. 20160621
STATUS _alarm_spawn(void (*func)(UINT32), UINT32 arg, UINT32 increment, UINT32 cycle, UINT32 *alid,  UINT32 work)
{
	STATUS status = E_OK;

	status = _alarm_create(func, arg, increment, cycle, alid, work);
	
	if (status == E_OK)
	{
		alarm_start(*alid);
	}

	return status;
}

//modified by @phj. 20160621
STATUS _alarm_create(void (*func)(UINT32), UINT32 arg, UINT32 increment, UINT32 cycle, UINT32 *alid, UINT32 work)
{
	STATUS status = E_OK;
	ALARM *alarm;

	if ((alarm = nos_malloc(sizeof(struct _alarm))) == NULL)
	{
		status = E_ALARM_ID;
	}
	else
	{		
		alarm->alid			= (UINT32) alarm;
		alarm->increment	= increment;  
		alarm->cycle 		= cycle;
		alarm->work			= work; //2016.06.22 @phj.
		alarm->handler		= func;
		alarm->arg			= arg;

		init_dnode(&alarm->alarm_dnode, os_alarm_exe, (UINT32)alarm);

		*alid = (UINT32)alarm;
	}

	service_error_check(S_ALARM_CREATE, status);

	return status;
}

STATUS alarm_destroy(UINT32 alid)
{
	STATUS status = E_OK;
	ALARM *alarm = (ALARM *)alid;
	
	if (alarm == NULL)
	{
		status = E_ALARM_INVALID;
	}
	else 
	{
		os_sched_lock();
		
		nos_free(alarm);

		os_sched_unlock();
	}

	service_error_check(S_ALARM_DESTROY, status);
	
	return status;
}

STATUS alarm_start(UINT32 alid)
{
	STATUS status = E_OK;
	ALARM *alarm = (ALARM *)alid;

	if (alarm == NULL)
	{
		status = E_ALARM_INVALID;
	}
	else 
	{
		os_sched_lock();

		tickq_Push(&alarm->alarm_dnode, alarm->increment);

		os_sched_unlock();
	}

	service_error_check(S_ALARM_START, status);
	
	return status;
}

STATUS alarm_stop(UINT32 alid)
{
	STATUS status = E_OK;
	ALARM *alarm = (ALARM *)alid;

	if (alarm == NULL)
	{
		status = E_ALARM_INVALID;
	}
	else 
	{
		os_sched_lock();

		tickq_Remove(&alarm->alarm_dnode);

		os_sched_unlock();
	}

	service_error_check(S_ALARM_STOP, status);
	
	return status;
}

/* os_alarm_handler is executed in ISR mode */
static void os_alarm_exe(UINT32 alid) {
	UINT32 calibrate_period = 0;
	ALARM *alarm = (ALARM *)alid;

	/* 
	   execute alarm handler since it is expired.
	   alarm function is executed in ISR mode. 
	*/

	(alarm->handler)(alarm->arg);

	if (tick_q.head != NULL) {
		calibrate_period = tick_q.head->delta;
	} // end if

	if (calibrate_period <= alarm->work) {
		calibrate_period = 0;
	} else {
		calibrate_period = calibrate_period - alarm->work;
	} // end if

	tick_q.head->delta = calibrate_period;

	/* schedule the next alarm */
	if (alarm->cycle) { /* cyclic alarm */
		if (SysTick_Reload_OverFlow == 0) {
			tickq_Push(&alarm->alarm_dnode, (alarm->cycle - alarm->work));
		} // end if
	} // end if
} // end func
