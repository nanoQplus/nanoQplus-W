//===================================================================
//
// uart_puti.c (@sheart)
// Description : INT8, UINT8, INT16 print (-32768 ~ 32767)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "uart.h"
#include "strlib.h"

#include "arch.h"
#include "critical_section.h"


void nos_uart_puti(UINT8 port_num, int val)
{
	char pBuf[8];
        itoa(val, pBuf, 10);
        nos_uart_puts(port_num, pBuf);
}

