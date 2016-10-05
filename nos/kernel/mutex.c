//===================================================================
//
// mutex.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "mutex.h"
#include "critical_section.h"
#include "heap.h"
#include "sched.h"
#include "thread.h"
#include "thread_table.h"
#include "queue_thread.h"
#include "error.h"

//#define OFFSET_OF(TYPE, MEMBER) ((unsigned int)(&((TYPE *)0)->MEMBER))

STATUS mutex_create(UINT32 *muid, UINT32 ceil_priority)
{
	STATUS status = E_OK;
    MUTEX *mutex;

	mutex = nos_malloc(sizeof(struct _mutex));

	if (mutex == NULL)
	{
		status = E_SYS_MEMORY;
	}
	else
	{
		/* if ceil_priority == 0, then ceiling priority protocol is not applied */
		mutex->ceil_priority = ceil_priority; 
		mutex->owner = NULL;
		mutex->lock_level = 0;
		
		init_tqueue(&mutex->wait_queue);

		*muid = (UINT32)mutex;
	}

	service_error_check(S_MUTEX_CREATE, status);

    return status;
}

STATUS mutex_destroy(UINT32 muid)
{
    STATUS status = E_OK;
    MUTEX *mutex = (MUTEX *)muid;

    if (mutex == NULL)
   	{
        status = E_MUTEX_INVALID;
   	}
    else
    {
		os_sched_lock();

       	nos_free(mutex);

		os_sched_unlock();
	}

	service_error_check(S_MUTEX_DESTROY, status);

        return status;
}

STATUS mutex_lock(UINT32 muid)
{
    STATUS status = E_OK;
    MUTEX *mutex = (MUTEX *)muid;

    if (NOS_IS_ISR_MODE())
	{
		status = E_OS_PERMISSION;
	}
	else if (mutex == NULL)
	{
        status = E_MUTEX_INVALID;
	}
    else 
	{	
		os_sched_lock();
		if (mutex->owner == NULL)
		{
			mutex->owner = current_thread;

			/* apply the priority ceiling protocol */
			if (mutex->ceil_priority && (current_thread->priority < mutex->ceil_priority))
			{
				os_qRemove(current_thread);	
				
	        	mutex->saved_priority  = current_thread->priority;
				current_thread->priority = mutex->ceil_priority; // Ceil the thread priority	
									
				os_qPush(current_thread);
			}	
			
			/* context switch is not needed */
			os_sched_unlock();
		}
		else if (mutex->owner == current_thread)
		{
			mutex->lock_level++;
				
			os_sched_unlock();
		}
		else
		{			
			os_qRemove(current_thread);
			
			push_tnode(&mutex->wait_queue, current_thread);
			
			current_thread->state = TS_WAIT;

			os_sched_unlock_switch();
		}
   	}

	service_error_check(S_MUTEX_LOCK, status);

	return status;
}

STATUS mutex_unlock(UINT32 muid)
{

    STATUS status = E_OK;
    MUTEX *mutex = (MUTEX *)muid;

    if (NOS_IS_ISR_MODE())
	{
		status = E_OS_PERMISSION;
	}
    else if (mutex == NULL)
	{
        status = E_MUTEX_INVALID;
	}
	else
	{
		os_sched_lock();
		if (mutex->owner != current_thread)
		{
			os_sched_unlock();
			
			status = E_MUTEX_ACCESS;
		}
		else if (mutex->lock_level > 0)
		{			
			mutex->lock_level--;
			
			os_sched_unlock();
		}
		else
		{
			//NODE *node;
			THREAD *thread;
			
			/* apply the priority ceiling protocol */
			if (mutex->ceil_priority && (current_thread->priority > mutex->saved_priority))
			{
				os_qRemove(current_thread);
				
				current_thread->priority = mutex->saved_priority;
						
				os_qPush(current_thread);
			}

			//node = pop_node(&mutex->wait_queue);
			thread = pop_tnode(&mutex->wait_queue);
			
			if (thread != NULL)
			{
				//THREAD *thread;

				//thread = (THREAD *) ((UINT32)node - OFFSET_OF(_TCB, rdy_node));
				mutex->owner = thread;

				/* wake up the popped thread */
				os_qPush(thread);

				thread->state = TS_READY;
			}
			else
			{
				mutex->owner = NULL;
			}
			
			os_sched_unlock_switch();
		}
	}

	service_error_check(S_MUTEX_UNLOCK, status);

	return status;
}

