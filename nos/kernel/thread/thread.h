//===================================================================
//
// thread.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef THREAD_H
#define THREAD_H
#include "kconf.h"

#include "nos_common.h"

#include "hal_thread.h"
#include "hal_sched.h"
#include "queue_delta.h"

// the maximum number of threads supported
#define MAX_NUM_USER_THREAD     (255)  // the number of user threads

#define MAX_NUM_TOTAL_THREAD	(MAX_NUM_USER_THREAD+1)  // the number of user threads + idle thread

// thread states
#define TS_READY	(0x00)
#define TS_WAIT		(0x01)
#define TS_SLEEP	(0x03)
#define TS_SUSPEND	(0x04)

#define SELF			((UINT32) current_thread)

// option
#define FIFO	(0)
#define RR		(1)

typedef struct cpucontext
{
	UINT32 *reg0;
	UINT32 *reg1;
	UINT32 *reg2;
	UINT32 *reg3;
	UINT32 *reg4;
	UINT32 *reg5;
	UINT32 *reg6;
	UINT32 *reg7;
	UINT32 *reg8;
	UINT32 *reg9;
	UINT32 *reg10;
	UINT32 *reg11;
	UINT32 *reg12; // r12 scratch register. 

	UINT32 *primask;
	UINT32 *psr;
	UINT32 *lr;
	UINT32 *pc;
	
	//UINT32 *lrex;
} CPUcontext;

typedef struct _tcb
{
	CPUcontext  *context;
	struct _tcb	*ptr;   	  // thread pointer
	void 		(*func)(void *);  // function pointer
	void		*args_data;	  // function arguments
	UINT32 		state;		  // thread state
	UINT32   	priority; 	  // thread priority
    //STACK_PTR 	sptr;             //added by phj. @160229 // thread stack pointer
	STACK_PTR 	stack_start;
	STACK_PTR	stack_bottom;
	UINT32		stack_size;	  // thread stack size

	/* for event handling */
	UINT32		set_em;
	UINT32		wait_em;
	
	/* for sleep handling */
	DNODE		sleep_dnode;

	/* for ready queue handling */
	//NODE 		rdy_node;
	struct _tcb	*prev;
	struct _tcb	*next;

	/* misc */
	UINT32		vid;
	UINT32		option;
}_TCB;


typedef _TCB THREAD;

void thread_entry(void);
UINT32 thread_create(void (*func)(void *args), void *args_data, UINT32 stack_size, UINT32 priority, UINT32 option, UINT32 *threadId);
UINT32 thread_spawn(void (*func)(void *args), void *args_data, UINT32 stack_size, UINT32 priority, UINT32 option, UINT32 *threadId);
UINT32 thread_terminate(UINT32 tid);
UINT32 thread_activate(UINT32 tid);
UINT32 thread_chain(UINT32 tid);
UINT32 thread_sleep(UINT32 tick);
UINT32 thread_wait(UINT32 tid);
UINT32 thread_wakeup(UINT32 tid);
void thread_yield(void);

// returns thread information
#define get_thread_id()			((UINT32) current_thread)
#define get_thread_vid()		(current_thread->vid)
#define get_thread_state(threadId)	(((THREAD *)(threadId))->state)
#define get_thread_priority(threadId)	(((THREAD *)(threadId))->basePriority)
#define get_thread_stack_pointer(threadId) (((THREAD *)(threadId))->sptr)

#endif // ~THREAD_H
