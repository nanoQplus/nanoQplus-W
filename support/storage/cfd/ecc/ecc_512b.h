/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : ecc_512b.h                                                */
/*  PURPOSE : Header file for handling ECC                              */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*  Authors : Zeen Information Technologies, Inc.                       */
/*            (info@zeen.snu.ac.kr)                                     */
/*----------------------------------------------------------------------*/
/*  The copyright of this software is subject to the copyright of ETRI  */
/*  Nano OS. For more information, please contact authorities of ETRI.  */
/*----------------------------------------------------------------------*/
/*  REVISION HISTORY (Ver 2.0)                                          */
/*                                                                      */
/*  - 01/07/2003 [Dong-Hee Lee]  : first writing                        */
/*  - 01/12/2003 [Sung-Kwan Kim] : Trimmed & Adapted for ZFS project    */
/*                                                                      */
/************************************************************************/

#ifndef _ECC_H
#define _ECC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

/* ECC comparison results */

enum ECC_COMPARE_RESULT {
    ECC_NO_ERROR                = 0,        /* no error */
    ECC_CORRECTABLE_ERROR       = 1,        /* correctable bit-flip errors */
    ECC_ECC_ERROR               = 2,        /* ECC error */
    ECC_UNCORRECTABLE_ERROR     = 3         /* uncorrectable errors */
};

/* macro for reading 32-bit integer */

#define GET32(p_src) \
        ( ((uint32_t)(p_src)[0]) | ((uint32_t)(p_src[1]) << 8) | \
          ((uint32_t)(p_src)[2] << 16) | ((uint32_t)(p_src)[3] << 24) )

/* for debug */

#ifdef DEBUG_ECC
#define ECC_DBG_PRINT(x)        printf(x)
#else
#define ECC_DBG_PRINT(x)
#endif

/*----------------------------------------------------------------------*/
/*  External Function Declarations                                      */
/*----------------------------------------------------------------------*/

/* for 512 byte page */

extern void     make_ecc_512byte    (uint8_t *ecc_buf, uint8_t *data_buf);
extern int32_t  compare_ecc_512byte (uint8_t *ecc_data1, uint8_t *ecc_data2, 
                                     uint8_t *page_data, int32_t *offset, 
                                     uint8_t *corrected);

/* for 256 word page */

extern void     make_ecc_256word    (uint16_t *ecc_buf, uint16_t *data_buf);
extern int32_t  compare_ecc_256word (uint8_t *ecc_data1, uint8_t *ecc_data2, 
                                     uint16_t *page_data, int32_t *offset, 
                                     uint16_t *corrected);

#ifdef __cplusplus
}
#endif /* __cplusplus  */

#endif /* _ECC_H */
