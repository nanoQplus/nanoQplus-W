//===================================================================
//
// taskq.h (@sheart, @haekim)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef TASKQ_H
#define TASKQ_H
#include "kconf.h"

#include "nos_common.h"

#define TASKQ_LEN          (5) // task queue length

typedef struct _task
{
	void (*func)(void *args);
	void *args;
} TASK;

typedef struct _taskq
{
	UINT8 head, tail;
	TASK task[TASKQ_LEN];
} TASKQ;

extern TASKQ taskq;

#define OS_TASKQ_IS_EMPTY()	(taskq.head == taskq.tail)

void os_taskq_init(void);
void os_taskq_exe(void);
UINT32 taskq_register(void (*func)(void *args), void *args);

#endif // ~TASKQ_H
