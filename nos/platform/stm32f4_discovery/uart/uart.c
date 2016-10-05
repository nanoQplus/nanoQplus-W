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


#include "kconf.h"
//#ifdef UART_M
#if 0
#include "stm32f4xx.h"
#include "uart.h"
#include "critical_section.h"
#include "platform.h"

#ifndef CONFIG_UART1_BAUDRATE
#define CONFIG_UART1_BAUDRATE 115200
#endif
#ifndef CONFIG_UART2_BAUDRATE
#define CONFIG_UART2_BAUDRATE 115200
#endif


void nos_uart_dma_init(DMA_Channel_TypeDef *channel)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* USART TX DMA Config */
    DMA_DeInit(channel);
    if (channel == UART1_TX_DMA_CH || channel == UART1_RX_DMA_CH)
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
    else /*if (channel == UART2_TX_DMA_CH || channel == UART2_RX_DMA_CH)*/
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));
    if (channel == UART1_TX_DMA_CH || channel == UART2_TX_DMA_CH)
    {
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    }
    else
    {
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    }
    DMA_InitStructure.DMA_BufferSize = 0;   //CNDTR
    DMA_InitStructure.DMA_MemoryBaseAddr = NULL;    //CMAR
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(channel, &DMA_InitStructure);
    DMA_ITConfig(channel, DMA_IT_TC, ENABLE);
    
}


#define CONCAT(a,b)     a##b
#define GPIO_Pin(n)     CONCAT(GPIO_Pin_, n)
void nos_uart_init(uint8_t uart_ch)
{
    USART_InitTypeDef usart_init;
    GPIO_InitTypeDef gpio_init;

    if (uart_ch == UART1_CH)
    {
        nos_uart_dma_init(UART1_TX_DMA_CH);
        nos_uart_dma_init(UART1_RX_DMA_CH);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

        if (USART1_TX_PORT == GPIOA && USART1_TX_PIN == 9 && USART1_RX_PORT == GPIOA && USART1_RX_PIN == 10)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        }
        else if (USART1_TX_PORT == GPIOB && USART1_TX_PIN == 6 && USART1_RX_PORT == GPIOB && USART1_RX_PIN == 7)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
        }

        /// Default USART1 pin configuration
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_init.GPIO_Pin = GPIO_Pin(USART1_TX_PIN);
        GPIO_Init(USART1_TX_PORT, &gpio_init);
        gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init.GPIO_Pin = GPIO_Pin(USART1_RX_PIN);
        GPIO_Init(USART1_RX_PORT, &gpio_init);
        
        usart_init.USART_WordLength = USART_WordLength_8b;
        usart_init.USART_StopBits = USART_StopBits_1;
        usart_init.USART_Parity = USART_Parity_No;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        usart_init.USART_BaudRate = CONFIG_UART1_BAUDRATE;
        USART_Init(USART1, &usart_init);
        USART_Cmd(USART1, ENABLE);    
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }

    else if (uart_ch == UART2_CH)
    {
        nos_uart_dma_init(UART2_TX_DMA_CH);
        nos_uart_dma_init(UART2_RX_DMA_CH);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

        if (USART2_TX_PORT == GPIOA && USART2_TX_PIN == 2 && USART2_RX_PORT == GPIOA && USART2_RX_PIN == 3)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        }
        else if (USART2_TX_PORT == GPIOD && USART2_TX_PIN == 5 && USART2_RX_PORT == GPIOD && USART2_RX_PIN == 6)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
        }

        /// Default USART2 pin configuration
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_init.GPIO_Pin = GPIO_Pin(USART2_TX_PIN);
        GPIO_Init(USART2_TX_PORT, &gpio_init);
        gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init.GPIO_Pin = GPIO_Pin(USART2_RX_PIN);
        GPIO_Init(USART2_RX_PORT, &gpio_init);
        
        usart_init.USART_WordLength = USART_WordLength_8b;
        usart_init.USART_StopBits = USART_StopBits_1;
        usart_init.USART_Parity = USART_Parity_No;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        usart_init.USART_BaudRate = CONFIG_UART2_BAUDRATE;
        USART_Init(USART2, &usart_init);
        USART_Cmd(USART2, ENABLE);    
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    }
}


void nos_uart_dma_tx(uint8_t uart_ch, uint8_t* buf, uint16_t buflen)
{
    //@haekim 0818 NOS_ENTER_CRITICAL_SECTION();
    if (uart_ch == UART1_CH)
    {
        DMA_Cmd(UART1_TX_DMA_CH, DISABLE);
        UART1_TX_DMA_CH->CMAR = (uint32_t)buf;
        DMA_SetCurrDataCounter(UART1_TX_DMA_CH, buflen);    //CNDTR config
        DMA_Cmd(UART1_TX_DMA_CH, ENABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
        while (DMA_GetFlagStatus(UART1_TX_DMA_FLAG_TC) == RESET);
        DMA_ClearFlag(UART1_TX_DMA_FLAG_TC);
        USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
    }
    else if (uart_ch == UART2_CH)
    {
        DMA_Cmd(UART2_TX_DMA_CH, DISABLE);
        UART2_TX_DMA_CH->CMAR = (uint32_t)buf;
        DMA_SetCurrDataCounter(UART2_TX_DMA_CH, buflen);
        DMA_Cmd(UART2_TX_DMA_CH, ENABLE);
        USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
        while (DMA_GetFlagStatus(UART2_TX_DMA_FLAG_TC) == RESET);
        DMA_ClearFlag(UART2_TX_DMA_FLAG_TC);
        USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);        
    }
    //@haekim 0818 NOS_EXIT_CRITICAL_SECTION();
}

void nos_uart_dma_rx(uint8_t uart_ch, uint8_t* buf, uint16_t buflen)
{
    //@haekim 0818 NOS_ENTER_CRITICAL_SECTION();
    if (uart_ch == UART1_CH)
    {
        DMA_Cmd(UART1_RX_DMA_CH, DISABLE);
        UART1_RX_DMA_CH->CMAR = (uint32_t)buf;
        DMA_SetCurrDataCounter(UART1_RX_DMA_CH, buflen);    //CNDTR config
        DMA_Cmd(UART1_RX_DMA_CH, ENABLE);
        USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
        while (DMA_GetFlagStatus(UART1_RX_DMA_FLAG_TC) == RESET);
        DMA_ClearFlag(UART1_RX_DMA_FLAG_TC);
        USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
    }
    else if (uart_ch == UART2_CH)
    {
        DMA_Cmd(UART2_RX_DMA_CH, DISABLE);
        UART2_RX_DMA_CH->CMAR = (uint32_t)buf;
        DMA_SetCurrDataCounter(UART2_RX_DMA_CH, buflen);
        DMA_Cmd(UART2_RX_DMA_CH, ENABLE);
        USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
        while (DMA_GetFlagStatus(UART2_RX_DMA_FLAG_TC) == RESET);
        DMA_ClearFlag(UART2_RX_DMA_FLAG_TC);
        USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);        
    }
    //@haekim 0818 NOS_EXIT_CRITICAL_SECTION();
}



// For Debugging. This is the fastest uart tx function.
void nos_uart_putc(uint8_t uart_ch, const uint8_t data)
{
    //@haekim 0805 NOS_ENTER_CRITICAL_SECTION();
    switch (uart_ch)
    {
        case UART1_CH:
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
            USART_SendData(USART1, data);
            break;
        case UART2_CH:
            while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
            USART_SendData(USART2, data);
            break;
        #if 0
        case UART3_CH:
            while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
            USART_SendData(USART3, data);
            break;
        case UART4_CH:
            while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
            USART_SendData(UART4, data);
            break;
        case UART5_CH:
            while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
            USART_SendData(UART5, data);
            break;
        #endif
        default:
            break;
    }
    //@haekim 0805 NOS_EXIT_CRITICAL_SECTION();
}

uint8_t nos_uart_getc(uint8_t uart_ch)
{
    //@haekim 0805 NOS_ENTER_CRITICAL_SECTION();
    switch (uart_ch)
    {
        case UART1_CH:
            while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
            //@haekim 0805 NOS_EXIT_CRITICAL_SECTION();
            return (uint8_t)USART_ReceiveData(USART1);
        case UART2_CH:
            while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
            //@haekim 0805 NOS_EXIT_CRITICAL_SECTION();
            return (uint8_t)USART_ReceiveData(USART2);
    #if 0
        case UART3_CH:
            while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
            return USART_ReceiveData(USART3);
        case UART4_CH:
            while (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == RESET);
            return USART_ReceiveData(UART4);
        case UART5_CH:
            while (USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == RESET);
            return USART_ReceiveData(UART5);
    #endif
        default:
            NOS_EXIT_CRITICAL_SECTION();
            return 0;
    }
}

void nos_uart_puts(uint8_t uart_ch, const uint8_t *buf)
{
    int i=0;
    while( (*(buf+i) != '\0') && (i < 255) )
    {
        nos_uart_putc(uart_ch, *(buf+i));
        ++i;
    }
}


#endif	// UART_M
