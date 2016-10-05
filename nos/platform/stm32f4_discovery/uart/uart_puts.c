//===================================================================
//
// uart_puts.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#include "uart.h"

#include "arch.h"
#include "critical_section.h"

void nos_uart_puts(UINT8 port_num, const char *str)
{
	//os_sched_unlock();
    UINT8 i=0;
    while( (*(str+i) != '\0') && (i < 255) )
    {
        if ( *(str+i) == '\n' )
        {
			nos_uart_putc(port_num, '\r');
        }
        nos_uart_putc(port_num, *(str+i));
		++i;
    }
	//os_sched_unlock_switch();
}
