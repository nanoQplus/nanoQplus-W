/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_utils.c                                                */
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

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_utils.h"

/*----------------------------------------------------------------------*/
/*  External Function Definitions                                       */
/*----------------------------------------------------------------------*/

/* function for translating 'a number of 2^n' into 'n' */

extern uint32_t 
BITS(uint32_t n)
{
    uint32_t bits = 0;

    while (n > 1) {
        n >>= 1;
        bits++;
    }
    
    return(bits);
}

#endif /* CFD_M */
