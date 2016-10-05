/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : nor_lld.h                                                 */
/*  PURPOSE : Low-Level Device Driver for embedded NOR flash            */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  Authors : Zeen Information Technologies, Inc.                       */
/*            (info@zeen.snu.ac.kr)                                     */
/*----------------------------------------------------------------------*/
/*  The copyright of this software is subject to the copyright of ETRI  */
/*  Nano OS. For more information, please contact authorities of ETRI.  */
/*----------------------------------------------------------------------*/
/*  NOTES                                                               */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 0.9)                                          */
/*                                                                      */
/*  - 2016.01.15 [Joosun Hahn]   : First writing                        */
/*  - 2016.02.17 [Sung-Kwan Kim] : Revised to adapt to CFD framework    */
/*                                                                      */
/************************************************************************/

#ifndef __NOR_LLD_H
#define __NOR_LLD_H

#include "stm32f4xx.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

#define NOR_NUM_CHIPS           1

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  Global Function Declarations                                        */
/*----------------------------------------------------------------------*/

int32_t  nor_init         (void);
int32_t  nor_open         (uint16_t chip);
int32_t  nor_close        (uint16_t chip);
int32_t  nor_read_page    (uint16_t chip, uint32_t block, uint16_t page,
                           uint8_t *dbuf, uint8_t *sbuf);
int32_t  nor_read_bytes   (uint16_t chip, uint32_t block, uint16_t page,
                           uint32_t num_bytes, uint8_t *dbuf);
int32_t  nor_write_page   (uint16_t chip, uint32_t block, uint16_t page,
                           uint8_t *dbuf, uint8_t *sbuf, bool_t is_last);
int32_t  nor_write_bytes  (uint16_t chip, uint32_t block, uint16_t page,
                           uint32_t num_bytes, uint8_t *dbuf);
int32_t  nor_erase        (uint16_t chip, uint32_t block);
int32_t  nor_sync         (uint16_t chip, uint16_t prev_command);
bool_t   nor_is_bad_block (uint16_t chip, uint32_t block);

#endif /* __NOR_LLD_H */
