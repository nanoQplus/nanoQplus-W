/**
 * Test CFD module
 */
#include "kconf.h"
#include "stm32f4xx.h"

#ifdef CFD_M
extern void cfd_test(uint16_t chip_id);
#endif

void app_init(void)
{
#ifdef CFD_M
    cfd_test(0);    // CFD test for NAND flash
    cfd_test(1);    // CFD test for NOR flash
#endif
}

