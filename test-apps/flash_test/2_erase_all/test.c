/************************************************************************/
/*                                                                      */
/*  PROJECT : CFD (Common Flash Driver)                                 */
/*  FILE    : test.c                                                    */
/*  PURPOSE : Test code for CFD                                         */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 1.0)                                          */
/*                                                                      */
/*  - 2016.03.01 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_if.h"

#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------*/
/*  Code for erasing all flash memory blocks                            */
/*----------------------------------------------------------------------*/

void erase_all(uint16_t chip_id)
{
    int32_t err;

#if 0
    err = fd_erase_all(chip_id, TRUE);    // if you want to check initial bad blocks before erasing
#else
    err = fd_erase_all(chip_id, FALSE);   // if you want to erase all blocks including initial bad blocks
#endif
    if (err) printf("Error(%d) occurred!!\r\n", (int)err);
        
    return;
}

#endif /* CFD_M */
