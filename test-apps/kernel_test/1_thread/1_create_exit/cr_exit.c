//========================================================================
// File		: create_exit.c
// Author	: @jun361, @sheart, @haekim
// Date		: 2007.11.14
// Description : "thread_create" and "thread_exit" test program. "thread_exit" is called by kernel automatically when it returns.
//			User thread can be created up to 5 threads or 15 threads with thread_ext module.
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 tid1, tid2;
UINT8 token;

void do_loop(UINT8 id)
{
	UINT8 i;

	for (i=0; i<1; i++)
	{
	ENTER_CRITICAL();
	uart_printf("\nTask%d is now working. - ", id);
	EXIT_CRITICAL();
		delay_ms(100);	// To slow down uart printf speed
	}
}

void task1(void *args)
{
	UINT32 my_id = get_thread_vid();
	while (1)
	{
		do_loop(my_id);

		thread_activate(tid2);
	}
}

void task2(void *args)
{
	UINT32 my_id = get_thread_vid();

	//while (1)
	{
		do_loop(my_id);
	}
}

void app_init(void)
{
	token = 0;

	thread_create(task1, NULL, 0, PRIORITY_NORMAL, FIFO, &tid1); 
	thread_create(task2, NULL, 0, PRIORITY_HIGH, FIFO, &tid2); 
	
	thread_activate(tid1);
}

