/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_physical.h                                             */
/*  PURPOSE : Header file for Flash Device Physical Interface Layer     */
/*            (PFD)                                                     */
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

#ifndef _FD_PHYSICAL_H
#define _FD_PHYSICAL_H

#include "fd_config.h"
#include "fd_if.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable... DO NOT EDIT)      */
/*----------------------------------------------------------------------*/

enum FLASH_OPERATION {
    OP_NONE                     = 0x00,
    OP_READ                     = 0x01,
    OP_WRITE                    = 0x02,
    OP_ERASE                    = 0x03,
    OP_COPYBACK                 = 0x04
};

/* there might be some operational constraints for the flash memory to use;
   the following constants can be bit-ORed to express them */

enum FLASH_OPERATIONAL_CONSTRAINTS {
    CN_NO_CONSTRAINTS           = 0x00000000,
    CN_ODD_EVEN_COPYBACK        = 0x00000001,   /* copyback can be performed
                                                   only between odd pages or
                                                   between even pages */
    CN_VERY_SMALL_SIZE          = 0x00000002,   /* if this is set, CFD does not
                                                   store the partition table in
                                                   flash and just keeps the 
                                                   default table in RAM */
};

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/* type for LLD initialization function */

typedef int32_t (*lld_init_fp_t)(void);

/* data structure for low-level flash device operations */

typedef struct {
    int32_t (*open)             (uint16_t chip_id);
    int32_t (*close)            (uint16_t chip_id);
    int32_t (*read_page)        (uint16_t chip_id, uint32_t block, uint16_t page,
                                 uint8_t *dbuf, uint8_t *sbuf);
    int32_t (*read_bytes)       (uint16_t chip_id, uint32_t block, uint16_t page,
                                 uint32_t num_bytes, uint8_t *dbuf);
    int32_t (*write_page)       (uint16_t chip_id, uint32_t block, uint16_t page,
                                 uint8_t *dbuf, uint8_t *sbuf, bool_t is_last);
    int32_t (*write_bytes)      (uint16_t chip_id, uint32_t block, uint16_t page,
                                 uint32_t num_bytes, uint8_t *dbuf);
    int32_t (*copyback)         (uint16_t chip_id, 
                                 uint32_t src_block, uint16_t src_page, 
                                 uint32_t dst_block, uint16_t dst_page);
    int32_t (*erase)            (uint16_t chip_id, uint32_t block);
    int32_t (*sync)             (uint16_t chip_id, uint16_t prev_command);
    bool_t  (*is_bad_block)     (uint16_t chip_id, uint32_t block);
} flash_chip_ops_t;

/* data structure for describing flash operation */

typedef struct {
    uint16_t          local_chip_id;
    uint16_t          command;
    union {
        struct /* for read operation */ {
            uint32_t  block;
            uint16_t  page;
            uint8_t  *dbuf;
            uint8_t  *sbuf;
        }   read;
        
        struct /* for write operation */ {
            uint32_t  block;
            uint16_t  page;
            uint8_t  *dbuf;
            uint8_t  *sbuf;
            bool_t    is_last;
        }   write;
        
        struct /* for copyback operation */ {
            uint32_t  src_block;
            uint16_t  src_page;
            uint32_t  dst_block;
            uint16_t  dst_page;
        }   copyback;
        
        struct /* for erase operation */ {
            uint32_t  block;
        }   erase;
    }   param;
} op_desc_t;

/* data structure for physical flash device */

typedef struct {
    /* struct members from 'type' to 'num_blocks' are the same
       with struct 'flash_dev_spec_t', which is defined in fd_if.h */
    
    uint16_t        type;                   /* flash memory type (NOR or NAND) */
    uint16_t        page_size;              /* page size (bytes) */
    uint16_t        data_size;              /* page main area size (bytes) */
    uint16_t        spare_size;             /* page spare area size (bytes) */
    uint16_t        sectors_per_page;       /* number of sectors per page */
    uint16_t        pages_per_block;        /* number of pages per block */
    uint32_t        block_size;             /* block size (bytes) */
    uint32_t        num_blocks;             /* total number of blocks */

    uint16_t        num_dies_per_ce;        /* number of dies per chip enable */
    uint16_t        num_planes;             /* number of planes */
    uint32_t        max_num_bad_blocks;     /* maximum number of bad blocks */
    uint32_t        constraints;            /* operational constraints */
} flash_chip_spec_t;

typedef struct {
    uint16_t        chip_id;                /* pfdev_t array index for self reference */
    uint16_t        local_chip_id;          /* chip ID (0, 1, ...) in each class */

    flash_chip_spec_t   chip_spec;          /* flash chip specification */
    flash_chip_ops_t    chip_ops;           /* flash chip operations */
    op_desc_t           prev_op;            /* previous operation information */
    
    uint32_t        bm_area_start_block;    /* starting block number of BM area */
    uint16_t        bm_area_num_blocks;     /* number of blocks in BM area */
    uint32_t        part_table_block;       /* block number of the partition table */
    uint32_t        actual_num_blocks;      /* equals (total_num_blocks - bm_area_num_blocks - 1) */
    
    uint8_t         usage_count;            /* usage count */

    uint8_t         buf_index;              /* indicates the buffer currently available */
    uint8_t         buf[2][MAX_PAGE_SIZE];  /* double buffering */
} pfdev_t;

/*----------------------------------------------------------------------*/
/*  External Variable Declarations                                      */
/*----------------------------------------------------------------------*/

extern pfdev_t      PDev[NUM_FLASH_CHIPS];
extern uint16_t     pdev_count;

extern lld_init_fp_t  lld_init[NUM_FLASH_CHIPS + 1];

/*----------------------------------------------------------------------*/
/*  External Function Declarations                                      */
/*----------------------------------------------------------------------*/

extern int32_t  pfd_init                    (void);
extern int32_t  pfd_shutdown                (int32_t forced);

extern int32_t  pfd_register_flash_chip     (uint16_t           local_chip_id, 
                                             flash_chip_spec_t  *chip_spec, 
                                             flash_chip_ops_t   *chip_ops);
extern uint16_t pfd_get_num_chips           (void);
extern pfdev_t *pfd_get_device              (uint16_t       chip_id);

extern int32_t  pfd_open                    (uint16_t       chip_id);
extern int32_t  pfd_close                   (uint16_t       chip_id);
extern int32_t  pfd_erase_all               (uint16_t       chip_id, 
                                             bool_t         skip_initial_bad);

/* functions for common physical I/O */

extern int32_t  pfd_read_page           (pfdev_t *pdev, uint32_t block, uint16_t page,
                                         uint8_t *dbuf, uint8_t *sbuf);
extern int32_t  pfd_read_bytes          (pfdev_t *pdev, uint32_t block, uint16_t page,
                                         uint32_t num_bytes, uint8_t *dbuf);
extern int32_t  pfd_write_page          (pfdev_t *pdev, uint32_t block, uint16_t page,
                                         uint8_t *dbuf, uint8_t *sbuf, bool_t is_last);
extern int32_t  pfd_write_bytes         (pfdev_t *pdev, uint32_t block, uint16_t page,
                                         uint32_t num_bytes, uint8_t *dbuf);
extern int32_t  pfd_copyback            (pfdev_t *pdev, 
                                         uint32_t src_block, uint16_t src_page, 
                                         uint32_t dst_block, uint16_t dst_page);
extern int32_t  pfd_erase               (pfdev_t *pdev, uint32_t block);
extern int32_t  pfd_sync                (pfdev_t *pdev);

/* miscellaneous */

extern uint8_t *pfd_get_temp_buf        (pfdev_t *pdev);

#endif /* _FD_PHYSICAL_H */
