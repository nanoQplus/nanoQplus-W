//===================================================================
//
// common.h (@sheart, @jun361, @haekim)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef COMMON_H
#define COMMON_H

#include "typedef.h"
#include "nos_exit.h"

// ASCII Characters
#define _BS    (0x08)    // Back Space (ASCII)
#define _CR    (0x0D)    // Carriage Return (ASCII)
#define _LF    (0x0A)    // LineFeed
#define _SPACE (0x20)    //Space

#ifdef __IAR_SYSTEMS_ICC__
#undef _MIN
#undef _MAX
#endif

#ifndef _MIN
#define _MIN(n,m) (((n) < (m)) ? (n) : (m)) // get a minimum number among n and m
#endif

#ifndef _MAX
#define _MAX(n,m) (((n) < (m)) ? (m) : (n)) // get a max number among n and m
#endif

#ifndef _ABS
#define _ABS(n)  (((n) < 0) ? (-(n)) : (n)) // get absolute number 
#endif

#ifndef NULL
#define NULL		(0)
#endif

#define _BIT_SET(reg,n)         ((reg) |= (1 << (n)))
#define _BIT_CLR(reg,n)         ((reg) &= ~(1 << (n)))
#define _IS_SET(reg,n)          ((((reg) & (1 << (n))) != 0) ? TRUE : FALSE)
#define _REG_MASK(reg,masking)  ((reg) |= (masking))
#define _REG_UNMASK(reg,masking)  ((reg) &= ~(masking))

#define _CONCAT2BYTES(low, high)    ( (uint16_t)(low) | (((uint16_t)(high))<<8) )
#define _CONCAT2BYTES_BE(high, low) _CONCAT2BYTES(low, high)

#endif // COMMON_H
