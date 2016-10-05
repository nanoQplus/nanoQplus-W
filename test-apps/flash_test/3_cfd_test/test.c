/************************************************************************/
/*                                                                      */
/*  PROJECT : CFD (Common Flash Driver)                                 */
/*  FILE    : test.c                                                    */
/*  PURPOSE : Test code for CFD                                         */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 1.0)                                          */
/*                                                                      */
/*  - 2016.03.01 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_if.h"

#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------*/
/*  Test code for CFD module                                            */
/*----------------------------------------------------------------------*/

void cfd_test(uint16_t chip_id)
{
    int32_t  err, i, j;
    uint32_t dev_id, b;
    uint16_t p;
    flash_dev_spec_t dev_info;

    static uint8_t tx_dbuf[2048], tx_sbuf[64];
    static uint8_t rx_dbuf[2048], rx_sbuf[64];

    dev_id = SET_DEV_ID(LFD_BLK_DEVICE_RAW, SET_RAWDEV_SERIAL(chip_id, 0));
    
    printf("--------------------------------------------------\r\n");
    printf("    CFD Test for %s flash, partition-0\r\n", (chip_id == 0 ? "NAND" : "NOR"));
    printf("--------------------------------------------------\r\n");

    err = fd_open(dev_id);
    if (err) {
        printf("[CFD TEST] fd_open() failed (err = %d).\r\n", (int)err);
        return;
    }
    
    err = fd_get_device_info(dev_id, &dev_info);
    if (err) {
        printf("[CFD TEST] fd_get_device_info() failed (err = %d).\r\n", (int)err);
        fd_close(dev_id);
        return;
    }
    
    printf("[CFD TEST] flash device spec:\r\n");
    printf("--------------------------------------------------\r\n");
    printf("type               = %s flash\r\n", (dev_info.type == NOR_FLASH ? "NOR" : "NAND"));
    printf("page_size          = %d\r\n", (int)dev_info.page_size);
    printf("data_size          = %d\r\n", (int)dev_info.data_size);
    printf("spare_size         = %d\r\n", (int)dev_info.spare_size);
    printf("sectors_per_page   = %d\r\n", (int)dev_info.sectors_per_page);
    printf("pages_per_block    = %d\r\n", (int)dev_info.pages_per_block);
    printf("block_size         = %d\r\n", (int)dev_info.block_size);
    printf("num_blocks         = %d\r\n", (int)dev_info.num_blocks);
    printf("--------------------------------------------------\r\n");
    
    /* main test starts... erase each block and fill data in all the pages */
    for (b = 0; b < dev_info.num_blocks; b++) {
        printf("[CFD TEST] erase block %d ... ", (int)b);
        
        /* erase block */
        err = fd_erase(dev_id, b);
        if (err) {
            printf("fd_erase() failed (err = %d).\r\n", (int)err);
            fd_close(dev_id);
            return;
        }
        
        printf("done. Writing pages in block %d ... ", (int)b);

        /* write each page in the block */
        for (p = 0; p < 64; p++) {
            
            /* fill the buffer */
            for (i = 0; i < 2048; i++) tx_dbuf[i] = i;
            for (i = 0; i <   64; i++) tx_sbuf[i] = 0xFF;

            *((uint32_t *)(tx_dbuf)) = p;
            *((uint32_t *)(tx_dbuf + 4)) = b;
            
            err = fd_write_page(dev_id, b, p, tx_dbuf, tx_sbuf, 1);
            if (err) {
                printf("fd_write_page() failed (err = %d, page = %d).\r\n", (int)err, (int)p);
                fd_close(dev_id);
                return;
            }
        }
        
        printf("OK.\r\n");
    }

    /* now verify the data in the pages */
    for (b = 0; b < dev_info.num_blocks; b++) {
        printf("[CFD TEST] read block %d and verifying the data ... ", (int)b);
        
        /* read each page in the block */
        for (p = 0; p < 64; p++) {
            err = fd_read_page(dev_id, b, p, rx_dbuf, rx_sbuf);
            if (err) {
                printf("fd_read_page() failed (err = %d, page = %d).\r\n", (int)err, (int)p);
                fd_close(dev_id);
                return;
            }

            if (*((uint32_t *)(rx_dbuf)) != p ||
                *((uint32_t *)(rx_dbuf + 4)) != b ||
                memcmp(rx_dbuf+8, tx_dbuf+8, 2040) != 0) {
                printf("verify failed (page = %d).\r\n", (int)p);
                
                printf("\r\nRead data (block = %d, page = %d):\r\n", (int)b, (int)p);
                printf("------------------------------------------------------\r\n");
                for (j = 0; j < 2048; j++) {
                    if (j % 16 == 0) printf("%03x: ", (unsigned int)j);
                    if (j % 8 == 0) printf(" ");
                    printf("%02x ", (int)rx_dbuf[j]);
                    if (j % 16 == 15) printf("\r\n");
                    if (j % 512 == 511) printf("\r\n");
                }
                for (j = 0; j < 64; j++) {
                    if (j % 16 == 0) printf("%03x: ", (unsigned int)j + 2048);
                    if (j % 8 == 0) printf(" ");
                    printf("%02x ", (int)rx_sbuf[j]);
                    if (j % 16 == 15) printf("\r\n");
                }
                printf("------------------------------------------------------\r\n");
                
                fd_close(dev_id);
                return;
            }
        }
        
        printf("OK.\r\n");
    }
    
    fd_close(dev_id);
    printf("[CFD TEST] test finished successfully.\r\n");
}

#endif /* CFD_M */
