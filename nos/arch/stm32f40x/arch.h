/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @defgroup noslib_stm32f10x
 * @brief noslib - STM32F10X MCU library
  */

/**
 * @file arch.h
 * @brief STM32F10x MCU basic header
 * @author Haeyong Kim (ETRI)
 * @date 2014. 5. 8.
 * @ingroup noslib_stm32f10x
 * @copyright GNU General Public License v3
 */


#ifndef ARCH_H
#define ARCH_H
#include "kconf.h"

#include "stm32f4xx.h"
#include "nos_common.h"
#include "critical_section.h"
#include "nos_timer.h"

/* Endianess */
#ifdef LITTLE_ENDIAN
	#undef LITTLE_ENDIAN
	#define LITTLE_ENDIAN   1
#endif
//#define BIG_ENDIAN      1

/* bit ordering */
#define LSB_FIRST 1     // RS-232, ethernet
#define BIT_ORDER_LITTLE_ENDIAN     LSB_FIRST
//#define MSB_FIRST 1
//#define BIT_ORDER_BIG_ENDIAN        MSB_FIRST

/* Memory Align */
#define ALIGN_MOD 4

/* Memory management */
/* Refer "platform.ld" (_Min_Heap_Size, _Min_Stack_Size) */
/*RAM START: 0x20000000ul */
#if (defined STM32F103CB || defined STM32F103RB)
#define RAMEND 0x20004FFFul    /* bottom of STACK. size =20KB*/

#elif (defined STM32F101RC || defined STM32F103VC)
#define RAMEND 0x2000BFFFul    /* bottom of STACK. size =48KB */

#elif (defined STM32F103RF || defined STM32F103VG)
#define RAMEND 0x20017FFFul    /* bottom of STACK. size =96KB */

#elif (defined STM32F407VG)
#define RAMEND 0x2001FFFFul    /* bottom of STACK. size =128KB */
#endif

#define KERNEL_STACK_SIZE        0x1000
#define USER_STACK_DEFAULT_SIZE  0x1000  //for user_thread
#define KERNEL_STACK_TOP_ADDR  (RAMEND - KERNEL_STACK_SIZE + 1)   // may need to be aligned

//Inserted by phj. @160226
#define DEFAULT_STACK_SIZE		1024 // default stack size
#define SYSTEM_STACK_SIZE		DEFAULT_STACK_SIZE // system thread stack size. Heap does not use this area
#define STACK_GUARD_SIZE		128
//

/// MCU initialization.
void nos_arch_init(void);
void system_abort(UINT8 ecode);

/// System Reset
#define NOS_RESET()                             \
    do { \
        NVIC_SystemReset(); \
        while(1); \
    } while(0)



///Convert 2-byte unsigned integer @p a from network to byte byte order.
uint16_t ntohs(uint16_t a);

///Convert 2-byte unsigned integer @p a from host to byte byte order.
#define htons(a) ntohs(a)

///Convert 4-byte unsigned integer @p a from network to host byte order.
uint32_t ntohl(uint32_t a);

///Convert 4-byte unsigned integer @p a from host to byte order.
#define htonl(a) ntohl(a)


#endif // ~ARCH_H
 
