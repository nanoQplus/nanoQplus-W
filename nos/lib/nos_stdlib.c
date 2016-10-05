/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file nos_stdlib.c
 * @brief NanoQplus standard library
 * @author Haeyong Kim (ETRI)
 * @date 2015.03.06.
 * @ingroup 
 * @copyright GNU General Public License v3
 */


#define NDEBUG
#include "nos_debug.h"

#include <string.h>
#include "nos_stdlib.h"


void nos_swap(uint8_t *data1, uint8_t *data2, uint8_t len)
{
    uint8_t tmp_data[NOS_MAX_SWAP_LEN];

    if (len > NOS_MAX_SWAP_LEN)
        return;
    memcpy(tmp_data, data1, len);
    memcpy(data1, data2, len);
    memcpy(data2, tmp_data, len);
}

void nos_stoa(uint8_t *array, uint16_t data16)
{
    array[1] = (uint8_t)data16;
    array[0] = (uint8_t)(data16 >> 8);
}
void nos_ltoa(uint8_t *array, uint32_t data32)
{
    array[3] = (uint8_t)data32;
    array[2] = (uint8_t)(data32 >> 8);
    array[1] = (uint8_t)(data32 >> 16);
    array[0] = (uint8_t)(data32 >> 24);
}  

uint16_t nos_atos(uint8_t *array)
{
    uint16_t data16;
    data16 = array[0];
    data16 <<= 8;
    data16 |= array[1];
    return data16;
}

uint32_t nos_atol(uint8_t *array)
{
    uint32_t data32;
    data32 = array[0];
    data32 <<= 8;
    data32 |= array[1];
    data32 <<= 8;
    data32 |= array[2];
    data32 <<= 8;
    data32 |= array[3];
    return data32;
}

