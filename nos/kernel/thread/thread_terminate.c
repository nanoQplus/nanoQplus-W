//===================================================================
//
// thread_terminate.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"

#include "arch.h"
#include "critical_section.h"
#include "sched.h"
#include "error.h"
#include "thread_table.h"
#include "tick.h"

extern THREAD *highest_thread;
extern THREAD *current_thread;

STATUS thread_terminate(UINT32 tid)
{
	STATUS status = E_OK;
	THREAD *thread = (THREAD *)tid;

	if (NOS_IS_ISR_MODE())  
	{
		status = E_OS_PERMISSION;
	}
	else
	{
		os_sched_lock();

		if (thread == current_thread) /* self-terminate */
		{
			/* this thread is removed from ready queue now */
			os_qRemove(current_thread);
			
			current_thread->state = TS_SUSPEND;
			
			os_sched_unlock_bottom_half();

			status = E_OS_PERMISSION; /* NEVER REACH HERE */
		}
		else /* terminate other */
		{
			if (thread->state == TS_READY)
			{
				os_qRemove(thread);
			}
			else if (thread->state == TS_SLEEP)
			{
				tickq_Remove(&thread->sleep_dnode);
			}

			thread->state = TS_SUSPEND;
			
			os_sched_unlock_switch();
		}
	}

	service_error_check(S_THREAD_EXIT, status);

	/* NEVER REACH HERE */
	return status;
}
