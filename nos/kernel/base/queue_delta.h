//===================================================================
//
// queue_delta.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#ifndef QUEUE_DELTA_H
#define QUEUE_DELTA_H
#include "typedef.h"

typedef struct _dnode
{
	UINT32 delta;
	void (*handler)(UINT32);
	UINT32 arg;
	struct _dnode *prev;
	struct _dnode *next;
} DNODE;

typedef struct _dqueue
{
	DNODE *head;
	DNODE *tail;
	UINT32 count;
} DQUEUE;

void init_dqueue(DQUEUE *q);
void init_dnode(DNODE *node, void (*handler)(UINT32), UINT32 arg);
void push_dnode(DQUEUE *q, DNODE *node);
void insert_dnode(DQUEUE *q, DNODE *node, DNODE *new_node);
//int add_dnode(DQUEUE *q, DNODE *node, DNODE *new_node);
void delete_first_dnode(DQUEUE *q, DNODE *node);
void delete_dnode(DQUEUE *q, DNODE *node);
DNODE *pop_dnode(DQUEUE *q);
int is_dnode_in_dqueue(DNODE *node, DQUEUE *q);
void print_dqueue(DQUEUE *q);
void print_dnode(DNODE *node);
#endif
