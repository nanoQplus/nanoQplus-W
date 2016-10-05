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
 * Buttons
 *
 * @author Duk-Kyun Woo (ETRI)
 * @date 2015. 10. 27.
 */

/**
 * @brief Buttons 
 * @date 2013. 5. 13.
 * @author Haeyong Kim (ETRI)
 */

#include "kconf.h"
#ifdef BUTTON_M
#include "button.h"
#include "stm32f4xx_gpio.h"
#include "platform.h"

void (*nos_button_callback[BUTTON_NUM])(void*) = {NULL};
uint8_t nos_button_callback_arg[BUTTON_NUM];


#ifdef KERNEL_M
#include "taskq.h"
#include "critical_section.h"
#endif


void nos_button_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    // Enable GPIOA clock
    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Configure gpio as input : Button 0
    GPIO_InitStructure.GPIO_Pin = BUTTON0_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure the Interrupt and NVIC*/

    SYSCFG_EXTILineConfig(BUTTON0_PORTSRC, BUTTON0_PINSRC);
    //GPIO_EXTILineConfig(BUTTON0_PORTSRC, BUTTON0_PINSRC);   //Selects the GPIO pin used as EXTI Line.
    EXTI_InitStructure.EXTI_Line    = BUTTON0_EXTIPIN;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}


void nos_button_set_callback(uint8_t id, void (*func)(void*))
{
    nos_button_callback[id] = func;
}

void nos_button_isr(uint8_t id)
{
    if (nos_button_callback[id] != NULL)
    {
        #ifdef KERNEL_M
        nos_button_callback_arg[id] = id;
        taskq_register(nos_button_callback[id], (void*)&(nos_button_callback_arg[id]));
        #else
        (nos_button_callback[id])((void*)&id);
        #endif
    }
}


#endif //BUTTON_M
