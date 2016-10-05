/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : CFD (Common Flash Driver)                                 */
/*  FILE    : fd_config.h                                               */
/*  PURPOSE : Header file for CFD module configuration                  */
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

#ifndef _FD_CONFIG_H
#define _FD_CONFIG_H

#include "fd_oal.h"

/*----------------------------------------------------------------------*/
/*  Configuration for CFD-wide Configurable Constants                   */
/*----------------------------------------------------------------------*/

#define NUM_FLASH_CHIPS         2       /* # of all flash memory chips on your
                                           target platform; for example,
                                           if there are two NAND chips and one
                                           NOR chip, this value should be 3 */

/*----------------------------------------------------------------------*/
/*  Configuration for Driver-Level Bad Block Management (DLBM) Module   */
/*----------------------------------------------------------------------*/

#define USE_DLBM                1       /* 1: use DLBM function (default)
                                           0: don't use DLBM function */

/*----------------------------------------------------------------------*/
/*  Configuration for Physical Flash Device (PFD) Layer                 */
/*----------------------------------------------------------------------*/

/* ecc methods (select one from the following methods) */

#define NO_ECC                  0       /* no ecc */
#define HW_ECC                  1       /* hardware ecc */
#define SW_ECC                  2       /* software ecc */

#define ECC_METHOD              HW_ECC

/* copy-back implementation (select one from the following methods) */

#define SW_COPYBACK             0       /* S/W copy-back, i.e. implementing
                                           copy-back with read & write */
#define HW_COPYBACK             1       /* use device's copy-back function */
#define SAFE_HW_COPYBACK        2       /* use device's copy-back function
                                           only after ...
                                           1. read the source page
                                           2. if the page has no ECC error */

#define COPYBACK_METHOD         SW_COPYBACK

/*----------------------------------------------------------------------*/
/*  Configuration for Debugging                                         */
/*----------------------------------------------------------------------*/

#define STRICT_CHECK            0       /* 1: enable strict parameter check
                                           0: disable strict check (default) */

#endif /* _FD_CONFIG_H */
