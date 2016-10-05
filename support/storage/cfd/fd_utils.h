/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_utils.h                                                */
/*  PURPOSE : Utility functions and data structures                     */
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

#ifndef _FD_UTILS_H
#define _FD_UTILS_H

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))

#define DELAY_IN_USEC(usec)                     \
        do {                                    \
            int i;                              \
            for (i = usec * 100; i >= 0; i--);  \
        } while (0)

/*----------------------------------------------------------------------*/
/*  External Variable Declarations                                      */
/*----------------------------------------------------------------------*/

extern uint32_t  BITS(uint32_t n);  /* lookup function for log of base 2 */

#endif /* _FD_UTILS_H */
