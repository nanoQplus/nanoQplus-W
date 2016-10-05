
#include "lowpower.h"
#include "pwmgmt.h"

#include "bkpsram.h"

UINT32 actual_idle_tick;
RTC_InitTypeDef  RTC_InitStructure;
UINT32 __cpu_context_backup_point;
volatile void *pjmp;

// to register system save callback function
UINT32 (*callback_save_standby)(UINT32);
UINT32 (*callback_save_pwroff)(UINT32);

void RTC_Configuration(uint32_t tick)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	//uart_printf("RTC_Configuration :: Entering rtc_configuration, tick = %d\r\n", tick);

	//__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Clear Wakeup flag */
	PWR_ClearFlag(PWR_FLAG_WU);

	/* Check if the StandBy flag is set */
	if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
	{
	//uart_printf("RTC_Configuration :: Entering rtc_configuration_standby, tick = %d\r\n", tick);
		/* Clear StandBy flag */
		PWR_ClearFlag(PWR_FLAG_SB);

		/* Reset Backup Domain */
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);

		/* The RTC Clock may varies due to LSI frequency dispersion. */   
		/* Enable the LSI OSC */ 
		RCC_LSICmd(ENABLE);

		/* Wait till LSI is ready */  
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{
		}

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

		//	SynchPrediv = 0xFF;
		//	AsynchPrediv = 0x7C;


		/* Enable the RTC Clock */
		RCC_RTCCLKCmd(ENABLE);
		///////////////////////////////////

		/* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
		RTC_WaitForSynchro();
		/* No need to configure the RTC as the RTC config(clock source, enable,
		   prescaler,...) are kept after wake-up from STANDBY */

		/* RTC Wakeup Interrupt Generation: Clock Source: RTCCLK_Div16, Wakeup Time Base: ~4s

		   Wakeup Time Base = (16 / (LSE or LSI)) * WakeUpCounter
		   */
		//RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);
		//RTC_SetWakeUpCounter(tick);
		//RTC_SetWakeUpCounter(0x36b0);
		//RTC_SetWakeUpCounter(0x1F3F);
		//RTC_SetWakeUpCounter(0x400);
	
		if (tick < 300) { //not over 3sec...
			tick = (tick) * RTC_SCALE;
			RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);

			//uart_printf("RTC_Configuration : tick < 300, modified tick = %d\r\n", tick);

			RTC_SetWakeUpCounter(tick);
			goto L2;
		} // end if

		if (tick >= 4000) tick = 3999; // maximum 40sec

		//not over 40sec... 40sec are an arbitrary value
		RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
		tick = tick * (RTC_SCALE / 8);
		//uart_printf("RTC_Configuration : tick < 4000, modified tick = %d\r\n", tick);
		RTC_SetWakeUpCounter(tick);

L2:



		/* Enable the Wakeup Interrupt */
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		/* Connect EXTI_Line22 to the RTC Wakeup event */
		EXTI_ClearITPendingBit(EXTI_Line22);
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable the RTC Wakeup Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
	//uart_printf("RTC_Configuration :: Entering rtc_configuration_stop, tick = %d\r\n", tick);
		/* RTC Configuration ******************************************************/
		/* Reset Backup Domain */
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);
		/* The RTC Clock may varies due to LSI frequency dispersion. */   
		/* Enable the LSI OSC */ 
		RCC_LSICmd(ENABLE);



		/* Wait till LSI is ready */  
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{
		}

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

		//SynchPrediv = 0xFF;
		//AsynchPrediv = 0x7C;


		/* Enable the RTC Clock */
		RCC_RTCCLKCmd(ENABLE);

		/* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
		RTC_WaitForSynchro();

		/* RTC Wakeup Interrupt Generation: Clock Source: RTCCLK_Div16, Wakeup Time Base: ~4s

		   Wakeup Time Base = (16 / (LSE or LSI)) * WakeUpCounter
		   */
		//RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);
		//RTC_SetWakeUpCounter(tick);

		if (tick < 300) { //not over 3sec...
			tick = (tick) * RTC_SCALE;
			RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);

			//uart_printf("RTC_Configuration : tick < 300, modified tick = %d\r\n", tick);

			RTC_SetWakeUpCounter(tick);
			goto L1;
		} // end if

		if (tick >= 4000) tick = 3999; // maximum value is 40sec.

		//not over 40sec... 40sec are an arbitrary value
		RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
		tick = tick * (RTC_SCALE / 8);
		//uart_printf("RTC_Configuration : tick < 4000, modified tick = %d\r\n", tick);
		RTC_SetWakeUpCounter(tick);



		// SPRE mode are not used
		// more than 40sec...
		/*
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;
		RTC_Init(&RTC_InitStructure);
		//tick = (tick*0.89)/100; //  calibration 0.89, 0.9
		tick = (tick) / 100; //  

		uart_printf("RTC_Configuration : tick > 4000, modified tick = %d\r\n", tick);

		RTC_SetWakeUpCounter(tick);
		*/

L1:

		/* Enable the Wakeup Interrupt */
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		/* Connect EXTI_Line22 to the RTC Wakeup event */
		EXTI_ClearITPendingBit(EXTI_Line22);
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable the RTC Wakeup Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

	}
}

void SYSCLKConfig_STOP(void)
{
	/* After wake-up from STOP reconfigure the system clock */
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
	{}

	/* Enable PLL */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{}

	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	while (RCC_GetSYSCLKSource() != 0x08)
	{}
}



// returns low power idle mode
//
UINT32 lp_get_idle_mode(UINT32 delta) {

	//uart_printf("lp_get_idle_mode:: delta = %d\r\n", delta);

#if PWRMODE_IDLE_THRESHOLD != 0
	if (delta < PWRMODE_IDLE_THRESHOLD / 10) {
		//uart_printf("lp_get_idle_mode :: Entering IDLE Mode!!\r\n");
		return IDLE_MODE;
	} // end if
#endif // PWRMODE_IDLE_THRESHOLD

#if PWRMODE_SLEEP_THRESHOLD != 0
	if (delta < PWRMODE_SLEEP_THRESHOLD / 10) {
		//uart_printf("lp_get_idle_mode :: Entering SLEEP Mode!! delta = %d\r\n", delta);
		return SLEEP_MODE;
	} // end if
#endif // PWRMODE_SLEEP_THRESHOLD

#if PWRMODE_STOP_THRESHOLD != 0
	if (delta < PWRMODE_STOP_THRESHOLD / 10) {
		//uart_printf("lp_get_idle_mode :: Entering STOP Mode!!\r\n");
		return STOP_MODE;
	} // end if
#endif // PWRMODE_STOP_THRESHOLD

#if PWRMODE_STANDBY_THRESHOLD != 0
	if (delta < PWRMODE_STANDBY_THRESHOLD / 10) {
		//uart_printf("lp_get_idle_mode :: Entering STANDBY Mode!!\r\n");
		return STANDBY_MODE;
	} // end if
#endif // PWRMODE_STANDBY_THRESHOLD

#if PWRMODE_PWROFF_THRESHOLD != 0
	if (delta < PWRMODE_PWROFF_THRESHOLD / 10) {
		//uart_printf("lp_get_idle_mode :: Entering PWROFF Mode!!\r\n");
		return POWEROFF_MODE;
	} // end if
#endif // PWRMODE_PWROFF_THRESHOLD

	return IDLE_MODE;
} // end func

void lp_enter_sleep_mode(UINT32 delta) {

// FIXME : sleep mode still has tick (10msec)

	set_power_mode(PWR_SLEEP_MODE, FREQ_STAYING, PERI_STAYING);
} // end func

extern UINT32 SysTick_Reload_OverFlow;

void lp_enter_stop_mode(UINT32 delta) {

	//UINT32 rtc_time;

	os_timer_tick_stop();

	uart_printf("lp_enter_stop_mode:: Entering stop mode, delta = %d\r\n", delta);

	//rtc_time = delta * RTC_SCALE;
	//RTC_Configuration(rtc_time);
	RTC_Configuration(delta);
	RTC_WakeUpCmd(ENABLE);
	RTC_ClearFlag(RTC_FLAG_WUTF);	/* Clear WakeUp (WUTF) pending flag */

	set_power_mode(PWR_STOP_LowPwrRegFlashPwrDown, FREQ_STAYING, PERI_STAYING);

	//__gcounter += delta * 10;

	SYSCLKConfig_STOP();
	SysTick_Reload_OverFlow = 0;

	os_timer_tick_set(0);
} // end func



__attribute__ ((always_inline)) static __INLINE void __save_cpu_context(UINT32 p) {
	__ASM volatile (
			"mov	r12, %0			\n\t"
			"stmia	r12!, {r0-r11}		\n\t"
			"mrs	r0, apsr		\n\t"
			"mrs	r1, msp			\n\t"
			"mrs	r2, psp			\n\t"
			"mrs	r3, control		\n\t"
			"mrs	r4, faultmask		\n\t"
			"mrs	r5, basepri		\n\t"
			"mrs	r6, primask		\n\t"
			"stmia	r12!, {r0-r6}		\n\t"
			"str	r13, [r12]		\n\t"
			"add	r12, r12, #4		\n\t"
			"str	r14, [r12]		\n\t"
			"add	r12, r12, #4		\n\t"
			"mov	r1, r15			\n\t"
			"add	r1, r1, #48		\n\t"
			"str	r1, [r12]		\n\t"
		       ::"r" (p));
} // end func


// STANDBY_TEST_MODE is to debug full cold restart using power-off mode
//#define STANDBY_TEST_MODE 1

#ifdef STANDBY_TEST_MODE
void lp_enter_standby_mode(UINT32 delta) {

	UINT32 rtc_time;
//    uint32_t stime, etime;
    
//    start_MT();
//	stime = MT_get_time();

	os_timer_tick_stop();
	pjmp = &&here1; 	// pjmp saves restore jmp point

	//	BKSRAM_Set_Flag();

	//rtc_time = delta * RTC_SCALE;
	//FIXME: calc of extraction remaining time

	//RTC_Configuration(rtc_time);
	RTC_Configuration(delta);
	RTC_WakeUpCmd(ENABLE);

	NOS_DISABLE_GLOBAL_INTERRUPT();
	__save_cpu_context(__cpu_context_backup_point);
	NOS_ENABLE_GLOBAL_INTERRUPT();
	if (callback_save_standby) {
		if (callback_save_standby(delta * 10)) {
			uart_printf("lp_enter_standby_mode::halt!!!\r\n");
			system_abort(0);
		} // end if
	} else {
		uart_printf("lp_enter_standby_mode:: call back function is not defined.  ignored.\r\n");
	} // end if

//    MT_stop();
//	etime = MT_get_time();
//	printf("Duration for saving snapshot = %d (ms)\r\n", (int)(etime - stime));

	set_power_mode(PWR_STANDBY_BkpSramOnRtcOn, FREQ_STAYING, PERI_STAYING);

	// when restore, jumps here
here1:
	__NOP();
	__NOP();

	NOS_ENABLE_GLOBAL_INTERRUPT();
	SYSCLKConfig_STOP();
	SysTick_Reload_OverFlow = 0;

	RTC_Configuration(1);
	RTC_WakeUpCmd(ENABLE);
	RTC_ClearFlag(RTC_FLAG_WUTF);	/* Clear WakeUp (WUTF) pending flag */

	uart_printf("lp_enter_standby_mode: restore & exit\r\n");

	os_timer_tick_set(0);
} // end func
#endif // #ifdef STANDBY_TEST_MODE


// originally, standby mode has half cold restart mode
#ifndef STANDBY_TEST_MODE
void lp_enter_standby_mode(UINT32 delta) {

	//UINT32 rtc_time;

	os_timer_tick_stop();

	RTC_Configuration(delta);
	RTC_WakeUpCmd(ENABLE);

	if (callback_save_standby) {
		if (callback_save_standby(delta * 10)) {
			uart_printf("lp_enter_standby_mode:: halt!!!\r\n");
			system_abort(0);
		} // end if
	} else {
		uart_printf("lp_enter_standby_mode:: callback function is not defined.  ignored.\r\n");
	} // end if

	set_power_mode(PWR_STANDBY_BkpSramOnRtcOn, FREQ_STAYING, PERI_STAYING);

	// on half cold restart mode, never reaching below code
	__NOP();
	__NOP();

} // end func
#endif // #ifndef STANDBY_TEST_MODE


void lp_enter_pwroff_mode(UINT32 delta) {
	UINT32 rtc_time;

	os_timer_tick_stop();
	pjmp = &&here2;

	rtc_time = delta * RTC_SCALE;
	// FIXME: calc of extraction remaining time

	NOS_DISABLE_GLOBAL_INTERRUPT();
	__save_cpu_context(__cpu_context_backup_point);
	NOS_ENABLE_GLOBAL_INTERRUPT();

	if (callback_save_pwroff) {
		if (callback_save_pwroff(delta * 10)) {
			uart_printf("lp_enter_pwroff_mode::halt!!!\r\n");
			for (;;) { ; } 
		} // end if
	} else {
		uart_printf("lp_enter_pwroff_mode:: callback function is not defined.  halt!!\r\n");
		for (;;) {;}
	} // end if

here2:
	__NOP();
	__NOP();

	uart_printf("lp_enter_pwroff_mode: restore & exit\r\n");

	NOS_ENABLE_GLOBAL_INTERRUPT();
	SYSCLKConfig_STOP();
	SysTick_Reload_OverFlow = 0;

	RTC_Configuration(1);
	RTC_WakeUpCmd(ENABLE);
	RTC_ClearFlag(RTC_FLAG_WUTF);	/* Clear WakeUp (WUTF) pending flag */

	os_timer_tick_set(0);

} // end func

UINT32 register_callback_save_standby(UINT32(*func)(UINT32)) {
	
	if (func) {
		callback_save_standby = func;
	} // end if
	
	return 0;
} // end func

UINT32 register_callback_save_pwroff(UINT32(*func)(UINT32)) {

	if (func) {
		callback_save_pwroff = func;
	} // end if
	
	return 0;
} // end func
