/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_oal.h                                                  */
/*  PURPOSE : OS Adaptation Layer (OAL)                                 */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  Authors : Zeen Information Technologies, Inc.                       */
/*            (info@zeen.snu.ac.kr)                                     */
/*----------------------------------------------------------------------*/
/*  The copyright of this software is subject to the copyright of ETRI  */
/*  Nano OS. For more information, please contact authorities of ETRI.  */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 1.0)                                          */
/*                                                                      */
/*  - 2011.10.01 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#ifndef _FD_OAL_H
#define _FD_OAL_H

#include "kconf.h"
#include "stm32f4xx.h"
#include "heap.h"

#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

#define TRUE                        1
#define FALSE                       0

/* macro for malloc() */

#define MALLOC(size)                nos_malloc(size)
#define FREE(mem)                   do {                                    \
                                        if ((mem) != NULL) nos_free(mem);   \
                                    } while(0)

/* macro for memory manipulation */

#define MEMSET(mem, value, size)    memset(mem, value, size)
#define MEMCPY(dst, src, size)      memcpy(dst, src, size)
#define MEMCMP(mem1, mem2, size)    memcmp(mem1, mem2, size)

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

typedef uint8_t                     bool_t;

#endif /* _FD_OAL_H */
