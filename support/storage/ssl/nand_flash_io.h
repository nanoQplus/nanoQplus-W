#include "kconf.h"
#include "stm32f4xx_fsmc.h"
#include "fsmc_nand.h"

#define FSMC_NAND_BANK              FSMC_Bank2
#define NAND_BASE_ADDR              ((uint32_t)0x70000000)
#define NAND_ATTR_ADDR              ((uint32_t)0x78000000)
#define ROW_ADDRESS(block, page)    ((page) + ((block) * NAND_BLOCK_SIZE))

#define PCR_ECCEN_SET               ((uint32_t)0x00000040)
#define PCR_ECCEN_RESET             ((uint32_t)0x000FFFBF)

#define DMA_RD_STREAM               DMA2_Stream1
#define DMA_RD_TCIF                 DMA_FLAG_TCIF1
#define DMA_DATA_SIZE_MASK          ((uint32_t)0xFFFF87FF)
#define RESERVED_MASK               ((uint32_t)0x0F7D0F7D)

#if 0
#define DATA_READ(buf, size)        DATA_READ_CPU(buf, size)
#else
#define DATA_READ(buf, size)        DATA_READ_DMA(buf, size)
#endif

//---------------------------------------------------------------------------

#define WAIT_FLASH_READY()                                                  \
{                                                                           \
    static uint8_t bitstatus __SSL_EXTRA_MEM_ATTR;                          \
                                                                            \
    bitstatus = 0;                                                          \
    while (bitstatus == 0) {                                                \
        if ((GPIOD->IDR & GPIO_Pin_6) != (uint32_t)Bit_RESET) {             \
            bitstatus = (uint8_t)Bit_SET;                                   \
        }                                                                   \
        else {                                                              \
            bitstatus = (uint8_t)Bit_RESET;                                 \
        }                                                                   \
    }                                                                       \
}

//---------------------------------------------------------------------------

#define DATA_READ_CPU(buf, size)                                            \
{                                                                           \
    static uint32_t i __SSL_EXTRA_MEM_ATTR;                                 \
                                                                            \
    for (i = 0; (uint32_t)(buf + i) & 0x03; i++) {                          \
        (buf)[i] = *(vu8 *)(NAND_ATTR_ADDR | DATA_AREA);                    \
    }                                                                       \
                                                                            \
    for ( ; i < size - 3; i += 4) {                                         \
        *(uint32_t *)(buf + i) = *(vu32 *)(NAND_ATTR_ADDR | DATA_AREA);     \
    }                                                                       \
                                                                            \
    for ( ; i < size; i++) {                                                \
        (buf)[i] = *(vu8 *)(NAND_ATTR_ADDR | DATA_AREA);                    \
    }                                                                       \
}

//---------------------------------------------------------------------------

#define DATA_READ_DMA(buf, size)                                            \
{                                                                           \
    static bool_t    word_aligned __SSL_EXTRA_MEM_ATTR;                     \
    static uint32_t  timeout      __SSL_EXTRA_MEM_ATTR;                     \
                                                                            \
    word_aligned = TRUE;                                                    \
                                                                            \
    if ((uint32_t)(buf) & 0x03) word_aligned = FALSE;                       \
    if (size & 0x03) word_aligned = FALSE;                                  \
                                                                            \
    while ((DMA_RD_STREAM->CR & (uint32_t)DMA_SxCR_EN) != 0);               \
                                                                            \
    DMA_RD_STREAM->M0AR = (uint32_t)(buf);                                  \
    DMA_RD_STREAM->CR  &= DMA_DATA_SIZE_MASK;                               \
                                                                            \
    if (word_aligned) {                                                     \
        DMA_RD_STREAM->CR  |= DMA_PeripheralDataSize_Word;                  \
        DMA_RD_STREAM->CR  |= DMA_MemoryDataSize_Word;                      \
        DMA_RD_STREAM->NDTR = (size >> 2);                                  \
    }                                                                       \
    else {                                                                  \
        DMA_RD_STREAM->CR  |= DMA_PeripheralDataSize_Byte;                  \
        DMA_RD_STREAM->CR  |= DMA_MemoryDataSize_Byte;                      \
        DMA_RD_STREAM->NDTR = size;                                         \
    }                                                                       \
                                                                            \
    DMA2->LIFCR = (uint32_t)(DMA_RD_TCIF & RESERVED_MASK);                  \
    DMA_RD_STREAM->CR |= (uint32_t)DMA_SxCR_EN;     /* DMA enable */        \
                                                                            \
    timeout = 0x100000;                                                     \
    while (((DMA_RD_STREAM->CR & (uint32_t)DMA_SxCR_EN) == 0) && (--timeout > 0));      \
    if (timeout == 0) {                                                                 \
        printf("[SSL] %s: DMA command timeout (buf = %p, size = %d) !!\r\n",            \
               __FUNCTION__, buf, (int)size);                                           \
        return(SSL_CRITICAL_ERROR);                                                     \
    }                                                                                   \
    else {                                                                              \
        timeout = 0x100000;                                                             \
        while (((DMA2->LISR & DMA_RD_TCIF & RESERVED_MASK) == 0) && (--timeout > 0));   \
        if (timeout == 0) {                                                             \
            printf("[SSL] %s: DMA completion timeout (buf = %p, size = %d) !!\r\n",     \
                   __FUNCTION__, buf, (int)size);                           \
            return(SSL_CRITICAL_ERROR);                                     \
        }                                                                   \
        else {                                                              \
            /* DMA completion OK */                                         \
        }                                                                   \
    }                                                                       \
}

//---------------------------------------------------------------------------

#define ECC_MASK28              0x0FFFFFFF  /* 28 valid ECC parity bits */
#define ECC_MASK                0x05555555  /* 14 ECC parity bits       */

enum ECC_COMPARE_RESULT {
    ECC_NO_ERROR                = 0,        /* no error */
    ECC_CORRECTABLE_ERROR       = 1,        /* correctable bit-flip errors */
    ECC_ECC_ERROR               = 2,        /* ECC error */
    ECC_UNCORRECTABLE_ERROR     = 3         /* uncorrectable errors */
};

#define ECC_CORRECT_DATA(ecc_calc, ecc_read, data, result)                  \
{                                                                           \
    static uint32_t count       __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t bit_num     __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t byte_addr   __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t mask        __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t syndrome    __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t ecc_even    __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t ecc_odd     __SSL_EXTRA_MEM_ATTR;                       \
    static int32_t  __result    __SSL_EXTRA_MEM_ATTR;                       \
                                                                            \
    ecc_calc ^= 0xFFFFFFFF;                                                 \
    ecc_read ^= 0xFFFFFFFF;                                                 \
    syndrome = (ecc_calc ^ ecc_read) & ECC_MASK28;                          \
                                                                            \
    if (syndrome == 0) {                                                    \
        __result = ECC_NO_ERROR;                                            \
        goto end;                                                           \
    }                                                                       \
                                                                            \
    ecc_odd  = syndrome & ECC_MASK;                                         \
    ecc_even = (syndrome >> 1) & ECC_MASK;                                  \
                                                                            \
    if ((ecc_odd ^ ecc_even) == ECC_MASK) {                                 \
        bit_num = (ecc_even & 0x01) |                                       \
                  ((ecc_even >> 1) & 0x02) |                                \
                  ((ecc_even >> 2) & 0x04);                                 \
                                                                            \
        byte_addr = ((ecc_even >> 6) & 0x001) |                             \
                    ((ecc_even >> 7) & 0x002) |                             \
                    ((ecc_even >> 8) & 0x004) |                             \
                    ((ecc_even >> 9) & 0x008) |                             \
                    ((ecc_even >> 10) & 0x010) |                            \
                    ((ecc_even >> 11) & 0x020) |                            \
                    ((ecc_even >> 12) & 0x040) |                            \
                    ((ecc_even >> 13) & 0x080) |                            \
                    ((ecc_even >> 14) & 0x100) |                            \
                    ((ecc_even >> 15) & 0x200) |                            \
                    ((ecc_even >> 16) & 0x400) ;                            \
                                                                            \
        data[byte_addr] ^= (1 << bit_num);                                  \
                                                                            \
        __result = ECC_CORRECTABLE_ERROR;                                   \
        goto end;                                                           \
    }                                                                       \
                                                                            \
    count = 0;                                                              \
    mask  = 0x00800000;                                                     \
    while (mask) {                                                          \
        if (syndrome & mask) count++;                                       \
        mask >>= 1;                                                         \
    }                                                                       \
                                                                            \
    if (count == 1) {                                                       \
        __result = ECC_ECC_ERROR;                                           \
        goto end;                                                           \
    }                                                                       \
                                                                            \
    __result = ECC_UNCORRECTABLE_ERROR;                                     \
end:                                                                        \
    result = __result;                                                      \
}

//---------------------------------------------------------------------------

#define NAND_READ_PAGE(block, page, dbuf, sbuf)                             \
{                                                                           \
    static int32_t  result      __SSL_EXTRA_MEM_ATTR;                       \
    static int32_t  retry       __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t ecc_calc    __SSL_EXTRA_MEM_ATTR;                       \
    static uint32_t ecc_read    __SSL_EXTRA_MEM_ATTR;                       \
                                                                            \
    err = 0;                                                                \
    retry = 0;                                                              \
                                                                            \
    /* send the page-read command and page address */                       \
    *(vu8 *)(NAND_BASE_ADDR | CMD_AREA) = NAND_CMD_READ_1;                  \
                                                                            \
    *(vu8 *)(NAND_BASE_ADDR | ADDR_AREA) = 0x00;                            \
    *(vu8 *)(NAND_BASE_ADDR | ADDR_AREA) = 0x00;                            \
    *(vu8 *)(NAND_BASE_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS(block, page));    \
    *(vu8 *)(NAND_BASE_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS(block, page));    \
                                                                            \
    *(vu8 *)(NAND_BASE_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE;               \
                                                                            \
    WAIT_FLASH_READY();                                                     \
                                                                            \
    /* enable ECC */                                                        \
    FSMC_NAND_BANK->PCR2 |= PCR_ECCEN_SET;                                  \
                                                                            \
    /* read the main area data */                                           \
    DATA_READ(dbuf, NAND_PAGE_SIZE);                                        \
                                                                            \
    /* read ECC parity bytes from the ECC register */                       \
    ecc_calc = FSMC_NAND_BANK->ECCR2;                                       \
    ecc_calc ^= 0xFFFFFFFF;                                                 \
                                                                            \
    /* disable ECC */                                                       \
    FSMC_NAND_BANK->PCR2 &= PCR_ECCEN_RESET;                                \
                                                                            \
    /* send the random output command to read data in the spare area */     \
    *(vu8 *)(NAND_BASE_ADDR | CMD_AREA) = NAND_CMD_RANDOMOUT;               \
                                                                            \
    *(vu8 *)(NAND_BASE_ADDR | ADDR_AREA) = 0x00;                            \
    *(vu8 *)(NAND_BASE_ADDR | ADDR_AREA) = 0x08;                            \
                                                                            \
    *(vu8 *)(NAND_BASE_ADDR | CMD_AREA) = NAND_CMD_RANDOMOUT_TRUE;          \
                                                                            \
    /* read the spare area data */                                          \
    DATA_READ(sbuf, NAND_SPARE_AREA_SIZE);                                  \
                                                                            \
    /* ecc check */                                                         \
    ecc_read = *(uint32_t *)(sbuf + 8);                                     \
                                                                            \
ecc_retry:                                                                  \
    if (ecc_calc != ecc_read) {                                             \
                                                                            \
        /* ECC mismatch; try to correct errors */                           \
        ECC_CORRECT_DATA(ecc_calc, ecc_read, dbuf, result);                 \
                                                                            \
        switch (result) {                                                   \
        case ECC_NO_ERROR:                                                  \
            break;                                                          \
                                                                            \
        case ECC_CORRECTABLE_ERROR:                                         \
            break;                                                          \
                                                                            \
        case ECC_ECC_ERROR:                                                 \
            if (retry == 0) {                                               \
                retry = 1;                                                  \
                ecc_read = *(uint32_t *)(sbuf + 12);                        \
                goto ecc_retry;                                             \
            }                                                               \
            /* retry failed; fall through below */                          \
                                                                            \
        default:                                                            \
            err = 1;                                                        \
            printf("[SSL] %s: UNCORRECTABLE ECC ERROR (block = %d, page = %d) !!\r\n",  \
                   __FUNCTION__, (int)block, (int)page);                                \
            break;                                                          \
        }                                                                   \
    }                                                                       \
                                                                            \
    if (err) return(SSL_CRITICAL_ERROR);                                    \
}
