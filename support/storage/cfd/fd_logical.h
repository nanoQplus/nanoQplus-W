/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_logical.h                                              */
/*  PURPOSE : Header file for Logical Flash Device (LFD) Abstraction    */
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

#ifndef _FD_LOGICAL_H
#define _FD_LOGICAL_H

#include "fd_config.h"
#include "fd_if.h"
#include "fd_physical.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable... DO NOT EDIT)      */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/* data structure for a real flash partition */

typedef struct {
    pfdev_t         *pdev;          /* pointer to a physical device */
    flash_part_t    *part;          /* pointer to a flash partition info 
                                       structure in the designated physical
                                       device */
} flash_rpart_t;

/* data structure for a virtual flash partition (a group of partitions) */

typedef struct {
    uint16_t        num_partitions;         /* # of the constituent partitions */
    flash_rpart_t   parts[NUM_FLASH_CHIPS]; /* info about the partitions */
} flash_vpart_t;

/* data structure for a logical flash device */

typedef struct {
    uint32_t            dev_id;             /* unique dev ID used by upper layers */
    flash_vpart_t       part_info;          /* partition information */
    flash_dev_spec_t    dev_spec;           /* device specification */
    uint16_t            interleaving_level; /* level of chip interleaving */
    uint8_t             usage_count;        /* usage count */
} lfdev_t;

/*----------------------------------------------------------------------*/
/*  External Variable Declarations                                      */
/*----------------------------------------------------------------------*/

extern lfdev_t          LDev[MAX_FLASH_DEVICES];
extern uint16_t         ldev_count;

extern flash_parttab_t  part_table[NUM_FLASH_CHIPS];
extern flash_parttab_t  default_part_table[NUM_FLASH_CHIPS];

extern flash_vpart_t    raw_dev_table[MAX_FLASH_PARTITIONS];
extern flash_vpart_t    ftl_dev_table[MAX_FLASH_PARTITIONS];

/*----------------------------------------------------------------------*/
/*  External Function Declarations                                      */
/*----------------------------------------------------------------------*/

extern int32_t  lfd_init                    (void);
extern int32_t  lfd_shutdown                (int32_t forced);

extern int32_t  lfd_open                    (uint32_t dev_id);
extern int32_t  lfd_close                   (uint32_t dev_id);
extern lfdev_t *lfd_get_device              (uint32_t dev_id);

/* functions for flash memory low-level partition management */

extern int32_t  lfd_read_partition_table    (uint16_t chip_id, 
                                             flash_parttab_t *part_tab);
extern int32_t  lfd_write_partition_table   (uint16_t chip_id, 
                                             flash_parttab_t *part_tab);
extern int32_t  lfd_erase_partition         (uint16_t chip_id, 
                                             uint16_t part_no);

extern flash_rpart_t *lfd_get_part_info     (lfdev_t *ldev, uint32_t *block);

#endif /* _FD_LOGICAL_H */
