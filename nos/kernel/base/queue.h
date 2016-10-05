//===================================================================
//
// queue.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef QUEUE_H
#define QUEUE_H
#include "typedef.h"

typedef struct _node
{
    UINT32 value;
    struct _node *prev;
    struct _node *next;
} NODE;

typedef struct _queue
{
    NODE *head;
    NODE *tail;
	UINT32 count;
} QUEUE;

void init_queue(QUEUE *q);
void init_node(NODE *node);
void push_node(QUEUE *q, NODE *node);
int add_node(QUEUE *q, NODE *node, NODE *new_node);
int delete_node(QUEUE *q, NODE *node);
NODE *pop_node(QUEUE *q);
int is_node_in_queue(NODE *node, QUEUE *q);
void print_queue(QUEUE *q);
void print_node(NODE *node);
#endif
