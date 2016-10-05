/**
 * Test CFD module
 */
#include "kconf.h"
#include "stm32f4xx.h"

#ifdef CFD_M
extern void erase_all(uint16_t chip_id);
#endif

void app_init(void)
{
#ifdef CFD_M
    erase_all(0);       // for NAND flash
    erase_all(1);       // for NOR flash
#endif
}

