//===================================================================
//
// thread_create.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"
#include "arch.h"
#include "platform.h"
#include "critical_section.h"
#include "heap.h"
#include "sched.h"
#include "hal_sched.h"
#include "error.h"
#include "queue_thread.h"


UINT32 global_vid_counter = 1;

extern void os_tsleep_exe(UINT32 tid);
	
void thread_entry(void)
{
	/* pre-process if any */

	/* execute the corresponding function by the thread */
	(current_thread->func)(current_thread->args_data);

	/* post-process */
	thread_terminate(SELF);

	/* DO NOT REACH HERE */
	system_abort(99);
}

#if 0
void os_thread_context_init(THREAD *thread)
{	
	thread->context = (CPUcontext *)(stack_bottom(thread) - (sizeof(struct cpucontext) >> 2));

	thread->context->cpsr 		= (UINT32 *) 0x00000013; // enable Interrupt(Unmask), SVC Mode
#if 0
	thread->context->reg[0] 	= (UINT32 *) 0x00000000;
	thread->context->reg[1] 	= (UINT32 *) 0x00000000;
	thread->context->reg[2] 	= (UINT32 *) 0x00000000;
	thread->context->reg[3] 	= (UINT32 *) 0x00000000;
	thread->context->reg[4] 	= (UINT32 *) 0x00000000;
	thread->context->reg[5] 	= (UINT32 *) 0x00000000;
	thread->context->reg[6] 	= (UINT32 *) 0x00000000;
	thread->context->reg[7] 	= (UINT32 *) 0x00000000;
	thread->context->reg[8] 	= (UINT32 *) 0x00000000;
	thread->context->reg[9] 	= (UINT32 *) 0x00000000;
	thread->context->reg[10] 	= (UINT32 *) 0x00000000;
	thread->context->reg[11] 	= (UINT32 *) 0x00000000;
	thread->context->reg[12] 	= (UINT32 *) 0x00000000;
#endif
	thread->context->lr 		= (UINT32 *) thread_entry;
	thread->context->pc 		= (UINT32 *) thread_entry;
}
#endif

#if 0
/* to measure the stack being used */
void os_memset_zero(STACK_PTR mem_s, STACK_PTR mem_e)
{
	STACK_PTR  p = mem_s;

	while (p != mem_e)
	{
		*p = 0x00000000; // write 4 zero bytes
		p++;
	}
}
#endif

STATUS thread_create(void (*func)(void *args), void *args_data, UINT32 stack_size, UINT32 priority, UINT32 option, UINT32 *threadId)
{
	STATUS status = E_OK;

	// Priority check
	if (priority >= PRIORITY_LEVEL_COUNT)
	{
		status = E_THREAD_PRIORITY;
	}
	else 
	{		
		THREAD *thread;
		
		// tcb (thread control block) settings when creating thread
		// A thread has id (*ptid), initialized stack memory, stack pointer and the number of sleeping ticks (sleep_tick) if sleeping.
		if ((thread = nos_malloc(sizeof(struct _tcb))) == NULL) 
		{
			status = E_SYS_MEMORY;
			service_error_check(S_THREAD_CREATE, status);

			return status;
		}
		else
		{	
			UINT32 align;
			STACK_PTR stack;

			stack_size += DEFAULT_STACK_SIZE;
			align = stack_size % (sizeof(STACK_ENTRY));
			
			if ( align )
			{
				stack_size = stack_size + sizeof(STACK_ENTRY) - align;
			}
			
			if ((stack = nos_malloc(stack_size+STACK_GUARD_SIZE)) == NULL)
			{
				status = E_SYS_MEMORY;
				service_error_check(S_THREAD_CREATE, status);

				return status;
			}
			else
			{

				*threadId = (UINT32) thread;
				
				thread->ptr 			= thread;				
				thread->priority 	= priority;

				/* stack management: structure's size */
				UINT32 th_stack_bott_addr, th_size_cpucontext, th_context;
				thread->stack_start		= stack;
				thread->stack_size 		= stack_size;
				thread->stack_bottom   	= stack_bottom(thread);
				//Edited by phj.  @phj
				th_stack_bott_addr		= (UINT32) thread->stack_bottom;
				align						= (sizeof(struct cpucontext) >> 2)%4;
				th_size_cpucontext		= (sizeof(struct cpucontext) >> 2) + (4-align);
				th_context				= th_stack_bott_addr - th_size_cpucontext;
				thread->context			= (CPUcontext*) th_context;
				
				// To measure the amount of stack used so far
				//os_memset_zero(tcb[tid]->stack_mem_start, tcb[tid]->stack_mem_end);

				thread->func		  	= func;
				thread->args_data	  	= args_data;
				thread->vid 			= global_vid_counter++;		//dummy data.

				// event processing	101201 @sheart
				thread->set_em			= 0;
				thread->wait_em			= 0;

				init_dnode(&thread->sleep_dnode, os_tsleep_exe, (UINT32)thread);
			
				init_tnode(thread);
				//thread->rdy_node.value = thread->vid;

				thread->state = TS_SUSPEND;
				thread->option = option;
					
				os_thread_context_init(thread->context);

				if (option != FIFO && option != RR)
				{
					status = E_THREAD_OPTION;			
				}
			}
		}
	}

	service_error_check(S_THREAD_CREATE, status);

	return status;
}

STATUS thread_spawn(void (*func)(void *args), void *args_data, UINT32 stack_size, UINT32 priority, UINT32 option, UINT32 *threadId)
{
	STATUS status = E_OK;

	status = thread_create(func, args_data, stack_size, priority, option, threadId);
	if (status == E_OK)
	{
		status = thread_activate(*threadId);
	}

	return status;	
}
