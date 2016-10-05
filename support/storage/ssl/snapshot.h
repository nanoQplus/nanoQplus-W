/************************************************************************/
/*                                                                      */
/*  PROJECT : Flash Memory Management S/W for ETRI Nano OS              */
/*  MODULE  : SSL (Snapshot Support Layer)                              */
/*  FILE    : snapshot2.h                                               */
/*  PURPOSE : Header file for saving/restoring snapshot images          */
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
/*  - 2016.03.20 [Sung-Kwan Kim] : First writing                        */
/*                                                                      */
/************************************************************************/

#ifndef _SNAPSHOT2_H
#define _SNAPSHOT2_H

#include "fd_config.h"

/*----------------------------------------------------------------------*/
/*  Constant & Macro Definitions                                        */
/*----------------------------------------------------------------------*/

#define MAX_CPU_REGS            64
#define MAX_IO_REGS             64

enum SSL_API_RETURN_VALUES {
    SSL_SUCCESS                 = 0,
    SSL_OPEN_FAIL               = 1,
    SSL_DEVICE_NOT_OPEN         = 2,
    SSL_NO_VALID_SNAPSHOT       = 3,
    SSL_ERROR                   = 4,
    SSL_CRITICAL_ERROR          = 5
};

/*----------------------------------------------------------------------*/
/*  Type Definitions                                                    */
/*----------------------------------------------------------------------*/

/* data structure for memory range description */

typedef struct {
    void           *start_addr;
    uint32_t        num_bytes;
} mem_desc_t;

/* data structure for device snapshot */

typedef struct {
    uint32_t        num_cpu_regs;
    uint32_t        cpu_reg[MAX_CPU_REGS];
    
    uint32_t        num_io_regs;
    uint32_t        io_reg[MAX_IO_REGS];
    
    mem_desc_t      mem_desc;
} snapshot_t;

/* SSL variable attribute for variable allocation in extra RAM */

#define __SSL_EXTRA_MEM_ATTR  __attribute__((section (".ccmram")))

/*----------------------------------------------------------------------*/
/*  External Variable Declarations                                      */
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/*  External Function Declarations                                      */
/*----------------------------------------------------------------------*/

extern int32_t  ssl_open        (uint16_t chip_id, uint16_t part_id);
extern int32_t  ssl_close       (void);

extern int32_t  ssl_reset       (void);
extern int32_t  ssl_save        (snapshot_t *snapshot);
extern int32_t  ssl_restore     (snapshot_t *snapshot);

#endif /* _SNAPSHOT2_H */
