#include "kconf.h"
#include "stm32f4xx_fsmc.h"

#define NOR_PAGE_SIZE               512

//---------------------------------------------------------------------------

#define NOR_READ_PAGE(block, page, dbuf)                                    \
{                                                                           \
    static int32_t  i       __SSL_EXTRA_MEM_ATTR;                           \
    static int32_t  addr    __SSL_EXTRA_MEM_ATTR;                           \
                                                                            \
    addr = 0x08020000 + (block * 0x20000);                                  \
    addr += (page * NOR_PAGE_SIZE);                                         \
                                                                            \
    switch ((uint32_t)dbuf & 0x03) {                                        \
        case 0:                                                             \
            for (i = 0; i < NOR_PAGE_SIZE; i += 4, addr += 4) {             \
                *(__IO uint32_t *)(dbuf + i) = *(__IO uint32_t *)addr;      \
            }                                                               \
            break;                                                          \
                                                                            \
        case 2:                                                             \
            for (i = 0; i < NOR_PAGE_SIZE; i += 2, addr += 2) {             \
                *(__IO uint16_t *)(dbuf + i) = *(__IO uint16_t *)addr;      \
            }                                                               \
            break;                                                          \
                                                                            \
        default:                                                            \
            for (i = 0; i < NOR_PAGE_SIZE; i++, addr++) {                   \
                *(__IO uint8_t *)(dbuf + i) = *(__IO uint8_t *)addr;        \
            }                                                               \
            break;                                                          \
    }                                                                       \
}
