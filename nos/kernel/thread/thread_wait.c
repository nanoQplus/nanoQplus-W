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
#include "thread_table.h"
#include "error.h"

STATUS thread_wait(UINT32 tid)
{
	STATUS status = E_OK;
	THREAD *thread = (THREAD *)tid;

	if (NOS_IS_ISR_MODE())  
	{
		status = E_OS_PERMISSION;
	}
	else if (thread == NULL)
	{
		status = E_THREAD_INVALID;
	}
	else if (thread->state != TS_READY)
	{
		status = E_THREAD_STATE; // no need to wait
	}
	else 
	{
		os_sched_lock();
		
		os_qRemove(thread);
		thread->state = TS_WAIT;

		os_sched_unlock_switch();
	}
	
	service_error_check(S_THREAD_WAIT, status);

    return status;
}
