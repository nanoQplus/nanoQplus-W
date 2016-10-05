// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
/*
 * Copyright (c) 2006-2012
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
 * @author Haeyong Kim (ETRI)
 * @date 2009. 2. 23.
 * @brief User button test application
 */

#include "nos.h"

#if defined(BUTTON_M) && defined(LED_M)

extern void (*nos_button_callback[BUTTON_NUM])(void*);


void button_cb(void* args)
{
    uint8_t id;
    id = *((uint8_t*)args);
    printf("\n\r\n\rButton (ID: %u) is pressed!\n\r\n\r", id);
}

void idle(void *args)
{
    while(1)
    {
        led_toggle(0);
        uart_putc(_BS);
        uart_putc('-');
        delay_ms(200);

        led_toggle(1);
        uart_putc(_BS);
        uart_putc('\\');
        delay_ms(200);

        led_toggle(2);
        uart_putc(_BS);
        uart_putc('|');
        delay_ms(200);

        led_toggle(3);
        uart_putc(_BS);
        uart_putc('/');
        delay_ms(200);
    }
}

void app_init(void)
{
    UINT32 tid, i;
    
    for (i=0; i<LED_NUM; i++)
        nos_led_on(i);

    for (i=0; i<BUTTON_NUM; i++)
        nos_button_set_callback(i, button_cb);

#ifdef KERNEL_M
    thread_create(idle, NULL, 0, PRIORITY_NORMAL, FIFO, &tid);
    thread_activate(tid);
#else
    idle(NULL);
#endif
}

#endif
