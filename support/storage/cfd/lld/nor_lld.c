/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : nor_lld.c                                                 */
/*  PURPOSE : Low-Level Device Driver for embedded NOR flash            */
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
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 0.9)                                          */
/*                                                                      */
/*  - 2016.01.15 [Joosun Hahn]   : First writing                        */
/*  - 2016.02.17 [Sung-Kwan Kim] : Revised to adapt to CFD framework    */
/*                                                                      */
/************************************************************************/

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_if.h"
#include "fd_physical.h"
#include "fd_utils.h"

#include "stm32f4xx_flash.h"
#include "nor_lld.h"
#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

/* base address of the flash sectors;
   here, in NOR flash, 'sector' means 'block' in NAND flash memory;
   users (such as storage stacks like this) can use sectors starting from sector 5;
   so, we use block numbering as follows:
   
   block 0 (128 KB): sector  5 -- used for storage
   block 1 (128 KB): sector  6 -- used for storage
   block 2 (128 KB): sector  7 -- used for storage
   block 3 (128 KB): sector  8 -- used for storage
   block 4 (128 KB): sector  9 -- used for storage
   block 5 (128 KB): sector 10 -- used for storage
   block 6 (128 KB): sector 11 -- used for storage
   
   for exact address translation, refer to the function 'calc_addr()' */

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000)  /* base address of sector 0,   16 KB */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000)  /* base address of sector 1,   16 KB */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000)  /* base address of sector 2,   16 KB */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000)  /* base address of sector 3,   16 KB */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000)  /* base address of sector 4,   64 KB */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000)  /* base address of sector 5,  128 KB */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000)  /* base address of sector 6,  128 KB */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000)  /* base address of sector 7,  128 KB */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000)  /* base address of sector 8,  128 KB */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000)  /* base address of sector 9,  128 KB */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000)  /* base address of sector 10, 128 KB */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000)  /* base address of sector 11, 128 KB */

#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_5     /* start address of user flash area */
#define INVALID_ADDR            0xFFFFFFFF

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  External Variable Definitions                                       */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Local Variable Definitions                                          */
/*----------------------------------------------------------------------*/

static flash_chip_spec_t    flash_spec[NOR_NUM_CHIPS] = {
    {   /* for chip-0         */
        /* type               */    NOR_FLASH,
        /* page_size          */    512,
        /* data_size          */    512,
        /* spare_size         */    0,
        /* sectors_per_page   */    1,
        /* pages_per_block    */    256,
        /* block_size         */    (512 * 256),
        /* num_blocks         */    7,
        /* num_dies_per_ce    */    1,
        /* num_planes         */    1,
        /* max_num_bad_blocks */    0,
        /* constraints        */    CN_VERY_SMALL_SIZE
    }
};

/*----------------------------------------------------------------------*/
/*  Local Function Declarations                                         */
/*----------------------------------------------------------------------*/

static uint32_t calc_addr(uint16_t chip, uint32_t block, uint16_t page);
static uint32_t get_sector(uint16_t chip, uint32_t block);
static uint32_t nor_get_status(uint16_t chip);


/*======================================================================*/
/*  Function Definitions                                                */
/*======================================================================*/

/*----------------------------------------------------------------------*/
/*  Global Function Definitions                                         */
/*----------------------------------------------------------------------*/

int32_t nor_init(void)
{
    int32_t           err;
    uint16_t          i;
    flash_chip_ops_t  flash_ops;

    /* clear pending flags (if any) */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    /* register flash operations */
    MEMSET((void *)&flash_ops, 0, sizeof(flash_chip_ops_t));
    flash_ops.open          = nor_open;
    flash_ops.close         = nor_close;
    flash_ops.read_page     = nor_read_page;
    flash_ops.read_bytes    = nor_read_bytes;
    flash_ops.write_page    = nor_write_page;
    flash_ops.write_bytes   = nor_write_bytes;
    flash_ops.erase         = nor_erase;
    flash_ops.is_bad_block  = nor_is_bad_block;
    flash_ops.sync          = NULL;

    /* register flash memory chips to CFD (Common Flash Driver) */
    for (i = 0; i < NOR_NUM_CHIPS; i++) {

        /* check if NOR flash chip is ready */
        if (nor_get_status(i) != FLASH_COMPLETE) return(FM_INIT_FAIL);
    
        /* register each physical flash device (chip) */
        err = pfd_register_flash_chip(i,
                                      &flash_spec[i],
                                      &flash_ops);
        if (err) return(FM_INIT_FAIL);
    }
    
    return(FM_SUCCESS);
}


int32_t nor_open(uint16_t chip)
{
    return(FM_SUCCESS);
}


int32_t nor_close(uint16_t chip)
{
    return(FM_SUCCESS);
}


int32_t nor_read_page(uint16_t chip, uint32_t block, uint16_t page,
                      uint8_t *dbuf, uint8_t *sbuf)
{
    uint32_t  i, addr;
    
    if (dbuf == NULL) return(FM_ERROR);
    
    addr = calc_addr(chip, block, page);
    if (addr == INVALID_ADDR) return(FM_ILLEGAL_ACCESS);
    
    switch ((uint32_t)dbuf & 0x03) {
        case 0:
            for (i = 0; i < flash_spec[chip].data_size; i += 4, addr += 4) {
                *(__IO uint32_t *)(dbuf + i) = *(__IO uint32_t *)addr;
            }
            break;
        
        case 2:
            for (i = 0; i < flash_spec[chip].data_size; i += 2, addr += 2) {
                *(__IO uint16_t *)(dbuf + i) = *(__IO uint16_t *)addr;
            }
            break;
        
        default:
            for (i = 0; i < flash_spec[chip].data_size; i++, addr++) {
                *(__IO uint8_t *)(dbuf + i) = *(__IO uint8_t *)addr;
            }
            break;
    }

    return(FM_SUCCESS);
}


int32_t nor_read_bytes(uint16_t chip, uint32_t block, uint16_t page,
                       uint32_t num_bytes, uint8_t *dbuf)
{
    uint32_t  i, addr;
    
    if (dbuf == NULL) return(FM_ERROR);
    if (num_bytes < 4) return(FM_ERROR);
    
    addr = calc_addr(chip, block, page);
    if (addr == INVALID_ADDR) return(FM_ILLEGAL_ACCESS);
    
    switch ((uint32_t)dbuf & 0x03) {
        case 0:
            for (i = 0; i < num_bytes - 3; i += 4, addr += 4) {
                *(__IO uint32_t *)(dbuf + i) = *(__IO uint32_t *)addr;
            }
            break;
        
        case 2:
            for (i = 0; i < num_bytes - 1; i += 2, addr += 2) {
                *(__IO uint16_t *)(dbuf + i) = *(__IO uint16_t *)addr;
            }
            break;
        
        default:
            for (i = 0; i < num_bytes; i++, addr++) {
                *(__IO uint8_t *)(dbuf + i) = *(__IO uint8_t *)addr;
            }
            break;
    }
    
    for ( ; i < num_bytes; i++, addr++) {
        *(__IO uint8_t *)(dbuf + i) = *(__IO uint8_t *)addr;
    }

    return(FM_SUCCESS);
}


int32_t nor_write_page(uint16_t chip, uint32_t block, uint16_t page,
                       uint8_t *dbuf, uint8_t *sbuf, bool_t is_last)
{
    int32_t   err = FM_SUCCESS;
    uint32_t  i, addr;
    
    if (dbuf == NULL) return(FM_ERROR);

    addr = calc_addr(chip, block, page);
    if (addr == INVALID_ADDR) return(FM_ILLEGAL_ACCESS);
    
    FLASH_Unlock();
    
    switch ((uint32_t)dbuf & 0x03) {
        case 0:
            for (i = 0; i < flash_spec[chip].data_size; i += 4, addr += 4) {
                if (FLASH_ProgramWord(addr, *(uint32_t *)(dbuf + i)) != FLASH_COMPLETE) {
                    err = FM_WRITE_ERROR;
                    break;
                }
            }
            break;
            
        case 2:
            for (i = 0; i < flash_spec[chip].data_size; i += 2, addr += 2) {
                if (FLASH_ProgramHalfWord(addr, *(uint16_t *)(dbuf + i)) != FLASH_COMPLETE) {
                    err = FM_WRITE_ERROR;
                    break;
                }
            }
            break;
        
        default:
            for (i = 0; i < flash_spec[chip].data_size; i++, addr++) {
                if (FLASH_ProgramByte(addr, *(uint8_t *)(dbuf + i)) != FLASH_COMPLETE) {
                    err = FM_WRITE_ERROR;
                    break;
                }
            }
            break;
    }
    
    FLASH_Lock();

    return(err);
}


int32_t nor_write_bytes(uint16_t chip, uint32_t block, uint16_t page,
                        uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t   err = FM_SUCCESS;
    uint32_t  i, addr;
    
    if (dbuf == NULL) return(FM_ERROR);
    if (num_bytes < 4) return(FM_ERROR);
    
    addr = calc_addr(chip, block, page);
    if (addr == INVALID_ADDR) return(FM_ILLEGAL_ACCESS);
    
    FLASH_Unlock();
    
    switch ((uint32_t)dbuf & 0x03) {
        case 0:
            for (i = 0; i < num_bytes - 3; i += 4, addr += 4) {
                if (FLASH_ProgramWord(addr, *(uint32_t *)(dbuf + i)) != FLASH_COMPLETE) {
                    err = FM_WRITE_ERROR;
                    break;
                }
            }
            break;
        
        case 2:
            for (i = 0; i < num_bytes - 1; i += 2, addr += 2) {
                if (FLASH_ProgramHalfWord(addr, *(uint16_t *)(dbuf + i)) != FLASH_COMPLETE) {
                    err = FM_WRITE_ERROR;
                    break;
                }
            }
            break;
        
        default:
            for (i = 0; i < num_bytes; i++, addr++) {
                if (FLASH_ProgramByte(addr, *(uint8_t *)(dbuf + i)) != FLASH_COMPLETE) {
                    err = FM_WRITE_ERROR;
                    break;
                }
            }
            break;
    }
    
    for ( ; i < num_bytes; i++, addr++) {
        if (FLASH_ProgramByte(addr, *(uint8_t *)(dbuf + i)) != FLASH_COMPLETE) {
            err = FM_WRITE_ERROR;
        }
    }
    
    FLASH_Lock();

    return(err);
}


int32_t nor_erase(uint16_t chip, uint32_t block)
{
    int32_t  err = FM_SUCCESS;
    uint32_t sector;

    sector = get_sector(chip, block);
    if (sector == INVALID_ADDR) return(FM_ILLEGAL_ACCESS);
    
    FLASH_Unlock();
    
    if (FLASH_EraseSector(sector, VoltageRange_3) != FLASH_COMPLETE) {
        err = FM_ERASE_ERROR;
    }
    
    FLASH_Lock();
    
    return(err);
}


int32_t nor_sync(uint16_t chip, uint16_t prev_command)
{
    return(FM_SUCCESS);
}


bool_t nor_is_bad_block(uint16_t chip, uint32_t block)
{
    return(FALSE);
}


/*----------------------------------------------------------------------*/
/*  Local Function Definitions                                          */
/*----------------------------------------------------------------------*/

static uint32_t calc_addr(uint16_t chip, uint32_t block, uint16_t page)
{
    uint32_t addr;
    
    if (block >= flash_spec[chip].num_blocks) return(INVALID_ADDR);
    
    addr = 0x08020000 + (block * 0x20000);
    addr += (page << BITS(flash_spec[chip].data_size));
    
    return(addr);
}


static uint32_t get_sector(uint16_t chip, uint32_t block)
{
    if (block >= flash_spec[chip].num_blocks) return(INVALID_ADDR);
        
    return(FLASH_Sector_5 + 0x08 * block);
}


static uint32_t nor_get_status(uint16_t chip)
{
    return((uint32_t)FLASH_GetStatus());
}

#endif /* CFD_M */
