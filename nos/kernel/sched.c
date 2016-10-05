//===================================================================
//
// sched.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "sched.h"
#include "hal_sched.h"	
#include "heap.h"
#include "arch.h"
#include "critical_section.h"
#include "intr.h"

#include "hal_thread.h"
#include "taskq.h"
#include "thread.h"
#include "error.h"
#include "uart.h"
#include "event.h"
#include "thread_table.h"
#include "queue_thread.h"
#include "tick.h"
#include "pwmgmt.h"

#include "lowpower.h"

/* extern variables */
extern THREAD *highest_thread;
extern void (*sched_callback)(void);   /* used for setting the scheduling handler */

/* extern functions */
extern void app_init(void);
extern void mysystem_save(unsigned long);

extern UINT32 Tick_Period;
extern UINT32 Remain_Tick_Value;
extern UINT32 SysTick_Reload_OverFlow;

/* local variables */
COUNT	os_sched_lock_level = 0;
THREAD *current_thread;
THREAD *idle_thread;
THREAD *super_thread;
EVENT_MASK super_event;

void (*usr_init)(void) = app_init; /* usr app init function */

TQUEUE 	os_rdy_q[PRIORITY_LEVEL_COUNT]; /* ready queue is an array of QUEUEs */

/* local functions */
static void os_sched_callback(void (*func)(void));
static void os_idle_task(void *args);
static void os_super_task(void *args);
static void display_kernel_info(void);



static void os_sched_handler(void)
{
	os_sched_lock_level++;

	/*  tickq_Expired() will handle all housekeeping works. */
	tickq_Expired(); 

	os_sched_lock_level--;
}

void os_sched_init(void)
{
	UINT32 i;
	UINT32 super_tid;
	UINT32 idle_tid;
	
	/* STEP1 : Setting of periodic scheduler interrupt */
	nos_sched_hal_init();

	/* STEP2 : Set scheduler callback function */
	os_sched_callback( os_sched_handler );
	
	/* STEP3 : Initialize Ready Queue */	
	for (i=0; i<PRIORITY_LEVEL_COUNT; i++)
	{
		init_tqueue(&os_rdy_q[i]);
	}

	/* STEP4 : Prepare Idle THREAD */
	thread_create(os_idle_task, NULL, 0, PRIORITY_IDLE_THREAD, FIFO, &idle_tid);
	idle_thread = (THREAD *)idle_tid;

	/* STEP5 : Prepare Super thread */
	thread_create(os_super_task, NULL, 0, PRIORITY_SUPER_THREAD, FIFO, &super_tid);
	super_thread = (THREAD *)super_tid;
		
	display_kernel_info(); // added by @sheart 20101228

	/* STEP6 : Initialize Tick queue */
	tickq_Init();
	
}

void os_start(void)
{
	/* start idle thread */
	NOS_DISABLE_GLOBAL_INTERRUPT();

	/* push idle thread to the ready queue */
	os_qPush(idle_thread);
	idle_thread->state = TS_READY;	

	/* launch the super thread and update the highest thread */	
	/* push super thread to the ready queue */
	os_qPush(super_thread);
	super_thread->state = TS_READY;
	current_thread = super_thread;	
	
	os_load_context(current_thread);
}

void os_sched_lock(void)
{
	NOS_ENTER_CRITICAL_SECTION();
}

void os_sched_unlock(void)
{
	NOS_EXIT_CRITICAL_SECTION();	
}

void os_sched_unlock_switch(void)
{
	if (NOS_IS_TASK_MODE()) /* context switch immediately */
	{
		if (highest_thread != current_thread)
		{
			THREAD *prev_thread = current_thread;
			
			current_thread = highest_thread;

			os_sched_lock_level--;
			os_switch_context(prev_thread, current_thread);
			NOS_ENABLE_GLOBAL_INTERRUPT();
		}
		else
		{
			NOS_EXIT_CRITICAL_SECTION();	
		}
	}
	else
	{
		NOS_EXIT_CRITICAL_SECTION();	
	}
}

void os_sched_unlock_bottom_half(void)
{
	if (NOS_IS_TASK_MODE())
	{
		if (highest_thread != current_thread)
		{
			THREAD *prev_thread = current_thread;
			
			current_thread = highest_thread;

			os_sched_lock_level--;
			
			prev_thread->context = (CPUcontext *)prev_thread->stack_bottom;
			os_zero_switch_context(prev_thread, current_thread);
		}
		else
		{
			NOS_EXIT_CRITICAL_SECTION();	
		}
	}
	else
	{
		NOS_EXIT_CRITICAL_SECTION();	
	}
	
}

void os_idle_task(void *args) {
	
	for(;;) {
	  
#ifdef PWM_M
	switch (lp_get_idle_mode(actual_idle_tick)) {
			case IDLE_MODE:
				//for (;;) {
					__ASM volatile ("nop");
				//} // end for
				break;
			case SLEEP_MODE:
				lp_enter_sleep_mode(actual_idle_tick);
				break;
			case STOP_MODE:
				lp_enter_stop_mode(actual_idle_tick);
				break;
			case STANDBY_MODE:
				lp_enter_standby_mode(actual_idle_tick);
				break;					
			case POWEROFF_MODE:
				lp_enter_pwroff_mode(actual_idle_tick);
				break;
			default:
				break;
		} // end switch
#endif		

	} // end for
} // end func

static void os_super_task(void *args)
{	
	NOS_ENABLE_GLOBAL_INTERRUPT();
	
	/* invoke usr app_init() function */
	usr_init();

	/* set super event */
	super_event = EVENT(0);

	/* clear super event */
	event_clear(super_event);
	
	while (1)
	{
		event_wait(super_event);
		
		/* Process Task Queue here */
		if (!OS_TASKQ_IS_EMPTY())
		{
			os_taskq_exe();
		}
		
		event_clear(super_event);
	}
}

static void os_sched_callback(void (*func)(void))
{
    /* Set scheduler interrupt callback function */
    sched_callback = func;
}


static void display_kernel_info(void)
{
#ifdef UART_M
	uart_printf("\n");
	uart_printf("====================================\n");
	uart_printf(" *** ETRI OS Kernel Starts ***\n");
	uart_printf("====================================\n");
#endif	
}

