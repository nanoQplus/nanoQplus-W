/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_bm.c                                                   */
/*  PURPOSE : Code for Flash Device Bad Block Management Layer (BM)     */
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

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_if.h"
#include "fd_bm.h"
#include "fd_physical.h"
#include "fd_lld_wrapper.h"
#include "fd_utils.h"

#if USE_DLBM

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Rarely Configurable)                  */
/*----------------------------------------------------------------------*/

#define COPY_WRITE_BAD_PAGE     0               /* copy the bad page itself
                                                   after block replacement
                                                   for a write bad block?
                                                   1: yes
                                                   0: no (default) */

#define DELAY_FOR_ERASE         4000            /* in usec; this delay is 
                                                   inserted before bad block 
                                                   handling when an erase bad
                                                   block is detected */

#define DETECT_FACTORY_BAD_BLK  1               /* detect factory bad block
                                                   using bad block information
                                                   in the spare area?
                                                   1: yes (default)
                                                   0: no  (try to erase) */

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable)                     */
/*----------------------------------------------------------------------*/

#define MAP_PAGE_SIGNATURE      0x4C6A7B0E
#define END_OF_LIST             0xFFFF

#define MAX_NUMBER_OF_MAP_PAGES 16

/* BM (Bad block Management) module's state; BM module's facility can be
   used only when it is in the 'BM_FORMATTED' state */

enum BAD_BLOCK_MANAGEMENT_MODULE_STATE {
    BM_CLOSED                   = 0x00,
    BM_OPEN                     = 0x01,
    BM_FORMATTED                = 0x02
};

/* macro for calculating the DLBM area size */

#define CALC_BM_AREA_SIZE(chip)                                             \
        (PDev[(chip)].chip_spec.max_num_bad_blocks > 0 ?                    \
         PDev[(chip)].chip_spec.max_num_bad_blocks + NUMBER_OF_MAP_BLOCKS : \
         0)

/* macros for calculating block numbers & offsets in the DLBM area */

#define GET_BLK_OFFSET(block)   ((block) - pdev->bm_area_start_block)
#define GET_BLK_NUMBER(offset)  (pdev->bm_area_start_block + (offset))

#define MAP_OFFSET(blk_offset)  \
        ((uint16_t)((blk_offset) / BADBLK_ENTRIES_PER_PAGE))

/* each swap table entry corresponds to a block in the DLBM area;
   by looking up the entries, we can know how each DLBM area block
   is used currently -- followings are the possible usages;
   
   free block        : a DLBM area block is not used yet
   map block         : a DLBM area block is used to store the swap table
   bad block itself  : a DLBM area block is bad itself
   replacement block : used as a replacement block for a bad block
   
   to save the above information in the 32-bit long swap table entries,
   together with some aditional information for a registered bad block,
   the following bit field struture is used;

   31           24            16             8             0
   +-------------+-------------+-------------+-------------+
   |    Byte3    |    Byte2    |    Byte1    |    Byte0    |
   +-------------+-------------+-------------+-------------+
    `-----+-----'  | `---- replaced bad block number -----'
          |        |
          |        +------ special entry indicator
          |
          +--------------- additional bad block information

   bit 31..30 : flash memory operation that caused the bad block
   bit 29..24 : reserved (may be used to hold page number)
   bit 23     : special entry indicator 
                0 - entry for bad block replacement (swap entry)
                1 - entry for free block, map block, bad block itself
   bit 22...0 : replaced (original) bad block number 
*/

/* special swap table entries */
   
#define ENTRY_FREE_BLOCK        0x00FFFFFF      /* not used; free to use */
#define ENTRY_MAP_BLOCK         0x00FFFFFC      /* block is used for map */
#define ENTRY_BAD_BLOCK         0x00FFFFF9      /* block is bad itself */

/* macros to extract a specific bit field from the swap table entries */

#define OP_INFO_MASK            0xC0000000      /* mask for operation info */
#define OP_INFO_SHIFT           30
#define BLK_NUMBER_MASK         0x00FFFFFF      /* mask for block number &
                                                   special entry indicator */

#define IS_SPECIAL_ENTRY(ent)   ((uint32_t)ent & 0x00800000)
#define IS_SWAP_ENTRY(ent)      (~((uint32_t)ent) & 0x00800000)

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  External Variable Definitions                                       */
/*----------------------------------------------------------------------*/

bm_mod_t                bm_mod[NUM_FLASH_CHIPS];    /* BM module struct */

/*----------------------------------------------------------------------*/
/*  Local Variable Definitions                                          */
/*----------------------------------------------------------------------*/

static uint32_t         __pbuf[MAX_PAGE_SIZE / 4];
static uint8_t          *pbuf = (uint8_t *)__pbuf;
static bm_map_page_t    *map_page = (bm_map_page_t *)__pbuf;

/*----------------------------------------------------------------------*/
/*  Local Function Declarations                                         */
/*----------------------------------------------------------------------*/

static void     init_bm_module(pfdev_t *pdev, bm_mod_t *mod);
static void     construct_fast_lookup_table(bm_mod_t *mod);
static uint16_t get_swap_table_free_block_entry(bm_mod_t *mod);
static uint16_t get_swap_table_last_free_block_entry(bm_mod_t *mod);
static int32_t  save_swap_table(pfdev_t *pdev, bm_mod_t *mod, uint16_t map_offset);
static int32_t  read_map_page(pfdev_t *pdev, uint32_t block, uint16_t page);
static int32_t  write_map_page(pfdev_t *pdev, bm_mod_t *mod, uint16_t map_offset,
                               uint32_t block, uint16_t page);
static int32_t  flash_write_page(pfdev_t *pdev, uint32_t block, uint16_t page, 
                                 uint8_t *dbuf, uint8_t *sbuf);
static int32_t  flash_copy_page(pfdev_t *pdev, uint32_t src_block, 
                                uint32_t dst_block, uint16_t page);
static int32_t  flash_erase_block(pfdev_t *pdev, uint32_t block);
static uint32_t calc_checksum(uint32_t *data, uint16_t size);
static bool_t   is_all_FF(uint8_t *buf, uint32_t size);
static int32_t  no_free_block_panic(pfdev_t *pdev, bm_mod_t *mod);


/*======================================================================*/
/*  External Function Definitions                                       */
/*======================================================================*/

extern int32_t    
bm_init(void)
{
    int i;
    pfdev_t  *pdev;
    bm_mod_t *mod;
    
    /* initialize the bad block management modules */
    for (i = 0; i < NUM_FLASH_CHIPS; i++) {
        pdev = &PDev[i];
        mod = &bm_mod[i];
        mod->swap_table_size = CALC_BM_AREA_SIZE(i);
        
        if (mod->swap_table_size > 0) {
            /* allocate memory for BM modules */
            mod->swap_table = (uint32_t *) MALLOC(sizeof(uint32_t)*mod->swap_table_size);
            if (mod->swap_table == NULL) {
                return(FM_MEM_ERROR);
            }
            
            mod->next_link = (uint16_t *) MALLOC(sizeof(uint16_t)*mod->swap_table_size);
            if (mod->next_link == NULL) {
                FREE(mod->swap_table);
                return(FM_MEM_ERROR);
            }
        }
        else {
            mod->swap_table = NULL;
            mod->next_link = NULL;
        }

        /* initialize other BM module variables */
        init_bm_module(pdev, mod);

        mod->initial_bad_block_scan_start = 0;
    }

    return(FM_SUCCESS);
}


extern int32_t
bm_shutdown(int32_t forced)
{
    int i;
    bm_mod_t *mod;
    
    for (i = 0; i < NUM_FLASH_CHIPS; i++) {
        mod = &bm_mod[i];

        if (mod->swap_table != NULL) {
            FREE(mod->swap_table);
            mod->swap_table = NULL;
        }

        if (mod->next_link != NULL) {
            FREE(mod->next_link);
            mod->next_link = NULL;
        }
    }
    
    return(FM_SUCCESS);
}


extern uint32_t   
bm_calc_bm_area_num_blocks(uint16_t chip_id)
{
    /* return the number of blocks in the DLBM area */
    return(CALC_BM_AREA_SIZE(chip_id));
}


extern int32_t    
bm_open(uint16_t chip_id)
{
    pfdev_t  *pdev;
    bm_mod_t *mod;
    int32_t  err;
    uint32_t block, j;
    uint32_t seq[MAX_NUMBER_OF_MAP_PAGES], map_block_seq[NUMBER_OF_MAP_BLOCKS];
    uint16_t i, page, map_blocks_found;
    
    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];
    mod = &bm_mod[chip_id];
    
    /* if the max number of bad blocks for this chip is 0, just return here */
    if (pdev->chip_spec.max_num_bad_blocks == 0) goto end;

    /* already open? */
    if (mod->state > BM_CLOSED) return(FM_SUCCESS);

    /* initialize local variables */
    mod->num_map_pages = (mod->swap_table_size - 1) / BADBLK_ENTRIES_PER_PAGE + 1;
    for (i = 0; i < NUMBER_OF_MAP_BLOCKS; i++) map_block_seq[i] = 0;
    map_blocks_found = 0;
    
    /* starting from the last block in the DLBM area, 
       find #(NUMBER_OF_MAP_BLOCKS) of map blocks */
    for (block = pdev->chip_spec.num_blocks - 1;
         block >= pdev->bm_area_start_block; block--) {
        
        /* skip bad blocks */
        if (lld_is_bad_block(pdev, block)) continue;
        
        /* read the first page in this block;
           if it is not valid, proceed to check the next block */
        err = read_map_page(pdev, block, 0);
        if (err != FM_SUCCESS) continue;
        
        /* a valid map page (thus a valid map block) found;
           keep the block number and the map block sequence number 
           (i.e. the sequence number of the first map page) */
        mod->map_block[map_blocks_found] = block;
        map_block_seq[map_blocks_found] = map_page->sequence;
        map_blocks_found++;

        /* has found all map blocks? */
        if (map_blocks_found >= NUMBER_OF_MAP_BLOCKS) break;
    }

    if (map_blocks_found == 0) {
        /* no map blocks found; format required */
        return(FM_SUCCESS);
    }

    mod->state = BM_OPEN;

get_latest_map_block:
    /* find the most up-to-date map block, i.e. a map block 
       that has the largest sequence number */
    mod->sequence = 0;
    for (i = 0; i < NUMBER_OF_MAP_BLOCKS; i++) {
        if (mod->sequence < map_block_seq[i]) {
            mod->sequence = map_block_seq[i];
            mod->cur_map_block_idx = i;
        }
    }

    if (mod->sequence == 0) {
        /* no valid map blocks found; re-format required */
        return(FM_SUCCESS);
    }

    /* the latest map block is the current map block */
    block = mod->map_block[mod->cur_map_block_idx];
    
    for (i = 0; i < mod->num_map_pages; i++) seq[i] = 0;
    
    /* read map pages from the current map block */
    for (page = 0; page < pdev->chip_spec.pages_per_block; page++) {

        /* read one page */
        err = read_map_page(pdev, block, page);
        if (err != FM_SUCCESS) break;
        
        /* a valid map page found; check its sequence number */
        if (seq[map_page->map_offset] < map_page->sequence) {
            
            /* up-to-date map found; keep this in memory */
            for (i = 0, j = map_page->map_offset * BADBLK_ENTRIES_PER_PAGE;
                 i < BADBLK_ENTRIES_PER_PAGE && j < mod->swap_table_size; 
                 i++, j++) {
                mod->swap_table[j] = map_page->swap_table[i];
            }
            seq[map_page->map_offset] = map_page->sequence;
            if (map_page->too_many_bad_blocks) mod->too_many_bad_blocks = TRUE;
        }
    }
    
    /* check if all swap table segments are found */
    for (i = 0; i < mod->num_map_pages; i++) {
        if (seq[i] == 0) {
            /* some part of the map pages are missing;
               this means that the current map block is corrupt;
               try to find the map pages in another map block */
            map_block_seq[mod->cur_map_block_idx] = 0;
            goto get_latest_map_block;
        }
        if (mod->sequence < seq[i]) {
            mod->sequence = seq[i];
        }
    }
    
    /* if we need to find more map blocks, do it here */
    if (map_blocks_found < NUMBER_OF_MAP_BLOCKS) {

        /* start the map block scan all over again using the swap table */
        for (i = 0, j = 0; i < mod->swap_table_size; i++) {
            
            /* is this swap table entry indicates a map block? */
            if (mod->swap_table[i] == ENTRY_MAP_BLOCK) {
                mod->map_block[j] = GET_BLK_NUMBER(i);
                if (mod->map_block[j] == block) mod->cur_map_block_idx = j;
                j++;
                if (j >= NUMBER_OF_MAP_BLOCKS) break;
            }
        }

        if (j < NUMBER_OF_MAP_BLOCKS) {
            /* some of map blocks are missing; re-format required */
            return(FM_SUCCESS);
        }
    }

    /* set the current map page number to be the last page number,
       so that updated map pages could be written to a new map block */
    mod->cur_map_page_num = pdev->chip_spec.pages_per_block - 1;

    /* construct the hash table indexing the swap table */
    construct_fast_lookup_table(mod);

end:
    /* low-level BM format identified */
    mod->state = BM_FORMATTED;
    
    printf("[CFD] chip-%d: # of bad blocks = %d (reserved max. %d)\r\n", 
           (int)chip_id,
           (int)bm_get_num_bad_blocks(chip_id), 
           (int)pdev->chip_spec.max_num_bad_blocks);
           
    if (mod->too_many_bad_blocks) {
        printf("[CFD] chip-%d: CRITICAL ERROR -- TOO MANY BAD BLOCKS !!!\r\n", chip_id);
    }
    
    return(FM_SUCCESS);
}


extern int32_t    
bm_format(uint16_t chip_id, bool_t forced)
{
    pfdev_t  *pdev;
    bm_mod_t *mod;
    int32_t  err;
    uint32_t block;
    uint16_t i, good_blocks_found;
    
    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];
    mod = &bm_mod[chip_id];
    
    /* if the max number of bad blocks for this chip is 0, just return here */
    if (pdev->chip_spec.max_num_bad_blocks == 0) return(FM_SUCCESS);
    
    /* if not open yet, open the device first */
    if (mod->state == BM_CLOSED) {
        err = bm_open(chip_id);
        if (err) return(err);
    }
    
    /* already formatted? */
    if (mod->state == BM_FORMATTED && !forced) return(FM_SUCCESS);
    
    /* now, formatting starts; initialize bad block management module */
    init_bm_module(pdev, mod);
    mod->num_map_pages = (mod->swap_table_size - 1) / BADBLK_ENTRIES_PER_PAGE + 1;
    good_blocks_found = 0;
    
    /* starting from the last block, check all DLBM area blocks;
       for initial bad blocks in the area, mark them in the swap table;
       during the check, locate map blocks too */

    for (block = pdev->chip_spec.num_blocks - 1;
         block >= pdev->bm_area_start_block; block--) {
            
        /* is this a bad block? */
#if DETECT_FACTORY_BAD_BLK
        if (lld_is_bad_block(pdev, block)) {
#else 
        if (flash_erase_block(pdev, block) != FM_SUCCESS) {
#endif
            /* mark the corresponding swap table entry as a bad block */
            mod->swap_table[GET_BLK_OFFSET(block)] = ENTRY_BAD_BLOCK;
            continue;
        }
      
        /* a good block found; register it as a map block if necessary */
        if (good_blocks_found < NUMBER_OF_MAP_BLOCKS) {
            mod->map_block[good_blocks_found] = block;
            good_blocks_found++;

            /* mark the corresponding swap table entry as a map block */
            mod->swap_table[GET_BLK_OFFSET(block)] = ENTRY_MAP_BLOCK;
        }
    }
    
    /* check if a sufficient number of map blocks are found */
    if (good_blocks_found < NUMBER_OF_MAP_BLOCKS) return(FM_INIT_FAIL);

    /* starting from the 'initial_bad_block_scan_start' block of the chip, 
       check all blocks; if a initial bad block found, register it 
       in the swap table */
    for (block = mod->initial_bad_block_scan_start, i = 0;
         block < pdev->bm_area_start_block;
         block++) {

        /* is this a bad block? */
#if DETECT_FACTORY_BAD_BLK
        if (lld_is_bad_block(pdev, block)) {
#else 
        if (flash_erase_block(pdev, block) != FM_SUCCESS) {
#endif
            /* find a free block entry in the bad block swap table */
            while (mod->swap_table[i] != ENTRY_FREE_BLOCK) {

                if (i == pdev->chip_spec.num_blocks - 1) {
                    /* too many initial bad blocks */
                    return(FM_INIT_FAIL);
                }
                i++;
            }
            
            /* register this block in the swap table */
            mod->swap_table[i] = block;
        }
    }

    /* construct the hash table indexing the swap table */
    construct_fast_lookup_table(mod);

    /* store the bad block swap table into each of the map blocks */
    for (i = 0; i < NUMBER_OF_MAP_BLOCKS; i++) {
        mod->cur_map_block_idx = i;
        mod->cur_map_page_num = pdev->chip_spec.pages_per_block - 1;
        err = save_swap_table(pdev, mod, 0);
        if (err) return(FM_INIT_FAIL);
    }

    mod->state = BM_FORMATTED;
    return(FM_SUCCESS);
}


extern bool_t   
bm_is_formatted(uint16_t chip_id)
{
    if (bm_mod[chip_id].state == BM_FORMATTED) {
        return(TRUE);
    }

    return(FALSE);
}


extern int32_t  
bm_get_num_bad_blocks(uint16_t chip_id)
{
    bm_mod_t *mod;
    uint32_t i;
    int32_t  n = 0;
    
    /* existing chip id? */
    if (chip_id >= pdev_count) return(-1);
    mod = &bm_mod[chip_id];
    
    /* formatted? */
    if (mod->state != BM_FORMATTED) return(-1);
    
    for (i = 0; i < mod->swap_table_size; i++) {
        
        /* check if a bad block is registered in this swap table entry, or
           if the corresponding block for this entry is a bad block itself */
        if (IS_SWAP_ENTRY(mod->swap_table[i]) ||
            (mod->swap_table[i] & BLK_NUMBER_MASK) == ENTRY_BAD_BLOCK) {
            n++;
        }
    }
    
    return(n);
}


extern int32_t    
bm_swap_write_bad_block(uint16_t chip_id, uint32_t block, uint16_t page)
{
    pfdev_t  *pdev;
    bm_mod_t *mod;
    uint16_t i, j;
    uint32_t new_block, org_block;
    int32_t  err;
    
    pdev = &PDev[chip_id];
    mod = &bm_mod[chip_id];
    
    /* if the max number of bad blocks for this chip is 0, just return here */
    if (pdev->chip_spec.max_num_bad_blocks == 0) return(FM_ERROR);
    
    /* 1) find a free block to replace this block with;
       2) erase the free block found;
       3) copy valid pages in the old block to the new block */
       
get_free_block:
    /* find a free block entry from the swap table */
    i = get_swap_table_free_block_entry(mod);
    if (i == END_OF_LIST) {
        /* there's no free block to replace this block with */
        err = no_free_block_panic(pdev, mod);
        if (err == FM_TRY_AGAIN) goto get_free_block;
        else return(err);
    }
    
    /* a new free block found */
    new_block = GET_BLK_NUMBER(i);

    /* erase the free block first */
    err = flash_erase_block(pdev, new_block);
    if (err) {
        /* erase failed; 
           for an erase bad block, some delay should be inserted before
           bad block handling; at power off time, an erase operation may 
           fail (thus a false bad block detected) but a write operation 
           for saving the updated swap table in flash memory may succeed;
           delay is inserted to prevent such incorrect processing */
        DELAY_IN_USEC(DELAY_FOR_ERASE);
    
        /* mark the corresponding swap table entry as a bad block */
        mod->swap_table[i] = ((uint32_t)OP_ERASE << OP_INFO_SHIFT) 
                             | ENTRY_BAD_BLOCK;
        
        /* save the swap table */
        err = save_swap_table(pdev, mod, MAP_OFFSET(i));
        if (err) return(FM_ERROR);
        
        /* find another free block */
        goto get_free_block;
    }

    /* erase done; copy valid pages in the old block to the new block */
    for (j = 0; j < pdev->chip_spec.pages_per_block; j++) {
        
#if (COPY_WRITE_BAD_PAGE == 0)
        /* skip the page for which write failed */
        if (j == page) continue;
#endif
        /* copy one page */
        err = flash_copy_page(pdev, block, new_block, j);
        if (err) {
            /* write failed;
               mark the corresponding swap table entry as a bad block */
            mod->swap_table[i] = ((uint32_t)OP_WRITE << OP_INFO_SHIFT) 
                                 | ENTRY_BAD_BLOCK;
            
            /* save the swap table */
            err = save_swap_table(pdev, mod, MAP_OFFSET(i));
            if (err) return(FM_ERROR);
            
            /* find another free block */
            goto get_free_block;
        }
    }
    
    /* ok, all valid pages have been successfully copied;
       now, replace this block with the new block;
       this is done by registering this block in the swap table */
    if (block < pdev->bm_area_start_block) {

        /* annotate the bad block number with information about the 
           operation that caused the block to be bad */
        mod->swap_table[i] = ((uint32_t)OP_WRITE << OP_INFO_SHIFT) | block;
    }
    else {
        /* a replacement block itself has become a bad block;
           find the original block swapped with the replacement block */
        j = (uint16_t) GET_BLK_OFFSET(block);
        org_block = mod->swap_table[j];
        
        /* mark the replacement block itself as bad */
        mod->swap_table[j] = ((uint32_t)OP_WRITE << OP_INFO_SHIFT) 
                             | ENTRY_BAD_BLOCK;
        
        /* save the swap table in flash memory if necessary */
        if (MAP_OFFSET(j) != MAP_OFFSET(i)) {
            if (save_swap_table(pdev, mod, MAP_OFFSET(j)) != FM_SUCCESS)
                return(FM_ERROR);
        }
        
        /* register the original block in the current swap table entry */
        mod->swap_table[i] = org_block;
    }
    
    /* update the swap table hash lists */
    construct_fast_lookup_table(mod);
    
    /* save the modified part of the swap table in flash memory */
    return(save_swap_table(pdev, mod, MAP_OFFSET(i)));
}


extern int32_t    
bm_swap_erase_bad_block(uint16_t chip_id, uint32_t block)
{
    pfdev_t  *pdev;
    bm_mod_t *mod;
    uint16_t i, j;
    uint32_t org_block;
    int32_t  err;

    pdev = &PDev[chip_id];
    mod = &bm_mod[chip_id];
    
    /* if the max number of bad blocks for this chip is 0, just return here */
    if (pdev->chip_spec.max_num_bad_blocks == 0) return(FM_ERROR);

    /* for an erase bad block, some delay should be inserted before
       bad block handling; at power off time, an erase operation may 
       fail (thus a false bad block detected) but a write operation 
       for saving the updated swap table in flash memory may succeed;
       delay is inserted to prevent such incorrect processing */
    DELAY_IN_USEC(DELAY_FOR_ERASE);

get_free_block:
    /* find a free block entry from the swap table */
    i = get_swap_table_free_block_entry(mod);
    if (i == END_OF_LIST) {
        /* there's no free block to replace this block with */
        err = no_free_block_panic(pdev, mod);
        if (err == FM_TRY_AGAIN) goto get_free_block;
        else return(err);
    }
    
    /* replace this block with the free block found;
       this is done by registering this block in the swap table */
    if (block < pdev->bm_area_start_block) {

        /* annotate the bad block number with information about the 
           operation that caused the block to be bad */
        mod->swap_table[i] = ((uint32_t)OP_ERASE << OP_INFO_SHIFT) | block;
    }
    else {
        /* a replacement block itself has become a bad block;
           find the original block swapped with the replacement block */
        j = (uint16_t) GET_BLK_OFFSET(block);
        org_block = mod->swap_table[j];
        
        /* mark the replacement block itself as bad */
        mod->swap_table[j] = ((uint32_t)OP_ERASE << OP_INFO_SHIFT) 
                             | ENTRY_BAD_BLOCK;
        
        /* save the swap table in flash memory if necessary */
        if (MAP_OFFSET(j) != MAP_OFFSET(i)) {
            if (save_swap_table(pdev, mod, MAP_OFFSET(j)) != FM_SUCCESS)
                return(FM_ERROR);
        }
        
        /* register the original block in the current swap table entry */
        mod->swap_table[i] = org_block;
    }

    /* update the swap table hash lists */
    construct_fast_lookup_table(mod);
    
    /* save the modified part of the swap table in flash memory */
    return(save_swap_table(pdev, mod, MAP_OFFSET(i)));
}


extern uint32_t   
bm_get_swapping_block(uint16_t chip_id, uint32_t block)
{
    register pfdev_t  *pdev;
    register bm_mod_t *mod;
    register uint16_t i;

    uint16_t hash;

    pdev = &PDev[chip_id];
    mod = &bm_mod[chip_id];
    
    /* if the max number of bad blocks for this chip is 0, just return here */
    if (pdev->chip_spec.max_num_bad_blocks == 0) return(block);

    /* look up the swap table */
    hash = (uint16_t)(block & (SWAP_TABLE_HASH_SIZE - 1));
    for (i = mod->swap_table_hash_list[hash]; i != END_OF_LIST;
         i = mod->next_link[i]) {
    
        /* check if this block is registered as a bad block */
        if (block == (mod->swap_table[i] & BLK_NUMBER_MASK)) {
            
            /* found; return the replacement block */
            return(GET_BLK_NUMBER(i));
        }
    }
    
    /* this block has not been registered as a bad block;
       simply return the original block number */
    return(block);
}


extern uint32_t 
bm_get_original_block(uint16_t chip_id, uint32_t block)
{
    pfdev_t  *pdev = &PDev[chip_id];
    bm_mod_t *mod = &bm_mod[chip_id];

    /* if the given block is not in the DLBM area, just return it */
    if (block < pdev->bm_area_start_block) {
        return(block);
    }

    return(mod->swap_table[GET_BLK_OFFSET(block)] & BLK_NUMBER_MASK);
}


extern void
bm_set_initial_bad_block_scan_start(uint16_t chip_id, uint32_t block)
{
    bm_mod_t *mod = &bm_mod[chip_id];
    
    mod->initial_bad_block_scan_start = block;
}


extern void
bm_mark_bad_block(pfdev_t *pdev, uint32_t block)
{
    /* mark the block as bad ** in flash memory **;
       the function result is ignored here because it might fail */
    MEMSET(pbuf, 0x00, 16);
    flash_write_page(pdev, block, 0, NULL, pbuf);
    flash_write_page(pdev, block, (uint16_t)(pdev->chip_spec.pages_per_block - 1),
                     NULL, pbuf);
}


/*======================================================================*/
/*  Local Function Definitions                                          */
/*======================================================================*/

static void
init_bm_module(pfdev_t *pdev, bm_mod_t *mod)
{
    uint32_t i;

    mod->sequence = 0;
    mod->state = BM_CLOSED;

    for (i = 0; i < mod->swap_table_size; i++) {
        mod->swap_table[i] = ENTRY_FREE_BLOCK;
    }
}


static void
construct_fast_lookup_table(bm_mod_t *mod)
{
    uint16_t i, hash;
    
    /* initialize the hash lists */
    for (i = 0; i < SWAP_TABLE_HASH_SIZE; i++) {
        mod->swap_table_hash_list[i] = END_OF_LIST;
    }
    
    mod->free_block_list = END_OF_LIST;
    
    /* construct the hash lists */
    for (i = mod->swap_table_size - 1; ; i--) {
        
        /* check if a bad block is registered in this swap table entry */
        if (IS_SWAP_ENTRY(mod->swap_table[i])) {
            
            /* a registered bad block found; calculate hash function */
            hash = (uint16_t)(mod->swap_table[i] & (SWAP_TABLE_HASH_SIZE - 1));
            
            /* insert this entry in the swap table hash list */
            mod->next_link[i] = mod->swap_table_hash_list[hash];
            mod->swap_table_hash_list[hash] = i;
        }
        
        /* check if this is a free block entry */
        else if (mod->swap_table[i] == ENTRY_FREE_BLOCK) {
            
            /* insert this entry in the free block list;
               'free_block_list' is maintained in the ascending order
               of block offsets within the DLBM area */
            mod->next_link[i] = mod->free_block_list;
            mod->free_block_list = i;
        }

        if (i == 0) break;
    }
}


static uint16_t   
get_swap_table_free_block_entry(bm_mod_t *mod)
{
    uint16_t i;
    
    i = mod->free_block_list;
    if (i != END_OF_LIST) {
        
        /* free block entry found; detach this entry from the list */
        mod->free_block_list = mod->next_link[i];
    }
    
    return(i);
}


static uint16_t   
get_swap_table_last_free_block_entry(bm_mod_t *mod)
{
    uint16_t i, prev_i;
    
    if (mod->free_block_list == END_OF_LIST) return(END_OF_LIST);
    
    /* go to the end of the list */
    for (i = mod->free_block_list, prev_i = END_OF_LIST;
         mod->next_link[i] != END_OF_LIST; 
         prev_i = i, i = mod->next_link[i]);

    /* free block entry found; detach this entry from the list */
    if (prev_i == END_OF_LIST) {
        mod->free_block_list = END_OF_LIST;
    }
    else {
        mod->next_link[prev_i] = END_OF_LIST;
    }
    
    return(i);
}


static int32_t    
save_swap_table(pfdev_t *pdev, bm_mod_t *mod, uint16_t map_offset)
{
    int32_t  err = FM_ERROR;
    uint32_t cur_map_block, bad_block = 0;
    uint16_t i, page;
    bool_t   bad_block_marking_delayed = FALSE;
    
    /* identify the next map page to write */
    mod->cur_map_page_num++;
    mod->cur_map_page_num %= pdev->chip_spec.pages_per_block;
    cur_map_block = mod->map_block[mod->cur_map_block_idx];
    
    /* check if a map page should be written in the next map block */
    if (mod->cur_map_page_num == 0) {
        
        /* get the next map block */
        mod->cur_map_block_idx++;
        mod->cur_map_block_idx %= NUMBER_OF_MAP_BLOCKS;
        cur_map_block = mod->map_block[mod->cur_map_block_idx];

erase_block:
        /* current map block changed; erase this new map block first */
        err = flash_erase_block(pdev, cur_map_block);
        if (err) {
            /* erase failed; 
               for an erase bad block, some delay should be inserted before
               bad block handling; at power off time, an erase operation may
               fail (thus a false bad block detected) but a write operation 
               for saving the updated swap table in flash memory may succeed;
               delay is inserted to prevent such incorrect processing */
            DELAY_IN_USEC(DELAY_FOR_ERASE);
        
            /* explicit bad block marking is required for this map block */
            bm_mark_bad_block(pdev, cur_map_block);
            
            /* mark the corresponding swap table entry as a bad block */
            mod->swap_table[GET_BLK_OFFSET(cur_map_block)] = 
                ((uint32_t)OP_ERASE << OP_INFO_SHIFT) | ENTRY_BAD_BLOCK;

get_free_block:
            /* find a free block to replace this bad map block with */
            i = get_swap_table_last_free_block_entry(mod);
            if (i == END_OF_LIST) {
                /* there's no more free block; critical error!! */
                err = no_free_block_panic(pdev, mod);
                if (err == FM_TRY_AGAIN) goto get_free_block;
                else return(err);
            }
            
            /* found; allocate this free block to save map data */
            mod->swap_table[i] = ENTRY_MAP_BLOCK;
            
            /* register this new map block */
            cur_map_block = GET_BLK_NUMBER(i);
            mod->map_block[mod->cur_map_block_idx] = cur_map_block;

            /* ok, try to erase the current map block again */
            goto erase_block;
        }

        /* for the ease of maintenance, all map pages should be saved in 
           each map block; so write all map pages in this new map block */
        for (page = 0; page < mod->num_map_pages; page++) {
            err = write_map_page(pdev, mod, page, cur_map_block, page);
            if (err) {
                /* write failed; explicit bad block marking is required 
                   for this map block */
                bm_mark_bad_block(pdev, cur_map_block);

                /* mark the corresponding swap table entry as a bad block */
                mod->swap_table[GET_BLK_OFFSET(cur_map_block)] = 
                    ((uint32_t)OP_WRITE << OP_INFO_SHIFT) | ENTRY_BAD_BLOCK;
                    
                /* try to allocate a new map block again */
                goto get_free_block;
            }
        }
        
        /* keep the current map page number (i.e. last page number saved) */
        mod->cur_map_page_num = page - 1;
    }
    
    else {
        /* the given map page can be saved in the current map block */
        err = write_map_page(pdev, mod, map_offset, cur_map_block, 
                             mod->cur_map_page_num);
        if (err) {
            /* write failed; 
               explicit bad block marking is required for this map block,
               but this bad block marking should be delayed until a new map
               block is allocated and all map pages are completely saved in
               the new map block */
            bad_block_marking_delayed = TRUE;
            bad_block = cur_map_block;
               
            /* mark the corresponding swap table entry as a bad block */
            mod->swap_table[GET_BLK_OFFSET(cur_map_block)] = 
                ((uint32_t)OP_WRITE << OP_INFO_SHIFT) | ENTRY_BAD_BLOCK;
                
            /* try to allocate a new map block again */
            goto get_free_block;
        }
    }

    /* if an explicit bad block marking has been delayed, do it now */
    if (bad_block_marking_delayed) {
        bm_mark_bad_block(pdev, bad_block);
    }
    
    return(FM_SUCCESS);
}


static int32_t
read_map_page(pfdev_t *pdev, uint32_t block, uint16_t page)
{
    int32_t err;
    
    /* read the designated map page */
    err = lld_read_page(pdev, block, page, pbuf, NULL);
    if (err != FM_SUCCESS) return(err);
    
    /* check if this page is a valid map page */
    if (map_page->signature != MAP_PAGE_SIGNATURE) return(FM_ERROR);
    if (map_page->checksum != calc_checksum((uint32_t *)map_page, 
                                            SECTOR_SIZE - 4)) {
        return(FM_ERROR);
    }
    
    /* OK, a valid map page has been found */
    return(FM_SUCCESS);
}


static int32_t
write_map_page(pfdev_t *pdev, bm_mod_t *mod, uint16_t map_offset, 
               uint32_t block, uint16_t page)
{
    int32_t  err;
    uint32_t i, j;
    
    /* first, check if the target page is all 0xFF */
    err = lld_read_page(pdev, block, page, 
                        pbuf, pbuf + pdev->chip_spec.data_size);
    if (err) return(FM_ERROR);
    if (!is_all_FF(pbuf, pdev->chip_spec.page_size)) return(FM_ERROR);
    
    /* next, clear the map page buffer */
    MEMSET(pbuf, 0xff, pdev->chip_spec.page_size);
    
    /* now, fill the map page buffer */
    map_page->signature = MAP_PAGE_SIGNATURE;
    map_page->sequence  = ++(mod->sequence);
    map_page->map_offset = map_offset;
    for (i = 0, j = map_offset * BADBLK_ENTRIES_PER_PAGE;
         i < BADBLK_ENTRIES_PER_PAGE && j < mod->swap_table_size;
         i++, j++) {
        map_page->swap_table[i] = mod->swap_table[j];
    }
    map_page->too_many_bad_blocks = mod->too_many_bad_blocks;
    
    /* save the current version of the DLBM scheme;
       currently, it is not used and just cleared */
    MEMSET(map_page->version, 0, 12);
    
    /* calculate the checksum */
    map_page->checksum = calc_checksum((uint32_t *)map_page, SECTOR_SIZE - 4);
        
    /* finally, write the map page */
    return(flash_write_page(pdev, block, page,
                            pbuf, pbuf + pdev->chip_spec.data_size));
}


static int32_t
flash_write_page(pfdev_t *pdev, uint32_t block, uint16_t page, 
                 uint8_t *dbuf, uint8_t *sbuf)
{
    /* write the given data into the given page */
    if (lld_write_page(pdev, block, page, dbuf, sbuf, TRUE) != FM_SUCCESS) {
        return(FM_WRITE_ERROR);
    }
    
    /* for asynchronous devices, sync should also be performed here */
    if (lld_sync(pdev) != FM_SUCCESS) {
        pdev->prev_op.command = OP_NONE;
        return(FM_WRITE_ERROR);
    }
    
    return(FM_SUCCESS);
}

                            
static int32_t    
flash_copy_page(pfdev_t *pdev, uint32_t src_block, uint32_t dst_block, uint16_t page)
{
    /* read one page from the source block;
       in this case, it's needless to check the result */
    lld_read_page(pdev, src_block, page,
                  pbuf, pbuf + pdev->chip_spec.data_size);

    /* check if this page contains any valid data */
    if (is_all_FF(pbuf, pdev->chip_spec.page_size)) return(FM_SUCCESS);

    /* write the page into the destination block */
    return(flash_write_page(pdev, dst_block, page, 
                            pbuf, pbuf + pdev->chip_spec.data_size));
}


static int32_t
flash_erase_block(pfdev_t *pdev, uint32_t block)
{
    /* erase the given block */
    if (lld_erase(pdev, block) != FM_SUCCESS) {
        return(FM_ERASE_ERROR);
    }
    
    /* for asynchronous devices, sync should also be performed here */
    if (lld_sync(pdev) != FM_SUCCESS) {
        pdev->prev_op.command = OP_NONE;
        return(FM_ERASE_ERROR);
    }
    
    return(FM_SUCCESS);
}


static uint32_t
calc_checksum(uint32_t *data, uint16_t size)
{
    uint32_t checksum = 0;

    for ( ; size > 0; size -= 4, data++) {
        checksum += *data;
    }
    
    return(checksum);
}


static bool_t
is_all_FF(uint8_t *buf, uint32_t size)
{
    uint32_t i;
    
    for (i = 0; i < size; i++) {
        if (buf[i] != 0xFF) return(FALSE);
    }
    
    return(TRUE);
}


static int32_t
no_free_block_panic(pfdev_t *pdev, bm_mod_t *mod)
{
#if 1
    bool_t marked_in_flash = mod->too_many_bad_blocks;
    
    mod->too_many_bad_blocks = TRUE;
    if (! marked_in_flash) {
        save_swap_table(pdev, mod, 0);
    }
#else
    if (pdev->chip_id == 0) {
        /* if this device corresponds to chip 0, halt the system */
        while (1);
    }
#endif

    return(FM_ERROR);
}

#endif /* USE_DLBM */
#endif /* CFD_M */
