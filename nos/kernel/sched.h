//===================================================================
//
// sched.h (@jun361, @sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef SCHED_H
#define SCHED_H
#include "kconf.h"

#include "nos_common.h"
#include "thread.h"
//#include "hal_context.s"

// priority setting
// LOWEST <<----------------->> HIGHEST
//   0     			  255
#define PRIORITY_LEVEL_COUNT	256 // 256 prorities available

#define PRIORITY_ULTRA		255  // reserved for the system thread
#define PRIORITY_HIGHEST	5//254	
#define PRIORITY_HIGH		4//200
#define PRIORITY_NORMAL		3//100
#define PRIORITY_LOW		2//50
#define PRIORITY_LOWEST		1   // reserved for the idle thread

#define	PRIORITY_IDLE_THREAD	0
#define	PRIORITY_SUPER_THREAD	255

#define PRIORITY(N)		N

extern THREAD 	*current_thread;
extern THREAD 	*idle_thread;

void (*sched_callback)(void);	// this varable indicates the scheduler is working or not

void os_zero_switch_context(THREAD *prev, THREAD *next);
void os_switch_context(THREAD *prev, THREAD *next);
void os_load_context(THREAD *thread);
void os_sched_init(void);
void os_start(void);
void os_sched_lock(void);
void os_sched_unlock(void);
void os_sched_unlock_switch(void);
void os_sched_unlock_bottom_half(void);

#endif // ~SCHED_H
