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
#include "snapshot.h"

#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------*/
/*  Test code for saving & restoring snapshot data                      */
/*----------------------------------------------------------------------*/

#define RANDOM_NUM      552092

static uint8_t mem1[128],  mem1_bak[128];
static uint8_t mem2[256],  mem2_bak[256];
static uint8_t mem3[512],  mem3_bak[512];
static uint8_t mem4[1024], mem4_bak[1024];
static uint8_t mem5[2048], mem5_bak[2048];

static snapshot_t snapshot;

void snapshot_test(uint16_t chip_id)
{
    int32_t  err;
    uint32_t i, n = RANDOM_NUM;
    
    printf("--------------------------------------------------\r\n");
    printf("    Snapshot save/restore test for %s flash\r\n", (chip_id == 0 ? "NAND" : "NOR"));
    printf("--------------------------------------------------\r\n");

#if 1
    err = ssl_open(chip_id, 0);     // for a given chip, 1st partition is used
    if (err) {
        printf("ssl_open() for ssl_save() failed.\r\n");
        return;
    }

    /* fill patterns in memory chunks */
    MEMCPY(mem1, (void *)0x08001000, 128);
    MEMCPY(mem1_bak, mem1, 128);
    
    MEMCPY(mem2, (void *)0x08002000, 256);
    MEMCPY(mem2_bak, mem2, 256);
    
    MEMCPY(mem3, (void *)0x08003000, 512);
    MEMCPY(mem3_bak, mem3, 512);
    
    MEMCPY(mem4, (void *)0x08004000, 1024);
    MEMCPY(mem4_bak, mem4, 1024);
    
    MEMCPY(mem5, (void *)0x08005000, 2048);
    MEMCPY(mem5_bak, mem5, 2048);

#if 0
    printf("------------------------------------------------------\r\n");
    for (i = 0; i < 1024; i++) {
        if (i % 16 == 0) printf("%03x: ", (unsigned int)i);
        if (i % 8 == 0) printf(" ");
        printf("%02x ", (int)mem4[i]);
        if (i % 16 == 15) printf("\r\n");
        if (i % 512 == 511) printf("\r\n");
    }
    printf("------------------------------------------------------\r\n");
#endif

    /* fill the snapshot structure */
    snapshot.num_cpu_regs  = 16;
    snapshot.num_io_regs   = 32;
    snapshot.num_mem_descs = 5;
    
    for (i = 0; i < snapshot.num_cpu_regs; i++) {
        snapshot.cpu_reg[i] = n++;
    }
    
    for (i = 0; i < snapshot.num_io_regs; i++) {
        snapshot.io_reg[i] = n++;
    }
    
    snapshot.mem_desc[0].start_addr = mem1;
    snapshot.mem_desc[0].num_bytes  = 128;
    
    snapshot.mem_desc[1].start_addr = mem2;
    snapshot.mem_desc[1].num_bytes  = 256;
    
    snapshot.mem_desc[2].start_addr = mem3;
    snapshot.mem_desc[2].num_bytes  = 512;
    
    snapshot.mem_desc[3].start_addr = mem4;
    snapshot.mem_desc[3].num_bytes  = 1024;
    
    snapshot.mem_desc[4].start_addr = mem5;
    snapshot.mem_desc[4].num_bytes  = 2048;

    /* save the snapshot to flash memory */
    err = ssl_save(&snapshot);
    if (err) {
        printf("ssl_save() failed.\r\n");
        return;
    }
    
    ssl_close();
#endif

#if 1
    err = ssl_open(chip_id, 0);     // for a given chip, 1st partition is used
    if (err) {
        printf("ssl_open() for ssl_restore() failed.\r\n");
        return;
    }

    /* for testing, reset the snapshot and the memory chunks */
    MEMSET(&snapshot, 0, sizeof(snapshot_t));
    MEMSET(mem1, 0, 128);
    MEMSET(mem2, 0, 256);
    MEMSET(mem3, 0, 512);
    MEMSET(mem4, 0, 1024);
    MEMSET(mem5, 0, 2048);

    /* restore the snapshot from flash memory */
    err = ssl_restore(&snapshot);
    if (err) {
        if (err == SSL_NO_VALID_SNAPSHOT)
            printf("ssl_restore() : no snapshot to restore.\r\n");
        else
            printf("ssl_restore() failed.\r\n");
        
        return;
    }

    /* compare the restored snapshot with the original */
    printf("Save and restore completed.\r\n");
    printf("Restored snapshot:\r\n");
    printf("num_cpu_regs  = %d\r\n", (int)snapshot.num_cpu_regs);
    printf("num_io_regs   = %d\r\n", (int)snapshot.num_io_regs);
    printf("num_mem_descs = %d\r\n", (int)snapshot.num_mem_descs);

    for (i = 0; i < snapshot.num_cpu_regs; i++) {
        printf("cpu_reg[%d] = %d\r\n", (int)i, (int)snapshot.cpu_reg[i]);
    }
    
    for (i = 0; i < snapshot.num_io_regs; i++) {
        printf("io_reg[%d] = %d\r\n", (int)i, (int)snapshot.io_reg[i]);
    }

    printf("mem_desc[%d].num_bytes = %d ", 0, (int)snapshot.mem_desc[0].num_bytes);
    if (MEMCMP(mem1, mem1_bak, 128) == 0) printf("(OK)\r\n");
    else printf("(ERROR)\r\n");
    
    printf("mem_desc[%d].num_bytes = %d ", 1, (int)snapshot.mem_desc[1].num_bytes);
    if (MEMCMP(mem2, mem2_bak, 256) == 0) printf("(OK)\r\n");
    else printf("(ERROR)\r\n");
    
    printf("mem_desc[%d].num_bytes = %d ", 2, (int)snapshot.mem_desc[2].num_bytes);
    if (MEMCMP(mem3, mem3_bak, 512) == 0) printf("(OK)\r\n");
    else printf("(ERROR)\r\n");
    
    printf("mem_desc[%d].num_bytes = %d ", 3, (int)snapshot.mem_desc[3].num_bytes);
    if (MEMCMP(mem4, mem4_bak, 1024) == 0) printf("(OK)\r\n");
    else printf("(ERROR)\r\n");
    
    printf("mem_desc[%d].num_bytes = %d ", 4, (int)snapshot.mem_desc[4].num_bytes);
    if (MEMCMP(mem5, mem5_bak, 2048) == 0) printf("(OK)\r\n");
    else printf("(ERROR)\r\n");
    
    ssl_close();
#endif
}

#endif /* CFD_M */
