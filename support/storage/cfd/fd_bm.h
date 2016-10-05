/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_bm.h                                                   */
/*  PURPOSE : Header file for Flash Device Bad Block Management Layer   */
/*            (BM)                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  Authors : Zeen Information Technologies, Inc.                       */
/*            (info@zeen.snu.ac.kr)                                     */
/*----------------------------------------------------------------------*/
/*  The copyright of this software is subject to the copyright of ETRI  */
/*  Nano OS. For more information, please contact authorities of ETRI.  */
/*----------------------------------------------------------------------*/
/*  NOTES                                                               */
/*                                                                      */
/*  - This module's functionality is called DLBM                        */
/*    (Driver-Level Bad Block Management).                              */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 1.0)                                          */
/*                                                                      */
/*  - 2010.10.01 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#ifndef _FD_BM_H
#define _FD_BM_H

#include "fd_config.h"
#include "fd_physical.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable... DO NOT EDIT)      */
/*----------------------------------------------------------------------*/

#define NUMBER_OF_MAP_BLOCKS    2               /* must be >= 2 */
#define BADBLK_ENTRIES_PER_PAGE 120             /* in 512 byte page */
#define SWAP_TABLE_HASH_SIZE    16              /* must be a kind of (2^n) */

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/* data structure for instance of bad block management module */

typedef struct {
    uint32_t    sequence;
    uint32_t    map_block[NUMBER_OF_MAP_BLOCKS];
    uint16_t    cur_map_block_idx;
    uint16_t    cur_map_page_num;
    uint16_t    num_map_pages;
    uint16_t    state;

    uint32_t   *swap_table;
    uint16_t    swap_table_size;
    uint16_t    swap_table_hash_list[SWAP_TABLE_HASH_SIZE];
    uint16_t    free_block_list;
    uint16_t   *next_link;
    
    uint32_t    too_many_bad_blocks;
    uint32_t    initial_bad_block_scan_start;
} bm_mod_t;

/* structure for bad block map page (512 bytes) */
typedef struct {
    uint32_t    signature;
    uint32_t    sequence;
    uint32_t    map_offset;
    uint32_t    swap_table[BADBLK_ENTRIES_PER_PAGE];
    uint32_t    too_many_bad_blocks;
    uint8_t     version[12];
    uint32_t    checksum;
} bm_map_page_t;

/*----------------------------------------------------------------------*/
/*  External Variable Declarations                                      */
/*----------------------------------------------------------------------*/

extern bm_mod_t bm_mod[NUM_FLASH_CHIPS];

/*----------------------------------------------------------------------*/
/*  External Function Declarations                                      */
/*----------------------------------------------------------------------*/

#if USE_DLBM

extern int32_t  bm_init(void);
extern int32_t  bm_shutdown(int32_t forced);

extern uint32_t bm_calc_bm_area_num_blocks(uint16_t chip_id);
extern int32_t  bm_open(uint16_t chip_id);
extern int32_t  bm_format(uint16_t chip_id, bool_t forced);
extern bool_t   bm_is_formatted(uint16_t chip_id);
extern int32_t  bm_get_num_bad_blocks(uint16_t chip_id);

extern int32_t  bm_swap_write_bad_block(uint16_t chip_id, uint32_t block, uint16_t page);
extern int32_t  bm_swap_erase_bad_block(uint16_t chip_id, uint32_t block);
extern uint32_t bm_get_swapping_block(uint16_t chip_id, uint32_t block);
extern uint32_t bm_get_original_block(uint16_t chip_id, uint32_t block);

extern void     bm_set_initial_bad_block_scan_start(uint16_t chip_id, uint32_t block);
extern void     bm_mark_bad_block(pfdev_t *pdev, uint32_t block);

#endif /* USE_DLBM */
#endif /* _FD_BM_H */
