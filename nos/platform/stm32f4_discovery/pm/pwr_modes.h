/**
  ******************************************************************************
  * @file    STM32F4xx_Current_Consumption_Measuring/inc/test_config.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-January-2014
  * @brief   This file contains the headers of test configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_pwr_modes_H
#define __STM32F4xx_pwr_modes_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
	 
/*************************** ART accelerator configuration*********************/	
	 
/* Uncomment the macros to enable ART with or without prefetch buffer */
	 
/*Enable ART*/ 
  #define ART_Enable
	 
/*Enbale prefetch when ART is enabled */	 
  #define  Prefetch_Enable

/******************************************************************************/
	 
/********************* Power supply selection *********************************/

/* Uncomment the macros to select 3,3 V or 1,8 V as power supply */	 

/* Select Vdd = 3,3 V */
 #define VDD3_3		

/* Select Vdd = 1,8 V */
 /* #define VDD1_8 */
	 
/******************************************************************************/	 


/************************* Power sub-modes selection **************************/

/* Uncomment macro(s) to select sub-mode(s) to be measured for each power mode*/
	
/*------------------------- RUN sub-modes selection -------------------------*/
#ifdef RUN_MODE		   
   
  #if !defined (PWR_RunPeriphDisabled180Mhz) && !defined (PWR_RunPeriphEnabled180Mhz) \
  && !defined (PWR_RunPeriphDisabled168Mhz) && !defined (PWR_RunPeriphDisabled160Mhz) \
  && !defined (PWR_RunPeriphDisabled144Mhz) && !defined (PWR_RunPeriphDisabled1120Mhz) \
  && !defined (PWR_RunPeriphEnabled168Mhz) && !defined (PWR_RunPeriphEnabled160Mhz) \
  && !defined (PWR_RunPeriphEnabled144Mhz ) && !defined (PWR_RunPeriphEnabled120Mhz)	 
	 
    #if defined (STM32F429_439xx)
      #if !defined (VDD1_8)
        #define RunPeriphDisabled180Mhz
        #define RunPeriphEnabled180Mhz
      #endif /* VDD1_8 */

      #define RunPeriphDisabled168Mhz
      #define RunPeriphEnabled168Mhz	
				
      #define RunPeriphDisabled144Mhz
      #define RunPeriphEnabled144Mhz	 

      #define RunPeriphDisabled120Mhz	
      #define RunPeriphEnabled120Mhz	
    #endif /* STM32F429_439xx */

    #if defined (STM32F40_41xxx)
      #if defined (VDD1_8)
        #define RunPeriphDisabled160Mhz
        #define RunPeriphEnabled160Mhz
    
      #elif defined (VDD3_3) /* not VDD1_8 */
        #define RunPeriphDisabled168Mhz
        #define RunPeriphEnabled168Mhz	
    #endif /* VDD1_8 */
    
      #define RunPeriphDisabled144Mhz
      #define RunPeriphEnabled144Mhz	 
 
      #define RunPeriphDisabled120Mhz	
      #define RunPeriphEnabled120Mhz	
    #endif /* STM32F40_41xxx */
				 
    #if defined (STM32F401xx)
      #define RunPeriphDisabled84Mhz
      #define RunPeriphEnabled84Mhz
    
      #define RunPeriphDisabled60Mhz
      #define RunPeriphEnabled60Mhz
    #endif /* STM32F401xx */	
  #endif				
		
  #if defined (RunPeriphDisabled180Mhz) && defined (VDD1_8) || \
      defined (RunPeriphEnabled180Mhz) && defined (VDD1_8)
   #error "This mode is not available at 1,8 V ,you can reach only 168 MHz"
  #endif

  #if defined (RunPeriphDisabled180Mhz) && defined (STM32F401xx) || \
      defined (RunPeriphEnabled180Mhz) && defined (STM32F40_41xxx)
   #error "This mode is available only for STM32F429_439xx device"
  #endif

  #if defined (RunPeriphDisabled168Mhz) && defined (STM32F40_41xxx) && defined (VDD1_8) ||\
      defined (RunPeriphEnabled168Mhz) && defined (STM32F40_41xxx) && defined (VDD1_8)
    #error "This mode is not available at 1,8 V for STM32F40_41xxx devices, you can reach only 160 MHz "
  #endif
#endif /* RUN_MODE */
      
/*----------------------- Sleep sub-modes selection  ------------------------*/
#ifdef SLEEP_MODE
  #if !defined (PWR_SleepPeriphDisabled180Mhz) && !defined (PWR_SleepPeriphEnabled180Mhz) \
  && !defined (PWR_SleepPeriphDisabled168Mhz) && !defined (PWR_SleepPeriphDisabled160Mhz) \
  && !defined (PWR_SleepPeriphDisabled144Mhz) && !defined (PWR_SleepPeriphDisabled1120Mhz) \
  && !defined (PWR_SleepPeriphEnabled168Mhz) && !defined (PWR_SleepPeriphEnabled160Mhz) \
  && !defined (PWR_SleepPeriphEnabled144Mhz ) && !defined (PWR_SleepPeriphEnabled120Mhz)

    #if defined (STM32F429_439xx)
      #if !defined (VDD1_8)
        #define SleepPeriphDisabled180Mhz
        #define SleepPeriphEnabled180Mhz
      #endif /* VDD1_8 */
				 
      #define SleepPeriphDisabled168Mhz
      #define SleepPeriphEnabled168Mhz	
				 
      #define SleepPeriphDisabled144Mhz
      #define SleepPeriphEnabled144Mhz	 

      #define SleepPeriphDisabled120Mhz	
      #define SleepPeriphEnabled120Mhz		
    #endif /* STM32F429_439xx */

    #if defined (STM32F40_41xxx)
      #if defined (VDD1_8)
        #define SleepPeriphDisabled160Mhz
        #define SleepPeriphEnabled160Mhz	
    
      #elif defined (VDD3_3) /* not VDD1_8 */
        #define SleepPeriphDisabled168Mhz
        #define SleepPeriphEnabled168Mhz	
      #endif /* VDD1_8 */		
    
      #define SleepPeriphDisabled144Mhz
      #define SleepPeriphEnabled144Mhz	 

      #define SleepPeriphDisabled120Mhz	
      #define SleepPeriphEnabled120Mhz			
    #endif /* STM32F40_41xxx */

    #if defined (STM32F401xx)
      #define SleepPeriphDisabled84Mhz
      #define SleepPeriphEnabled84Mhz

      #define SleepPeriphDisabled60Mhz
      #define SleepPeriphEnabled60Mhz
    #endif /* STM32F401xx */
  #endif

  #if defined (SleepPeriphDisabled180Mhz) && defined (VDD1_8) || \
      defined (SleepPeriphEnabled180Mhz) && defined (VDD1_8)
  #error "This mode is not available at 1,8 V ,you can reach only 168 MHz"
  #endif

  #if defined (SleepPeriphDisabled180Mhz) && defined (STM32F401xx) || \
      defined (RSleepPeriphEnabled180Mhz) && defined (STM32F40_41xxx)
  #error "This mode is available only for STM32F429_439xx device"
  #endif

  #if defined (SleepPeriphDisabled168Mhz) && defined (STM32F40_41xxx) && defined (VDD1_8) || \
      defined (SleepPeriphEnabled168Mhz) && defined (STM32F40_41xxx) && defined (VDD1_8)
    #error "This mode is not available at 1,8 V for STM32F40_41xxx devices, you can reach only 160 MHz"
  #endif
#endif /* SLEEP_MODE */

/*----------------------- STOP sub-modes selection ---------------------------*/
#ifdef STOP_MODE
  #if !defined (StopMainRegFlashStop) && !defined (StopMainRegFlashPwrDown) \
  && !defined (StopMainRegUnderDriveFlashPwrDown) && !defined (StopLowPwrRegFlashStop) \
  && !defined (StopLowPwrRegFlashPwrDown) && \
  !defined (StopLowPwrRegUnderDriveFlashPwrDown)

    #define StopMainRegFlashStop
    #define StopMainRegFlashPwrDown
    #define StopLowPwrRegFlashStop
    #define StopLowPwrRegFlashPwrDown		
    #if defined (STM32F429_439xx)
      #define StopMainRegUnderDriveFlashPwrDown
      #define StopLowPwrRegUnderDriveFlashPwrDown
    #endif /* STM32F429_439xx */

  #endif

  #if !defined (STM32F429_439xx) && defined (StopMainRegUnderDriveFlashPwrDown) || \
      !defined (STM32F429_439xx) && defined (StopLowPwrRegUnderDriveFlashPwrDown)
     #error "This mode is available only for STM32F429_439xx device"
  #endif
	 
#endif /* STOP_MODE */
      
/*--------------------- Standby sub-mode selection ---------------------------*/
#ifdef STANDBY_MODE
      
  #if !defined (StandbyBkpSramOnRtcON) && !defined (StandbyBkpSramOnRtcOff)  \
  && !defined (StandbyBkpSramOffRtcOn) && !defined (StandbyBkpSramOffRtcOff)
 
  /* "You can only choose one Standby mode to be executed" */
    #if !defined (STM32F401xx)
   #define StandbyBkpSramOnRtcON
    #endif /* STM32F401xx) */

#define StandbyBkpSramOffRtcOn

      #if !defined (STM32F401xx)
#define StandbyBkpSramOnRtcOff
      #endif /* STM32F401xx) */

#define StandbyBkpSramOffRtcOff

  #endif

  #if defined (STM32F401xx) && defined (StandbyBkpSramOnRtcOff)
    #error "This mode is not available in STM32F401xx device , you should choose a sub-mode without BkpSram" 
  #endif 

   #if defined (STM32F401xx) && defined (StandbyBkpSramOnRtcON)
     #error "This mode is not available in STM32F401xx device , you should choose a sub-mode without BkpSram" 
   #endif 

#endif /* STANDBY_MODE */
    
/* Exported macro ------------------------------------------------------------*/
/* function prototype */

 /****************************** RUN mode  ************************************/
	 
/* Run mode, 180 MHz O.Drive_scale1-ALL_Periph_Disabled */
void PWR_RunPeriphDisabled180Mhz(void); 

/* Run mode, 180 MHz O.Drive_scale1-ALL_Periph_Enabled */
void PWR_RunPeriphEnabled180Mhz(void);	

/* Run mode, 168 MHz_scale1-ALL_Periph_Disabled */
void PWR_RunPeriphDisabled168Mhz(void);	 

 /* Run mode, 168 MHz_scale1-ALL_Periph_Enabled */
void PWR_RunPeriphEnabled168Mhz(void);	

/* Run mode, 160 MHz _scale1-ALL_Periph_Disabled */
void PWR_RunPeriphDisabled160Mhz(void);

/* Run mode, 160 MHz_scale1-ALL_Periph_Enabled */
void PWR_RunPeriphEnabled160Mhz(void);	 

/* Run mode, 144 MHz_scale2-ALL_Periph_Disabled */
void PWR_RunPeriphDisabled144Mhz(void);	 

 /* Run mode, 144 MHz_scale2-ALL_Periph_Enabled */
void PWR_RunPeriphEnabled144Mhz(void);	

/* Run mode, 120 MHz_scale3-ALL_Periph_disabled */
void PWR_RunPeriphDisabled120Mhz(void);

/* Run mode, 120 MHz_scale3-ALL_Periph_Enabled */
void PWR_RunPeriphEnabled120Mhz(void);	 

/* Run mode, 84 MHz_ALL_Periph_Disabled */
void PWR_RunPeriphDisabled84Mhz(void);

/* Run mode, 84 MHz_ALL_Periph_Enabled */
void PWR_RunPeriphEnabled84Mhz(void);	 

/* Run mode, 60 MHz_ALL_Periph_Disabled */
void PWR_RunPeriphDisabled60Mhz(void);	 

/* Run mode, 60 MHz_ALL_Periph_Enabled */
void PWR_RunPeriphEnabled60Mhz(void);	

 /**************************** SLEEP mode  ************************************/	

 /* Sleep mode, 180 MHz O.Drive_scale1-ALL_Periph_disabled */
void PWR_SleepPeriphDisabled180Mhz(void);

/* Sleep mode, 180 MHz O.Drive_scale1-ALL_Periph_enabled */
void PWR_SleepPeriphEnabled180Mhz(void);  

/* Sleep mode, 168 MHz_scale1-ALL_Periph_disabled */
void PWR_SleepPeriphDisabled168Mhz(void); 

/* Sleep mode, 168 MHz_scale1-ALL_Periph_enabled */
void PWR_SleepPeriphEnabled168Mhz(void);  

/* Sleep mode, 160 MHz_scale1-ALL_Periph_Disabled */
void PWR_SleepPeriphDisabled160Mhz(void); 

/* Sleep mode, 160 MHz_scale1-ALL_Periph_Enabled */
void PWR_SleepPeriphEnabled160Mhz(void);  

 /* Sleep mode, 144 MHz_scale2-ALL_Periph_disabled */
void PWR_SleepPeriphDisabled144Mhz(void);

/* Sleep mode, 144 MHz_scale2-ALL_Periph_enabled */
void PWR_SleepPeriphEnabled144Mhz(void);  

/* Sleep mode, 120 MHz_scale3 - ALL_Periph_disabled */
void PWR_SleepPeriphDisabled120Mhz(void); 

/* Sleep mode, 120 MHz_scale3 - ALL_Periph_Enabled */
void PWR_SleepPeriphEnabled120Mhz(void);  

/* Sleep mode, 84 MHz_ALL_Periph_Disabled */
void PWR_SleepPeriphDisabled84Mhz(void);  

/* Sleep mode, 84 MHz_ALL_Periph_Enabled */
void PWR_SleepPeriphEnabled84Mhz(void);	  

/* Sleep mode, 60 MHz_ALL_Periph_Disabled */
void PWR_SleepPeriphDisabled60Mhz(void);  

/* Sleep mode, 60 MHz_ALL_Periph_Enabled */
void PWR_SleepPeriphEnabled60Mhz(void); 

/***************************** STOP mode  *************************************/

/* Stop mode, Main Regulator with Flash Stop */
void PWR_StopMainRegFlashStop (void);

/* Stop mode, Main Regulator with Flash Power down */
void PWR_StopMainRegFlashPwrDown (void);

/* Stop mode, Low power Regulator with Flash Stop */
void PWR_StopLowPwrRegFlashStop (void);  

/* Stop mode, Low power Regulator with Flash Power down */
void PWR_StopLowPwrRegFlashPwrDown (void); 

/* Stop mode, Main Under-drive Regulator with Flash Power down */
void PWR_StopMainRegUnderDriveFlashPwrDown (void); 

/* Stop mode, Low power Under-drive Regulator with Flash Power down */
void PWR_StopLowPwrRegUnderDriveFlashPwrDown (void); 

/**************************** STANDBY mode  ***********************************/

/* Standby mode, Backup SRAM ON, RTC ON */
void PWR_StandbyBkpSramOnRtcON (void);

/* Standby mode, Backup SRAM ON, RTC OFF */
void PWR_StandbyBkpSramOnRtcOff (void);

/* Standby mode, Backup SRAM OFF, RTC ON */
void PWR_StandbyBkpSramOffRtcOn (void);

/* Standby mode, Backup SRAM OFF, RTC OFF */
void PWR_StandbyBkpSramOffRtcOff (void);

/*defines---------------------------------------------------------------------*/ 

#if defined (STM32F429_439xx)  /* Define STM32F429_439xx peripherals  */

#define ALL_GPIOs         (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC | \
                           RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | \
                           RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH |RCC_AHB1Periph_GPIOI | \
                           RCC_AHB1Periph_GPIOJ |RCC_AHB1Periph_GPIOK)

#define ALL_AHB1_PERIPH    (RCC_AHB1Periph_CRC| RCC_AHB1Periph_BKPSRAM| RCC_AHB1Periph_CCMDATARAMEN| \
                           RCC_AHB1Periph_DMA1| RCC_AHB1Periph_DMA2| RCC_AHB1Periph_ETH_MAC | \
                           RCC_AHB1Periph_ETH_MAC_Tx| RCC_AHB1Periph_ETH_MAC_Rx | \
                           RCC_AHB1Periph_ETH_MAC_PTP| RCC_AHB1Periph_OTG_HS | \
                           RCC_AHB1Periph_OTG_HS_ULPI| RCC_AHB1Periph_DMA2D | \
                           RCC_AHB1Periph_SRAM1| RCC_AHB1Periph_SRAM2| RCC_AHB1Periph_SRAM3 | \
                           RCC_AHB1Periph_FLITF)

#define ALL_AHB2_PERIPH    (RCC_AHB2Periph_DCMI| RCC_AHB2Periph_CRYP| RCC_AHB2Periph_HASH | \
                           RCC_AHB2Periph_RNG| RCC_AHB2Periph_OTG_FS)

#define ALL_AHB3_PERIPH	   RCC_AHB3Periph_FMC

#define ALL_APB1_PERIPH    (RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM3 |RCC_APB1Periph_TIM4 | \
                           RCC_APB1Periph_TIM5|RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7 | \
                           RCC_APB1Periph_TIM12|RCC_APB1Periph_TIM13| RCC_APB1Periph_TIM14 | \
                           RCC_APB1Periph_WWDG| RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3 | \
                           RCC_APB1Periph_USART2| RCC_APB1Periph_USART3 |RCC_APB1Periph_UART4	| \
                           RCC_APB1Periph_UART5| RCC_APB1Periph_I2C1 | RCC_APB1Periph_I2C2 | \
                           RCC_APB1Periph_I2C3| RCC_APB1Periph_CAN1 |RCC_APB1Periph_CAN2 | \
                           RCC_APB1Periph_PWR|RCC_APB1Periph_DAC | RCC_APB1Periph_UART7 | \
                           RCC_APB1Periph_UART8)                                             
                          
#define ALL_APB2_PERIPH    (RCC_APB2Periph_TIM1| RCC_APB2Periph_TIM8| RCC_APB2Periph_USART1 | \
                           RCC_APB2Periph_USART6| RCC_APB2Periph_ADC1| RCC_APB2Periph_ADC2 | \
                           RCC_APB2Periph_ADC3|RCC_APB2Periph_SDIO|RCC_APB2Periph_SPI1 | \
                           RCC_APB2Periph_SPI4|RCC_APB2Periph_SYSCFG|RCC_APB2Periph_TIM9 | \
                           RCC_APB2Periph_TIM10| RCC_APB2Periph_TIM11| RCC_APB2Periph_SPI5 | \
                           RCC_APB2Periph_SPI6| RCC_APB2Periph_LTDC| RCC_APB2Periph_SAI1)                       

#elif defined (STM32F40_41xxx) /* not STM32F429_439xx */	
/* Define STM32F40_41xxx peripherals  */

#define ALL_GPIOs           (RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOC | \
                            RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE| RCC_AHB1Periph_GPIOF | \
                            RCC_AHB1Periph_GPIOG| RCC_AHB1Periph_GPIOH| RCC_AHB1Periph_GPIOI)

#define ALL_AHB1_PERIPH     (RCC_AHB1Periph_CRC| RCC_AHB1Periph_BKPSRAM | \
                            RCC_AHB1Periph_CCMDATARAMEN| RCC_AHB1Periph_DMA1 | \
                            RCC_AHB1Periph_DMA2| RCC_AHB1Periph_ETH_MAC | \
                            RCC_AHB1Periph_ETH_MAC_Tx| RCC_AHB1Periph_ETH_MAC_Rx | \
                            RCC_AHB1Periph_ETH_MAC_PTP| RCC_AHB1Periph_OTG_HS | \
                            RCC_AHB1Periph_OTG_HS_ULPI |RCC_AHB1Periph_DMA2D | \
                            RCC_AHB1Periph_SRAM1| RCC_AHB1Periph_SRAM2 | \
                            RCC_AHB1Periph_SRAM3| RCC_AHB1Periph_FLITF)
                         

#define ALL_AHB2_PERIPH     RCC_AHB2Periph_OTG_FS

#define ALL_AHB3_PERIPH	    RCC_AHB3Periph_FSMC

#define ALL_APB1_PERIPH     (RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM3| RCC_APB1Periph_TIM4 | \
                            RCC_APB1Periph_TIM5| RCC_APB1Periph_TIM6| RCC_APB1Periph_TIM7 | \
                            RCC_APB1Periph_TIM12| RCC_APB1Periph_TIM13| RCC_APB1Periph_TIM14 | \
                            RCC_APB1Periph_WWDG| RCC_APB1Periph_SPI2| RCC_APB1Periph_SPI3 | \
                            RCC_APB1Periph_USART2| RCC_APB1Periph_USART3| RCC_APB1Periph_UART4 | \
                            RCC_APB1Periph_UART5| RCC_APB1Periph_I2C1| RCC_APB1Periph_I2C2 | \
                            RCC_APB1Periph_I2C3| RCC_APB1Periph_CAN1| RCC_APB1Periph_CAN2 | \
                            RCC_APB1Periph_PWR| RCC_APB1Periph_DAC )                                             
                          
#define ALL_APB2_PERIPH     (RCC_APB2Periph_TIM1| RCC_APB2Periph_TIM8| RCC_APB2Periph_USART1 | \
                            RCC_APB2Periph_USART6| RCC_APB2Periph_ADC1| RCC_APB2Periph_ADC2 | \
                            RCC_APB2Periph_ADC3| RCC_APB2Periph_SDIO| RCC_APB2Periph_SPI1 | \
                            RCC_APB2Periph_SYSCFG| RCC_APB2Periph_TIM9| RCC_APB2Periph_TIM10| \
                            RCC_APB2Periph_TIM11) 

#elif defined (STM32F401xx)  /* not STM32F40_41xxx*/	
/* Define STM32F401xx peripherals  */

#define ALL_GPIOs           (RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOC | \
                            RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE| RCC_AHB1Periph_GPIOH )
							  

#define ALL_AHB1_PERIPH     (RCC_AHB1Periph_CRC| RCC_AHB1Periph_CCMDATARAMEN|\
                            RCC_AHB1Periph_DMA1| RCC_AHB1Periph_DMA2)
                              
							
#define ALL_AHB2_PERIPH     (RCC_AHB2Periph_OTG_FS)
															

#define ALL_APB1_PERIPH     (RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM3| RCC_APB1Periph_TIM4 | \
                            RCC_APB1Periph_TIM5| RCC_APB1Periph_WWDG| RCC_APB1Periph_SPI2 | \
                            RCC_APB1Periph_SPI3| RCC_APB1Periph_PWR| RCC_APB1Periph_USART2 | \
                            RCC_APB1Periph_I2C1 | RCC_APB1Periph_I2C2|RCC_APB1Periph_I2C3 ) 
                       		                                              
                          
#define ALL_APB2_PERIPH     (RCC_APB2Periph_TIM1|RCC_APB2Periph_USART1| RCC_APB2Periph_USART6 | \
                            RCC_APB2Periph_ADC1| RCC_APB2Periph_SDIO | RCC_APB2Periph_SPI1 | \
                            RCC_APB2Periph_SPI4| RCC_APB2Periph_SYSCFG |RCC_APB2Periph_TIM9 | \
                            RCC_APB2Periph_TIM10|RCC_APB2Periph_TIM11)
                                          
#endif  /* STM32F40_41xxx */

#endif /* __STM32F4xx_pwr_modes_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
