/**
  ******************************************************************************
  * @file    PWR_STOP/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <nos.h>
#include "stm32f4xx_it.h"
#include "kconf.h"
#include "platform.h"

#ifdef KERNEL_M
#include "thread.h"
extern THREAD *highest_thread;
extern THREAD *current_thread;
#endif

volatile UINT32 global_os_counter;

extern DQUEUE tick_q;
extern UINT32 Tick_Period;
extern UINT32 Remain_Tick_Value;
extern UINT32 SysTick_Reload_OverFlow;

extern UINT32 __gcounter;

/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup PWR_STOP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay = 0;
__IO uint8_t UserButtonStatus= 0x00;
__IO uint32_t SysTick_CNT = 0;
__IO uint32_t TIM2_CNT = 0;
__IO uint32_t MeasureTimer_CNT = 0;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{
    /*
     * Get the appropriate stack pointer, depending on our mode,
     * and use it as the parameter to the C handler. This function
     * will never return
     */
    __asm("TST   LR, #4");
    __asm("ITE   EQ");
    __asm("MRSEQ R0, MSP");
    __asm("MRSNE R0, PSP");
    __asm("B HardFault_HandlerC");
}

void HardFault_HandlerC(uint32_t *stack_pointer)
{
    uint32_t stacked_r0;
    uint32_t stacked_r1;
    uint32_t stacked_r2;
    uint32_t stacked_r3;
    uint32_t stacked_r12;
    uint32_t stacked_lr;
    uint32_t stacked_pc;
    uint32_t stacked_psr;

    stacked_r0 = ((uint32_t) stack_pointer[0]);
    stacked_r1 = ((uint32_t) stack_pointer[1]);
    stacked_r2 = ((uint32_t) stack_pointer[2]);
    stacked_r3 = ((uint32_t) stack_pointer[3]);

    stacked_r12 = ((uint32_t) stack_pointer[4]);
    stacked_lr =  ((uint32_t) stack_pointer[5]);
    stacked_pc =  ((uint32_t) stack_pointer[6]);
    stacked_psr = ((uint32_t) stack_pointer[7]);

    printf("\n\r\n\r[HardFault]\n\r");
    printf("R0        = %08x\n\r", (unsigned int)stacked_r0);
    printf("R1        = %08x\n\r", (unsigned int)stacked_r1);
    printf("R2        = %08x\n\r", (unsigned int)stacked_r2);
    printf("R3        = %08x\n\r", (unsigned int)stacked_r3);
    printf("R12       = %08x\n\r", (unsigned int)stacked_r12);
    printf("LR [R14]  = %08x - Subroutine Call return address\n\r", (unsigned int)stacked_lr);
    printf("PC [R15]  = %08x - Program Counter\n\r", (unsigned int)stacked_pc);
    printf("PSR       = %08x\n\r", (unsigned int)stacked_psr);
    printf("BFAR      = %08x - Bus Fault SR/Address causing bus fault\n\r",
           (unsigned int) (*((volatile uint32_t *)(0xE000ED38))));
    printf("CFSR      = %08x - Config. Fault SR\n\r",
           (unsigned int) (*((volatile uint32_t *)(0xE000ED28))));
    if((*((volatile uint32_t *)(0xE000ED28)))&(1<<25))
    {
        printf("  :UsageFault->DivByZero\n\r");
    }
    if((*((volatile uint32_t *)(0xE000ED28)))&(1<<24))
    {
        printf("  :UsageFault->Unaligned access\n\r");
    }
    if((*((volatile uint32_t *)(0xE000ED28)))&(1<<18))
    {
        printf("  :UsageFault->Integrity check error\n\r");
    }
    if((*((volatile uint32_t *)(0xE000ED28)))&(1<<0))
    {
        printf("  :MemFault->Data access violation\n\r");
    }
    if((*((volatile uint32_t *)(0xE000ED28)))&(1<<0))
    {
        printf("  :MemFault->Instruction access violation\n\r");
    }
    printf("HFSR      = %08x - Hard Fault SR\n\r",
           (unsigned int)(*((volatile uint32_t *)(0xE000ED2C))));
    if((*((volatile uint32_t *)(0xE000ED2C)))&(1UL<<1))
    {
        printf("  :VECTBL, Failed vector fetch\n\r");
    }
    if((*((volatile uint32_t *)(0xE000ED2C)))&(1UL<<30))
    {
        printf("  :FORCED, Bus fault/Memory management fault/usage fault\n\r");
    }
    if((*((volatile uint32_t *)(0xE000ED2C)))&(1UL<<31))
    {
        printf("  :DEBUGEVT, Hard fault triggered by debug event\n\r");
    }
    printf("DFSR      = %08x - Debug Fault SR\n\r", (unsigned int)(*((volatile uint32_t *)(0xE000ED30))));
    printf("MMAR      = %08x - Memory Manage Address R\n\r", (unsigned int)(*((volatile uint32_t *)(0xE000ED34))));
    printf("AFSR      = %08x - Auxilirary Fault SR\n\r", (unsigned int)(*((volatile uint32_t *)(0xE000ED3C))));
    printf("SCB->SHCSR= %08x - System Handler Control and State R (exception)\n\r", (unsigned int)SCB->SHCSR);

#if 0
    while (1)
    {
        nos_led_on(0);
        nos_led_on(1);
        nos_led_on(2);
        nos_led_on(3);
        nos_delay_ms(500);
        nos_led_off(0);
        nos_led_off(1);
        nos_led_off(2);
        nos_led_off(3);
        nos_delay_ms(500);
    }
#endif
}


/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

#ifdef KERNEL_M
/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	 NOS_CTX_SW_PENDING_CLEAR();
   	 NOS_DISABLE_GLOBAL_INTERRUPT();
	 if (highest_thread != current_thread)
	 {
		THREAD *prev_thread = current_thread;
			
		current_thread = highest_thread;
   		os_switch_context(prev_thread, current_thread);   // may return here with global interrupt SET
	 }
    	 NOS_ENABLE_GLOBAL_INTERRUPT();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  * @phj
  */
void SysTick_Handler(void) {

	UINT32 SysTickCTRL;
	//UINT32 SysTickCTRL, j = 0;
	//DNODE *dnode = tick_q.head;
	TimingDelay--;

	if (SysTick_Reload_OverFlow == 0) {

		NOS_DISABLE_GLOBAL_INTERRUPT();
		
		global_os_counter++;

		if (sched_callback) {
			sched_callback();
		} // end if


		SysTick_CNT++;
		//__gcounter += 11;

		NOS_ENABLE_GLOBAL_INTERRUPT();

		NOS_CTX_SW_PENDING_SET();

	} else {

		SysTickCTRL = SysTick->CTRL;

		if (Tick_Period != 0) {
			if ((SysTickCTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0x10000) {
				Tick_Period--;
				SysTick->LOAD = MAX_SYSTICK_VALUE;
				SysTick->VAL = 0;
				SysTick_CNT++;
				//__gcounter += 99;

				NOS_CTX_SW_PENDING_SET();
			} // end if
		} else {
			if ((Tick_Period == 0) && ((SysTickCTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0x10000)) {
				if (Remain_Tick_Value != 0) {
					SysTick_Reload_OverFlow = 0;
					SysTick->LOAD = Remain_Tick_Value * TICK_UNIT;
					SysTick->VAL = 0;

					SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
					SysTick_CNT++;

					//FIXME
					//__gcounter += Remain_Tick_Value;

					NOS_CTX_SW_PENDING_SET();
				} else {
					SysTick_Reload_OverFlow = 0;
					SysTick->LOAD = (TICK_UNIT * 10); // 10msec
					SysTick->VAL = 0;
					
					SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

					NOS_DISABLE_GLOBAL_INTERRUPT();

					global_os_counter++;

					if (sched_callback) {
						sched_callback();
					} // end if

					//__gcounter += 11;

					NOS_ENABLE_GLOBAL_INTERRUPT();

					NOS_CTX_SW_PENDING_SET();
				} // end if
			} // end if
		} // end if
	} // end if
} // end func

void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update); // Clear the interrupt flag
    }
    MeasureTimer_CNT++;
}

#endif

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/******************************************************************************/
/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  *  @phj
  */
void RTC_WKUP_IRQHandler(void)
{
#ifdef PWM_M
  if (RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
    /* Clears the RTC's interrupt pending for WakeUp Timer  */
    RTC_ClearITPendingBit(RTC_IT_WUT);

    /* Clear the user push-button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line22);
  }
#endif
}

/**
  * @brief  This function handles External lines 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    /* Clear the user push-button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);

#if defined (BUTTON_M)
    nos_button_isr(BUTTON_USER);
#endif
  }
}
#ifdef KERNEL_M

void TIM2_IRQHandler(void) {
	TIM2_CNT++; // @phj.
#if 0
   if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
   {
   	 NOS_DISABLE_GLOBAL_INTERRUPT();
   	global_os_counter++;

		if (sched_callback)
		{
			sched_callback();
		}

		/* Clear TIM2 Update event interrupt pending bit*/
       TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	NOS_ENABLE_GLOBAL_INTERRUPT();
	}
	// pendsv_handler call.
	NOS_CTX_SW_PENDING_SET();
#endif
} // end func
#endif

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
