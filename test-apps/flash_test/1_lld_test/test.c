#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_if.h"
#include "fsmc_nand.h"
#include "fsmc_nand_lld.h"
#include "nor_lld.h"

#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------*/
/*  Test code for FSMC NAND flash driver                                */
/*----------------------------------------------------------------------*/

void fsmc_nand_test(void)
{
    int32_t  i, j, err, read_err_count = 0, verify_err_count = 0;
    uint32_t b, bad_block[25], num_bad_blocks = 0;
    uint32_t test_start_block = 0, test_end_block = 1023;
    uint16_t p;
    
    static uint8_t tx_dbuf[NAND_PAGE_SIZE], tx_sbuf[NAND_SPARE_AREA_SIZE];
    static uint8_t rx_dbuf[NAND_PAGE_SIZE], rx_sbuf[NAND_SPARE_AREA_SIZE];
    
    printf("--------------------------------------------------\r\n");
    printf("    Test for FSMC NAND flash low-level driver \r\n");
    printf("--------------------------------------------------\r\n");

    /* fill the buffer */
    for (i = 0; i < 2048; i++) tx_dbuf[i] = i;
    for (i = 0; i <   64; i++) tx_sbuf[i] = 0xFF;

    /* test starts... erase each block and fill data in all the pages */
    for (b = test_start_block; b <= test_end_block; b++) {
        printf("Erase block %d ... ", (int)b);
        
        /* erase block */
        err = fsmc_nand_erase(0, b);
        if (err) {
            printf("FAILED !!\r\n");
            bad_block[num_bad_blocks] = b;
            num_bad_blocks++;
            continue;
        }
        
        err = fsmc_nand_sync(0, 0);
        if (err) {
            printf("FAILED !!\r\n");
            bad_block[num_bad_blocks] = b;
            num_bad_blocks++;
            continue;
        }
        
        printf("done. Writing pages in block %d ... ", (int)b);

        /* write each page in the block */
        for (p = 0; p < 64; p++) {
            *((uint32_t *)(tx_dbuf)) = p;
            *((uint32_t *)(tx_dbuf + 4)) = b;
            
            err = fsmc_nand_write_page(0, b, p, tx_dbuf, tx_sbuf, 1);
            if (err) {
                printf("FAILED (page = %d) !!\r\n", (int)p);
                bad_block[num_bad_blocks] = b;
                num_bad_blocks++;
                break;
            }
            
            err = fsmc_nand_sync(0, 0);
            if (err) {
                printf("FAILED (page = %d) !!\r\n", (int)p);
                bad_block[num_bad_blocks] = b;
                num_bad_blocks++;
                break;
            }
        }
        if (err) continue;
        
        printf("OK.\r\n");
    }
    
    /* now verify the data in the pages */
    for (b = test_start_block, i = 0; b <= test_end_block; b++) {
        printf("Read block %d and verifying the data ... ", (int)b);
        if (i < num_bad_blocks && bad_block[i] == b) {
            printf("skipped (bad block)\r\n");
            i++;
            continue;
        }
        
        /* read each page in the block */
        for (p = 0; p < 64; p++) {
#if 1
            err = fsmc_nand_read_page(0, b, p, rx_dbuf, rx_sbuf);
            if (err) {
                printf("read FAILED (page = %d) !!\r\n", (int)p);
                read_err_count++;
            }

            if (*((uint32_t *)(rx_dbuf)) != p ||
                *((uint32_t *)(rx_dbuf + 4)) != b ||
                memcmp(rx_dbuf+8, tx_dbuf+8, 2040) != 0 ||
                memcmp(rx_sbuf, tx_sbuf, 64) != 0) {
                printf("verify FAILED (page = %d) !!\r\n", (int)p);
                verify_err_count++;
                err = -1;
            }
#else
            err = fsmc_nand_read_bytes(0, b, p, 8, rx_dbuf);
            if (err) {
                printf("read FAILED (page = %d) !!\r\n", (int)p);
                read_err_count++;
            }

            if (*((uint32_t *)(rx_dbuf)) != p ||
                *((uint32_t *)(rx_dbuf + 4)) != b) {
                printf("verify FAILED (page = %d) !!\r\n", (int)p);
                verify_err_count++;
                err = -1;
            }
#endif

            if (err) {
                printf("\r\nRead data (block = %d, page = %d):\r\n", (int)b, (int)p);
                printf("--------------------------------------------------\r\n");
                for (j = 0; j < NAND_PAGE_SIZE; j++) {
                    if (j % 16 == 0) printf("%03x: ", (unsigned int)j);
                    if (j % 8 == 0) printf(" ");
                    printf("%02x ", (int)rx_dbuf[j]);
                    if (j % 16 == 15) printf("\r\n");
                    if (j % 512 == 511) printf("\r\n");
                }
                for (j = 0; j < NAND_SPARE_AREA_SIZE; j++) {
                    if (j % 16 == 0) printf("%03x: ", (unsigned int)j + NAND_PAGE_SIZE);
                    if (j % 8 == 0) printf(" ");
                    printf("%02x ", (int)rx_sbuf[j]);
                    if (j % 16 == 15) printf("\r\n");
                }
                printf("--------------------------------------------------\r\n");
                continue;
            }
        }
        
        if (!err) printf("OK.\r\n");
    }
    
    printf("--------------------------------------------------\r\n");
    printf("Total %d bad blocks found", (int)num_bad_blocks);
    if (num_bad_blocks > 0) {
        printf(": ");
        for (b = 0; b < num_bad_blocks; b++) printf("%d ", (int)bad_block[b]);
    }
    printf("\r\n");
    
    printf("Total %d read errors.\r\n", (int)read_err_count);
    printf("Total %d verify errors.\r\n", (int)verify_err_count);
}


/*----------------------------------------------------------------------*/
/*  Test code for embedded NOR flash driver                             */
/*----------------------------------------------------------------------*/

void nor_test(void)
{
    int32_t  i, j, err, verify_err_count = 0;
    uint32_t b, test_start_block = 0, test_end_block = 6;
    uint16_t p;
    
    static uint8_t tx_dbuf[512];
    static uint8_t rx_dbuf[512];
    
    printf("--------------------------------------------------\r\n");
    printf("   Test for embedded NOR flash low-level driver \r\n");
    printf("--------------------------------------------------\r\n");

    /* fill the buffer */
    for (i = 0; i < 512; i++) tx_dbuf[i] = i;

#if 1
    /* test starts... erase each block and fill data in all the pages */
    for (b = test_start_block; b <= test_end_block; b++) {
        printf("Erase block %d ... ", (int)b);
        
        /* erase block */
        err = nor_erase(0, b);
        if (err) {
            printf("FAILED !!\r\n");
            continue;
        }
        
        printf("done. Writing pages in block %d ... ", (int)b);

        /* write each page in the block */
        for (p = 0; p < 256; p++) {
            *((uint32_t *)(tx_dbuf)) = p;
            *((uint32_t *)(tx_dbuf + 4)) = b;
            
            err = nor_write_page(0, b, p, tx_dbuf, NULL, 1);
            if (err) {
                printf("FAILED (page = %d) !!\r\n", (int)p);
                break;
            }
        }
        if (err) continue;
        
        printf("OK.\r\n");
    }
#endif

    /* now verify the data in the pages */
    for (b = test_start_block; b <= test_end_block; b++) {
        printf("Read block %d and verifying the data ... ", (int)b);
        
        /* read each page in the block */
        for (p = 0; p < 256; p++) {
#if 1
            err = nor_read_page(0, b, p, rx_dbuf, NULL);
            if (err) {
                printf("read FAILED (page = %d) !!\r\n", (int)p);
            }

            if (*((uint32_t *)(rx_dbuf)) != p ||
                *((uint32_t *)(rx_dbuf + 4)) != b ||
                memcmp(rx_dbuf+8, tx_dbuf+8, 504) != 0) {
                printf("verify FAILED (page = %d) !!\r\n", (int)p);
                verify_err_count++;
                err = -1;
            }
#else
            err = nor_read_bytes(0, b, p, 8, rx_dbuf);
            if (err) {
                printf("read FAILED (page = %d) !!\r\n", (int)p);
            }

            if (*((uint32_t *)(rx_dbuf)) != p ||
                *((uint32_t *)(rx_dbuf + 4)) != b) {
                printf("verify FAILED (page = %d) !!\r\n", (int)p);
                verify_err_count++;
                err = -1;
            }
#endif

            if (err) {
                printf("\r\nRead data (block = %d, page = %d):\r\n", (int)b, (int)p);
                printf("--------------------------------------------------\r\n");
                for (j = 0; j < 512; j++) {
                    if (j % 16 == 0) printf("%03x: ", (unsigned int)j);
                    if (j % 8 == 0) printf(" ");
                    printf("%02x ", (int)rx_dbuf[j]);
                    if (j % 16 == 15) printf("\r\n");
                }
                printf("--------------------------------------------------\r\n");
                continue;
            }
        }
        
        if (!err) printf("OK.\r\n");
    }
    
    printf("--------------------------------------------------\r\n");
    printf("Total %d verify errors.\r\n", (int)verify_err_count);
}

#endif /* CFD_M */
