//===================================================================
//
// api.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef API_H
#define API_H

#ifndef arm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "error.h"

// common definitions
#include "os_init.h"
#define os_init()		OS_INIT()

#include "heap.h"
#define malloc(length)		nos_malloc(length)	//void *os_malloc(UINT16 len)
#define free(ptr)		nos_free(ptr)	//void os_free(void *p)

#include "critical_section.h"
#define ENTER_CRITICAL()	os_sched_unlock()
#define EXIT_CRITICAL()		os_sched_unlock_switch()

#include "kernel.h"
//#define thread_terminate()	thread_terminate(_rtid)

#endif	//~API_H
