#ifndef NOS_H
#define NOS_H

#include "api.h"

#include "kconf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arch.h"
#include "platform.h"
#include "nos_timer.h"
#include "nos_rtc.h"
//#include "utc_clock.h"

#include "heap.h"
#include "msgq.h"

/**
 * @brief NanoQplus OS initialization
 *
 * It should be called by main() when startup.
 */
void nos_init(void);
void RTC_Config(void);
void SysTick_Configuration(void);
void Delay(__IO uint32_t nTime);
void SYSCLKConfig_STOP(void);

#include "critical_section.h"

#include "arch.h"
#define mcu_reboot()                            NOS_RESET()
#define delay_us(time)                          nos_delay_us(time) /**< @link nos_delay_us() @endlink */
#define delay_ms(time)                          nos_delay_ms(time) /**< @link nos_delay_ms() @endlink */

#ifdef LED_M
#include "led.h"
#define led_on(n)                               nos_led_on(n) /**< @link nos_led_on() @endlink */
#define led_off(n)                              nos_led_off(n) /**< @link nos_led_off() @endlink */
#define led_toggle(n)                           nos_led_toggle(n) /**< @link nos_led_toggle() @endlink */
#else //~LED_M
// Dummy macros to prevent compile errors.
#define led_on(n)
#define led_off(n)
#define led_toggle(n)
#endif /* LED_M */
#ifdef UART_M 
#include "uart.h"
extern void uart_printf(const char *msg, ...);
extern void nos_uart_putu(UINT8 port_num, int val);
#define uart_putc(byte)                    nos_uart_putc(2, byte)
#define uart_puts(byte)                    nos_uart_puts(2, byte)
#define uart_getc(byte)                    nos_uart_getc(2, byte)
#define uart_puti(int)			   nos_uart_puti(2, int)	//void os_uart_puti(UINT8 port_num, INT16 val)
#define uart_putu(uint)			   nos_uart_putu(2, uint)	//void os_uart_putu(UINT8 port_num, UINT16 val)

#define enable_uart_rx_intr(p)                  nos_uart_enable_rx_intr(p)
#define disable_uart_rx_intr(p)                 nos_uart_disable_rx_intr(p)
#define uart_set_rx_callback(p,func)            nos_uart_set_rx_callback(p, func)
#endif

//mapping tickless os_function to nos_function
#define nos_zero_switch_context(prev, next)		os_zero_switch_context(prev,next)
#define nos_switch_context(prev, next)			os_switch_context(prev, next)
#define nos_load_context(thread)					os_load_context(thread)
#define nos_sched_init							os_sched_init
#define nos_start									os_start
#define nos_sched_lock							os_sched_lock
#define nos_sched_unlock							os_sched_unlock
#define nos_sched_unlock_switch					os_sched_unlock_switch
#define nos_sched_unlock_bottom_half				os_sched_unlock_bottom_half

#endif /* NOS_H */


