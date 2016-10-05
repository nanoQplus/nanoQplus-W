/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_lld_wrapper.h                                          */
/*  PURPOSE : Header file for LLD (Low-Level Device Driver) wrapper     */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  Authors : Zeen Information Technologies, Inc.                       */
/*            (info@zeen.snu.ac.kr)                                     */
/*----------------------------------------------------------------------*/
/*  The copyright of this software is subject to the copyright of ETRI  */
/*  Nano OS. For more information, please contact authorities of ETRI.  */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 1.0)                                          */
/*                                                                      */
/*  - 2010.10.01 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#ifndef _FD_LLD_WRAPPER_H
#define _FD_LLD_WRAPPER_H

#include "fd_config.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

/* LLD function call syntax prefix */

#define LLD(func_name)          (pdev->chip_ops.func_name)

/* macro for checking if a specific LLD function is defined or not */

#define LLD_DEFINED(func_name)  (LLD(func_name) != NULL)

/*----------------------------------------------------------------------*/
/*  LLD Interface (Macros)                                              */
/*----------------------------------------------------------------------*/

#define lld_open(pdev)                                                  \
        LLD(open(pdev->local_chip_id))

#define lld_close(pdev)                                                 \
        LLD(close(pdev->local_chip_id))

/*----------------------------------------------------------------------*/
/*  LLD Interface (Functions)                                           */
/*----------------------------------------------------------------------*/

extern int32_t  lld_read_page   (pfdev_t *pdev, uint32_t block, uint16_t page,
                                 uint8_t *dbuf, uint8_t *sbuf);
extern int32_t  lld_read_bytes  (pfdev_t *pdev, uint32_t block, uint16_t page,
                                 uint32_t num_bytes, uint8_t *dbuf);
extern int32_t  lld_write_page  (pfdev_t *pdev, uint32_t block, uint16_t page,
                                 uint8_t *dbuf, uint8_t *sbuf, bool_t is_last);
extern int32_t  lld_write_bytes (pfdev_t *pdev, uint32_t block, uint16_t page,
                                 uint32_t num_bytes, uint8_t *dbuf);
extern int32_t  lld_copyback    (pfdev_t *pdev, 
                                 uint32_t src_block, uint16_t src_page, 
                                 uint32_t dst_block, uint16_t dst_page);
extern int32_t  lld_erase       (pfdev_t *pdev, uint32_t block);
extern int32_t  lld_erase_verify(pfdev_t *pdev, uint32_t block);
extern bool_t   lld_is_bad_block(pfdev_t *pdev, uint32_t block);
extern int32_t  lld_sync        (pfdev_t *pdev);

#endif /* _FD_LLD_WRAPPER_H */
