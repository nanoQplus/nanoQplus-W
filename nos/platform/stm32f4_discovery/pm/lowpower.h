#ifndef __LOWPOWER_H__
#define __LOWPOWER_H__

#include "nos.h"

#define IDLE_MODE	0
#define SLEEP_MODE	1
#define STOP_MODE	2
#define STANDBY_MODE	3
#define POWEROFF_MODE	4

// defines power mode threshold, if 0, ignored (default value)
#ifndef PWRMODE_IDLE_THRESHOLD
	#define PWRMODE_IDLE_THRESHOLD		0
#endif // PWRMODE_IDLE_THRESHOLD

#ifndef PWRMODE_SLEEP_THRESHOLD
	#define PWRMODE_SLEEP_THRESHOLD		500
#endif // PWRMODE_SLEEP_THRESHOLD

#ifndef PWRMODE_STOP_THRESHOLD
	#define PWRMODE_STOP_THRESHOLD		1000
#endif // PWRMODE_STOP_THRESHOLD

#ifndef PWRMODE_STANDBY_THRESHOLD
	#define PWRMODE_STANDBY_THRESHOLD	3500
#endif // PWRMODE_STANDBY_THRESHOLD

#ifndef PWRMODE_PWROFF_THRESHOLD
	#define PWRMODE_PWROFF_THRESHOLD	170000
#endif // PWRMODE_PWROFF_THRESHOLD



#ifndef RTC_SCALE
#define RTC_SCALE       145
#endif // RTC_SCLE


extern UINT32 actual_idle_tick;
extern UINT32 __cpu_context_backup_point;
extern volatile void *pjmp;


// to register system save callback function
extern UINT32 (*callback_save_standby)(UINT32);
extern UINT32 (*callback_save_pwroff)(UINT32);



void RTC_Configuration(uint32_t tick);
void SYSCLKConfig_STOP(void);

UINT32 lp_get_idle_mode(UINT32);
void lp_enter_sleep_mode(UINT32);
void lp_enter_stop_mode(UINT32);
void lp_enter_standby_mode(UINT32);
void lp_enter_pwroff_mode(UINT32);



UINT32 register_callback_save_standby(UINT32(*func)(UINT32));
UINT32 register_callback_save_pwroff(UINT32(*func)(UINT32));


#endif //__LOWPOWER_H__
