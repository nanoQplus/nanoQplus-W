//===================================================================
//
// thread_wakeup.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"
#include "sched.h"

#include "arch.h"
#include "critical_section.h"
#include "sched.h"
#include "thread_table.h"
#include "tick.h"
#include "error.h"

STATUS thread_wakeup(UINT32 tid)
{
	THREAD *thread = (THREAD *) tid;
	STATUS status = E_OK;

	if (thread == NULL)
	{
		status = E_THREAD_INVALID;
	}
	else if (thread == current_thread)
	{
		status = E_THREAD_ID;
	}
	else 
	{
		os_sched_lock();

		if (thread->state == TS_SLEEP)
		{
			tickq_Remove(&thread->sleep_dnode);
		}
		
		if (thread->state & TS_WAIT) /* TS_SLEEP state falls on here, too */
		{
			os_qPush(thread);
		}

		thread->state = TS_READY;
			
		os_sched_unlock_switch();
	}

	service_error_check(S_THREAD_WAKEUP, status);

   	return status;
}

