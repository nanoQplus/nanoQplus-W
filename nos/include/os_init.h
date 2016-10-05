//===================================================================
//
// os_init.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef OS_INIT_H
#define OS_INIT_H
#include "kconf.h"
#include "nos_common.h"

#include "platform.h"	// Platform type
#include "arch.h"	// MCU type 
#include "intr.h"

#include "kernel.h"

/*
#define OS_INIT() do{ \
				NOS_DISABLE_GLOBAL_INTERRUPT(); \
				nos_arch_init(); \
				nos_kernel_init(); \
			}while (0)
	*/		
/*	
#define NOS_ENTER_CRITICAL_SECTION()            \
				do {										\
					NOS_DISABLE_GLOBAL_INTERRUPT(); 		\
					++_nested_intr_cnt; 					\
				} while (0)
*/

#endif //~OS_INIT_H

