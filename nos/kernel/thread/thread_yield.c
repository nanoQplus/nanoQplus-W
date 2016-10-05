//===================================================================
//
// thread_yield.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"

#include "arch.h"
#include "critical_section.h"
#include "sched.h"
#include "queue_thread.h"
#include "thread_table.h"

extern THREAD *current_thread;
extern TQUEUE os_rdy_q[PRIORITY_LEVEL_COUNT]; /* ready queue is an array of QUEUEs */

void thread_yield(void)
{
	os_sched_lock();

	if (os_rdy_q[current_thread->priority].head->next != NULL)
	{
		os_qRemove(current_thread);
		os_qPush(current_thread);
	}
	
	os_sched_unlock_switch();
}

