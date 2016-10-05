/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_config.c                                               */
/*  PURPOSE : Definitions of customizable variables and functions       */
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
/*  - 2010.10.01 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_logical.h"
#include "fd_physical.h"

#include "fsmc_nand_lld.h"
#include "nor_lld.h"

/*----------------------------------------------------------------------*/
/*  LLD Initialization Function Registration (CUSTOMIZE THESE VALUES)   */
/*----------------------------------------------------------------------*/

lld_init_fp_t  lld_init[NUM_FLASH_CHIPS + 1] = {
    fsmc_nand_init,
    nor_init,
    NULL
};

/* example:

lld_init_fp_t lld_init[NUM_FLASH_CHIPS + 1] = {
    init_function_name_for_LLD1,
    init_function_name_for_LLD2,
    NULL                                        // last entry should be NULL
};  */

/*----------------------------------------------------------------------*/
/*  Default Partition Table (CUSTOMIZE THESE VALUES)                    */
/*----------------------------------------------------------------------*/

flash_parttab_t  default_part_table[NUM_FLASH_CHIPS] = 
{
    /* for flash memory chip-0 (NAND flash) */
    {
        /* num_partitions       */      2,
        /* part[]               */
        {
            /* for partition-0  */      // for CPU & RAM snapshots for fast re-boot 
            {
                /* start_block  */      0,
                /* num_blocks   */      200,
                /* dev_class    */      LFD_BLK_DEVICE_RAW,
                /* in_dev_table */      FALSE,
                /* dev_index    */      0,
                /* protected    */      FALSE
            },
            /* for partition-1  */      // for a file system
            {
                /* start_block  */      200,
                /* num_blocks   */      1024    /* total blocks */
                                        - 200   /* # of blocks in partition-0 */
                                        - 27    /* the size of DLBM (bad-block mgmt.) area */
                                        - 1,    /* flash memory partition table block */
                /* dev_class    */      LFD_BLK_DEVICE_FTL,
                /* in_dev_table */      TRUE,
                /* dev_index    */      0,
                /* protected    */      FALSE
            },
            /* for partition-2  */
            /* and so on...     */
        }
    },

    /* for flash memory chip-1 (NOR flash) */
    {
        /* num_partitions       */      2,
        /* part[]               */
        {
            /* for partition-0  */      // for CPU & RAM snapshots for fast re-boot 
            {
                /* start_block  */      0,
                /* num_blocks   */      3,
                /* dev_class    */      LFD_BLK_DEVICE_RAW,
                /* in_dev_table */      FALSE,
                /* dev_index    */      0,
                /* protected    */      FALSE
            },
            /* for partition-1  */      // for a file system
            {
                /* start_block  */      3,
                /* num_blocks   */      7       /* total blocks */
                                        - 3     /* # of blocks in partition-0 */
                                        - 0     /* the size of DLBM (bad-block mgmt.) area */
                                        - 0,    /* flash memory partition table block */
                /* dev_class    */      LFD_BLK_DEVICE_FTL,
                /* in_dev_table */      TRUE,
                /* dev_index    */      0,
                /* protected    */      FALSE
            },
            /* for partition-2  */
            /* and so on...     */
        }
    },
    
    /* for flash memory chip-2  */
    /* and so on...             */
};

#endif /* CFD_M */
