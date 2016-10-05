// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
/*
 * Copyright (c) 2006-2014
 * Electronics and Telecommunications Research Institute (ETRI)
 * All Rights Reserved.
 *
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 *
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 *
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 *
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 *
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS," WITHOUT A WARRANTY OF ANY KIND. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 */

/**
 * @name stdio_retarget.c
 * @breif STDIO retargeting to UART
 * @author Haeyong Kim (ETRI)
 * @date 2014. 5. 8
 */

#include "uart.h"
#include "stm32f4xx.h"
#ifdef UART_M

#if (defined (__GNUC__))
int _write (int fd, char *ptr, int len)
{
#if 1
    if (len == 1)
        nos_uart_putc(STDIO, (uint8_t)(ptr[0]));
    else
        nos_uart_dma_tx(STDIO, (uint8_t*)ptr, (uint16_t)len);
    return len;
#endif
    return 0;
}

int _read(int fd, char *ptr, int len)
{
#if 0
    if (len == 1)
        ptr[0] = (char)nos_uart_getc(STDIO);
    else
        nos_uart_dma_rx(STDIO, (uint8_t*)ptr, (uint16_t)len);
    return len;
#endif
    return 0;
}




#elif (defined (__ICCARM__))
#include <yfuns.h>
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    if (buffer == 0)
    {
        /* This means that we should flush internal buffers.  Since we don't we just return.
            (Remember, "handle" == -1 means that all  handles should be flushed.) */
        return 0;
    }
    /* This template only writes to "standard out" and "standard err", for all other file handles it returns failure. */
    if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
        return _LLIO_ERROR;

    if (size == 1)
        nos_uart_putc(STDIO, (uint8_t)(*buffer));
    else
        nos_uart_dma_tx(STDIO, (uint8_t*)buffer, (uint16_t)size);
    return size;
}

size_t __read(int handle, unsigned char * buffer, size_t size)
{
    /* This template only reads from "standard in", for all other file handles it returns failure. */
    if (handle != _LLIO_STDIN)
        return _LLIO_ERROR;

    if (size == 1)
        *buffer = (unsigned char)nos_uart_getc(STDIO);
    else
        nos_uart_dma_rx(STDIO, (uint8_t*)buffer, (uint16_t)size);
    /* Clear interrupts */
    if (STDIO == UART1_CH)
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    else if ( STDIO == UART2_CH)
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    return size;
}




#elif (defined (__CC_ARM))
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
struct __FILE
{
    int handle;
};

FILE __stdout;

int fputc(int ch, FILE *f)
{
    nos_uart_putc(STDIO, ch);
    return ch;
}




#endif

#else	// UART_M

int _write (int fd, char *ptr, int len)
{
    return 0;
}

int _read(int fd, char *ptr, int len)
{
    return 0;
}

#endif

