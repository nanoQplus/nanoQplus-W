//========================================================================
// File		: mutex_ex.c 
// Author	: sheart@etri.re.kr
// Date		: 2010.12.03
// Description : 
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 tid1, tid2, tid3, tid4;
UINT32 alid1, alid2;
UINT32 key;

void task1(void* );
void task2(void* );
void task3(void* );
void task4(void* );

void do_loop(UINT32 id)
{
	UINT32 i;

	for (i=0; i<10; i++) 
	{
		uart_printf("Task id = %d - Got %d\n", id, i);
		delay_ms(100);
	}
}

void task1(void *args)
{
	mutex_lock(key);
	uart_printf("\nTask1 lock Resource\n");
	do_loop(1);
	uart_printf("\nTask1 unlock Resource\n");
	mutex_unlock(key);
	delay_ms(1000);
}

void task2(void *args)
{
	mutex_lock(key);
	uart_printf("\nTask2 lock Resource\n");
	do_loop(2);
	uart_printf("\nTask2 unlock Resource\n");
	mutex_unlock(key);
}

void app_init(void)
{
	thread_spawn(task1, NULL, 9, PRIORITY_NORMAL, FIFO, &tid1); 
	thread_spawn(task2, NULL, 9, PRIORITY_NORMAL, FIFO, &tid2); 

	mutex_create(&key, 0);

	alarm_create((ALARM_HANDLER)thread_activate, tid1, SEC(2), SEC(2), &alid1);
	alarm_create((ALARM_HANDLER)thread_activate, tid2, SEC(3), SEC(3), &alid2);

	alarm_start(alid1);
	alarm_start(alid2);
}
