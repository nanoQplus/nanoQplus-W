//===================================================================
//
// thread_sleep.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"

#include "arch.h"
#include "critical_section.h"
#include "sched.h"
#include "tick.h"
#include "thread_table.h"
#include "queue_thread.h"
#include "error.h"

extern THREAD *current_thread;
extern TQUEUE os_rdy_q[PRIORITY_LEVEL_COUNT]; /* ready queue is an array of QUEUEs */

STATUS thread_sleep(UINT32 tick)
{
	STATUS status = E_OK;

   	if (NOS_IS_ISR_MODE())
    {
        status = E_OS_PERMISSION;
    }
	else
	{
		os_sched_lock();
		
		if (tick != 0)
		{
			os_qRemove(current_thread);

			current_thread->state = TS_SLEEP;

			tickq_Push(&current_thread->sleep_dnode, tick);
		}
		else /* tick == 0 */
		{
			/* cpu yield to the thread of the same priority if necessary */
			if (os_rdy_q[current_thread->priority].head->next != NULL)
			{
				os_qRemove(current_thread);
				os_qPush(current_thread);
			}
		}
		
		os_sched_unlock_switch(); /* if tick is zero, only scheduling occurs */
	}

	service_error_check(S_THREAD_SLEEP, status);
	
	return status;
}

/* os_tsleep_exe handler is executed in ISR mode. 
   this function is used in thread_create() function.*/
void os_tsleep_exe(UINT32 tid)
{
	THREAD *thread = (THREAD *)tid;

	os_qPush(thread);
	thread->state = TS_READY;
}
