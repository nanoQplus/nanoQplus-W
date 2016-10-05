/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_if.c                                                   */
/*  PURPOSE : Code for Flash Device Common Interface                    */
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
#include "fd_logical.h"
#include "fd_bm.h"
#include "fd_physical.h"
#include "fd_utils.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Configurable)                         */
/*----------------------------------------------------------------------*/

/* CFD level locking (mutual exclusion) mechanism */

#define CFD_LOCK_INIT
#define CFD_LOCK
#define CFD_UNLOCK

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  External Variable Definitions                                       */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Local Variable Definitions                                          */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Local Function Declarations                                         */
/*----------------------------------------------------------------------*/

static int32_t sw_copy_back(pfdev_t *src_pdev, uint32_t src_block, uint16_t src_page, 
                            pfdev_t *dst_pdev, uint32_t dst_block, uint16_t dst_page);


/*======================================================================*/
/*  External Function Definitions                                       */
/*======================================================================*/

extern int32_t  
fd_init(void)
{
    int32_t  err;
    uint16_t i;
    flash_parttab_t part_tab;    

    /*--------------------------------------------------------------*/
    /*  first, initialize flash device driver modules               */
    /*--------------------------------------------------------------*/

    /* initialize physical flash device interface layer;
       in this function call, physical flash devices are installed */
    err = pfd_init();
    if (err) return(err);    

#if USE_DLBM
    /* initialize bad block management layer */
    err = bm_init();
    if (err) return(err);

    /* for each flash chip, scan initial bad blocks starting from block 0 */
    for (i = 0; i < pdev_count; i++) {
        bm_set_initial_bad_block_scan_start(i, 0);
    }
#endif

    /* initialize logical flash device interface layer */
    err = lfd_init();
    if (err) return(err);

    /*--------------------------------------------------------------*/
    /*  second, open the device & recognize logical device info      */
    /*--------------------------------------------------------------*/
    
    /* open each of the registered physical devices */
    for (i = 0; i < pdev_count; i++) {
        err = pfd_open(i);
        if (err) return(err);
    }

#if USE_DLBM
    /* perform BM open for each physical device */
    for (i = 0; i < pdev_count; i++) {
        err = bm_open(i);
        if (err) return(err);
    }

    /* perform BM format for each physical device;
       it's safe -- format would be bypassed if it already done */
    for (i = 0; i < pdev_count; i++) {
        err = bm_format(i, FALSE); 
        if (err) return(err);
    }
#endif

    /* read low-level flash partition information */
    for (i = 0; i < pdev_count; i++) {
        err = lfd_read_partition_table(i, &part_tab);
        if (err) return(err);

        /* if there is no low-level partition table yet, or
           if the existing partition table differs from the default 
           partition table, then save the default partition table */
#if 1
        if ((part_tab.num_partitions == 0)) {
#else
        if ((part_tab.num_partitions == 0) || 
            (MEMCMP(&part_tab, &default_part_table[i],
                    (uint32_t)&part_tab.part[part_tab.num_partitions] -
                    (uint32_t)&part_tab) != 0)) {
#endif
            err = lfd_write_partition_table(i, &default_part_table[i]);
            if (err) return(err);
        }
    }

    CFD_LOCK_INIT;

    printf("[CFD] fd_init() finished successfully.\r\n"); 
    return(FM_SUCCESS);
}


extern int32_t  
fd_shutdown(int32_t forced)
{
    lfd_shutdown(forced);

#if USE_DLBM
    bm_shutdown(forced);
#endif

    pfd_shutdown(forced);
    
    return(FM_SUCCESS);
}


/*----------------------------------------------------------------------*/
/*  Flash Memory Device Driver APIs                                     */
/*----------------------------------------------------------------------*/

extern int32_t  
fd_open(uint32_t dev_id)
{
    int32_t  err;
    lfdev_t  *ldev;

    /* check if it is already open */
    ldev = lfd_get_device(dev_id);
    if (ldev != NULL) {
        ldev->usage_count++;
        return(FM_SUCCESS);
    }

    CFD_LOCK;    
    err = lfd_open(dev_id);
    CFD_UNLOCK; 
    
    return(err);
}


extern int32_t
fd_close(uint32_t dev_id)
{
    int32_t err;
    
    CFD_LOCK;
    err = lfd_close(dev_id);
    CFD_UNLOCK;
    
    return(err);
}


extern int32_t  
fd_read_page(uint32_t dev_id, uint32_t block, uint16_t page,
             uint8_t *dbuf, uint8_t *sbuf)
{
    int32_t       err;
    uint32_t      i, first_block;
    lfdev_t       *ldev;
    pfdev_t       *pdev;
    flash_part_t  *part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

#if STRICT_CHECK
    /* check if the block and page numbers are within the valid ranges */
    if (block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
    if (page >= ldev->dev_spec.pages_per_block) return(FM_ILLEGAL_ACCESS);
#endif

    /* identify the first real block number in this logical block */
    first_block = block << BITS(ldev->interleaving_level);
    
    /* perform the operation for each interleaved partition */
    for (i = 0; i < ldev->interleaving_level; i++) {
        
        /* calculate the block number in each partition */
        block = first_block + i;
        
        /* get the information about the partition where this block exists;
           the block number is also adjusted as a relative value 
           in the corresponding partition */
        part_info = lfd_get_part_info(ldev, &block);
        pdev = part_info->pdev;
        part = part_info->part;
        
        /* get the block number in the chip-wide address space */
        block += part->start_block;
        
        /* perform the physical device operation until it succeeds or fails */
        do {
            err = pfd_read_page(pdev, block, page, dbuf, sbuf);
        } while (err == FM_TRY_AGAIN);
        
        /* if an error occurs (ex. ECC_ERROR), return immediately */
        if (err) break;
        
        /* adjust buffer pointers */
        dbuf += pdev->chip_spec.data_size;
        sbuf += pdev->chip_spec.spare_size;
    }

    CFD_UNLOCK;
    return(err);
}


extern int32_t
fd_read_bytes(uint32_t dev_id, uint32_t block, uint16_t page,
              uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t       err;
    lfdev_t       *ldev;
    pfdev_t       *pdev;
    flash_part_t  *part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

#if STRICT_CHECK
    /* check if the block and page numbers are within the valid ranges */
    if (block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
    if (page >= ldev->dev_spec.pages_per_block) return(FM_ILLEGAL_ACCESS);
#endif

    /* we need to read only the leading bytes in this logical page;
       identify the first real block number in this logical block */
    block <<= BITS(ldev->interleaving_level);

    /* get the information about the partition where this block exists;
       the block number is also adjusted as a relative value 
       in the corresponding partition */
    part_info = lfd_get_part_info(ldev, &block);
    pdev = part_info->pdev;
    part = part_info->part;
    
    /* get the block number in the chip-wide address space */
    block += part->start_block;
    
    /* perform the physical device operation until it succeeds or fails */
    do {
        err = pfd_read_bytes(pdev, block, page, num_bytes, dbuf);
    } while (err == FM_TRY_AGAIN);

    CFD_UNLOCK;
    return(err);
}


extern int32_t  
fd_write_page(uint32_t dev_id, uint32_t block, uint16_t page,
              uint8_t *dbuf, uint8_t *sbuf, bool_t is_last)
{
    int32_t       err;
    uint32_t      i, first_block;
    lfdev_t       *ldev;
    pfdev_t       *pdev;
    flash_part_t  *part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

#if STRICT_CHECK
    /* check if the block and page numbers are within the valid ranges */
    if (block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
    if (page >= ldev->dev_spec.pages_per_block) return(FM_ILLEGAL_ACCESS);
#endif
    
    /* identify the first real block number in this logical block */
    first_block = block << BITS(ldev->interleaving_level);
    
    /* perform the operation for each interleaved partition */
    for (i = 0; i < ldev->interleaving_level; i++) {
        
        /* calculate the block number in each partition */
        block = first_block + i;
        
        /* get the information about the partition where this block exists;
           the block number is also adjusted as a relative value 
           in the corresponding partition */
        part_info = lfd_get_part_info(ldev, &block);
        pdev = part_info->pdev;
        part = part_info->part;
        
        /* check if this logical device is write protected */
        if (part->protected) {
            CFD_UNLOCK;
            return(FM_PROTECT_ERROR);
        }
        
        /* get the block number in the chip-wide address space */
        block += part->start_block;
        
        /* perform the physical device operation until it succeeds or fails */
        do {
            err = pfd_write_page(pdev, block, page, dbuf, sbuf, is_last);
        } while (err == FM_TRY_AGAIN);
        
        /* if an error occurs, return immediately */
        if (err) break;
        
        /* adjust buffer pointers */
        dbuf += pdev->chip_spec.data_size;
        sbuf += pdev->chip_spec.spare_size;
    }

    CFD_UNLOCK;
    return(err);
}


extern int32_t
fd_write_bytes(uint32_t dev_id, uint32_t block, uint16_t page,
               uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t       err;
    lfdev_t       *ldev;
    pfdev_t       *pdev;
    flash_part_t  *part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

#if STRICT_CHECK
    /* check if the block and page numbers are within the valid ranges */
    if (block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
    if (page >= ldev->dev_spec.pages_per_block) return(FM_ILLEGAL_ACCESS);
#endif

    /* we need to write only to the leading bytes in this logical page;
       identify the first real block number in this logical block */
    block <<= BITS(ldev->interleaving_level);

    /* get the information about the partition where this block exists;
       the block number is also adjusted as a relative value 
       in the corresponding partition */
    part_info = lfd_get_part_info(ldev, &block);
    pdev = part_info->pdev;
    part = part_info->part;
    
    /* check if this logical device is write protected */
    if (part->protected) {
        CFD_UNLOCK;
        return(FM_PROTECT_ERROR);
    }
    
    /* get the block number in the chip-wide address space */
    block += part->start_block;
    
    /* perform the physical device operation until it succeeds or fails */
    do {
        err = pfd_write_bytes(pdev, block, page, num_bytes, dbuf);
    } while (err == FM_TRY_AGAIN);

    CFD_UNLOCK;
    return(err);
}


extern int32_t  
fd_copyback(uint32_t dev_id, uint32_t src_block, uint16_t src_page, 
            uint32_t dst_block, uint16_t dst_page)
{
    int32_t       err;
    uint32_t      i, first_src_block, first_dst_block;
    lfdev_t       *ldev;
    pfdev_t       *src_pdev, *dst_pdev;
    flash_part_t  *src_part, *dst_part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

#if STRICT_CHECK
    /* check if the block and page numbers are within the valid ranges */
    if (src_block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
    if (dst_block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
    if (src_page >= ldev->dev_spec.pages_per_block) return(FM_ILLEGAL_ACCESS);
    if (dst_page >= ldev->dev_spec.pages_per_block) return(FM_ILLEGAL_ACCESS);
#endif
    
    /* identify the first real block numbers in logical blocks */
    first_src_block = src_block << BITS(ldev->interleaving_level);
    first_dst_block = dst_block << BITS(ldev->interleaving_level);
    
    /* perform the operation for each interleaved partition */
    for (i = 0; i < ldev->interleaving_level; i++) {
        
        /* calculate the block number in each partition */
        src_block = first_src_block + i;
        dst_block = first_dst_block + i;
        
        /* get the partition information for this logical device; 
           the block number is also adjusted as a relative value 
           in the corresponding partition */
        part_info = lfd_get_part_info(ldev, &src_block);
        src_pdev  = part_info->pdev;
        src_part  = part_info->part;    
        
        part_info = lfd_get_part_info(ldev, &dst_block);
        dst_pdev  = part_info->pdev;
        dst_part  = part_info->part;
        
        /* check if the target partition is write protected */
        if (dst_part->protected) {
            CFD_UNLOCK;
            return(FM_PROTECT_ERROR);
        }
    
        /* get the block numbers in the chip-wide address space */
        src_block += src_part->start_block;
        dst_block += dst_part->start_block;
        
        if (src_pdev == dst_pdev) {
            /* source & destination blocks are on the same chip;
               therefore, the copy-back operation can be used */
            do {
                err = pfd_copyback(src_pdev, src_block, src_page, 
                                   dst_block, dst_page);
            } while (err == FM_TRY_AGAIN);
        }
        else {
            /* source & destination blocks are on different chips;
               implement 'copy-back' using read & write */
            err = sw_copy_back(src_pdev, src_block, src_page, 
                               dst_pdev, dst_block, dst_page);
        }
        
        /* if an error occurs, return immediately */
        if (err) break;
    }

    CFD_UNLOCK;
    return(err);
}


extern int32_t  
fd_erase(uint32_t dev_id, uint32_t block)
{
    int32_t       err;
    uint32_t      i, first_block;
    lfdev_t       *ldev;
    pfdev_t       *pdev;
    flash_part_t  *part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

#if STRICT_CHECK
    /* check if the block number is within the valid range */
    if (block >= ldev->dev_spec.num_blocks) return(FM_ILLEGAL_ACCESS);
#endif

    /* identify the first real block number in this logical block */
    first_block = block << BITS(ldev->interleaving_level);
    
    /* perform the operation for each interleaved partition */
    for (i = 0; i < ldev->interleaving_level; i++) {
        
        /* calculate the block number in each partition */
        block = first_block + i;
        
        /* get the partition information for this logical device; 
           the block number is also adjusted as a relative value 
           in the corresponding partition */
        part_info = lfd_get_part_info(ldev, &block);
        pdev = part_info->pdev;
        part = part_info->part;
    
        /* check if this logical device is write protected */
        if (part->protected) {
            CFD_UNLOCK;
            return(FM_PROTECT_ERROR);
        }
    
        /* get the block number in the chip-wide address space */
        block += part->start_block;
    
        /* perform the physical device operation until it succeeds or fails */
        do {
            err = pfd_erase(pdev, block);
        } while (err == FM_TRY_AGAIN);
        
        /* if an error occurs, return immediately */
        if (err) break;
    }

    CFD_UNLOCK;
    return(err);
}


extern int32_t  
fd_sync(uint32_t dev_id)
{
    int32_t err = FM_SUCCESS;
    uint16_t i;
    lfdev_t *ldev;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);
    
    CFD_LOCK;

    /* perform the sync operation for each constituent physical devices */
    for (i = 0; i < ldev->part_info.num_partitions; i++) {
        do {
            err = pfd_sync(ldev->part_info.parts[i].pdev);
        } while (err == FM_TRY_AGAIN);
        if (err) break;
    }
    
    CFD_UNLOCK;
    return(err);
}


extern int32_t  
fd_get_device_info(uint32_t dev_id, flash_dev_spec_t *dev_info)
{
    lfdev_t *ldev;

    /* check if parameters are valid */
    if (dev_info == NULL) return(FM_ERROR);
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);
    
    /* get the original flash spec. of the designated logical device */
    MEMCPY(dev_info, &ldev->dev_spec, sizeof(flash_dev_spec_t));
    
    return(FM_SUCCESS);
}


extern int32_t  
fd_get_phy_block(uint32_t dev_id, uint32_t block, uint32_t *phy_block)
{
    lfdev_t       *ldev;
    pfdev_t       *pdev;
    flash_part_t  *part;
    flash_rpart_t *part_info;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);

    CFD_LOCK;

    /* we need to write only to the leading bytes in this logical page;
       identify the first real block number in this logical block */
    block <<= BITS(ldev->interleaving_level);

    /* get the information about the partition where this block exists;
       the block number is also adjusted as a relative value 
       in the corresponding partition */
    part_info = lfd_get_part_info(ldev, &block);
    pdev = part_info->pdev;
    part = part_info->part;

    /* get the block number in the chip-wide address space */
    block += part->start_block;
    
#if USE_DLBM
    /* get the replacement block if this block is a bad block */
    block = bm_get_swapping_block(pdev->chip_id, block);
#endif

    /* return the physical block number */
    *phy_block = block;

    CFD_UNLOCK;
    return(FM_SUCCESS);
}


/*----------------------------------------------------------------------*/
/*  Additional APIs for Flash Memory Volume Management                  */
/*----------------------------------------------------------------------*/

extern uint16_t
fd_get_num_chips(void)
{
    return(pfd_get_num_chips());
}


extern int32_t
fd_format(uint16_t chip_id)
{
#if USE_DLBM
    return(bm_format(chip_id, TRUE));
#else
    return(FM_SUCCESS);
#endif
}


extern int32_t 
fd_read_partition_table(uint16_t chip_id, flash_parttab_t *part_tab)
{
    int32_t err;

    CFD_LOCK;
    err = lfd_read_partition_table(chip_id, part_tab);
    CFD_UNLOCK;

    return(err);
}


extern int32_t 
fd_write_partition_table(uint16_t chip_id, flash_parttab_t *part_tab)
{
    int32_t err;

    CFD_LOCK;
    err = lfd_write_partition_table(chip_id, part_tab);
    CFD_UNLOCK;

    return(err);
}


extern int32_t  
fd_erase_all(uint16_t chip_id, bool_t skip_initial_bad)
{
    int32_t err;

    CFD_LOCK;
    err = pfd_erase_all(chip_id, skip_initial_bad);
    CFD_UNLOCK;

    return(err);
}


extern int32_t  
fd_erase_partition(uint16_t chip_id, uint16_t part_no)
{
    int32_t err;

    CFD_LOCK;
    err = lfd_erase_partition(chip_id, part_no);
    CFD_UNLOCK;

    return(err);
}


/*======================================================================*/
/*  Local Function Definitions                                          */
/*======================================================================*/

static int32_t
sw_copy_back(pfdev_t *src_pdev, uint32_t src_block, uint16_t src_page, 
             pfdev_t *dst_pdev, uint32_t dst_block, uint16_t dst_page)
{
    int32_t err;
    uint8_t *pbuf;

    /* get a temporary I/O buffer */
    pbuf = pfd_get_temp_buf(src_pdev);

    /* read the source page */
    do {
        err = pfd_read_page(src_pdev, src_block, src_page,
                            pbuf, pbuf + src_pdev->chip_spec.data_size);
    } while (err == FM_TRY_AGAIN);
    
    if (err) goto end;

    /* write data in the source page into the destination page */
    do {
        err = pfd_write_page(dst_pdev, dst_block, dst_page,
                             pbuf, pbuf + dst_pdev->chip_spec.data_size, 
                             TRUE);
    } while (err == FM_TRY_AGAIN);
    
end:
    return(err);
}

#endif /* CFD_M */
