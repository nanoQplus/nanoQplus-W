//===================================================================
//
// mutex.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef MUTEX_H
#define MUTEX_H
#include "kconf.h"
#include "thread.h"
#include "queue_thread.h"

#include "nos_common.h"

typedef struct _mutex
{
	UINT32	ceil_priority;
	UINT32	saved_priority;
	UINT32	lock_level;
	THREAD  *owner;
	TQUEUE  wait_queue;
} MUTEX;

#define NO_CEILING	(0)

UINT32 mutex_create(UINT32 *muid, UINT32 ceil_priority);
UINT32 mutex_destroy(UINT32 muid);
UINT32 mutex_lock(UINT32 muid);
UINT32 mutex_unlock(UINT32 muid);

#endif // ~MUTEX_H
