/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_physical.c                                             */
/*  PURPOSE : Code for Flash Device Physical Interface Layer (PFD)      */
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
#include "fd_if.h"
#include "fd_bm.h"
#include "fd_physical.h"
#include "fd_lld_wrapper.h"
#include "fd_utils.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable)                     */
/*----------------------------------------------------------------------*/

/* macro for calculating the plane number of a given block */

#define PFD_PLANE(block)        ((block) & (pdev->chip_spec.num_planes - 1))

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  External Variable Definitions                                       */
/*----------------------------------------------------------------------*/

pfdev_t     PDev[NUM_FLASH_CHIPS];            /* PFD table */
uint16_t    pdev_count;

bool_t      fd_ecc_corrected;   /* this variable is assigned TRUE or FALSE value
                                   in ecc.c and such value may be referenced
                                   in ftl.c to detect 1-bit error correction */

/*----------------------------------------------------------------------*/
/*  Local Variable Definitions                                          */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Local Function Declarations                                         */
/*----------------------------------------------------------------------*/

/* copy-back functions */

static int32_t do_copyback(pfdev_t *pdev, uint32_t src_block, uint16_t src_page, 
                        uint32_t dst_block, uint16_t dst_page, 
                        bool_t copyback_ok);

/* error recovery functions */

static int32_t err_recover(pfdev_t *pdev, uint32_t block, uint16_t page, int32_t err);

#if USE_DLBM
static int32_t recover_from_write_error(pfdev_t *pdev, uint32_t block, uint16_t page,
                        uint8_t *dbuf, uint8_t *sbuf);

static int32_t recover_from_copyback_error(pfdev_t *pdev, 
                        uint32_t src_block, uint16_t src_page, 
                        uint32_t dst_block, uint16_t dst_page);

static int32_t recover_from_erase_error(pfdev_t *pdev, uint32_t block);
#endif


/*======================================================================*/
/*  External Function Definitions                                       */
/*======================================================================*/

extern int32_t 
pfd_init(void)
{
    int32_t  err;
    uint16_t i;

    /* initialize the physical flash device table */
    for (i = 0; i < NUM_FLASH_CHIPS; i++) {
        MEMSET((void *)&PDev[i], 0, sizeof(pfdev_t) - 2*MAX_PAGE_SIZE);
        PDev[i].chip_id = i;
    }
    
    pdev_count = 0;
    
    /* initialize each LLD (Low-Level Device Driver) */
    for (i = 0; i < NUM_FLASH_CHIPS + 1; i++) {
        if (lld_init[i] == NULL) break;

        err = lld_init[i]();
        if (err) return(err);
    }

    return(FM_SUCCESS);
}


extern int32_t
pfd_shutdown(int32_t forced)
{
    uint16_t i;

#if 0
    if (! forced) {
#else
    if (1) {
#endif
        /* close each of the registered physical devices */
        for (i = 0; i < pdev_count; i++) {
            pfd_close(i);
        }
    }
    
    pdev_count = 0;
    
    return(FM_SUCCESS);
}


extern int32_t 
pfd_register_flash_chip(uint16_t local_chip_id, 
                        flash_chip_spec_t *chip_spec, 
                        flash_chip_ops_t *chip_ops)
{
    pfdev_t *pdev;
    
    if (pdev_count >= NUM_FLASH_CHIPS) return(FM_ERROR);
    if (chip_spec == NULL || chip_ops == NULL) return(FM_ERROR);
    
    /* check the availability of mandatory driver functions */
    if (chip_ops->open == NULL ||
        chip_ops->read_page == NULL ||
        chip_ops->read_bytes == NULL ||
        chip_ops->write_page == NULL ||
        chip_ops->write_bytes == NULL ||
        chip_ops->erase == NULL ||
        chip_ops->is_bad_block == NULL) {
        return(FM_ERROR);
    }
    
    /* register the given flash device in the table */
    pdev = &PDev[pdev_count++];
    pdev->local_chip_id = local_chip_id;
    MEMCPY((void *)&pdev->chip_spec, chip_spec, sizeof(flash_chip_spec_t));
    MEMCPY((void *)&pdev->chip_ops,  chip_ops,  sizeof(flash_chip_ops_t));
    
    /* calculate other device parameters */
#if USE_DLBM
    pdev->bm_area_num_blocks = bm_calc_bm_area_num_blocks(pdev->chip_id);
#else
    pdev->bm_area_num_blocks = 0;
#endif
    pdev->bm_area_start_block = pdev->chip_spec.num_blocks - pdev->bm_area_num_blocks;
    pdev->part_table_block = pdev->bm_area_start_block - 1;
    pdev->actual_num_blocks = pdev->part_table_block;

    pdev->prev_op.command = OP_NONE;
    return(FM_SUCCESS);
}


extern uint16_t    
pfd_get_num_chips(void)
{
    return(pdev_count);
}


extern pfdev_t *
pfd_get_device(uint16_t chip_id)
{
    return(&PDev[chip_id]);
}


extern int32_t    
pfd_open(uint16_t chip_id)
{
    pfdev_t *pdev;
    int32_t err;
    
    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];
    
    /* is this first open? */
    if (pdev->usage_count == 0) {
        err = lld_open(pdev);
    }
    else {
        err = FM_SUCCESS;
    }
    if (err == FM_SUCCESS) pdev->usage_count++;
    
    return(err);
}


extern int32_t    
pfd_close(uint16_t chip_id)
{
    pfdev_t *pdev;

    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];
    
    /* open now? */
    if (pdev->usage_count == 0) return(FM_DEVICE_NOT_OPEN);
    
    pdev->usage_count--;
    if (pdev->usage_count == 0) lld_close(pdev);
    
    return(FM_SUCCESS);
}


extern int32_t  
pfd_erase_all(uint16_t chip_id, bool_t skip_initial_bad)
{
    int32_t err;
    pfdev_t *pdev;
    uint32_t i, i_bad = 0, r_bad = 0;
    
    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];
    
    printf("------------------------------------------------------\r\n");
    printf("[CFD] Erasing all %d blocks in flash chip-%d ...\r\n", 
           (int)pdev->chip_spec.num_blocks, (int)chip_id); 
    
    for (i = 0; i < pdev->chip_spec.num_blocks; i++) {
        if (skip_initial_bad) {
            if (lld_is_bad_block(pdev, i)) {
                /* initial bad block... do not erase */
                i_bad++;
                printf("Block %d : an initial bad block.\r\n", (int)i);
                continue;
            } 
        }
        if ((err = lld_erase_verify(pdev, i)) == FM_SUCCESS) {
            continue;
        }
        else {
            pdev->prev_op.command = OP_NONE;
        }

        /* erase & verify failed; 
           write the bad block mark in the spare area of this block;
           then, continue to check the next blocks */
        bm_mark_bad_block(pdev, i);
        r_bad++;
        printf("Block %d : %s failed ... a run-time bad block.\r\n", (int)i,
               (err == FM_ERASE_ERROR || err == FM_PREV_ERASE_ERROR ? "erase" : "verify"));
    }
    
    printf("------------------------------------------------------\r\n");
    printf("[CFD] Total %d bad blocks found.\r\n", (int)(i_bad + r_bad));
    if (skip_initial_bad) {
        printf("[CFD] # of initial  bad blocks = %d\r\n", (int)i_bad);
        printf("[CFD] # of run-time bad blocks = %d\r\n", (int)r_bad);
    }
    printf("------------------------------------------------------\r\n");
    
    return(FM_SUCCESS);
}


/*----------------------------------------------------------------------*/
/*  Wrapping Functions for Common Physical I/O                          */
/*----------------------------------------------------------------------*/

extern int32_t  
pfd_read_page(pfdev_t *pdev, uint32_t block, uint16_t page,
              uint8_t *dbuf, uint8_t *sbuf)
{
    int32_t err;
    
#if USE_DLBM
    /* get the replacement block if this block is a bad block */
    block = bm_get_swapping_block(pdev->chip_id, block);
#endif

    /* perform the corresponding physical device operation */
    err = lld_read_page(pdev, block, page, dbuf, sbuf);
    
    /* perform error recovery if it is necessary and possible */
    if (err) {
        err = err_recover(pdev, block, page, err);
    }

    return(err);
}


extern int32_t
pfd_read_bytes(pfdev_t *pdev, uint32_t block, uint16_t page,
               uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t err;
    
#if USE_DLBM
    /* get the replacement block if this block is a bad block */
    block = bm_get_swapping_block(pdev->chip_id, block);
#endif

    /* perform the corresponding physical device operation */
    err = lld_read_bytes(pdev, block, page, num_bytes, dbuf);
    
    /* perform error recovery if it is necessary and possible */
    if (err) {
        err = err_recover(pdev, block, page, err);
    }

    return(err);
}


extern int32_t  
pfd_write_page(pfdev_t *pdev, uint32_t block, uint16_t page,
               uint8_t *dbuf, uint8_t *sbuf, bool_t is_last)
{
    int32_t err;
    
#if USE_DLBM
    /* get the replacement block if this block is a bad block */
    block = bm_get_swapping_block(pdev->chip_id, block);
#endif
    
    /* perform the corresponding physical device operation */
    err = lld_write_page(pdev, block, page, dbuf, sbuf, is_last);
    
    /* perform error recovery if it is necessary and possible */
    if (err) {
        err = err_recover(pdev, block, page, err);
    }

    return(err);
}


extern int32_t  
pfd_write_bytes(pfdev_t *pdev, uint32_t block, uint16_t page,
                uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t err;
    
#if USE_DLBM
    /* get the replacement block if this block is a bad block */
    block = bm_get_swapping_block(pdev->chip_id, block);
#endif
    
    /* perform the corresponding physical device operation */
    err = lld_write_bytes(pdev, block, page, num_bytes, dbuf);
    
    /* perform error recovery if it is necessary and possible */
    if (err) {
        err = err_recover(pdev, block, page, err);
    }

    return(err);
}


extern int32_t  
pfd_copyback(pfdev_t *pdev, 
             uint32_t src_block, uint16_t src_page, 
             uint32_t dst_block, uint16_t dst_page)
{
    int32_t  err;
    uint32_t blocks_per_die, src_b, dst_b, src_die, dst_die;
    bool_t   copyback_ok = TRUE;

#if USE_DLBM
    /* get replacement blocks if some of these blocks are bad blocks */
    src_block = bm_get_swapping_block(pdev->chip_id, src_block);
    dst_block = bm_get_swapping_block(pdev->chip_id, dst_block);
#endif    

    /* check if source & destination blocks are on the same plane */
    if (PFD_PLANE(src_block) != PFD_PLANE(dst_block)) copyback_ok = FALSE;
    
    /* if the physical device has more than one die but single chip 
       enable, there must be multiple groups of blocks across which 
       copy-backs cannot be performed */
    if (pdev->chip_spec.num_dies_per_ce > 1) {
        blocks_per_die = pdev->chip_spec.num_blocks >> 
                         BITS(pdev->chip_spec.num_dies_per_ce);
        for (src_b = src_block, src_die = 0; 
             src_b >= blocks_per_die; 
             src_b -= blocks_per_die, src_die++);
        for (dst_b = dst_block, dst_die = 0; 
             dst_b >= blocks_per_die; 
             dst_b -= blocks_per_die, dst_die++);
        if (src_die != dst_die) copyback_ok = FALSE;
    }

    /* check if copyback can be performed only between odd pages 
       or even pages */
    if (pdev->chip_spec.constraints & CN_ODD_EVEN_COPYBACK) {
        if ((src_page & 0x1) != (dst_page & 0x1)) copyback_ok = FALSE;
    }

    /* perform copy-back operation */
    err = do_copyback(pdev, src_block, src_page, 
                      dst_block, dst_page, copyback_ok);
    
    /* perform error recovery if it is necessary and possible */
    if (err) {
        err = err_recover(pdev, dst_block, dst_page, err);
    }
    
    return(err);
}


extern int32_t  
pfd_erase(pfdev_t *pdev, uint32_t block)
{
    int32_t err;
    
#if USE_DLBM
    /* get the replacement block if this block is a bad block */
    block = bm_get_swapping_block(pdev->chip_id, block);
#endif
    
    /* perform the corresponding physical device operation */
    err = lld_erase(pdev, block);

    /* perform error recovery if it is necessary and possible */
    if (err) {
        err = err_recover(pdev, block, 0, err);
    }

    return(err);
}


extern int32_t  
pfd_sync(pfdev_t *pdev)
{
    int32_t err;
    
    err = lld_sync(pdev);
    if (err) {
        err = err_recover(pdev, 0, 0, err);
    }
    
    return(err);
}


extern uint8_t *
pfd_get_temp_buf(pfdev_t *pdev)
{
    uint8_t *pbuf;
    
    pbuf = pdev->buf[pdev->buf_index];
    pdev->buf_index = 1 - pdev->buf_index;
    
    return(pbuf);
}


/*======================================================================*/
/*  Local Function Definitions                                          */
/*======================================================================*/

/*----------------------------------------------------------------------*/
/*  Copy-Back Functions                                                 */
/*----------------------------------------------------------------------*/

static int32_t    
do_copyback(pfdev_t *pdev, uint32_t src_block, uint16_t src_page, 
            uint32_t dst_block, uint16_t dst_page, bool_t copyback_ok)
{
    int32_t err;
    uint8_t *pbuf;

    /* check if the copy-back operation can be performed */
#if (COPYBACK_METHOD == HW_COPYBACK)
    if (LLD_DEFINED(copyback) && copyback_ok) {
#else
    if (0) {
#endif

#if (COPYBACK_METHOD == SAFE_HW_COPYBACK)
do_hw_copyback:
#endif
        /* perform the low-level device operation */
        err = lld_copyback(pdev, src_block, src_page,
                           dst_block, dst_page);
        if (err == FM_READ_ERROR) {
            /* we got an EDC (Error Detection Circuit) error returned from
               the flash memory chip; in this case, try to perform the 
               copy-back operation again using the S/W method */
            goto do_sw_copyback;
        }
    }
    else {
        /* implement 'copy-back' using read & write */

do_sw_copyback:        
        /* get a temporary I/O buffer */
        pbuf = pfd_get_temp_buf(pdev);
        
#if (COPYBACK_METHOD == SAFE_HW_COPYBACK)
        fd_ecc_corrected = FALSE;
#endif

        /* read the source page */
        err = lld_read_page(pdev, src_block, src_page,
                            pbuf, pbuf + pdev->chip_spec.data_size);
        if (err) return(err);
        
#if (COPYBACK_METHOD == SAFE_HW_COPYBACK)
        /* if copy-back is possible and there's no ECC error,
           perform the low-level copy-back operation;
           in this case, this copy-back operation can be thought of as safe 
           because the possibility that 2-bit error may occur during the 
           copy-back operation is very low */
        if (copyback_ok && !fd_ecc_corrected) {
            goto do_hw_copyback;
        }
#endif

        /* write data in the source page into the destination page */
        err = lld_write_page(pdev, dst_block, dst_page,
                             pbuf, pbuf + pdev->chip_spec.data_size, 
                             TRUE);
    }
    
    return(err);
}


/*----------------------------------------------------------------------*/
/*  Error Recovery Functions                                            */
/*----------------------------------------------------------------------*/
                       
static int32_t 
err_recover(pfdev_t *pdev, uint32_t block, uint16_t page, int32_t err)
{
#if USE_DLBM
    op_desc_t prev_op;

    MEMCPY(&prev_op, &pdev->prev_op, sizeof(op_desc_t));
#endif

    pdev->prev_op.command = OP_NONE;

#if USE_DLBM

    switch (err) {

    case FM_WRITE_ERROR:
        /* bad block detected during current write operation */
        err = bm_swap_write_bad_block(pdev->chip_id, block, page);
        if (!err) err = FM_TRY_AGAIN;
        break;
        
    case FM_ERASE_ERROR:
        /* bad block detected during current erase operation */
        err = bm_swap_erase_bad_block(pdev->chip_id, block);
        if (!err) err = FM_TRY_AGAIN;
        break;
        
    case FM_PREV_WRITE_ERROR:
        /* the previous operation should be 'write' or 'copyback' */
        if (prev_op.command == OP_WRITE) {
            err = recover_from_write_error(pdev, 
                                    prev_op.param.write.block,
                                    prev_op.param.write.page,
                                    prev_op.param.write.dbuf,
                                    prev_op.param.write.sbuf);
        }
        else if (prev_op.command == OP_COPYBACK) {
            err = recover_from_copyback_error(pdev, 
                                    prev_op.param.copyback.src_block,
                                    prev_op.param.copyback.src_page,
                                    prev_op.param.copyback.dst_block,
                                    prev_op.param.copyback.dst_page);
        }
        if (!err) err = FM_TRY_AGAIN;
        break;

    case FM_PREV_ERASE_ERROR:
        /* the previous operation should be 'erase' */
        if (prev_op.command == OP_ERASE) {
            err = recover_from_erase_error(pdev, 
                                    prev_op.param.erase.block);
        }
        if (!err) err = FM_TRY_AGAIN;
        break;

    default:
        /* some upper layer software will handle the other errors */
        printf("[PFD] unhandled error : %d\r\n", (int)err);
        break;
    }
#endif /* USE_DLBM */

    return(err);
}


#if USE_DLBM

static int32_t
recover_from_write_error(pfdev_t *pdev, uint32_t block, uint16_t page,
                         uint8_t *dbuf, uint8_t *sbuf)
{
    int32_t err;
    uint32_t org_block, phy_block;

    /* if the given block is a replacement block (for a bad block),
       we need to find out the original block number */
    org_block = bm_get_original_block(pdev->chip_id, block);
    
    /* register this block as a bad block and replace it here */
    err = bm_swap_write_bad_block(pdev->chip_id, block, page);
    if (!err) {

        /* ok, bad block has been replaced successfully;
           perform the previous write operation again */
        while (1) {
            phy_block = bm_get_swapping_block(pdev->chip_id, org_block);
            err = lld_write_page(pdev, phy_block, page, dbuf, sbuf, TRUE);
            if (!err) {
                err = lld_sync(pdev);
                if (!err) break;
                pdev->prev_op.command = OP_NONE;
            }

            /* error occurred again; perform recovery */
            if (err == FM_WRITE_ERROR) {
                err = bm_swap_write_bad_block(pdev->chip_id, phy_block, page);
            }
            if (err) break;
        }
    }
    
    return(err);
}


static int32_t
recover_from_copyback_error(pfdev_t *pdev, 
                            uint32_t src_block, uint16_t src_page, 
                            uint32_t dst_block, uint16_t dst_page)
{
    int32_t err;
    uint32_t org_block, phy_block;

    /* if the given block is a replacement block (for a bad block),
       we need to find out the original block number */
    org_block = bm_get_original_block(pdev->chip_id, dst_block);
    
    /* register this block as a bad one and replace it here */
    err = bm_swap_write_bad_block(pdev->chip_id, dst_block, dst_page);
    if (!err) {

        /* ok, bad block has been replaced successfully;
           perform the previous copy-back operation again */
        while (1) {
            phy_block = bm_get_swapping_block(pdev->chip_id, org_block);
            err = do_copyback(pdev, src_block, src_page,
                              phy_block, dst_page, FALSE);
            if (!err) {
                err = lld_sync(pdev);
                if (!err) break;
                pdev->prev_op.command = OP_NONE;
            }

            /* error occurred again; perform recovery */
            if (err == FM_WRITE_ERROR) {
                err = bm_swap_write_bad_block(pdev->chip_id, phy_block, dst_page);
            }
            if (err) break;
        }
    }

    return(err);
}


static int32_t
recover_from_erase_error(pfdev_t *pdev, uint32_t block)
{
    int32_t err;
    uint32_t org_block, phy_block;

    /* if the given block is a replacement block (for a bad block),
       we need to find out the original block number */
    org_block = bm_get_original_block(pdev->chip_id, block);
    
    /* register this block as a bad one and replace it here */
    err = bm_swap_erase_bad_block(pdev->chip_id, block);
    if (!err) {

        /* ok, bad block has been replaced successfully;
           perform the previous erase operation again */
        while (1) {
            phy_block = bm_get_swapping_block(pdev->chip_id, org_block);
            err = lld_erase(pdev, phy_block);
            if (!err) {
                err = lld_sync(pdev);
                if (!err) break;
                pdev->prev_op.command = OP_NONE;
            }

            /* error occurred again; perform recovery */
            if (err == FM_ERASE_ERROR) {
                err = bm_swap_erase_bad_block(pdev->chip_id, phy_block);
            }
            if (err) break;
        }
    }
    
    return(err);
}

#endif /* USE_DLBM */
#endif /* CFD_M */
