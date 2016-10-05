//========================================================================
// File		: timer.c 
// Author	: @sheart
// Date		: 2007.07.29
// Description : timer function
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32 tid1;
UINT32 alid1, alid2, alid3;

void cbf1(void)
{
	uart_printf("===This message will be shown every 1 sec\n");
}

void cbf2(UINT32 x)
{
	uart_printf("===This message will be shown every 5 secs\n");
}

#if 0
void cbf3(UINT32 x)
{
	//uart_printf("===This message will be shown every 10 secs\n");
	thread_activate(tid1);
}
#endif

void task1(void *args)
{
        UINT32 my_id = get_thread_vid();

       uart_printf("Task%d is now working. - \n", my_id);
}

void app_init(void)
{
        thread_create(task1, NULL, 0, PRIORITY_NORMAL, FIFO, &tid1);

	alarm_create((ALARM_HANDLER)cbf1, 0, SEC(1), SEC(1), &alid1);
	alarm_create(cbf2, 0, SEC(5), SEC(5), &alid2);
	alarm_create((ALARM_HANDLER)thread_activate, tid1, SEC(3), SEC(10), &alid3);

	alarm_start(alid1);
	alarm_start(alid2);
	alarm_start(alid3);
}

