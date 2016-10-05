/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_if.h                                                   */
/*  PURPOSE : Header file for Flash Device Common Interface             */
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

#ifndef _FD_IF_H
#define _FD_IF_H

#include "fd_config.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable... DO NOT EDIT)      */
/*----------------------------------------------------------------------*/

#define SECTOR_SIZE             512
#define SPARE_SIZE              16
#define MAX_SECTORS_PER_PAGE    4       /* max # of sectors per flash page */
#define MAX_PAGE_SIZE           ((SECTOR_SIZE + SPARE_SIZE) * MAX_SECTORS_PER_PAGE)

#define MAX_FLASH_DEVICES       8       /* max # of logical flash devices
                                           that can be opened simultaneously */
#define MAX_FLASH_PARTITIONS    8       /* max # of partitions in each flash
                                           memory chip (max 15) */

#define RAW_BLKDEV_TABLE        0xF     /* special ID used to indicate that a
                                           raw block device will be referenced
                                           by using an index value to the raw
                                           block device table instead of using
                                           the chip number & partition number
                                           pairs */

#define ENTIRE_PARTITION        0xF     /* special partition ID for the entire
                                           chip space (excluding the DLBM area
                                           and the partition table block) */

enum FLASH_TYPE {
    NOR_FLASH                   = 1,    /* NOR flash */
    SLC_NAND_FLASH              = 2,    /* Single-Level Cell NAND flash */
    MLC_NAND_FLASH              = 3     /* Multi-Level Cell NAND flash */
};

enum LOGICAL_FLASH_DEVICE_CLASS {
    LFD_BLK_DEVICE_RAW          = 137,  /* used as a RAW block device */
    LFD_BLK_DEVICE_FTL          = 138   /* used as a FTL block device */
};

/* return values of flash memory device driver API functions */

enum DRIVER_API_RETURN_VALUES {
    FM_SUCCESS                  = 0,
    FM_BAD_DEVICE_ID            = 1,
    FM_OPEN_FAIL                = 2,
    FM_INIT_FAIL                = 3,
    FM_NOT_FORMATTED            = 4,
    FM_INVALID_PARTITION        = 5,
    FM_ILLEGAL_ACCESS           = 6,
    FM_DEVICE_NOT_OPEN          = 7,
    FM_ECC_ERROR                = 8,
    FM_TRY_AGAIN                = 9,
    FM_MEM_ERROR                = 10,
    FM_ERROR                    = 11,
    
    FM_READ_ERROR               = 12,
    FM_WRITE_ERROR              = 13,
    FM_ERASE_ERROR              = 14,
    FM_PROTECT_ERROR            = 15,

    /* previous error return values should 
       have values greater than 15 (= 0xF) */

    FM_PREV_WRITE_ERROR         = 16,
    FM_PREV_ERASE_ERROR         = 17,
    FM_PREV_PROTECT_ERROR       = 18
};

#define FM_PREV_ERROR_MASK      0xF0

/* macros for decomposing device ID */

#define DEV_SERIAL_BITS         16
#define DEV_SERIAL_MASK         0xFFFF

#define GET_DEV_CLASS(dev_id)   ((uint16_t)((dev_id) >> DEV_SERIAL_BITS))
#define GET_DEV_SERIAL(dev_id)  ((uint16_t)((dev_id) & DEV_SERIAL_MASK))

/* definition for logical device ID used for 'RAW block device'

            +-----------+-----------+-----------+-----------+
   dev_id:  |   Byte3   |   Byte2   |   Byte1   |   Byte0   |
            +-----------+-----------+-----------+-----------+
             `---- device class ---' `--- serial number ---'

            +----------------------------------------------------------------+
            | device class  = LFD_BLK_DEVICE_RAW                             |
            |                                                                |
            | serial number = not used (Byte1) +                             |
            |                 chip number (Byte0, upper 4 bits) +            |
            |                 partition number (Byte0, lower 4 bits)         |
            |                                                                |
            |                 or                                             |
            |                                                                |
            |               = not used (Byte1) +                             |
            |                 RAW_BLKDEV_TABLE (Byte0, upper 4 bits) +       |
            |                 raw block device index (Byte0, lower 4 bits)   |
            +----------------------------------------------------------------+

            * special partition number for entire device space = 0xF

            * if a special number 'RAW_BLKDEV_TABLE' (= 0xF) is given in the 
              place of chip number, a raw block device can be referenced by 
              using the raw block device table and the index to the table; 
              this mechanism is required because there might be a raw block 
              device that is comprised of multiple partitions on multiple chips

            * examples for RAW block device's serial number:
              - chip-0, partition-0  = 0x0000
              - chip-1, partition-2  = 0x0012
              - chip-2, entire space = 0x002F
              - 1st device in the RAW block device table = 0x00F0
              - 2nd device in the RAW block device table = 0x00F1
*/

#define GET_RAWDEV_CHIP_ID(serial)  ((uint16_t)((serial) >> 4))
#define GET_RAWDEV_PART_ID(serial)  ((uint16_t)((serial) & 0xF))
#define GET_RAWDEV_INDEX(serial)    ((uint16_t)((serial) & 0xF))

/* definition for logical device ID used for 'FTL block device'

            +-----------+-----------+-----------+-----------+
   dev_id:  |   Byte3   |   Byte2   |   Byte1   |   Byte0   |
            +-----------+-----------+-----------+-----------+
             `---- device class ---' `--- serial number ---'

            +----------------------------------------------------------------+
            | device class  = LFD_BLK_DEVICE_FTL                             |
            |                                                                |
            | serial number = not used (Byte1) +                             |
            |                 FTL block device index (Byte0, upper 4 bits) + |
            |                 reserved (Byte0, lower 4 bits)                 |
            +----------------------------------------------------------------+

            * 'FTL block device index' is an index to the FTL block device 
              table, which maintains information about the constituent 
              partitions for each raw block device

            * 'reserved' bits of Byte0 is currently used by the file system 
              to store high-level (filesystem-level) partition information

            * examples for FTL block device's serial number:
              - 1st FTL block device = 0x0000
              - 2nd FTL block device = 0x0010
              - 3rd FTL block device = 0x0020
*/

#define GET_FTLDEV_INDEX(serial)  ((uint16_t)((serial) >> 4))

/* macros for composing device ID;
   you can make a 'dev_id' using one of the following macros:
   
   - dev_id for a RAW block device using (chip_id, part_id):
     SET_DEV_ID(LFD_BLK_DEVICE_RAW, SET_RAWDEV_SERIAL(chip_id, part_id))
   
   - dev_id for a RAW block device using the RAW_BLKDEV_TABLE index:
     SET_DEV_ID(LFD_BLK_DEVICE_RAW, 
                SET_RAWDEV_SERIAL(RAW_BLKDEV_TABLE, rawdev_index))
     
   - dev_id for an FTL block device:
     SET_DEV_ID(LFD_BLK_DEVICE_FTL, SET_FTLDEV_SERIAL(ftldev_index))
*/

#define SET_DEV_ID(dev_class, serial)       \
        ((((uint32_t)(dev_class))<<DEV_SERIAL_BITS) | ((serial)&DEV_SERIAL_MASK))

#define SET_RAWDEV_SERIAL(chip_id, part_id) \
        ((uint16_t)((((chip_id) & 0xF) << 4) | ((part_id) & 0xF)))

#define SET_FTLDEV_SERIAL(ftldev_index) \
        ((uint16_t)(((ftldev_index) & 0xF) << 4))

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/* data structure for dimensional flash device specification */

typedef struct {
    uint16_t    type;               /* flash memory type (NOR or NAND) */
    uint16_t    page_size;          /* page size (bytes) */
    uint16_t    data_size;          /* page main area size (bytes) */
    uint16_t    spare_size;         /* page spare area size (bytes) */
    uint16_t    sectors_per_page;   /* number of sectors per page */
    uint16_t    pages_per_block;    /* number of pages per block */
    uint32_t    block_size;         /* block size (bytes) */
    uint32_t    num_blocks;         /* total number of blocks */
} flash_dev_spec_t;

/* data structure for each flash partition */

typedef struct {
    uint32_t    start_block;       /* starting block # of partition */
    uint32_t    num_blocks;        /* number of blocks in partition */
    uint32_t    dev_class;         /* logical flash device class; 
                                      for linux, dev_class corresponds
                                      to the major device number */
    bool_t      in_dev_table;      /* indicates whether this partition is
                                      registered in one of the RAW and FTL
                                      block device tables; for an FTL block
                                      device partition, this value should
                                      be TRUE */
    uint8_t     dev_index;         /* device index for the RAW block device
                                      table or the FTL block device table
                                      (device tables are indexed separately);
                                      if multiple RAW (or FTL) block device 
                                      partitions share the same 'dev_index' 
                                      value, they'll be merged into a single
                                      virtual partition */
    bool_t      protected;        /* write/erase protection flag */
} flash_part_t;

/* data structure for flash partition table */

typedef struct {
    uint16_t        num_partitions;     /* # of partitions in a chip */
    flash_part_t    part[MAX_FLASH_PARTITIONS];
} flash_parttab_t;

/*----------------------------------------------------------------------*/
/*  External Variable Declarations                                      */
/*----------------------------------------------------------------------*/

extern bool_t   fd_ecc_corrected;       /* defined in fd_physical.c */

/*----------------------------------------------------------------------*/
/*  External Function Declarations                                      */
/*----------------------------------------------------------------------*/

/* init function (initializes all of the device driver layers) */

extern int32_t  fd_init             (void);
extern int32_t  fd_shutdown         (int32_t forced);

/* flash memory device driver APIs; 
   upper layer software (e.g. FTL) calls these functions 
   (dev_id = logical device ID) */

extern int32_t  fd_open             (uint32_t dev_id);
extern int32_t  fd_close            (uint32_t dev_id);
extern int32_t  fd_read_page        (uint32_t dev_id, uint32_t block, uint16_t page,
                                     uint8_t *dbuf, uint8_t *sbuf);
extern int32_t  fd_read_bytes       (uint32_t dev_id, uint32_t block, uint16_t page,
                                     uint32_t num_bytes, uint8_t *dbuf);
extern int32_t  fd_write_page       (uint32_t dev_id, uint32_t block, uint16_t page,
                                     uint8_t *dbuf, uint8_t *sbuf, bool_t is_last);
extern int32_t  fd_write_bytes      (uint32_t dev_id, uint32_t block, uint16_t page,
                                     uint32_t num_bytes, uint8_t *dbuf);
extern int32_t  fd_copyback         (uint32_t dev_id, 
                                     uint32_t src_block, uint16_t src_page, 
                                     uint32_t dst_block, uint16_t dst_page);
extern int32_t  fd_erase            (uint32_t dev_id, uint32_t block);
extern int32_t  fd_sync             (uint32_t dev_id);
extern int32_t  fd_get_device_info  (uint32_t dev_id, flash_dev_spec_t *dev_info);
extern int32_t  fd_get_phy_block    (uint32_t dev_id, uint32_t block, uint32_t *phy_block);
                                     
/* additional APIs for flash memory volume management
   (chip_id = physical device ID) */

extern uint16_t fd_get_num_chips        (void);
extern int32_t  fd_format               (uint16_t chip_id);
extern int32_t  fd_read_partition_table (uint16_t chip_id, flash_parttab_t *part_tab);
extern int32_t  fd_write_partition_table(uint16_t chip_id, flash_parttab_t *part_tab);
extern int32_t  fd_erase_all            (uint16_t chip_id, bool_t skip_initial_bad);
extern int32_t  fd_erase_partition      (uint16_t chip_id, uint16_t part_no);

#endif /* _FD_IF_H */
