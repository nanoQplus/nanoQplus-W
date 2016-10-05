#include "kconf.h"

#ifdef CFD_M
extern void fsmc_nand_test(void);
extern void nor_test(void);
#endif

void app_init(void)
{
#ifdef CFD_M
    fsmc_nand_test();
    nor_test();
#endif
}
