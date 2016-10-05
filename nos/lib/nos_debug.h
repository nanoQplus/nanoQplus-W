/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file nos_debug.h
 * @brief Debug library
 * @author Haeyong Kim (ETRI)
 * @date 2013. 1. 18.
 * @ingroup 
 * @copyright GNU General Public License v3
 */


#ifndef NOS_DEBUG_H
#define NOS_DEBUG_H

#include <stdio.h>
#include "kconf.h"


#if defined(NOS_DEBUG_M)
#define NOS_ERROR_PRINTF(arg)   do{printf("\n\r%s >>",__FUNCTION__); nos_debug_printf arg;}while(0)
    #if !defined(NDEBUG)
    #define NOS_DEBUG_PRINTF(arg)   nos_debug_printf arg
    #else
    #define NOS_DEBUG_PRINTF(arg)
    #endif
#else
#define NOS_ERROR_PRINTF(arg)
#define NOS_DEBUG_PRINTF(arg)
#endif


#if defined(NOS_DEBUG_M) && !defined(NDEBUG)
    #ifdef NOS_STACK_DEBUG_M
    #define  NOS_DEBUG_START   do{nos_debug_print_min_stack_ptr(); nos_debug_start(__FUNCTION__);}while(0)
    #else
    #define  NOS_DEBUG_START   (nos_debug_start(__FUNCTION__))
    #endif
    #define  NOS_DEBUG_END     (nos_debug_end(__FUNCTION__))
    #define  NOS_DEBUG_NOTIFY  (nos_debug_notify(__FUNCTION__, __LINE__))
    #define  NOS_DEBUG_ERROR   (nos_debug_error(__FUNCTION__, __LINE__))
#else
    #ifdef  NOS_STACK_DEBUG_M
    #define  NOS_DEBUG_START   (nos_debug_print_min_stack_ptr())
    #else
    #define  NOS_DEBUG_START
    #endif
    #define  NOS_DEBUG_END
    #define  NOS_DEBUG_NOTIFY
    #define  NOS_DEBUG_ERROR
#endif





void nos_debug_printf(char const *fmt, ...);
void nos_debug_start(char const * fName_p);
void nos_debug_end(char const * fName_p);
void nos_debug_notify(char const * fName_p, int lineNum);
void nos_debug_error(char const * fName_p, int lineNum);
void nos_debug_print_min_stack_ptr(void);


#endif  /* !NOS_DEBUG_H */
