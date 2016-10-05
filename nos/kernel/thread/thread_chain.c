//===================================================================
//
// thread_chain.c (@sheart)
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

extern UINT32 os_sched_lock_level;

STATUS thread_chain(UINT32 tid)
{
	THREAD *thread = (THREAD *) tid;
	STATUS status = E_OK;

	if (thread == NULL)
	{
		status = E_THREAD_INVALID;
	}
	else if (thread->state != TS_SUSPEND)
	{
		status = E_THREAD_STATE;
	}
	else if (NOS_IS_ISR_MODE())
	{
		status = E_OS_PERMISSION;
	}
	else 
	{
		os_sched_lock();

		os_qRemove(current_thread);
		current_thread->state = TS_SUSPEND;
		
		os_qPush(thread);
		thread->state = TS_READY;
		
		os_sched_lock_level--;

		current_thread->context = (CPUcontext *)current_thread->stack_bottom;

		os_zero_switch_context(current_thread, thread);

		status = E_OS_PERMISSION; /* NEVER REACH HERE */
	}

	service_error_check(S_THREAD_CHAIN, status);

   	return status;
}
