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
 * LEDs
 *
 * @author Duk-Kyun Woo (ETRI)
 * @date 2015. 10. 27.
 */

/**
 * LEDs
 *
 * @author Jongsoo Jeong (ETRI)
 * @date 2013. 12. 4.
 */

/**
 * @brief LED 
 * @date 2013. 5. 13.
 * @author Haeyong Kim (ETRI)
 */

#include "led.h"
#ifdef LED_M
#include <stdio.h>
#include "platform.h"

void nos_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIO clock (depends on LED ports)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* Configure gpio as output */
	GPIO_InitStructure.GPIO_Pin = LED0_PIN | LED1_PIN | LED2_PIN | LED3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    //gpio_D->MODER = gpio_D->MODER|0x55000000;
	
    nos_led_off(0);
    nos_led_off(1);
    nos_led_off(2);
    nos_led_off(3);
}


void nos_led_off(uint8_t n)
{
    if (n == 0) 
        GPIO_ResetBits(LED0_PORT, LED0_PIN);
    else if (n == 1)
        GPIO_ResetBits(LED1_PORT, LED1_PIN);
    else if (n == 2)
        GPIO_ResetBits(LED2_PORT, LED2_PIN);
    else if (n == 3)
        GPIO_ResetBits(LED3_PORT, LED3_PIN);
}

void nos_led_on(uint8_t n)
{
    if (n == 0) 
        GPIO_SetBits(LED0_PORT, LED0_PIN);
    else if (n == 1)
        GPIO_SetBits(LED1_PORT, LED1_PIN);
    else if (n == 2)
        GPIO_SetBits(LED2_PORT, LED2_PIN);
    else if (n == 3)
        GPIO_SetBits(LED3_PORT, LED3_PIN);
}

void nos_led_toggle(uint8_t n)
{
    if (n == 0)
        GPIO_ToggleBits(LED0_PORT, LED0_PIN);

    else if (n == 1)
        GPIO_ToggleBits(LED1_PORT, LED1_PIN);

    else if (n == 2)
        GPIO_ToggleBits(LED2_PORT, LED2_PIN);

    else if (n == 3)
        GPIO_ToggleBits(LED3_PORT, LED3_PIN);
}

bool nos_led_get_status(uint8_t n)
{
    if (n == 0) 
        return (bool)GPIO_ReadOutputDataBit(LED0_PORT, LED0_PIN);
    else if (n == 1)
        return (bool)GPIO_ReadOutputDataBit(LED1_PORT, LED1_PIN);
    else if (n == 2)
        return (bool)GPIO_ReadOutputDataBit(LED2_PORT, LED2_PIN);
    else  if(n == 3)
        return (bool)GPIO_ReadOutputDataBit(LED3_PORT, LED3_PIN);
    else return false;
}

void nos_led_num_display(uint8_t n)
{
    if (_IS_SET(n, 0)) nos_led_on(0);
    else nos_led_off(0);

    if (_IS_SET(n, 1)) nos_led_on(1);
    else nos_led_off(1);

    if (_IS_SET(n, 2)) nos_led_on(2);
    else nos_led_off(2);

    if (_IS_SET(n, 3)) nos_led_on(3);
    else nos_led_off(3);
}

void nos_led_alarm(uint8_t n)
{
    int i;
    
    for (i = 0; i < n; i++)
    {
        nos_led_on(0);
        nos_led_off(1);
        nos_led_off(2);
        nos_led_off(3);
        nos_delay_ms(250);

        nos_led_on(1);
        nos_led_off(0);
        nos_led_off(2);
        nos_led_off(3);
        nos_delay_ms(250);

        nos_led_on(2);
        nos_led_off(1);
        nos_led_off(0);
        nos_led_off(3);
        nos_delay_ms(250);

        nos_led_on(3);
        nos_led_off(0);
        nos_led_off(1);
        nos_led_off(2);
        nos_delay_ms(250);
    }
}

#ifdef UART_M
void nos_led_print_status(void)
{
    for(int i=0; i<LED_NUM; ++i) {
        if(nos_led_get_status(i))
            printf("LED(%d) is on.\n\r", i);
        else
            printf("LED(%d) is off.\n\r", i);
    }
}
#endif

#endif //LED_M

