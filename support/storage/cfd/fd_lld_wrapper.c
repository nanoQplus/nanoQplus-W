/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_lld_wrapper.c                                          */
/*  PURPOSE : Code for LLD (Low-Level Device Driver) wrapper            */
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
#include "fd_physical.h"
#include "fd_lld_wrapper.h"
#include "ecc_512b.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

#define VERIFY_AFTER_ERASE      0   /* 0: just erase (default)
                                       1: read-verify (all 0xFF) after erase
                                       2: write & read-verify after erase */

#define VERIFY_PATTERN          0x08090A0B

/*======================================================================*/
/*  External Function Definitions                                       */
/*======================================================================*/

extern int32_t 
lld_read_page(pfdev_t *pdev, uint32_t block, uint16_t page,
              uint8_t *dbuf, uint8_t *sbuf)
{
    int32_t err;
    
#if (ECC_METHOD == SW_ECC)
    int32_t ecc_err_offset, k, retry, ecc_error = 0;
    uint8_t ecc_buf[4 * MAX_SECTORS_PER_PAGE], *secc;
    uint8_t ecc_correct_data;
    uint8_t __sbuf[16 * MAX_SECTORS_PER_PAGE];
    
    if (pdev->chip_spec.type == SLC_NAND_FLASH && sbuf == NULL) sbuf = __sbuf;
#endif
    
    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);

    /* call the real device driver function */
    err = pdev->chip_ops.read_page(pdev->local_chip_id, block, page, 
                                   dbuf, sbuf);

#if (ECC_METHOD == SW_ECC)

    /* check ECC using S/W routine */
    if (pdev->chip_spec.type == SLC_NAND_FLASH && dbuf != NULL) {
        for (k = 0; k < pdev->chip_spec.sectors_per_page; k++) {
            
            /* re-calculate ECC based on the data just read */
            make_ecc_512byte(ecc_buf + (k<<2), dbuf + (k<<9));

            retry = 0;
            secc = &sbuf[8];
ecc_retry:
            /* compare the calculated ECC with the stored ECC */
            if (ecc_buf[0 + (k<<2)] != secc[0 + (k<<4)] ||
                ecc_buf[1 + (k<<2)] != secc[1 + (k<<4)] ||
                ecc_buf[2 + (k<<2)] != secc[2 + (k<<4)]) {
                
                /* there is an ECC error; try to correct it */
                err = compare_ecc_512byte(secc + (k<<4), ecc_buf + (k<<2), dbuf + (k<<9),
                                          &ecc_err_offset, &ecc_correct_data);
                switch (err) {
                case ECC_NO_ERROR:
                    break;
                
                case ECC_CORRECTABLE_ERROR:
                    printf("[LLD] ECC corrected %02x --> %02x (block = %d, page = %d)\r\n",
                           dbuf[ecc_err_offset + (k<<9)], ecc_correct_data,
                           (int)block, (int)page);
                    dbuf[ecc_err_offset + (k<<9)] = ecc_correct_data;
                    break;

                case ECC_ECC_ERROR:
                    if (retry == 0) {
                        retry = 1;
                        secc = &sbuf[8 + 3];
                        printf("[LLD] Trying again using an ECC copy ...\r\n");
                        goto ecc_retry;
                    }
                    /* retry failed; fall through below */

                default:
                    printf("[LLD] UNCORRECTABLE ECC ERROR (block = %d, page = %d) !!!\r\n",
                           (int)block, (int)page);
                    ecc_error = 1;
                    break;
                }
            }
        }
        
        if (ecc_error) err = FM_ECC_ERROR;
        else err = FM_SUCCESS;
    }
#endif

    /* the read operation always works in sychronous mode, so it is not
       required to save information about the 'previous operation' */

    return(err);
}


extern int32_t
lld_read_bytes(pfdev_t *pdev, uint32_t block, uint16_t page,
               uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t err;

    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);

    /* call the real device driver function */
    err = pdev->chip_ops.read_bytes(pdev->local_chip_id, block, page, 
                                    num_bytes, dbuf);

    /* the read operation always works in sychronous mode, so it is not
       required to save information about the 'previous operation' */
    
    return(err);
}


extern int32_t
lld_write_page(pfdev_t *pdev, uint32_t block, uint16_t page,
               uint8_t *dbuf, uint8_t *sbuf, bool_t is_last)
{
    int32_t err;

#if (ECC_METHOD == SW_ECC)
    int32_t k;
    uint8_t __sbuf[16 * MAX_SECTORS_PER_PAGE];
    
    if (pdev->chip_spec.type == SLC_NAND_FLASH && sbuf == NULL) {
        sbuf = __sbuf;
        for (k = 0; k < 16 * MAX_SECTORS_PER_PAGE; k++) sbuf[k] = 0xFF;
    }
#endif

    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);

#if (ECC_METHOD == SW_ECC)
    if (pdev->chip_spec.type == SLC_NAND_FLASH && dbuf != NULL) {
        for (k = 0; k < pdev->chip_spec.sectors_per_page; k++) {
            make_ecc_512byte(sbuf + (k<<4) + 8, dbuf + (k<<9));
            
            /* also store an ECC copy */
            MEMCPY(sbuf + (k<<4) + 11, sbuf + (k<<4) + 8, 3);
        }
    }
#endif

    /* call the real device driver function */
    err = pdev->chip_ops.write_page(pdev->local_chip_id, block, page, 
                                    dbuf, sbuf, is_last);
    if (!err) {
        
        /* if the device driver works in asynchronous mode, or
           if the write operation may work in cache programming mode,
           then save information about the 'previous command' */
           
        if (pdev->chip_ops.sync != NULL || is_last == FALSE) {
            pdev->prev_op.local_chip_id       = pdev->local_chip_id;
            pdev->prev_op.command             = OP_WRITE;
            pdev->prev_op.param.write.block   = block;
            pdev->prev_op.param.write.page    = page;
            pdev->prev_op.param.write.dbuf    = dbuf;
            pdev->prev_op.param.write.sbuf    = sbuf;
            pdev->prev_op.param.write.is_last = is_last;
        }
        else {
            /* clear information about the previous command */
            pdev->prev_op.command = OP_NONE;
        }
    }

    return(err);
}


extern int32_t
lld_write_bytes(pfdev_t *pdev, uint32_t block, uint16_t page,
                uint32_t num_bytes, uint8_t *dbuf)
{
    int32_t err;

    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);

    /* call the real device driver function */
    err = pdev->chip_ops.write_bytes(pdev->local_chip_id, block, page, 
                                     num_bytes, dbuf);
    
    /* if the device driver works in asynchronous mode, 
       then save information about the 'previous command' */
    if (!err && pdev->chip_ops.sync != NULL) {
        pdev->prev_op.local_chip_id       = pdev->local_chip_id;
        pdev->prev_op.command             = OP_WRITE;
        pdev->prev_op.param.write.block   = block;
        pdev->prev_op.param.write.page    = page;
        pdev->prev_op.param.write.dbuf    = dbuf;
        pdev->prev_op.param.write.sbuf    = NULL;
        pdev->prev_op.param.write.is_last = 1;
    }
    
    return(err);
}


extern int32_t
lld_copyback(pfdev_t *pdev, 
             uint32_t src_block, uint16_t src_page, 
             uint32_t dst_block, uint16_t dst_page)
{
    int32_t err;
    
    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);
    
    /* call the real device driver function */
    err = pdev->chip_ops.copyback(pdev->local_chip_id, src_block, src_page, 
                                  dst_block, dst_page);

    /* if the device driver works in asynchronous mode,
       then save information about the 'previous command' */
    if (!err && pdev->chip_ops.sync != NULL) {
        pdev->prev_op.local_chip_id            = pdev->local_chip_id;
        pdev->prev_op.command                  = OP_COPYBACK;
        pdev->prev_op.param.copyback.src_block = src_block;
        pdev->prev_op.param.copyback.src_page  = src_page;
        pdev->prev_op.param.copyback.dst_block = dst_block;
        pdev->prev_op.param.copyback.dst_page  = dst_page;
    }

    return(err);
}


extern int32_t 
lld_erase(pfdev_t *pdev, uint32_t block)
{
    int32_t err;

    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);
    
    /* call the real device driver function */
    err = pdev->chip_ops.erase(pdev->local_chip_id, block);

    /* if the device driver works in asynchronous mode,
       then save information about the 'previous command' */
    if (!err && pdev->chip_ops.sync != NULL) {
        pdev->prev_op.local_chip_id     = pdev->local_chip_id;
        pdev->prev_op.command           = OP_ERASE;
        pdev->prev_op.param.erase.block = block;
    }

    return(err);
}


extern int32_t
lld_erase_verify(pfdev_t *pdev, uint32_t block)
{
    int32_t err;

    err = lld_erase(pdev, block);
    if (err) return(err);
    
    err = lld_sync(pdev);
    if (err) return(err);
    
#if (VERIFY_AFTER_ERASE > 0)
    do {
        uint32_t p, k;
        uint8_t *pbuf;
    
        pbuf = pfd_get_temp_buf(pdev);

#if (VERIFY_AFTER_ERASE == 2)
        for (k = 0; k < pdev->chip_spec.data_size; k += 4) {
            *(uint32_t *)(pbuf + k) = VERIFY_PATTERN;
        }
#endif

        for (p = 0; p < pdev->chip_spec.pages_per_block; p++) {

#if (VERIFY_AFTER_ERASE == 2)
            err = lld_write_page(pdev, block, p, pbuf, NULL, 1);
            if (err) return(err);
            
            err = lld_sync(pdev);
            if (err) return(err);
#endif
            err = lld_read_page(pdev, block, p, pbuf, pbuf + pdev->chip_spec.data_size);
            if (err) return(err);

#if (VERIFY_AFTER_ERASE == 1)
            for (k = 0; k < pdev->chip_spec.page_size; k += 4) {
                if (*(uint32_t *)(pbuf + k) != 0xFFFFFFFF) return(FM_ERROR);
            }
#elif (VERIFY_AFTER_ERASE == 2)
            for (k = 0; k < pdev->chip_spec.data_size; k += 4) {
                if (*(uint32_t *)(pbuf + k) != VERIFY_PATTERN) return(FM_ERROR);
            }
#endif
        }

#if (VERIFY_AFTER_ERASE == 2)
        err = lld_erase(pdev, block);
        if (err) return(err);
        
        err = lld_sync(pdev);
        if (err) return(err);
#endif
    } while (0);
#endif

    return(FM_SUCCESS);
}


extern bool_t
lld_is_bad_block(pfdev_t *pdev, uint32_t block)
{
    int32_t err;

    /* before processing the current request, it should be checked 
       that there is no on-going operation in flash memory */
    err = lld_sync(pdev);
    if (err) return(err);
    
    /* call the real device driver function */
    return(pdev->chip_ops.is_bad_block(pdev->local_chip_id, block));
}


extern int32_t 
lld_sync(pfdev_t *pdev)
{
    int32_t err = FM_SUCCESS;
    
    /* check if there is an on-going operation */
    if (pdev->prev_op.command != OP_NONE) {
        
        /* if possible, call the driver's sync function */
        if (pdev->chip_ops.sync != NULL) {
            err = pdev->chip_ops.sync(pdev->local_chip_id, pdev->prev_op.command);

            if (err) {
                /* error occurred; generate an appropriate error code */
                switch (pdev->prev_op.command) {
                    case OP_WRITE:
                    case OP_COPYBACK:
                        err = FM_PREV_WRITE_ERROR;
                        break;
                        
                    case OP_ERASE:
                        err = FM_PREV_ERASE_ERROR;
                        break;
                    
                    default:
                        err = FM_ERROR;
                        break;
                }
            }
        }

        if (!err) {
            /* clear information about the previous command */
            pdev->prev_op.command = OP_NONE;
        }
    }
    
    return(err);
}

#endif /* CFD_M */
