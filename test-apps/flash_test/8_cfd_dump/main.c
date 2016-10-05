/**
 * Test CFD module
 */
#include "kconf.h"
#include "stm32f4xx.h"

#ifdef CFD_M
extern void cfd_dump(uint16_t chip_id, uint16_t part_id, uint32_t block, uint16_t page);
#endif

void app_init(void)
{
#ifdef CFD_M
    cfd_dump(0, 0, 0, 0);  // CFD dump for NAND flash, partition 0, block 0, page 0
#endif
}

