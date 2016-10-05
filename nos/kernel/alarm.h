//===================================================================
//
// alarm.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef ALARM_H
#define ALARM_H
#include "kconf.h"

#include "nos_common.h"
#include "hal_sched.h"
#include "queue_delta.h"

typedef struct _alarm
{
	UINT32	alid;				// alarm id
	UINT32  increment;			// first relative tick value
	UINT32  cycle;				// periodic cycle value
	UINT32	work;				// alarm function working time @phj.
	DNODE	alarm_dnode;
	void 	(*handler)(UINT32);	// function pointer
	UINT32 	arg;				// function argument
} ALARM;

STATUS _alarm_spawn(void (*func)(UINT32), UINT32 arg, UINT32 increment, UINT32 cycle, UINT32 *alid, UINT32 work);
UINT32 _alarm_create(void (*func)(UINT32), UINT32 arg, UINT32 increment, UINT32 cycle, UINT32 *alid, UINT32 work);

#define alarm_spawn(a1,a2,a3,a4,a5) (_alarm_spawn(a1,a2,a3,a4,a5,0))
#define alarm_create(b1,b2,b3,b4,b5) (_alarm_create(b1,b2,b3,b4,b5,0))

UINT32 alarm_destroy(UINT32 alid);
UINT32 alarm_start(UINT32 alid);
UINT32 alarm_stop(UINT32 alid);
UINT32 alarm_get_cycle(UINT32 alid);

#define get_alarm_cycle(alid)	(alarm->cycle)
typedef void 	(*ALARM_HANDLER)(UINT32);

#endif // ~USER_ALARM_H
