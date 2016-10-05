/************************************************************************/
/*                                                                      */
/*  PROJECT : CFD (Common Flash Driver)                                 */
/*  FILE    : test.c                                                    */
/*  PURPOSE : Test code for CFD dump                                    */
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
/*  Test code for CFD dump                                              */
/*----------------------------------------------------------------------*/

void cfd_dump(uint16_t chip_id, uint16_t part_id, uint32_t block, uint16_t page)
{
    int32_t  err, j;
    uint32_t dev_id;
    flash_dev_spec_t dev_info;

    static uint8_t rx_dbuf[2048], rx_sbuf[64];

    dev_id = SET_DEV_ID(LFD_BLK_DEVICE_RAW, SET_RAWDEV_SERIAL(chip_id, part_id));
    
    printf("--------------------------------------------------\r\n");
    printf("    CFD dump for %s flash, partition-%d\r\n", (chip_id == 0 ? "NAND" : "NOR"), (int)part_id);
    printf("                 block-%d, page-%d\r\n", (int)block, (int)page);
    printf("--------------------------------------------------\r\n");

    err = fd_open(dev_id);
    if (err) {
        printf("[CFD DUMP] fd_open() failed (err = %d).\r\n", (int)err);
        return;
    }
    
    err = fd_get_device_info(dev_id, &dev_info);
    if (err) {
        printf("[CFD DUMP] fd_get_device_info() failed (err = %d).\r\n", (int)err);
        fd_close(dev_id);
        return;
    }
    
    /* read page */
    err = fd_read_page(dev_id, block, page, rx_dbuf, rx_sbuf);
    if (err) {
        printf("fd_read_page() failed (err = %d, page = %d).\r\n", (int)err, (int)page);
        fd_close(dev_id);
        return;
    }

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
}

#endif /* CFD_M */
