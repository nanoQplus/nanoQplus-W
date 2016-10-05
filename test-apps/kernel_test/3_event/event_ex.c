//========================================================================
// File		: event.c 
// Author	: sheart@etri.re.kr
// Date		: 2010.12.01
// Description : Event Program
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 tid1, tid2, tid3, tid4, tid5;
UINT32 alid;

void task1(void* );
void task2(void* );
void task3(void* );
void task4(void* );
void task5(void* );

void cbf(void)
{
	uart_printf("Callback : Set Event4\n");
	event_set(tid5, EVENT(4));
}

void do_loop(UINT32 id)
{
	//for (i=0; i<10; i++) 
	{
		uart_printf("Task id = %d\n", id);
		delay_ms(100);
	}
}

void do_loop2(UINT32 id, UINT32 num)
{
	//for (i=0; i<10; i++) 
	{
		uart_printf("Task id = %d %d\n", id, num);
		delay_ms(100);
	}
}

void task1(void *args)
{
	do_loop2(1, 10);
	event_wait(EVENT(1));
	event_clear(EVENT(1));

	do_loop2(1, 20);
	event_wait(EVENT(2));
	event_clear(EVENT(2));

	do_loop2(1, 30);
	event_wait(EVENT(3));
	event_clear(EVENT(3));

	uart_printf("Alarm Starts\n");
	alarm_start(alid);

	uart_printf("Activate Task5\n");
	thread_activate(tid5);

	uart_printf("Task1 Terminates\n");
}

void task2(void *args)
{
	do_loop(2);

	uart_printf("EVENT(1) set\n");
	event_set(tid1, EVENT(1));

	uart_printf("Task2 Terminates\n");
}

void task3(void *args)
{
	do_loop(3);
	do_loop(3);

	uart_printf("EVENT(2) set\n");
	event_set(tid1, EVENT(2));

	uart_printf("Task3 Terminates\n");
}

void task4(void *args)
{
	do_loop(4);
	do_loop(4);
	do_loop(4);

	uart_printf("EVENT(3) set\n");
	event_set(tid1, EVENT(3));

	uart_printf("Task4 Terminates\n");
}

void task5(void *args)
{
	while (1)
	{
		uart_printf("Task5 is Waiting for EVENT(4)\n");

		event_wait(EVENT(4));
		event_clear(EVENT(4));
		do_loop(5);
	}
}

void app_init(void)
{
	thread_create(task1, NULL, 9, PRIORITY_HIGH, FIFO, &tid1); 	
	thread_create(task2, NULL, 9, PRIORITY_NORMAL, FIFO, &tid2); 
	thread_create(task3, NULL, 9, PRIORITY_NORMAL, FIFO, &tid3);
	thread_create(task4, NULL, 9, PRIORITY_NORMAL, FIFO, &tid4);
	thread_create(task5, NULL, 9, PRIORITY_LOW, FIFO, &tid5);

        alarm_create((ALARM_HANDLER)cbf, 0, SEC(5), SEC(5), &alid);

	thread_activate(tid1);
	thread_activate(tid2);
	thread_activate(tid3);
	thread_activate(tid4);
}
