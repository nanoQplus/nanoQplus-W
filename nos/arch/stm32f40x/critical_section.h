/*
 * Copyright (C) 2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file critical_section.h
 * @brief Critical section for ARM Cortex-M3
 * @author Haeyong Kim (ETRI)
 * @date 2013. 5. 22.
 */


#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H

#include "stm32f4xx.h"
#include "nos_common.h"
#include "intr.h" //@added by phj. @160228
//@added by phj. @160228
extern UINT32 nested_intr_cnt;		// the number of cli() or sli() that is called
extern INTR_STATUS intr_status;
extern UINT32 os_sched_lock_level;
//

#define NOS_DISABLE_GLOBAL_INTERRUPT() __disable_irq()
	
//__disable_irq()
#define NOS_ENABLE_GLOBAL_INTERRUPT()  __enable_irq()
//__enable_irq()

//extern volatile uint8_t _nested_intr_cnt;

/*
#define NOS_ENTER_CRITICAL_SECTION()            \
    do {                                        \
        NOS_DISABLE_GLOBAL_INTERRUPT();         \
        ++_nested_intr_cnt;                     \
    } while (0)

#define NOS_EXIT_CRITICAL_SECTION()                                 \
    do {                                                            \
        if (--_nested_intr_cnt == 0) NOS_ENABLE_GLOBAL_INTERRUPT(); \
    } while (0)


#define NOS_IS_CTX_SW_ALLOWABLE()   (!_nested_intr_cnt)
*/

#define NOS_ENTER_CRITICAL_SECTION() \
do { \
	NOS_DISABLE_GLOBAL_INTERRUPT(); \
	++os_sched_lock_level; \
} while (0)

#define NOS_EXIT_CRITICAL_SECTION() \
do { \
	--os_sched_lock_level; \
	if (!os_sched_lock_level) \
		NOS_ENABLE_GLOBAL_INTERRUPT(); \
} while (0)

	/*
#define OS_ENTER_ISR()	\
do { \
	++nested_intr_cnt; \
} while (0)

#define OS_EXIT_ISR()	\
do { \
	--nested_intr_cnt; \
} while (0)
*/
/*
#define OS_IS_CTX_SW_ALLOWABLE() ((!nested_intr_cnt)&&(!intr_status.cnt))
*/
#define NOS_IS_TASK_MODE() 	(!nested_intr_cnt)
#define NOS_IS_ISR_MODE() 	(nested_intr_cnt>0)



//printf("\n\rPRIMASK: %x, BASEPRI: %x, FAULTMASK: %x, _nested_intr_cnt:%d", __get_PRIMASK(), __get_BASEPRI(), __get_FAULTMASK(), _nested_intr_cnt);


#endif	// CRITICAL_SECTION_H
