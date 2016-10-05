//===================================================================
//
// thread_table.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef THREAD_TABLE_H
#define THREAD_TABLE_H

#include "nos_common.h"
#include "thread.h"

/* ready queue management */
void os_qPush(THREAD *thread);
void os_qRemove(THREAD *thread);
#endif
