// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
/*
 * Copyright (c) 2013
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
 * STM32F4 Discovery platform
 *
 * @author Duk-Kyun Woo, Haeyong Kim (ETRI)
 * @date 2015. 10. 27.
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "kconf.h"
#include "nos_common.h"
#include "stm32f4xx.h"

/*ARCH(SYSTEM)*/
#define SYSCLK 168000000
#define HCLK   168000000
#define PCLK1   42000000 // HCLK/4, setting in SetSysClock()
#define PCLK2   84000000 // HCLK/2, setting in SetSysClock()
//#define ADCCLK 36000000


/*TIMER channel usage*/
#define WPAN_DEV0_TIMER 0   //ed scan, wpan_dev_transmit
#define WPAN_DEV1_TIMER 1
#define WPAN_MAC_TIMER  2   //for indirect data transmit, active scan
#define PING6_TIMER  3   //ping



/*UART*/
#define USART1_TX_PORT GPIOA 
#define USART1_TX_PIN  9
#define USART1_RX_PORT GPIOA
#define USART1_RX_PIN  10
#define UART1_TX_DMA_CH         DMA1_Channel4
#define UART1_TX_DMA_FLAG_TC    DMA1_FLAG_TC4
#define UART1_RX_DMA_CH         DMA1_Channel5
#define UART1_RX_DMA_FLAG_TC    DMA1_FLAG_TC5

#define USART2_TX_PORT GPIOA
#define USART2_TX_PIN  2
#define USART2_RX_PORT GPIOA
#define USART2_RX_PIN  3
#define UART2_TX_DMA_CH         DMA1_Channel7
#define UART2_TX_DMA_FLAG_TC    DMA1_FLAG_TC7
#define UART2_RX_DMA_CH         DMA1_Channel6
#define UART2_RX_DMA_FLAG_TC    DMA1_FLAG_TC6


/*LED*/
/*LEDs are connected to same GPIO Port.*/
#define LED_NUM         4
#define LED_GPIO_PORT	GPIOD

/*PORT, PIN*/
#define LED0_PORT       GPIOD
#define LED0_PIN        GPIO_Pin_12
#define LED1_PORT       GPIOD
#define LED1_PIN        GPIO_Pin_13
#define LED2_PORT       GPIOD
#define LED2_PIN        GPIO_Pin_14
#define LED3_PORT	GPIOD
#define LED3_PIN	GPIO_Pin_15

/*Button*/
#define BUTTON_NUM          1
enum
{
    BUTTON_USER     = 0
};
// User Button
#define BUTTON0_PORT        GPIOA
#define BUTTON0_PIN         GPIO_Pin_0
#define BUTTON0_NVIC_IRQN   EXTI0_IRQn
#define BUTTON0_PORTSRC     EXTI_PortSourceGPIOA
#define BUTTON0_PINSRC      EXTI_PinSource0
#define BUTTON0_EXTIPIN	    EXTI_Line0


/*Slot1*/
//SPI 
#define SPI1_PORT           GPIOA
#define SPI1_NCS_PIN        GPIO_Pin_4
#define SPI1_CLK_PIN        GPIO_Pin_5
#define SPI1_MISO_PIN       GPIO_Pin_6
#define SPI1_MOSI_PIN       GPIO_Pin_7
// for DMA library 
#define SPI1_TX_DMA_CH      DMA1_Channel3
#define SPI1_TX_DMA_FLAG_TC DMA1_FLAG_TC3
#define SPI1_RX_DMA_CH      DMA1_Channel2
#define SPI1_RX_DMA_FLAG_TC DMA1_FLAG_TC2

// for gpio library
#define SLOT1_GPIO_PORT     GPIOD
#define SLOT1_GPIO0_PIN     GPIO_Pin_0
#define SLOT1_GPIO1_PIN     GPIO_Pin_1
#define SLOT1_GPIO2_PIN     GPIO_Pin_2
#define SLOT1_GPIO3_PIN     GPIO_Pin_3
#define SLOT1_GPIO4_PIN     GPIO_Pin_4
#define SLOT1_GPIO5_PIN     GPIO_Pin_5
#define SLOT1_GPIO6_PIN     GPIO_Pin_6
#define SLOT1_GPIO7_PIN     GPIO_Pin_7
// (exti related)
#define SLOT1_GPIO_PORTSRC  GPIO_PortSourceGPIOD
#define SLOT1_GPIO0_PINSRC  GPIO_PinSource0
#define SLOT1_GPIO1_PINSRC  GPIO_PinSource1
#define SLOT1_GPIO2_PINSRC  GPIO_PinSource2
#define SLOT1_GPIO3_PINSRC  GPIO_PinSource3
#define SLOT1_GPIO4_PINSRC  GPIO_PinSource4
#define SLOT1_GPIO5_PINSRC  GPIO_PinSource5
#define SLOT1_GPIO6_PINSRC  GPIO_PinSource6
#define SLOT1_GPIO7_PINSRC  GPIO_PinSource7
//for exti library
#define SLOT1_GPIO0_EXTI_LINE  EXTI_Line0
#define SLOT1_GPIO1_EXTI_LINE  EXTI_Line1
#define SLOT1_GPIO2_EXTI_LINE  EXTI_Line2
#define SLOT1_GPIO3_EXTI_LINE  EXTI_Line3
#define SLOT1_GPIO4_EXTI_LINE  EXTI_Line4
#define SLOT1_GPIO5_EXTI_LINE  EXTI_Line5
#define SLOT1_GPIO6_EXTI_LINE  EXTI_Line6
#define SLOT1_GPIO7_EXTI_LINE  EXTI_Line7
//for nvic library
#define SLOT1_GPIO0_NVIC_IRQN  EXTI0_IRQn
#define SLOT1_GPIO1_NVIC_IRQN  EXTI1_IRQn
#define SLOT1_GPIO2_NVIC_IRQN  EXTI2_IRQn
#define SLOT1_GPIO3_NVIC_IRQN  EXTI3_IRQn
#define SLOT1_GPIO4_NVIC_IRQN  EXTI4_IRQn
#define SLOT1_GPIO5_NVIC_IRQN  EXTI9_5_IRQn
#define SLOT1_GPIO6_NVIC_IRQN  EXTI9_5_IRQn
#define SLOT1_GPIO7_NVIC_IRQN  EXTI9_5_IRQn


/*Slot2*/
//SPI 
#define SPI2_PORT           GPIOB
#define SPI2_NCS_PIN        GPIO_Pin_12
#define SPI2_CLK_PIN        GPIO_Pin_13
#define SPI2_MISO_PIN       GPIO_Pin_14
#define SPI2_MOSI_PIN       GPIO_Pin_15

// for DMA library 
#define SPI2_TX_DMA_CH      
#define SPI2_TX_DMA_FLAG_TC 
#define SPI2_RX_DMA_CH      
#define SPI2_RX_DMA_FLAG_TC 
// GPIO
#define SLOT2_GPIO_PORT     GPIOD
#define SLOT2_GPIO0_PIN     GPIO_Pin_8
#define SLOT2_GPIO1_PIN     GPIO_Pin_9
#define SLOT2_GPIO2_PIN     GPIO_Pin_10
#define SLOT2_GPIO3_PIN     GPIO_Pin_11
#define SLOT2_GPIO4_PIN     GPIO_Pin_12
#define SLOT2_GPIO5_PIN     GPIO_Pin_13
#define SLOT2_GPIO6_PIN     GPIO_Pin_14
#define SLOT2_GPIO7_PIN     GPIO_Pin_15
// for gpio libraries
#define SLOT2_GPIO_PORTSRC  GPIO_PortSourceGPIOD
#define SLOT2_GPIO0_PINSRC  GPIO_PinSource8
#define SLOT2_GPIO1_PINSRC  GPIO_PinSource9
#define SLOT2_GPIO2_PINSRC  GPIO_PinSource10
#define SLOT2_GPIO3_PINSRC  GPIO_PinSource11
#define SLOT2_GPIO4_PINSRC  GPIO_PinSource12
#define SLOT2_GPIO5_PINSRC  GPIO_PinSource13
#define SLOT2_GPIO6_PINSRC  GPIO_PinSource14
#define SLOT2_GPIO7_PINSRC  GPIO_PinSource15
//for exti library
#define SLOT2_GPIO0_EXTI_LINE  EXTI_Line8
#define SLOT2_GPIO1_EXTI_LINE  EXTI_Line9
#define SLOT2_GPIO2_EXTI_LINE  EXTI_Line10
#define SLOT2_GPIO3_EXTI_LINE  EXTI_Line11
#define SLOT2_GPIO4_EXTI_LINE  EXTI_Line12
#define SLOT2_GPIO5_EXTI_LINE  EXTI_Line13
#define SLOT2_GPIO6_EXTI_LINE  EXTI_Line14
#define SLOT2_GPIO7_EXTI_LINE  EXTI_Line15
//for nvic library
#define SLOT2_GPIO0_NVIC_IRQN  EXTI9_5_IRQn
#define SLOT2_GPIO1_NVIC_IRQN  EXTI9_5_IRQn
#define SLOT2_GPIO2_NVIC_IRQN  EXTI15_10_IRQn
#define SLOT2_GPIO3_NVIC_IRQN  EXTI15_10_IRQn
#define SLOT2_GPIO4_NVIC_IRQN  EXTI15_10_IRQn
#define SLOT2_GPIO5_NVIC_IRQN  EXTI15_10_IRQn
#define SLOT2_GPIO6_NVIC_IRQN  EXTI15_10_IRQn
#define SLOT2_GPIO7_NVIC_IRQN  EXTI15_10_IRQn

#if 0

/*Network Slot*/
// ehternet devices
#ifndef SLOT1_ETHERNET_DEV
#define SLOT1_ETHERNET_DEV  0
#define ETH_DEV_CNT 1
#endif
#ifndef SLOT2_ETHERNET_DEV
#define SLOT2_ETHERNET_DEV  0
#define ETH_DEV_CNT 1
#endif
//#define ETH_DEV_NUM (SLOT1_ETHERNET_DEV+SLOT2_ETHERNET_DEV)
//#define ETH_DEV_CNT ETH_DEV_NUM

// wpan devices
#ifdef SLOT2_WPAN_DEV
#define WPAN_DEV_NUM    2
#elif defined SLOT1_WPAN_DEV
#define WPAN_DEV_NUM    1
#else
#define WPAN_DEV_NUM    0
#endif


#if defined SLOT1_CC2520
#define WPAN_DEV0_ID    WPAN_DEV_CC2520_ID
#elif defined SLOT1_CC1120
#define WPAN_DEV0_ID    WPAN_DEV_CC1120_ID
#elif defined SLOT1_AT86RF212
#define WPAN_DEV0_ID    WPAN_DEV_AT86RF212_ID
#else
#define WPAN_DEV0_ID    WPAN_DEV_NONE
#endif
#if defined SLOT2_CC2520
#define WPAN_DEV1_ID    WPAN_DEV_CC2520_ID
#elif defined SLOT2_CC1120
#define WPAN_DEV1_ID    WPAN_DEV_CC1120_ID
#elif defined SLOT2_AT86RF212
#define WPAN_DEV1_ID    WPAN_DEV_AT86RF212_ID
#else
#define WPAN_DEV1_ID    WPAN_DEV_NONE
#endif



/*Character LCD*/
#define RS_PIN      GPIOE, GPIO_Pin_0
#define NRW_PIN     GPIOE, GPIO_Pin_1
#define ENABLE_PIN  GPIOE, GPIO_Pin_2
#define D0_PIN      GPIOE, GPIO_Pin_3
#define D1_PIN      GPIOE, GPIO_Pin_4
#define D2_PIN      GPIOE, GPIO_Pin_5
#define D3_PIN      GPIOE, GPIO_Pin_6
#define D4_PIN      GPIOE, GPIO_Pin_7
#define D5_PIN      GPIOE, GPIO_Pin_8
#define D6_PIN      GPIOE, GPIO_Pin_9
#define D7_PIN      GPIOE, GPIO_Pin_10

#endif

#ifdef UART_M
#include "uart.h"
#endif
#ifdef LED_M
#include "led.h"
#endif
#ifdef BUTTON_M
#include "button.h"
#endif
#ifdef LMC1623_M
#include "lmc1623.h"
#endif
#ifdef WPAN_DEV_M
#include "wpan_dev_hal.h"
#endif
#ifdef NAND_M
#include "fsmc_nand.h"
#endif


void nos_platform_init(void);
/**
 * Delay @p us microseconds.
 */
void nos_delay_us(uint32_t us);

/**
 * Delay @p ms milliseconds.
 */
void nos_delay_ms(uint32_t ms);

#endif //PLATFORM_H_
