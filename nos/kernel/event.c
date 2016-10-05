//===================================================================
//
// event.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "event.h"

#include "sched.h"
#include "heap.h"
#include "critical_section.h"
#include "thread.h"
#include "thread_table.h"
#include "error.h"

extern THREAD *current_thread;

STATUS event_clear(UINT32 mask)
{
	STATUS status = E_OK;
	
	os_sched_lock();

	current_thread->set_em &= ~mask;

	os_sched_unlock();

	service_error_check(S_EVENT_CLEAR, status);

	return status;
}

STATUS event_get(UINT32 tid, UINT32 *event)
{
	STATUS status = E_OK;
	THREAD *thread = (THREAD *)tid;

	if (thread == NULL)
	{
		status = E_THREAD_INVALID;
	}
	else
	{
		os_sched_lock();

		*event = thread->set_em;
		
		os_sched_unlock();
	}

	service_error_check(S_EVENT_GET, status);

	return status;
}

STATUS event_set(UINT32 tid, UINT32 mask)
{
	STATUS status = E_OK;
	THREAD *thread = (THREAD *)tid;

	if (thread == NULL)
	{
		status = E_THREAD_INVALID;
	}
	else if (thread->state == TS_SUSPEND)
	{
		status = E_EVENT_STATE;
	}
	else
	{
		os_sched_lock();

		thread->set_em |= mask;

		/* 
		   if thread is in waiting state, it wakes up the thread immediately.
		   However, it is in sleeping state, it only sets the event mask.
		 */
		 
		if (((thread->wait_em & mask) == mask) && (thread->state == TS_WAIT))
		{
			os_qPush(thread);

			thread->state = TS_READY;
		}
		
		os_sched_unlock_switch();
	}

	service_error_check(S_EVENT_SET, status);

	return status;
}

STATUS event_wait(UINT32 mask)
{
	STATUS status = E_OK;

	if (NOS_IS_ISR_MODE())
	{
		status = E_EVENT_MODE;
	}
	else  
	{
		os_sched_lock();
		if ((current_thread->set_em & mask) == 0)
		{			
			current_thread->wait_em = mask;

			os_qRemove(current_thread);
			current_thread->state = TS_WAIT;
		}
		os_sched_unlock_switch();
	}

	service_error_check(S_EVENT_WAIT, status);

	return status;
}
