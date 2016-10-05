/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_logical.c                                              */
/*  PURPOSE : Code for Logical Flash Device (LFD) Abstraction           */
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
/*  Constant & Macro Definitions (Rarely Configurable)                  */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions (Non-Configurable)                     */
/*----------------------------------------------------------------------*/

#define PARTTABLE_SIGNATURE     0xBAB08282
#define INVALID_DEVICE_ID       0xFFFFFFFF  /* invalid device ID */

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/* data structure for flash memory page of partition table */

typedef struct {
    uint32_t            signature;      /* partition table magic number */
    flash_parttab_t     part_table;     /* partition table */
    uint16_t            checksum;       /* check sum for partition table */
} flash_partpage_t;

/*----------------------------------------------------------------------*/
/*  External Variable Definitions                                       */
/*----------------------------------------------------------------------*/

lfdev_t             LDev[MAX_FLASH_DEVICES];        /* LFD table */
uint16_t            ldev_count;

flash_parttab_t     part_table[NUM_FLASH_CHIPS];    /* flash partition table */

flash_vpart_t       raw_dev_table[MAX_FLASH_PARTITIONS];
flash_vpart_t       ftl_dev_table[MAX_FLASH_PARTITIONS];

/*----------------------------------------------------------------------*/
/*  Local Variable Definitions                                          */
/*----------------------------------------------------------------------*/

static flash_part_t entire_chip_space[NUM_FLASH_CHIPS];

/*----------------------------------------------------------------------*/
/*  Local Function Declarations                                         */
/*----------------------------------------------------------------------*/

static lfdev_t *    lfd_alloc_device(uint32_t dev_id);
static void         lfd_free_device(lfdev_t *ldev);
static bool_t       is_valid_partition_table(pfdev_t *pdev, flash_parttab_t *part_tab);
static int32_t      make_blk_dev_table(flash_parttab_t *part_tab, 
                                       flash_vpart_t *dev_table, uint16_t dev_class);
static uint16_t     calc_checksum(uint16_t *data, uint16_t size);


/*======================================================================*/
/*  External Function Definitions                                       */
/*======================================================================*/

extern int32_t 
lfd_init(void)
{
    int i;

    /* initialize the flash partition table */
    for (i = 0; i < NUM_FLASH_CHIPS; i++) {
        MEMSET((void *)&part_table[i], 0, sizeof(flash_parttab_t));
    }

    /* initialize the logical flash device table */
    for (i = 0; i < MAX_FLASH_DEVICES; i++) {
        MEMSET((void *)&LDev[i], 0, sizeof(lfdev_t));
        LDev[i].dev_id = INVALID_DEVICE_ID;
    }
    
    ldev_count = 0;
    return(FM_SUCCESS);
}


extern int32_t  
lfd_shutdown(int32_t forced)
{
    return(FM_SUCCESS);
}


extern int32_t  
lfd_open(uint32_t dev_id)
{
    int32_t  err, i, j;
    lfdev_t  *ldev;
    uint16_t dev_class, dev_index, serial, chip_id, part_id;
    bool_t   all_same_type = TRUE, all_same_size = TRUE, interleaved = TRUE;
    
    /* allocate a logical device structure */
    ldev = lfd_alloc_device(dev_id);
    if (ldev == NULL) return(FM_OPEN_FAIL);

    /* get device class & serial number out of device ID */
    dev_class = GET_DEV_CLASS(dev_id);
    serial = GET_DEV_SERIAL(dev_id);
    
    if (dev_class == LFD_BLK_DEVICE_RAW) { 
        /* extract chip ID and partition ID */
        chip_id = GET_RAWDEV_CHIP_ID(serial);
        part_id = GET_RAWDEV_PART_ID(serial);
        
        if (chip_id != RAW_BLKDEV_TABLE) {
            /* accessing a raw block device using the chip ID & part ID pair */
            if ((chip_id >= pdev_count) || 
                (part_id != ENTIRE_PARTITION && 
                 part_id >= part_table[chip_id].num_partitions)) {
                err = FM_BAD_DEVICE_ID;
                goto error;
            }
            
            /* set up the partition information for this logical device */
            ldev->part_info.num_partitions = 1;
            ldev->part_info.parts[0].pdev = &PDev[chip_id];
            if (part_id != ENTIRE_PARTITION) {
                ldev->part_info.parts[0].part = &part_table[chip_id].part[part_id];
            }
            else {
                ldev->part_info.parts[0].part = &entire_chip_space[chip_id];
    
                /* using the 'LFD_BLK_DEVICE_RAW' class and 'ENTIRE_PARTITION'
                   partition ID, the entire chip space can be accessed; the 
                   only area that cannot be accessed is the DLBM area and the
                   partition table block */
                entire_chip_space[chip_id].start_block = 0;
                entire_chip_space[chip_id].num_blocks = PDev[chip_id].actual_num_blocks;
            }
        }
        else {
            /* accessing a raw block device using the raw block device table */
            dev_index = part_id;
            if (dev_index >= MAX_FLASH_PARTITIONS || 
                raw_dev_table[dev_index].num_partitions == 0) {
                err = FM_BAD_DEVICE_ID;
                goto error;
            }
            
            /* get the partition information for this logical device */
            MEMCPY(&ldev->part_info, &raw_dev_table[dev_index], sizeof(flash_vpart_t));
        }
    }

    else if (dev_class == LFD_BLK_DEVICE_FTL) {
        /* check if the given device ID is valid */
        dev_index = GET_FTLDEV_INDEX(serial);
        if (dev_index >= MAX_FLASH_PARTITIONS || 
            ftl_dev_table[dev_index].num_partitions == 0) {
            err = FM_BAD_DEVICE_ID;
            goto error;
        }
        
        /* get the partition information for this logical device */
        MEMCPY(&ldev->part_info, &ftl_dev_table[dev_index], sizeof(flash_vpart_t));
    }
        
    else {
        /* not supported device ID */
        err = FM_BAD_DEVICE_ID;
        goto error;
    }
    
    /* for a virtual partition, the number of constituent partitions should be
       2^n values, which is equal to or less than MAX_FLASH_PARTITIONS */
    if ((ldev->part_info.num_partitions > MAX_FLASH_PARTITIONS) ||
        (ldev->part_info.num_partitions != 1 &&
         ldev->part_info.num_partitions != 2 &&
         ldev->part_info.num_partitions != 4 &&
         ldev->part_info.num_partitions != 8)) {
        err = FM_INVALID_PARTITION;
        goto error;
    }

    /* set up the device specification */
    MEMCPY(&ldev->dev_spec, 
           &ldev->part_info.parts[0].pdev->chip_spec, sizeof(flash_dev_spec_t));
    ldev->dev_spec.num_blocks = 0;

    for (i = 0; i < ldev->part_info.num_partitions; i++) {
        ldev->dev_spec.num_blocks += ldev->part_info.parts[i].part->num_blocks;
        
        /* check if all of the constituent partitions have the same type */
        if (ldev->part_info.parts[i].pdev->chip_spec.type !=
            ldev->part_info.parts[0].pdev->chip_spec.type) {
            all_same_type = FALSE;
        }
        if (ldev->part_info.parts[i].pdev->chip_spec.page_size !=
            ldev->part_info.parts[0].pdev->chip_spec.page_size) {
            all_same_type = FALSE;
        }
        if (ldev->part_info.parts[i].pdev->chip_spec.block_size !=
            ldev->part_info.parts[0].pdev->chip_spec.block_size) {
            all_same_type = FALSE;
        }
        if (! all_same_type) {
            err = FM_INVALID_PARTITION;
            goto error;
        }

        /* check if all of the constituent partitions have the same size */
        if (ldev->part_info.parts[i].part->num_blocks !=
            ldev->part_info.parts[0].part->num_blocks) {
            all_same_size = FALSE;
        }
        
        /* check if all of the constituent partitions come from different chips */
        for (j = i+1; j < ldev->part_info.num_partitions; j++) {
            if (ldev->part_info.parts[i].pdev == 
                ldev->part_info.parts[j].pdev) {
                interleaved = FALSE;
            }
        }
    }
    
    /* determine if the given partition is interleaved or not */
    if (all_same_size && interleaved) {
        ldev->interleaving_level          = ldev->part_info.num_partitions;
        ldev->dev_spec.page_size        <<= BITS(ldev->interleaving_level);
        ldev->dev_spec.data_size        <<= BITS(ldev->interleaving_level);
        ldev->dev_spec.spare_size       <<= BITS(ldev->interleaving_level);
        ldev->dev_spec.sectors_per_page <<= BITS(ldev->interleaving_level);
        ldev->dev_spec.block_size       <<= BITS(ldev->interleaving_level);
        ldev->dev_spec.num_blocks       >>= BITS(ldev->interleaving_level);
    }
    else {
        ldev->interleaving_level         = 1;
    }

    ldev->usage_count++;
    return(FM_SUCCESS);
    
error:
    lfd_free_device(ldev);
    return(err);
}


extern int32_t  
lfd_close(uint32_t dev_id)
{
    lfdev_t *ldev;
    
    /* get the designated logical device */
    ldev = lfd_get_device(dev_id);
    if (ldev == NULL) return(FM_DEVICE_NOT_OPEN);
        
    /* close the logical device referenced by dev_id */
    ldev->usage_count--;
    if (ldev->usage_count == 0) {
        lfd_free_device(ldev);
    }
    
    return(FM_SUCCESS);
}


extern lfdev_t *
lfd_get_device(uint32_t dev_id)
{
    register uint16_t i;
    
    /* search for the logical device referenced by the given dev_id */
    for (i = 0; i < MAX_FLASH_DEVICES; i++) {
        if (LDev[i].dev_id == dev_id) return(&LDev[i]);
    }
    
    return(NULL);
}


/*----------------------------------------------------------------------*/
/*  Functions for Flash Memory Low-Level Partition Management           */
/*----------------------------------------------------------------------*/

extern int32_t    
lfd_read_partition_table(uint16_t chip_id, flash_parttab_t *part_tab)
{
    int32_t  err = FM_SUCCESS;
    uint32_t part_block;
    uint8_t *pbuf;
    pfdev_t *pdev;
    flash_partpage_t *p;

    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];

#if USE_DLBM
    if (!bm_is_formatted(chip_id)) return(FM_NOT_FORMATTED);
#endif

    /* if the chip capacity is too small, then the partition table is not
       stored in flash memory; instead, the default partition table is used */
    if (pdev->chip_spec.constraints & CN_VERY_SMALL_SIZE) {
        MEMCPY(&part_table[chip_id], &default_part_table[chip_id], sizeof(flash_parttab_t));
        goto found;
    }

    /* get a temporary I/O buffer */
    pbuf = pfd_get_temp_buf(pdev);
    p = (flash_partpage_t *)pbuf;

    /* get the block where partition information is stored */
    part_block = pdev->part_table_block;

    /* read partition table from flash memory */
    do {
        err = pfd_read_page(pdev, part_block, 0, pbuf, NULL);
    } while (err == FM_TRY_AGAIN);

    if (err) return(err);

    /* verify the partition table using signature & checksum code */
    if (p->signature != PARTTABLE_SIGNATURE ||
        p->checksum != calc_checksum((uint16_t *)&p->part_table, 
                                     sizeof(flash_parttab_t))) {
        part_table[chip_id].num_partitions = 0;
    }
    else {
        /* copy the partition table into the global data structure */
        MEMCPY(&part_table[chip_id], &p->part_table, sizeof(flash_parttab_t));

found:
        /* make the RAW block device tables based on the partition table */
        err = make_blk_dev_table(part_table, raw_dev_table, LFD_BLK_DEVICE_RAW);
        if (err) return(err);
        
        /* make the FTL block device tables based on the partition table */
        err = make_blk_dev_table(part_table, ftl_dev_table, LFD_BLK_DEVICE_FTL);
        if (err) return(err);
    }
    
    /* also copy the partition table into the return parameter */
    if (part_tab != NULL) {
        MEMCPY(part_tab, &part_table[chip_id], sizeof(flash_parttab_t));
    }

    return(FM_SUCCESS);
}


extern int32_t    
lfd_write_partition_table(uint16_t chip_id, flash_parttab_t *part_tab)
{
    int32_t  err;
    uint32_t part_block;
    uint8_t *pbuf;
    pfdev_t *pdev;
    flash_partpage_t *p;

    /* check validity of parameters */
    if (part_tab == NULL) return(FM_ERROR);

    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);
    pdev = &PDev[chip_id];

#if USE_DLBM
    if (!bm_is_formatted(chip_id)) return(FM_NOT_FORMATTED);
#endif

    /* check the validity of the given partition table */
    if (!is_valid_partition_table(pdev, part_tab)) return(FM_ERROR);

    /*--------------------------------------------------------------*/
    /*  first, erase the partition block                            */
    /*--------------------------------------------------------------*/

    /* get the block where partition information is stored */
    part_block = pdev->part_table_block;

    /* perform the physical device operation until it succeeds or fails */
    do {
        err = pfd_erase(pdev, part_block);
    } while (err == FM_TRY_AGAIN);

    if (err) return(err);
    
    /*--------------------------------------------------------------*/
    /*  second, fill the partition page buffer                      */
    /*--------------------------------------------------------------*/

    /* get a temporary I/O buffer */
    pbuf = pfd_get_temp_buf(pdev);
    p = (flash_partpage_t *)pbuf;

    /* clear the page buffer */
    MEMSET(pbuf, 0xff, pdev->chip_spec.page_size);
    
    /* fill the partition table in the page buffer */
    p->signature = PARTTABLE_SIGNATURE;
    MEMCPY(&p->part_table, part_tab, sizeof(flash_parttab_t));
    p->checksum = calc_checksum((uint16_t *)&p->part_table, 
                                sizeof(flash_parttab_t));

    /*--------------------------------------------------------------*/
    /*  third, write the partition page buffer in flash             */
    /*--------------------------------------------------------------*/

    /* get the block where partition information is stored */
    part_block = pdev->part_table_block;

    /* perform the physical device operation until it succeeds or fails */
    do {
        err = pfd_write_page(pdev, part_block, 0, pbuf, NULL, TRUE);
        if (!err) err = pfd_sync(pdev);
    } while (err == FM_TRY_AGAIN);
    
    if (!err) {    
        /* copy the given partition table in the global data structure */
        MEMCPY(&part_table[chip_id], part_tab, sizeof(flash_parttab_t));
        
        /* make the RAW block device tables based on the partition table */
        make_blk_dev_table(part_table, raw_dev_table, LFD_BLK_DEVICE_RAW);
        
        /* make the FTL block device tables based on the partition table */
        make_blk_dev_table(part_table, ftl_dev_table, LFD_BLK_DEVICE_FTL);
    }
    
    return(err);
}


extern int32_t  
lfd_erase_partition(uint16_t chip_id, uint16_t part_no)
{
    pfdev_t *pdev;
    flash_parttab_t *part;
    uint32_t i, start_block, last_block, err = FM_SUCCESS;
    
    /* existing chip id? */
    if (chip_id >= pdev_count) return(FM_BAD_DEVICE_ID);

    pdev = &PDev[chip_id];
    part = &part_table[chip_id];
	
    if (part_no >= part->num_partitions && part_no != ENTIRE_PARTITION)
        return (FM_BAD_DEVICE_ID);

    /* determine the range of the blocks to be erased */
    if (part_no == ENTIRE_PARTITION) {
        start_block = 0;
        last_block  = pdev->part_table_block;
    }
    else {
        start_block = part->part[part_no].start_block;
        last_block  = start_block + part->part[part_no].num_blocks;
    }
    
    /* erase all blocks in the given partition */    
    for (i = start_block; i < last_block; i++) {
        do {
            err = pfd_erase(pdev, i);
        } while (err == FM_TRY_AGAIN);
        
        if (err) break;
    }
    
    return(err);
}


extern flash_rpart_t *
lfd_get_part_info(lfdev_t *ldev, uint32_t *block)
{
    uint32_t i, b = *block;

    flash_vpart_t *part_info = &ldev->part_info;
    flash_rpart_t *part = NULL;

#if STRICT_CHECK
    uint32_t org_block = *block;
#endif

    if (ldev->interleaving_level == 1) {
        /* non-interleaved case; 
           each partition may have a different number of blocks */

        for (i = 0; i < part_info->num_partitions; i++) {
            if (part_info->parts[i].part->num_blocks > b) {
                part = &part_info->parts[i];
                *block = b;
                break;
            }
            b -= part_info->parts[i].part->num_blocks;
        }
    }
    else {
        /* interleaved case;
           each partition must have the same number of blocks */

        /* in the interleaved partitions, identify the exact partition */
        part = &part_info->parts[b & (ldev->interleaving_level - 1)];
        
        /* re-calculate the block number in the corresponding partition */
        *block = b >> BITS(ldev->interleaving_level);
    }

#if STRICT_CHECK
    if (part == NULL) {
        printf("\n");
        printf("[LFD] no proper partition for block %d\n", org_block);
        printf("\n");
    }
#endif

    return(part);
}


/*======================================================================*/
/*  Local Function Definitions                                          */
/*======================================================================*/

static lfdev_t *
lfd_alloc_device(uint32_t dev_id)
{
    uint16_t i;
    
    /* search for an unallocated logical device */
    for (i = 0; i < MAX_FLASH_DEVICES; i++) {
        if (LDev[i].dev_id == INVALID_DEVICE_ID) break;
    }
    
    /* all logical devices are allocated? */
    if (i == MAX_FLASH_DEVICES) return(NULL);
    
    /* ok, allocate a logical device and initialize it */
    MEMSET((void *)&LDev[i], 0, sizeof(lfdev_t));
    LDev[i].dev_id = dev_id;
    ldev_count++;
    
    return(&LDev[i]);
}


static void
lfd_free_device(lfdev_t *ldev)
{
    ldev->dev_id = INVALID_DEVICE_ID;
    ldev_count--;
}


static bool_t
is_valid_partition_table(pfdev_t *pdev, flash_parttab_t *part_tab)
{
    uint16_t i;
    uint32_t cur_block = 0;
    flash_parttab_t tmp_part_tab[NUM_FLASH_CHIPS];
    flash_vpart_t   dev_table[MAX_FLASH_PARTITIONS];
    
    if (part_tab->num_partitions == 0 || 
        part_tab->num_partitions > MAX_FLASH_PARTITIONS)
        return(FALSE);
    
    for (i = 0; i < part_tab->num_partitions; i++) {
        
        /* a partition should contain at least one block */
        if (part_tab->part[i].num_blocks == 0) return(FALSE);
        
        /* partitions should not overlap with each other */
        if (part_tab->part[i].start_block < cur_block) return(FALSE);
        
        /* check if first block of each partition is multi-plane aligned */
        cur_block = part_tab->part[i].start_block;
#if 0
        if ((cur_block % pdev->chip_spec.num_planes) != 0) return(FALSE);
#endif
        
        /* check if last block of each partition is multi-plane aligned */
        cur_block += part_tab->part[i].num_blocks;
        if (cur_block > pdev->actual_num_blocks) return(FALSE);
#if 0
        if ((cur_block % pdev->chip_spec.num_planes) != 0) return(FALSE);
#endif
    }
    
    /* check if the info about the block device tables is correctly given */
    MEMCPY((void *)tmp_part_tab, (void *)part_table, sizeof(flash_parttab_t) * NUM_FLASH_CHIPS);
    MEMCPY(&tmp_part_tab[pdev->chip_id], part_tab, sizeof(flash_parttab_t));
    if (make_blk_dev_table(tmp_part_tab, dev_table, LFD_BLK_DEVICE_RAW) != FM_SUCCESS)
        return(FALSE);
    if (make_blk_dev_table(tmp_part_tab, dev_table, LFD_BLK_DEVICE_FTL) != FM_SUCCESS)
        return(FALSE);
    
    return(TRUE);
}


static int32_t
make_blk_dev_table(flash_parttab_t *part_tab, flash_vpart_t *dev_table, 
                   uint16_t dev_class)
{
    uint8_t  dev_index;
    uint16_t i, j, chip_id;
    
    /* first, initialize the given block device table */
    MEMSET((void *)dev_table, 0, sizeof(flash_vpart_t) * MAX_FLASH_PARTITIONS);
            
    /* make the device table based on the given partition table */
    for (chip_id = 0; chip_id < pdev_count; chip_id++) {
        for (i = 0; i < part_tab[chip_id].num_partitions; i++) {

            if (part_tab[chip_id].part[i].dev_class == dev_class &&
                part_tab[chip_id].part[i].in_dev_table) {

                dev_index = part_tab[chip_id].part[i].dev_index;
                if (dev_index >= MAX_FLASH_PARTITIONS) return(FM_ERROR);
                
                j = dev_table[dev_index].num_partitions;
                if (j > NUM_FLASH_CHIPS) return(FM_ERROR);
                
                /* register this partition to the list of merged partitions */
                dev_table[dev_index].parts[j].pdev = &PDev[chip_id];
                dev_table[dev_index].parts[j].part = &part_tab[chip_id].part[i];
                dev_table[dev_index].num_partitions++;
            }
        }
    }
    
    return(FM_SUCCESS);
}


static uint16_t 
calc_checksum(uint16_t *data, uint16_t size)
{
    uint16_t checksum = 0;

    for ( ; size > 0; size -= 2, data++) {
        checksum += *data;
    }
    
    return(checksum);
}

#endif /* CFD_M */
