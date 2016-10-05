/**
 * Test peripheral and system clock APIs for power management.
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

void test_peripheral_switching(int bus, UINT32 peri_mask)
{
  set_peripheral(bus, peri_mask, PWR_ON);

  ms_delay(3000);

  set_peripheral(bus, peri_mask, PWR_OFF);

  ms_delay(3000);

  set_peripheral(bus, peri_mask, PWR_ON);

  ms_delay(3000);
}

void test_peripheral1(void)
{
  ms_delay(5000);
  
  all_peripherals_off();
  
  ms_delay(5000);

  test_peripheral_switching(PWR_AHB1_PERIPH, ALL_GPIOs);

  test_peripheral_switching(PWR_AHB1_PERIPH, ALL_AHB1_PERIPH);

  test_peripheral_switching(PWR_AHB2_PERIPH, ALL_AHB2_PERIPH);

  test_peripheral_switching(PWR_APB1_PERIPH, ALL_APB1_PERIPH);

  test_peripheral_switching(PWR_APB2_PERIPH, ALL_APB2_PERIPH);

  ms_delay(5000);
  
  all_peripherals_off();
}

void test_peripheral2(void)
{
  ms_delay(5000);

  all_peripherals_off();

  ms_delay(5000);

  set_peripheral(PWR_AHB1_PERIPH, ALL_GPIOs, PWR_ON);

  ms_delay(5000);

  set_peripheral(PWR_AHB1_PERIPH, ALL_AHB1_PERIPH, PWR_ON);

  ms_delay(5000);

  set_peripheral(PWR_AHB2_PERIPH, ALL_AHB2_PERIPH, PWR_ON);

  ms_delay(5000);

  set_peripheral(PWR_APB1_PERIPH, ALL_APB1_PERIPH, PWR_ON);

  ms_delay(5000);

  set_peripheral(PWR_APB2_PERIPH, ALL_APB2_PERIPH, PWR_ON);

  ms_delay(5000);
}

void test_system_clock(void)
{
  ms_delay(5000);

  set_system_clock(FREQ_144MHz);

  ms_delay(5000);

  set_system_clock(FREQ_120MHz);

  ms_delay(5000);

  set_system_clock(FREQ_84MHz);

  ms_delay(5000);

  set_system_clock(FREQ_42MHz);

  ms_delay(5000);

  set_system_clock(FREQ_168MHz);

  ms_delay(5000);
}

#endif

void app_init(void)
{

#ifdef PWM_M  
  //test_peripheral1();
  //test_peripheral2();
  test_system_clock();

  while(1) {};
#endif

}

