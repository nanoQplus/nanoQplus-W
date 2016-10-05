//========================================================================
// File		: msgq.c 
// Author	: @sheart
// Date		: 2006.06.01
// Description : message queue test program. 
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================
#include "nos.h"

UINT32	mqid;
UINT32 	id1, id2;

void task1(void *args)
{
	UINT32 data = 0;
	UINT32 status;

	uart_printf("Message Queue 1\n");
        while (1)
        {
		status = msgq_send(mqid, &data);
		if (status == E_MSGQ_FULL)
		{
			ENTER_CRITICAL();
			uart_printf("TX : Message Queue Full Error data %d dropped\n", data);
			EXIT_CRITICAL();
			delay_ms(1000);
		}
		else
		{
			ENTER_CRITICAL();
			uart_printf("Task1 : TX %d \n", data); 
			EXIT_CRITICAL();
			data++;
		}
		thread_yield();
	}
}

void task2(void *args)
{
	UINT32 data;
	UINT32 status;

	uart_printf("Message Queue 2\n");

        while (1)
        {
		status = msgq_recv(mqid, &data); // immediately returns even if data is not received
		if (status == E_MSGQ_EMPTY)
		{
			ENTER_CRITICAL();
			uart_printf("RX : Message Queue Empty Error. No data received\n");
			EXIT_CRITICAL();
			delay_ms(1000);
		}
		else
		{
			ENTER_CRITICAL();
			uart_printf("Task2 : RX %d \n", data);
			EXIT_CRITICAL();
		}
		thread_yield();
	}
}

void app_init(void)
{
	uart_printf("\n=== Message queue test program ===\n");

	msgq_create(5, &mqid); // spawn a msgq with an array of 5 integers (total 10 bytes)

	thread_spawn(task1, NULL, 0, PRIORITY_NORMAL, FIFO, &id1);
	thread_spawn(task2, NULL, 0, PRIORITY_NORMAL, FIFO, &id2);
}
