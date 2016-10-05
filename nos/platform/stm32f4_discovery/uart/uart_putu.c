//===================================================================
//
// uart_putu.c (@sheart)
// Description : UINT8, UINT16 print
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#include "uart.h"
#include "strlib.h"

#include "arch.h"
#include "critical_section.h"


void nos_uart_putu(UINT8 port_num, int val)
{
	char pBuf[8];

        utoa(val, pBuf, 10);
        nos_uart_puts(port_num, pBuf);
}
