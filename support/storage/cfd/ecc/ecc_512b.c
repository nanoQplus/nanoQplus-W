/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : ecc_512b.c                                                */
/*  PURPOSE : Code for handling ECC (especially for 512 bytes data)     */
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

#include "kconf.h"
#ifdef CFD_M

#include "fd_config.h"
#include "fd_if.h"
#include "fd_physical.h"
#include "ecc_512b.h"

#if (ECC_METHOD == SW_ECC)
/************************************************************************/
/*                                                                      */
/* NAME                                                                 */
/*        make_ecc_512byte                                              */
/* DESCRIPTION                                                          */
/*        This function computes the ECC of a given data (page).        */
/* PARAMETERS                                                           */
/*        ecc_buf       the location where ECC should be stored         */
/*        data_buf      given data                                      */
/* RETURN VALUES                                                        */
/*        Nothing                                                       */
/*                                                                      */
/************************************************************************/
void make_ecc_512byte(uint8_t *ecc_buf, uint8_t *data_buf)
{
    uint32_t  iIndex;

    uint32_t  iLsum, iParity;

    uint32_t  iParityLo;
    uint32_t  iParityHi;

    uint8_t   iInvLow, iInvHigh;
    uint8_t   iLow, iHigh;

    uint8_t   iP1_1, iP1_2, iP2_1, iP2_2, iP4_1, iP4_2, iP8_1, iP8_2;
    uint8_t   iP16_1, iP16_2, iP32_1, iP32_2, iP64_1, iP64_2, iP128_1, iP128_2;
    uint8_t   iP256_1, iP256_2, iP512_1, iP512_2, iP1024_1, iP1024_2, iP2048_1, iP2048_2;

    uint32_t  *temp_datal = (uint32_t *)data_buf;

    iParityLo = iParityHi = iLsum = 0;

    // bit position [0 ~ 63]
    for (iIndex = 0; iIndex < 512/8; iIndex++ ) {
        iParityLo = iParityLo ^ (*(temp_datal + iIndex * 2));
        iParityHi = iParityHi ^ (*(temp_datal + iIndex * 2 + 1));
        //iParityLo = iParityLo ^ GET32(((uint8_t *) temp_datal + iIndex * 2 * 4));
        //iParityHi = iParityHi ^ GET32(((uint8_t *) temp_datal + iIndex * 2 * 4 + 1));
    }

    iParity = iParityLo ^ iParityHi;
    for (iIndex = 0; iIndex < 32; iIndex++)
    {
        iLsum ^= ((iParity >> iIndex) & 0x1);    
    }

    iParity = iParityLo ^ iParityHi;        // bit0
    iParity &= 0x55555555;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iLow = (iParity & 0x1);

    iParity = iParityLo ^ iParityHi;        // bit1
    iParity &= 0x33333333;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iLow |= ((iParity & 0x1) << 1);

    iParity = iParityLo ^ iParityHi;        // bit2
    iParity &= 0x0F0F0F0F;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iLow |= ((iParity & 0x1) << 2);

    iParity = iParityLo ^ iParityHi;        // bit3
    iParity &= 0x00FF00FF;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iLow |= ((iParity & 0x1) << 3);

    iParity = iParityLo ^ iParityHi;        // bit4
    iParity &= 0x0000FFFF;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iLow |= ((iParity & 0x1) << 4);

    iParity = iParityLo;                // bit5

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iLow |= ((iParity & 0x1) << 5);
    iLow &= 0x3f;

    if (iLsum == 0) 
       iInvLow = iLow;
    else 
       iInvLow = ~iLow & 0x3f;

    iParityLo = iParityHi = iLsum = 0;

    // byte position [0 ~ 63]
    for (iIndex = 0; iIndex < (512/8); iIndex++)
    {
        uint32_t iDataLo, iDataHi;

        iDataLo = (*(temp_datal + iIndex * 2));
        iDataHi = (*(temp_datal + iIndex * 2 + 1));
        //iDataLo = GET32(((uint8_t *) temp_datal + iIndex * 2 * 4));
        //iDataHi = GET32(((uint8_t *) temp_datal + iIndex * 2 * 4 + 1));
    
        iDataLo = iDataLo ^ iDataHi;
        iDataLo = (iDataLo >> 16) ^ iDataLo;
        iDataLo = (iDataLo >> 8) ^ iDataLo;
        iDataLo = (iDataLo >> 4) ^ iDataLo;
        iDataLo = (iDataLo >> 2) ^ iDataLo;
        iDataLo = (iDataLo >> 1) ^ iDataLo;

        if (iIndex < 32)
           iParityLo |= ((iDataLo & 0x01) << iIndex);
        else
           iParityHi |= ((iDataLo & 0x01) << (iIndex - 32));
           iLsum ^= (iDataLo & 0x1);
    }

    iParity = iParityLo ^ iParityHi;        // bit0
    iParity &= 0x55555555;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iHigh = (iParity & 0x1);

    iParity = iParityLo ^ iParityHi;        // bit1
    iParity &= 0x33333333;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iHigh |= ((iParity & 0x1) << 1);

    iParity = iParityLo ^ iParityHi;        // bit2
    iParity &= 0x0F0F0F0F;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iHigh |= ((iParity & 0x1) << 2);

    iParity = iParityLo ^ iParityHi;        // bit3
    iParity &= 0x00FF00FF;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iHigh |= ((iParity & 0x1) << 3);

    iParity = iParityLo ^ iParityHi;        // bit4
    iParity &= 0x0000FFFF;

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iHigh |= ((iParity & 0x1) << 4);

    iParity = iParityLo;                    // bit5

    iParity = (iParity >> 16) ^ iParity;
    iParity = (iParity >> 8) ^ iParity;
    iParity = (iParity >> 4) ^ iParity;
    iParity = (iParity >> 2) ^ iParity;
    iParity = (iParity >> 1) ^ iParity;

    iHigh |= ((iParity & 0x1) << 5);
    iHigh &= 0x3f;

    if (iLsum == 0) 
       iInvHigh = iHigh;
    else 
       iInvHigh = ~iHigh & 0x3f;

    iP2048_2 = (iHigh & 0x20) >> 5;
    iP2048_1 = (iInvHigh & 0x20) >> 4;
    iP1_2 = (iLow & 0x1) << 2;
    iP1_1 = (iInvLow & 0x1) << 3;
    iP2_2 = (iLow & 0x2) << 3;
    iP2_1 = (iInvLow & 0x2) << 4;
    iP4_2 = (iLow & 0x4) << 4;
    iP4_1 = (iInvLow & 0x4) << 5;

    iP8_2 = (iLow & 0x8) >> 3;
    iP8_1 = (iInvLow & 0x8) >> 2;
    iP16_2 = (iLow & 0x10) >> 2;
    iP16_1 = (iInvLow & 0x10) >> 1;
    iP32_2 = (iLow & 0x20) >> 1;
    iP32_1 = (iInvLow & 0x20);
    iP64_2 = (iHigh & 0x1) << 6;
    iP64_1 = (iInvHigh & 0x1) << 7;

    iP128_2 = (iHigh & 0x2) >> 1;
    iP128_1 = (iInvHigh & 0x2);
    iP256_2 = (iHigh & 0x4);
    iP256_1 = (iInvHigh & 0x4) << 1;
    iP512_2 = (iHigh & 0x8) << 1;
    iP512_1 = (iInvHigh & 0x8) << 2;
    iP1024_2 = (iHigh & 0x10) << 2;
    iP1024_1 = (iInvHigh & 0x10) << 3;

    *(ecc_buf + 0) = ~( iP64_1|iP64_2|iP32_1|iP32_2
                        |iP16_1|iP16_2|iP8_1|iP8_2 );
    *(ecc_buf + 1) = ~( iP1024_1|iP1024_2|iP512_1|iP512_2
                        |iP256_1|iP256_2|iP128_1|iP128_2 );
    *(ecc_buf + 2) = ~( iP4_1|iP4_2|iP2_1|iP2_2|iP1_1|iP1_2
                        |iP2048_1|iP2048_2 );
}
#endif /* (ECC_METHOD == SW_ECC) */


#if (ECC_METHOD == HW_ECC) || (ECC_METHOD == SW_ECC)
/************************************************************************/
/*                                                                      */
/* NAME                                                                 */
/*        compare_ecc_512byte                                           */
/* DESCRIPTION                                                          */
/*        This function compares two ECCs and indicates if there is     */
/*        an error.                                                     */
/* PARAMETERS                                                           */
/*        ecc_data1     one ECC to be compared                          */
/*        ecc_data2     the other ECC to be compared                    */
/*        page_data     content of data page                            */
/*        offset        byte offset where the error occurred            */
/*        corrected     correct data for the errorneous byte            */
/* RETURN VALUES                                                        */
/*        Upon successful completion, compare_ecc returns ECC_NO_ERROR. */
/*        Otherwise, corresponding error code is returned.              */
/*                                                                      */
/************************************************************************/
int32_t compare_ecc_512byte(uint8_t *ecc_data1, uint8_t *ecc_data2, 
                            uint8_t *page_data, int32_t *offset, 
                            uint8_t *corrected)
{
    uint32_t  i;
    uint8_t   tmp0_bit[8], tmp1_bit[8], tmp2_bit[8];
    uint8_t   comp0_bit[8], comp1_bit[8], comp2_bit[8];
    uint8_t   tmp[3];
    uint8_t   ecc_bit[24];
    uint8_t   ecc_sum = 0;
    uint8_t   ecc_value;
    uint8_t   find_bit = 0;
    uint32_t  find_byte = 0;

    for(i = 0; i <= 2; i++){
        *(ecc_data1 + i) = ~(*(ecc_data1 + i));
        *(ecc_data2 + i) = ~(*(ecc_data2 + i));
    }
    tmp0_bit[0] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;

    tmp0_bit[1] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;
    
    tmp0_bit[2] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;
    
    tmp0_bit[3] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;
    
    tmp0_bit[4] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;
    
    tmp0_bit[5] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;
    
    tmp0_bit[6] = *ecc_data1 % 2;
    *ecc_data1 = *ecc_data1 / 2;
    
    tmp0_bit[7] = *ecc_data1 % 2;

    tmp1_bit[0] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[1] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[2] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[3] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[4] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[5] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[6] = *(ecc_data1 + 1) % 2;
    *(ecc_data1+1) = *(ecc_data1 + 1) / 2;

    tmp1_bit[7] = *(ecc_data1 + 1) % 2;

    tmp2_bit[0] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[1] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[2] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[3] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[4] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[5] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[6] = *(ecc_data1 + 2) % 2;
    *(ecc_data1+2) = *(ecc_data1 + 2) / 2;

    tmp2_bit[7] = *(ecc_data1 + 2) % 2;

    comp0_bit[0] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[1] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[2] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[3] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[4] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[5] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[6] = *ecc_data2 % 2;
    *ecc_data2 = *ecc_data2 / 2;

    comp0_bit[7] = *ecc_data2 % 2;

    comp1_bit[0] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[1] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[2] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[3] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[4] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[5] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[6] = *(ecc_data2 + 1) % 2;
    *(ecc_data2+1) = *(ecc_data2 + 1) / 2;

    comp1_bit[7] = *(ecc_data2 + 1) % 2;

    comp2_bit[0] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[1] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[2] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[3] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[4] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[5] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[6] = *(ecc_data2 + 2) % 2;
    *(ecc_data2+2) = *(ecc_data2 + 2) / 2;

    comp2_bit[7] = *(ecc_data2 + 2) % 2;

    for(i = 0; i < 6; i++) {
        ecc_bit[i] = tmp2_bit[i + 2] ^ comp2_bit[i + 2];
    }

    for(i = 0; i < 8; i++) {
        ecc_bit[i + 6] = tmp0_bit[i] ^ comp0_bit[i];
    }

    for(i = 0; i < 8; i++) {
        ecc_bit[i + 14] = tmp1_bit[i] ^ comp1_bit[i];
    }
    
    ecc_bit[22] = tmp2_bit[0] ^ comp2_bit[0];
    ecc_bit[23] = tmp2_bit[1] ^ comp2_bit[1];

    for(i = 0; i < 24; i++) {
        ecc_sum += ecc_bit[i];
    }

    switch(ecc_sum) {
    case 0 :
        /* actually not reached. */
        /* if this function is not called because two ecc's are equal. */
        ECC_DBG_PRINT((TEXT("RESULT : no error\n")));
        return ECC_NO_ERROR;
    
    case 1 :
        ECC_DBG_PRINT((TEXT("RESULT : ecc error\n")));
        return ECC_ECC_ERROR;
    
    case 12 :
        find_byte = 
              (ecc_bit[23] << 8) + (ecc_bit[21] << 7) + (ecc_bit[19] << 6)
            + (ecc_bit[17] << 5) + (ecc_bit[15] << 4) + (ecc_bit[13] << 3)
            + (ecc_bit[11] << 2) + (ecc_bit[9] << 1) + ecc_bit[7];
        find_bit = (ecc_bit[5] << 2) + (ecc_bit[3] << 1) + ecc_bit[1];
        ecc_value = (page_data[find_byte] >> find_bit) % 2;

        if(ecc_value == 0) {
            ecc_value=1;
        } else {
            ecc_value=0;
        }

        *tmp = page_data[find_byte];
        tmp0_bit[0] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[1] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[2] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[3] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[4] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[5] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[6] = *tmp % 2;
        *tmp = *tmp / 2;
        tmp0_bit[7] = *tmp % 2;
        tmp0_bit[find_bit] = ecc_value;

        *tmp = (tmp0_bit[7] << 7) + (tmp0_bit[6] << 6) + (tmp0_bit[5] << 5)
             + (tmp0_bit[4] << 4) + (tmp0_bit[3] << 3) + (tmp0_bit[2] << 2)
             + (tmp0_bit[1] << 1) + tmp0_bit[0];

        ECC_DBG_PRINT((TEXT("RESULT : one bit error\n")));
        ECC_DBG_PRINT((TEXT("\toriginal page_data[%d] = %x\n"), 
                      find_byte, *tmp));
        ECC_DBG_PRINT((TEXT("\terror page_data[%d] = %x(%d -> %d)\n"),
                      find_byte, page_data[find_byte], 
                      ecc_value, (ecc_value + 1) % 2));

        if (offset != NULL) {
            *offset = (int32_t)find_byte;
        }
        if (corrected != NULL) {
            *corrected = *tmp;
        }
        //FD_ECC_Corrected = TRUE;
        return ECC_CORRECTABLE_ERROR;

    default :
        ECC_DBG_PRINT((TEXT("RESULT : uncorrectable error\n")));
        return ECC_UNCORRECTABLE_ERROR;
    }
}

#endif /* (ECC_METHOD == HW_ECC) || (ECC_METHOD == SW_ECC) */
#endif /* CFD_M */
