//===================================================================
//
// kernel.h (@sheart, @haekim)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef KERNEL_H
#define KERNEL_H
#include "kconf.h"
#include "sched.h"
#include "thread.h"
#include "taskq.h"
#include "alarm.h"
#include "event.h"
#include "mutex.h"
#include "msgq.h"
#include "time.h"

void nos_kernel_init(void);

#endif // ~KERNEL_H
