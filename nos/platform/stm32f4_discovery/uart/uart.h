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
 * @file uart.h
 * @breif UART module
 * @author Haeyong Kim (ETRI)
 * @date 2014. 5. 2
 */

#ifndef __UART_H__
#define __UART_H__
#include "kconf.h"
#ifdef UART_M

#include "nos_common.h"

enum UART_CHANNEL
{
    UART1_CH = 0,
    UART2_CH = 1,
    NO_UART  = 255,
};
#ifdef UART1_STDIO
#define STDIO   UART1_CH
#elif defined (UART2_STDIO)
#define STDIO   UART2_CH
#else
#define STDIO   NO_UART
#endif

#ifdef UART1_SLIPIO
#define SLIPIO   UART1_CH
#elif defined (UART2_SLIPIO)
#define SLIPIO   UART2_CH
#else
#define SLIPIO   UART1_CH
#endif

#ifdef UART1_PPPIO
#define PPPIO   UART1_CH
#elif defined (UART2_PPPIO)
#define PPPIO   UART2_CH
#else
#define PPPIO   NO_UART
#endif


#define STDOUT STDIO

/**
 * @brief Initialize UART.
 *
 * @param[in] uart_ch  UART channel to be initialized
 */
void nos_uart_init(uint8_t uart_ch);

/**
 * @brief Send a character to the specified UART channel.
 *
 * @param[in] uart_ch  UART channel number to send to (0-1)
 * @param[in] data      A byte to be sent
 */
void nos_uart_putc(uint8_t uart_ch, const uint8_t data);
void nos_uart_puts(UINT8 port_num, const char *str);

//void nos_uart_puts(uint8_t uart_ch, const uint8_t *buf);


/**
 * @brief Receive a character from the specified UART channel.
 *
 * @param[in] uart_ch  UART channel number to send to (0-1)
 * @return A received byte
 */
uint8_t nos_uart_getc(uint8_t uart_ch);

/**
 * @brief Send a character to the specified UART channel.
 *
 * @param[in] uart_ch  UART channel number to send to (0-1)
 * @param[in] buf        string pointer to be sent
 * @param[in] data      length of string
 */
void nos_uart_dma_tx(uint8_t uart_ch, uint8_t* buf, uint16_t buflen);

/**
 * @brief Receive a character from the specified UART channel.
 *
 * @param[in] uart_ch  UART channel number to send to (0-1)
 * @param[in] buf        string pointer to be received
 * @param[in] data      length of string
 */
void nos_uart_dma_rx(uint8_t uart_ch, uint8_t* buf, uint16_t buflen);

/**
 * @brief Set a callback function to be called when a character is received.
 *
 * @param[in] uart_ch  UART channel number
 * @param[in] func        Callback function pointer
 */
int nos_uart_set_rx_callback(uint8_t uart_ch, void (*func)(uint8_t uart_ch, uint8_t data));


/**
 * @brief Enable an UART Rx interrupt.
 *
 * @param[in] uart_ch  UART channel number
 */
int nos_uart_enable_rx_intr(uint8_t uart_ch);

/**
 * @brief Disable an UART Rx interrupt.
 *
 * @param[in] uart_ch  UART channel number
 */
int nos_uart_disable_rx_intr(uint8_t uart_ch);

/**
 * @brief Check whether an UART Rx interrupt is enabled or not.
 *
 * @param[in] uart_ch  UART channel number
 * @return TRUE when the interrupt is enabled. Otherwise, FALSE.
 */
bool nos_uart_rx_intr_is_set(uint8_t uart_ch);






#endif // UART_M
#endif // __UART_H__
