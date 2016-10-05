//===================================================================
//
// taskq.c (@sheart, @haekim)
//
// executes registered works by the system thread at the nearest time
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "taskq.h"

#include "critical_section.h"
#include "thread.h"
#include "sched.h"
#include "event.h"
#include "error.h"

TASKQ taskq;
extern THREAD *super_thread;
extern EVENT_MASK super_event;

/* Work Queue : a set of functions to be run by the system thread as soon as possible */
void os_taskq_init()
{
    taskq.head = taskq.tail = 0;
}


/* You must not use thread function in this function. */
STATUS taskq_register(void (*func)(void *args), void *args)
{
	STATUS status = E_OK;
	UINT32 foo;

	os_sched_lock();
	foo = (taskq.tail+1)%TASKQ_LEN;

	/* insert *func into work queue. */
	if (taskq.head == foo)		// if queue is full
	{
		os_sched_lock();
		
		status = E_TASKQ_FULL;
	}
	else
	{
		taskq.task[taskq.tail].func = func;
		taskq.task[taskq.tail].args = args;
		taskq.tail = foo;

		event_set((UINT32)super_thread, super_event);
		
		os_sched_unlock();
		
		thread_activate((UINT32)super_thread);
	}

	service_error_check(S_TASKQ_REGISTER, status);

	return status;
}

/* os_super_task() uses this function */
void os_taskq_exe(void) 
{
	UINT32 pos;

	while ( taskq.head != taskq.tail )	// not empty
	{
		pos = taskq.head;
		taskq.head = (taskq.head+1)%TASKQ_LEN;	// move to the next work
		
		(taskq.task[pos].func)(taskq.task[pos].args);	// execute a work function
    }
}
