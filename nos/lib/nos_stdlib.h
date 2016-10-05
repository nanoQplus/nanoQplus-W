/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file nos_stdlib.h
 * @brief NanoQplus standard library
 * @author Haeyong Kim (ETRI)
 * @date 2015.03.06.
 * @ingroup 
 * @copyright GNU General Public License v3
 */


#ifndef _NOS_STDLIB_H_
#define _NOS_STDLIB_H_

#include "nos_common.h"


#define NOS_MAX_SWAP_LEN    16
void nos_swap(uint8_t *data1, uint8_t *data2, uint8_t len);



void nos_stoa(uint8_t *array, uint16_t data16);
void nos_ltoa(uint8_t *array, uint32_t data32);
uint16_t nos_atos(uint8_t *array);
uint32_t nos_atol(uint8_t *array);



#endif
