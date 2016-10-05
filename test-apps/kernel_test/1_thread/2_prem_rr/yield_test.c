//========================================================================
// File		: preemption.c 
// Author	: jun361@etri.re.kr
// Date		: 2006.06.01
// Description : Basic preemption RR context switching program. 
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 tid1, tid2, tid3, tid4;

void task1(void* );
void task2(void* );
void task3(void* );
void task4(void* );

void do_loop(UINT8 id)
{
	uart_printf("Task id = %d\n", id);
	delay_ms(100);
}

void task1(void *args)
{
	do_loop(1);
}

void task2(void *args)
{
	while (1)
	{
		do_loop(2);
		thread_yield();
	}
}

void task3(void *args)
{
	while (1)
	{
		do_loop(3);
		thread_yield();
	}
}

void task4(void *args)
{
	while (1)
	{
		do_loop(4);
		thread_yield();
		//if (get_thread_state(tid1) == TS_SUSPEND)
		//	thread_activate(tid1);
	}
}

void app_init(void)
{
	thread_create(task1, NULL, 9, PRIORITY_HIGH, FIFO, &tid1); 	
	thread_create(task2, NULL, 9, PRIORITY_NORMAL, FIFO, &tid2); 
	thread_create(task3, NULL, 9, PRIORITY_NORMAL, FIFO, &tid3);
	thread_create(task4, NULL, 9, PRIORITY_NORMAL, FIFO, &tid4);

	thread_activate(tid1);
	thread_activate(tid2);
	thread_activate(tid3);
	thread_activate(tid4);
}
