//===================================================================
//
// thread_create_exit.c (@sheart)
//
//===================================================================
#include "thread.h"

#include "arch.h"
#include "critical_section.h"
#include "sched.h"
#include "thread_table.h"
#include "error.h"
#include "time.h"

extern THREAD **pAllocTHREAD;

STATUS thread_activate(UINT32 tid)
{
	THREAD *thread = (THREAD *) tid;
	STATUS status = E_OK;

	if (thread == NULL)
	{
		status = E_THREAD_INVALID;
	}
	else 
	{
		os_sched_lock();
		
		if (thread->state == TS_SUSPEND)
		{
			os_qPush(thread);
			
			thread->state = TS_READY;
		}

		os_sched_unlock_switch();
	}

	service_error_check(S_THREAD_ACTIVATE, status);

    return status;
}
