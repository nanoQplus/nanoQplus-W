//===================================================================
//
// event.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef EVENT_H
#define EVENT_H
#include "kconf.h"

#include "nos_common.h"
#include "hal_sched.h"

#define MAX_NUM_TOTAL_EVENT		(32) // the number of events 

#define EVENT(n)	(1 << n) /* 0< n <=31 */

typedef UINT32 EVENT_MASK;

UINT32 event_clear(UINT32 mask);
UINT32 event_get(UINT32 tid, UINT32 *event);
UINT32 event_set(UINT32 tid, UINT32 mask);
UINT32 event_wait(UINT32 mask);

#endif // EVENT_H
