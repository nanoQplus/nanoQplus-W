//===================================================================
//
// queue_thread.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H
#include "thread.h"
#include "typedef.h"


typedef struct _tqueue
{
    THREAD *head;
    THREAD *tail;
	UINT32 count;
} TQUEUE;

void init_tqueue(TQUEUE *q);
void init_tnode(THREAD *thread);
void push_tnode(TQUEUE *q, THREAD *thread);
int add_tnode(TQUEUE *q, THREAD *thread, THREAD *new_thread);
int delete_tnode(TQUEUE *q, THREAD *thread);
THREAD *pop_tnode(TQUEUE *q);
int is_tnode_in_queue(THREAD *thread, TQUEUE *q);
//void print_tqueue(TQUEUE *q);
//void print_tnode(THREAD *thread);
#endif
