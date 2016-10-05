/*
 * Copyright (C) 2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file typedef.h
 * @brief Basic type definitions
 * @author Haeyong Kim (ETRI)
 * @date 2014. 4. 29.
 * @ingroup noslib_stm32f10x
 * @copyright GNU General Public License v3
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t   UINT8;
typedef uint16_t  UINT16;
typedef uint32_t  UINT32;
typedef uint64_t  UINT64;
typedef int8_t    INT8;
typedef int16_t   INT16;
typedef int32_t   INT32;
typedef int64_t   INT64;
typedef bool      BOOL;
typedef uint32_t    MEMADDR_T;
typedef uint32_t    MEMENTRY_T;
/*
typedef unsigned char 			UINT8;
typedef unsigned short int		UINT16;
typedef unsigned int 			UINT32;
typedef unsigned long long 		UINT64;
*/
///////////////////////////////////////////////

//typedef UINT8	BOOL;
typedef UINT8	BYTE;
typedef UINT16	WORD;
typedef UINT32	DWORD;
typedef UINT64 	QWORD;



// bool,address,size,device driver status type
//typedef signed              int   bool_t; // dkwu, conflicting with type in flash manager
typedef unsigned            int   addr_t;
typedef unsigned            int   size_t;
typedef signed              int   status_t;   

typedef volatile unsigned long  vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char  vu_char;

typedef UINT32	STATUS;
typedef UINT32	COUNT;

#define NO_ERR		    0
#define INVALID_PARAMETER   -1
#define NOT_EXIST           -2
#define TIME_OUT            -3
#define INVALID_ADDRESS     -4
#define DEVICE_ERROR        -5
#define DEVICE_NOT_EXIST    -6
#define DEVICE_BUSY         -7
#define DEVICE_NOT_ACTIVE   -8
#define INVALID_STATE       -9
#define UNKNOWN_DEVICE	    -10
#define INVALID_SIZE	    -11	

#endif // TYPEDEF_H

//===================================================================
//
// typedef.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
