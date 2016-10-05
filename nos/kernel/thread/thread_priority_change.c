//===================================================================
//
// thread_priority_change.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"

#include "arch.h"
#include "critical_section.h"
#include "sched.h"
#include "thread_table.h"
#include "error.h"

// This function should not use local variables because it is used in os_sched_handler()
STATUS thread_priority_change(UINT32 tid, UINT32 new_priority)
{
	STATUS status = E_OK;
	THREAD *thread = (THREAD *) tid;

	if (thread->priority >= PRIORITY_LEVEL_COUNT)
	{
		status = E_THREAD_PRIORITY;
	}
	else
	{		
		if ((thread->state == TS_SUSPEND) || (thread->state & TS_WAIT)) /* thread is not in ready queue */
		{
			os_sched_lock();
			
			thread->priority = new_priority; // just change its priority
			
			os_sched_unlock();
		}
		else if (thread->state == TS_READY)
		{
			os_sched_lock();
			
			os_qRemove(thread);
			thread->priority = new_priority; // just change its priority
			os_qPush(thread);
			
			os_sched_unlock_switch();
		}
		else
		{
			status = E_THREAD_STATE;
		}
	}

	service_error_check(S_THREAD_PRIORITY_CHANGE, status);

	return status;
}
