/**
 * Test power mode APIs for power management.
 *
 * @author Duk-Kyun Woo (ETRI)
 * @date 2016. 3. 28.
 */
#include "kconf.h"

#ifdef PWM_M

#include "pwmgmt.h"

//Quick hack, approximately 1ms delay
void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}

void test_run_mode1(void)
{
  ms_delay(5000);
  
  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_OFF);
  
  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_120MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_84MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_42MHz, PERI_STAYING);

  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_STAYING);
}

void test_run_mode2(void)
{
  ms_delay(5000);
  
  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_ON);
  
  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_120MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_84MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_42MHz, PERI_STAYING);

  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_STAYING);
}

void test_sleep_mode1(void)
{
  ms_delay(5000);
  
  set_power_mode(PWR_SLEEP_MODE, FREQ_168MHz, PERI_OFF);
  
  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_120MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_84MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_42MHz, PERI_STAYING);  

  ms_delay(5000);
}

void test_sleep_mode2(void)
{
  all_peripherals_off();
  
  ms_delay(5000);
  
  set_power_mode(PWR_SLEEP_MODE, FREQ_168MHz, PERI_ON);
  
  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_120MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_84MHz, PERI_STAYING);  

  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_42MHz, PERI_STAYING);
}

void test_sleep_mode3(void)
{
  ms_delay(5000);

  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_ON);

  ms_delay(5000);

  set_power_mode(PWR_SLEEP_MODE, FREQ_STAYING, PERI_STAYING);

  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_STAYING);
  
  ms_delay(5000);
}

void test_stop_mode(void)
{
  WakeUpPinInt_configuration();

  ms_delay(5000);

  set_power_mode(PWR_STOP_MainRegFlashStop, FREQ_STAYING, PERI_STAYING);

  Mode_Exit();
  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_ON);
  ms_delay(5000);

  set_power_mode(PWR_STOP_MainRegFlashPwrDown, FREQ_STAYING, PERI_STAYING);

  Mode_Exit();
  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_ON);
  ms_delay(5000);

  set_power_mode(PWR_STOP_LowPwrRegFlashStop, FREQ_STAYING, PERI_STAYING);

  Mode_Exit();
  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_ON);
  ms_delay(5000);

  set_power_mode(PWR_STOP_LowPwrRegFlashPwrDown, FREQ_STAYING, PERI_STAYING);

  Mode_Exit();
  set_power_mode(PWR_RUN_MODE, FREQ_168MHz, PERI_ON);
  ms_delay(5000);
}

void test_standby_mode1(void)
{
  WakeUpPinInt_configuration();

  ms_delay(5000);

  set_power_mode(PWR_STNADBY_BkpSramOnRtcOn, FREQ_STAYING, PERI_STAYING);
}

void test_standby_mode2(void)
{
  WakeUpPinInt_configuration();

  ms_delay(5000);

  set_power_mode(PWR_STANDBY_BkpSramOffRtcOn, FREQ_STAYING, PERI_STAYING);
}

void test_standby_mode3(void)
{
  WakeUpPinInt_configuration();

  ms_delay(5000);

  set_power_mode(PWR_STANDBY_BkpSramOnRtcOff, FREQ_STAYING, PERI_STAYING);
}

void test_standby_mode4(void)
{
  WakeUpPinInt_configuration();

  ms_delay(5000);

  set_power_mode(PWR_STANDBY_BkpSramOffRtcOff, FREQ_STAYING, PERI_STAYING);
}

void test_power_mode(void)
{
  //test_run_mode1();
  //test_run_mode2();  
  //test_sleep_mode1();
  //test_sleep_mode2();
  //test_sleep_mode3();
  test_stop_mode();
  //test_standby_mode1();
  //test_standby_mode2();
  //test_standby_mode3();
  //test_standby_mode4();
}

#endif

void app_init(void)
{

#ifdef PWM_M
  test_power_mode();

  while(1) {};
#endif

}

