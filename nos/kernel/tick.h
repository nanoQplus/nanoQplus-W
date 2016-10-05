//===================================================================
//
// tick.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef TICK_H
#define TICK_H
#include "kconf.h"

#include "nos_common.h"
#include "hal_sched.h"

void tickq_Init(void);
void tickq_Push(DNODE *new_node, UINT32 delta);
void tickq_Remove(DNODE *old_node);
void tickq_Expired(void);

#endif // ~USER_ALARM_H

