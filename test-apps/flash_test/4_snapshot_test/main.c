/**
 * Test CFD module
 */
#include "kconf.h"
#include "stm32f4xx.h"

#ifdef SSL_M
extern void snapshot_test(uint16_t chip_id);
#endif

void app_init(void)
{
#ifdef SSL_M
    snapshot_test(0);   // use NAND flash for snapshot storage
    snapshot_test(1);   // use NOR flash for snapshot storage
#endif
}

