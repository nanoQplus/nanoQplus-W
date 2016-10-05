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
 * @file uart_rx_isr.c
 * @breif UART RX Interrupt functions
 * @author Haeyong Kim (ETRI)
 * @date 2014. 5. 9.
 */

#include "kconf.h"
#ifdef UART_M

#include "uart.h"
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "critical_section.h"

#define USART_FLAG_ERRORS (USART_FLAG_ORE | \
                           USART_FLAG_NE | \
                           USART_FLAG_FE | \
                           USART_FLAG_PE)

volatile uint8_t uart1_rx_data;
volatile uint8_t uart2_rx_data;

void (*nos_uart1_rx_callback)(uint8_t uart_ch, uint8_t data) =  NULL;
void (*nos_uart2_rx_callback)(uint8_t uart_ch, uint8_t data) =  NULL;


int nos_uart_set_rx_callback(uint8_t uart_ch, void (*func)(uint8_t uart_ch, uint8_t data))
{
    switch (uart_ch)
    {
        case UART1_CH:
            nos_uart1_rx_callback = func;
            return EXIT_SUCCESS;
        case UART2_CH:
            nos_uart2_rx_callback = func;
            return EXIT_SUCCESS;
        default:
            break;
    }
    return EXIT_FAIL;
}


int nos_uart_enable_rx_intr(uint8_t uart_ch)
{
    switch (uart_ch)
    {
        case UART1_CH:
            NVIC_EnableIRQ(USART1_IRQn);
            return EXIT_SUCCESS;
        case UART2_CH:
            NVIC_EnableIRQ(USART2_IRQn);
            return EXIT_SUCCESS;
        default:
            break;
    }
    return EXIT_FAIL;
}


int nos_uart_disable_rx_intr(uint8_t uart_ch)
{
    switch (uart_ch)
    {
        case UART1_CH:
            NVIC_DisableIRQ(USART1_IRQn);
            return EXIT_SUCCESS;
        case UART2_CH:
            NVIC_DisableIRQ(USART2_IRQn);
            return EXIT_SUCCESS;
        default:
            break;
    }
    return EXIT_FAIL;
}


bool nos_uart_rx_intr_is_set(uint8_t uart_ch)
{
    switch (uart_ch)
    {
        case UART1_CH:
            return (bool)((NVIC->ISER[((uint32_t)(USART1_IRQn) >> 5)] & (1 << ((uint32_t)(USART1_IRQn) & 0x1F))) != 0);
        case UART2_CH:
            return (bool)((NVIC->ISER[((uint32_t)(USART2_IRQn) >> 5)] & (1 << ((uint32_t)(USART2_IRQn) & 0x1F))) != 0);
        default:
            break;
    }
    return FALSE;
}


void USART1_IRQHandler(void)
{
    UINT16 st;
    st = USART1->SR;
    while (st & (USART_FLAG_RXNE | USART_FLAG_ERRORS))
    {
        uart1_rx_data = (char) USART_ReceiveData(USART1);
        if (!(st & USART_FLAG_ERRORS))
        {
            if (nos_uart1_rx_callback)
                nos_uart1_rx_callback(0, uart1_rx_data);
        }
        st = USART1->SR;
    }
}


void USART2_IRQHandler(void)
{
    UINT16 st;
    st = USART2->SR;
    while (st & (USART_FLAG_RXNE | USART_FLAG_ERRORS))
    {
        uart2_rx_data = (uint8_t)USART_ReceiveData(USART2);
        if (!(st & USART_FLAG_ERRORS))
        {
            if (nos_uart2_rx_callback)
                nos_uart2_rx_callback(1, uart2_rx_data);
        }
        st = USART2->SR;
    }
}


#endif // UART_M 
