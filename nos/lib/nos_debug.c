/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file nos_debug.c
 * @brief Debug library
 * @author Haeyong Kim (ETRI)
 * @date 2013. 1. 18.
 * @ingroup 
 * @copyright GNU General Public License v3
 */

#include <stdarg.h>
#include "kconf.h"
#include "nos_common.h"
#include "nos_debug.h"

#ifdef NOS_STACK_DEBUG_M

#ifdef __GNUC__
#include "arch.h"
extern MEMENTRY_T _end; /* Defined by the linker*/
register MEMENTRY_T *stack_ptr asm ("sp");
#elif defined (stm32f10x)
#include "stm32f10x.h"
#endif

volatile MEMADDR_T min_stack_ptr_value = 0xFFFFFFFF;

void nos_debug_print_min_stack_ptr()
{
    MEMADDR_T tmp_stack_ptr;
#ifdef __GNUC__
    tmp_stack_ptr = (MEMADDR_T)stack_ptr;
#elif defined (stm32f10x)
    tmp_stack_ptr = __get_MSP();
#endif
    if (tmp_stack_ptr < min_stack_ptr_value)
    {
        min_stack_ptr_value = tmp_stack_ptr;
        printf("\n\rSP(the lowest): 0x%lx", (MEMADDR_T)min_stack_ptr_value);
//#ifdef __GNUC__
//        printf("\n\rMEM bottom: 0x%lx", (MEMADDR_T)(&_end));
//#endif
    }
}
#endif


#ifdef NOS_DEBUG_M
void nos_debug_printf(char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
//    #if defined (__GNUC__)
//    vprintf(stderr, fmt, ap);
//    #else
    vprintf(fmt, ap);
//    #endif
    va_end(ap);
}

void nos_debug_start(char const * fName_p)
{
    printf("\n\r[Enter]: %s()", fName_p);
}

void nos_debug_end(char const * fName_p)
{
    printf("\n\r [Exit]: %s()", fName_p);
}

void nos_debug_notify(char const * fName_p, int lineNum)
{
    printf("\n\r\t-NOTIFY:%s(): %d", fName_p, lineNum);
}

void nos_debug_error(char const * fName_p, int lineNum)
{
    printf("\n\r\t\t-ERROR:%s(): %d", fName_p, lineNum);
}
#endif /* NOS_DEBUG_M */

