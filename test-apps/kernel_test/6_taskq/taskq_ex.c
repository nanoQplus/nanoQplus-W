//========================================================================
// File		: taskq.c 
// Author	: @sheart
// Date		: 2007.11.15
// Description  : Task queue test program
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 cnt;
UINT32 tid1, tid2, tid3;

void print_hello(void *args)
{
	UINT32 i,j;
	uart_printf("\nRegistered Task%d : I am running from TASK QUEUE, so I am the HIGHEST priority task.", cnt);
	for (i=0; i<10; i++) {
		ENTER_CRITICAL();
		uart_printf("\nRegistered Task%d is now working. - ", cnt);
		for (j=0; j<i; ++j)
		{
			uart_puts("*");
		}
		EXIT_CRITICAL();
		delay_ms(100);	// To slow down uart printf speed
	}
}

void do_loop(UINT32 id)
{
	UINT32 i,j;
	for (i=1; i<10; ++i)
	{
		ENTER_CRITICAL();
		uart_printf("\nTask%d is now working. - ", id);
		for (j=0; j<i; ++j)
		{
			uart_puts("*");
		}
		EXIT_CRITICAL();
		delay_ms(100);	// To slow down uart printf speed
	}
}


void task1(void *args)
{
	while (1)
	{
		uart_printf("\nTASK1 : I am a thread with LOW priority.");
		do_loop(1);
		taskq_register(print_hello, NULL);
		++cnt;
	}
}

void task2(void *args)
{
	while (1)
	{
		uart_printf("\nTASK2 : I am a thread with NORMAL priority.");
		do_loop(2);
		taskq_register(print_hello, NULL);
		++cnt;
		uart_printf("\nTASK2 : I'll sleep for a while for LOW priority thread.");
		thread_sleep(SEC(2));
	}
}

void task3(void *args)
{
	while (1)
	{
		uart_printf("\nTASK3 : I am a thread with LOW priority.\n");
		do_loop(3);
		taskq_register(print_hello, NULL);
		++cnt;
	}
}

void app_init(void)
{
	
	cnt = 0;

	thread_spawn(task1, NULL, 0, PRIORITY_LOW, FIFO, &tid1); 	
	thread_spawn(task2, NULL, 0, PRIORITY_NORMAL, FIFO, &tid2); 
	thread_spawn(task3, NULL, 0, PRIORITY_LOW, FIFO, &tid3); 
}
