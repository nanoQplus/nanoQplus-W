#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include "stm32f4xx.h"
#include "pwr_modes.h"

typedef uint32_t  UINT32;
typedef uint8_t  UINT8;

#define PWR_STAYING 100
#define PWR_ON 1
#define PWR_OFF 0

#define PERI_STAYING 100
#define PERI_ON 1
#define PERI_OFF 0

/* bus configurations for power management */
#define PWR_AHB1_PERIPH 1
#define PWR_AHB2_PERIPH 2
#define PWR_APB1_PERIPH 3
#define PWR_APB2_PERIPH 4

/* system clock configurations for power management */

#define FREQ_168MHz 0
#define FREQ_144MHz 1
#define FREQ_120MHz 2
#define FREQ_84MHz  3
#define FREQ_42MHz  4
#define FREQ_STAYING 100

/* power mode configurations for power management */
#define PWR_RUN_MODE     0x0  // (0000 0000)
#define PWR_SLEEP_MODE   0x1  // (0000 0001)
#define PWR_STOP_MODE    0x10 // (0001 0000)
#define PWR_STANDBY_MODE 0x80 // (1000 0000)

#define PWR_STOP_MainRegFlashStop      0x10 // (0001 0000)
#define PWR_STOP_MainRegFlashPwrDown   0x12 // (0001 0010)
#define PWR_STOP_LowPwrRegFlashStop    0x14 // (0001 0100)
#define PWR_STOP_LowPwrRegFlashPwrDown 0x16 // (0001 0110)

#define PWR_STANDBY_BkpSramOnRtcOn    0x80 // (1000 0000)
#define PWR_STANDBY_BkpSramOnRtcOff   0xa0 // (1010 0000)
#define PWR_STANDBY_BkpSramOffRtcOn   0xc0 // (1100 0000)
#define PWR_STANDBY_BkpSramOffRtcOff  0xe0 // (1110 0000)


int set_peripheral(int bus, UINT32 peri_mask, int pwr_on);
int set_system_clock(int level);
int set_power_mode(UINT8 mode, int freq, int peri_on);

void all_peripherals_off(void);
void all_peripherals_on(void);

void Mode_Exit (void);
void WakeUpPinInt_configuration(void);

#endif
