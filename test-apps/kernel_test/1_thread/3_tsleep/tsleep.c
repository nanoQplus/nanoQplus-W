//========================================================================
// File		: tsleep.c
// Author	: jun361@etri.re.kr
// Date		: 2006.06.01
// Description : Basic preemption RR context switching program. 
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 tid1, tid2;

void task1(void* );

void do_loop(UINT8 id)
{
	uart_printf("Task id = %d\n", id);
}

void task1(void *args)
{
	while (1)
	{
		do_loop(1);
		thread_sleep(SEC(1));
	}
}

void task2(void *args)
{
	while (1)
	{
		do_loop(2);
		thread_sleep(SEC(5));
	}
}

void app_init(void)
{
	thread_spawn(task1, NULL, 9, PRIORITY_HIGH, FIFO, &tid1);
	thread_spawn(task2, NULL, 9, PRIORITY_HIGH, FIFO, &tid2); 	
}
