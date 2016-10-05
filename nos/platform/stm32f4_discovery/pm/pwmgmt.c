#include "kconf.h"
#ifdef PWM_M

#include "pwmgmt.h"

//#define WakeupCounter 0xA000 /*specifies the WakeUp counter*/
#define WakeupCounter 0x5000 /*specifies the WakeUp counter*/

int set_peripheral(int bus, UINT32 peri_mask, int pwr_on)
{
  switch(bus) {

  case  PWR_AHB1_PERIPH :
    if(pwr_on) {
      RCC_AHB1PeriphClockCmd(peri_mask, ENABLE);
    }
    else { 
      RCC_AHB1PeriphClockCmd(peri_mask, DISABLE);
    }
    break;

  case  PWR_AHB2_PERIPH :
    if(pwr_on) {
      RCC_AHB2PeriphClockCmd(peri_mask, ENABLE);
    }
    else { 
      RCC_AHB2PeriphClockCmd(peri_mask, DISABLE);
    }
    break;

  case  PWR_APB1_PERIPH :
    if(pwr_on) {
      RCC_APB1PeriphClockCmd(peri_mask, ENABLE);
    }
    else { 
      RCC_APB1PeriphClockCmd(peri_mask, DISABLE);
    }
    break;

  case  PWR_APB2_PERIPH :
    if(pwr_on) {
      RCC_APB2PeriphClockCmd(peri_mask, ENABLE);
    }
    else { 
      RCC_APB2PeriphClockCmd(peri_mask, DISABLE);
    }
    break;

  default: 
    return 1;
  }

  return 0;
}

void all_peripherals_off(void)
{
  set_peripheral(PWR_AHB1_PERIPH, ALL_GPIOs, PWR_OFF);

  set_peripheral(PWR_AHB1_PERIPH, ALL_AHB1_PERIPH, PWR_OFF);

  set_peripheral(PWR_AHB2_PERIPH, ALL_AHB2_PERIPH, PWR_OFF);

  set_peripheral(PWR_APB1_PERIPH, ALL_APB1_PERIPH, PWR_OFF);

  set_peripheral(PWR_APB2_PERIPH, ALL_APB2_PERIPH, PWR_OFF);
}

void all_peripherals_on(void)
{
  set_peripheral(PWR_AHB1_PERIPH, ALL_GPIOs, PWR_ON);

  set_peripheral(PWR_AHB1_PERIPH, ALL_AHB1_PERIPH, PWR_ON);

  set_peripheral(PWR_AHB2_PERIPH, ALL_AHB2_PERIPH, PWR_ON);

  set_peripheral(PWR_APB1_PERIPH, ALL_APB1_PERIPH, PWR_ON);

  set_peripheral(PWR_APB2_PERIPH, ALL_APB2_PERIPH, PWR_ON);
}

static inline void set_clock_frequency(int flashws, int pll_m, int pll_n, int pll_p, int pll_q)
{
  /* Clock init configuration ------------------------------------------------*/
  RCC_DeInit();

  /* Flash 5 wait state */
  FLASH_SetLatency(flashws);

  /* Configures the AHB clock */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

  /* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div2);

  /* PCLK1 = HCLK/4 */
  RCC_PCLK1Config(RCC_HCLK_Div4);

  /* Configure the main PLL */
  RCC->PLLCFGR = pll_m | (pll_n << 6) | (((pll_p >> 1) -1) << 16) |
                 (RCC_PLLCFGR_PLLSRC_HSI) | (pll_q << 24);

  /* Enable PLL */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {}

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
  {}
}

int set_system_clock(int level)
{
  switch(level) {

  case FREQ_168MHz :
    set_clock_frequency(FLASH_Latency_5, 16, 336, 2, 7);
    break;

  case FREQ_144MHz :
    set_clock_frequency(FLASH_Latency_4, 16, 288, 2, 6);
    break;

  case FREQ_120MHz :
    set_clock_frequency(FLASH_Latency_3, 16, 240, 2, 5);
    break;

  case FREQ_84MHz :
    set_clock_frequency(FLASH_Latency_2, 16, 336, 4, 7);
    break;

  case FREQ_42MHz :
    set_clock_frequency(FLASH_Latency_1, 16, 252, 6, 6);
    break;

  default:
    return 1;
  }

  return 0;
}

static void run_mode(int freq, int peri_on)
{
  if(freq != FREQ_STAYING)
    set_system_clock(freq);

  if(peri_on == PERI_ON)
    all_peripherals_on();
  else if(peri_on == PERI_OFF)
    all_peripherals_off();
}

static void sleep_mode(int freq, int peri_on)
{
  if(freq != FREQ_STAYING)
    set_system_clock(freq);

  if(peri_on == PERI_ON)
    all_peripherals_on();
  else if(peri_on == PERI_OFF)
    all_peripherals_off();

  /* Configure Wakeup pin  */
  WakeUpPinInt_configuration();

  /* Enable the wakeup pin */
  PWR_WakeUpPinCmd(ENABLE);

  /* Request to enter SLEEP mode */
  __WFI();
}


static int stop_mode(int mode)
{
  /* Clear Wakeup flag*/
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Enable the wakeup pin */
  PWR_WakeUpPinCmd(ENABLE);

  switch(mode) {

  case PWR_STOP_MainRegFlashStop :
    /* Disable FLASH Deep Power Down Mode by clearing FPDS bit*/
    PWR_FlashPowerDownCmd(DISABLE);
    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
    break;

  case PWR_STOP_MainRegFlashPwrDown : 
    /* FLASH Deep Power Down Mode enabled */
    PWR_FlashPowerDownCmd(ENABLE);
    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
    break;

  case PWR_STOP_LowPwrRegFlashStop : 
    /* Disable FLASH Deep Power Down Mode by clearing FPDS bit*/
    PWR_FlashPowerDownCmd(DISABLE);
    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    break;

  case PWR_STOP_LowPwrRegFlashPwrDown : 
    /* FLASH Deep Power Down Mode enabled */
    PWR_FlashPowerDownCmd(ENABLE);
    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    break;

  default :
    return 1;
  }
  
  return 0;
}

static inline void rtc_on(void)
{
  /* LSI used as RTC source clock*/

  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}

static inline void rtc_wakeup_intr(void)
{
  /* RTC Wakeup Interrupt Generation: Clock Source: RTCCLK_Div16, Wakeup Time Base: ~20s
      RTC Clock Source LSI ~32KHz 

      Wakeup Time Base = (16 / LSI) * WakeUpCounter
   */

  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_SetWakeUpCounter(WakeupCounter);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable Wakeup Counter */
  RTC_WakeUpCmd(ENABLE);

  /* Clear WakeUp (WUTF) pending flag */
  RTC_ClearFlag(RTC_FLAG_WUTF);
}

static inline void bsram_on(void)
{
  /* Enable BKPRAM Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

  /* Enable the Backup SRAM low power Regulator */
  PWR_BackupRegulatorCmd(ENABLE);

  /* Wait until the Backup SRAM low power Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
  {}
}

static int standby_mode(int mode)
{
  switch(mode) {

  case PWR_STANDBY_BkpSramOnRtcOn : 
    /* Allow access to the backup domain (RTC registers, RTC
       backup data registers and backup SRAM) */
    PWR_BackupAccessCmd(ENABLE);
    rtc_on();
    bsram_on();
    rtc_wakeup_intr();
    break;

  case PWR_STANDBY_BkpSramOnRtcOff :
    PWR_BackupAccessCmd(ENABLE);
    bsram_on();
    break;

  case PWR_STANDBY_BkpSramOffRtcOn :
    PWR_BackupAccessCmd(ENABLE);
    rtc_on();
    rtc_wakeup_intr();
    break;

  case PWR_STANDBY_BkpSramOffRtcOff :
    break;

  default :
    return 1;
  }

  /* Enable the wakeup pin */
  PWR_WakeUpPinCmd(ENABLE);

  /* Clear standby flag */
  PWR_ClearFlag(PWR_FLAG_SB);

  /* Clear Wakeup flag*/
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Request to enter STANDBY mode*/
  PWR_EnterSTANDBYMode();

  return 0;
}

int set_power_mode(UINT8 mode, int freq, int peri_on)
{
  if(mode == PWR_RUN_MODE) {
    run_mode(freq, peri_on);
  }
  else if(mode == PWR_SLEEP_MODE) {
    sleep_mode(freq, peri_on);
  }
  else if(mode & PWR_STOP_MODE) {
    stop_mode(mode);
  }
  else if(mode & PWR_STANDBY_MODE) {
    standby_mode(mode);
  }
  else {
    return 1;
  }

  return 0;
}


/**
* @brief  Configure the PA0 pin as interrupt wakeup source
* @param  None
* @retval None
*/
void WakeUpPinInt_configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect EXTI Line0 to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure EXTI Line0 */
  EXTI_DeInit();
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Mode_Exit (void)
{
  /* Clock init configuration */
  RCC_DeInit();

  /* Disable HSE */
  RCC_HSEConfig(RCC_HSE_OFF);

  /* Enable HSI */
  RCC_HSICmd(ENABLE);

  /* Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}

  /* Select HSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  /* Wait till HSI is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00)
  {}

  /* Enable PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /*Configure Leds GPIOs */
  //LedsConfig();

  /* Configure Wakeup pin  */
  WakeUpPinInt_configuration();
}

#endif
