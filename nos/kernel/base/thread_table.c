//===================================================================
//
// thread_table.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "thread.h"
#include "sched.h"
#include "queue_thread.h"

//#define OFFSET_OF(TYPE, MEMBER) ((unsigned int)(&((TYPE *)0)->MEMBER))

extern TQUEUE 	os_rdy_q[PRIORITY_LEVEL_COUNT]; /* ready queue is an array of QUEUEs */

/* management of tables */

const UINT32 map_table[16]  = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
				 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000};

const UINT32 unmap_table[256]  = 
			    {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
				4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
				5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
				5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
				6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
				6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
				6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
				6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
				7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};

UINT32 ready_table[16] = {0, };
UINT32 ready_group = 0;

THREAD *highest_thread;

static UINT32 is_tqueue_empty(TQUEUE *queue);
static void os_calHighestThread(void);

static UINT32 is_tqueue_empty(TQUEUE *queue)
{
	return (queue->head == NULL);
}

void os_qPush(THREAD *thread)
{
	UINT32 prio = thread->priority;

	if (is_tqueue_empty(&os_rdy_q[prio]))
	{	
		ready_group             |= 	map_table[prio >>    4];
		ready_table[prio >> 4]	|= 	map_table[prio &  0x0f];
	}

	/* enqueue error cannot occur
	   already queue is checked if it is full 
	 */
	 
	push_tnode(&os_rdy_q[prio], thread);
	os_calHighestThread();
}

void os_qRemove(THREAD *thread)
{
	UINT32 prio = thread->priority;

	delete_tnode(&os_rdy_q[prio], thread);

	if (is_tqueue_empty(&os_rdy_q[prio]))
	{
		if ((ready_table[prio >> 4] &= (UINT32)(~map_table[prio & 0x0f])) == 0)
		{
			ready_group &= (UINT32)(~map_table[prio >> 4]);
		}
	}
	os_calHighestThread();
}

static void os_calHighestThread(void)
{
	UINT32 x, y, prio;
	//NODE *highestNode;

    	// find y
    	y = ready_group;
    	if (y>>8) // upper nibble
    	{
       		y = (UINT32)(unmap_table[y>>8] + 8);
    	}
    	else
    	{
       		y = unmap_table[y & 0xff]; // lower nibble
    	}

    	// find x
    	x = ready_table[y];
    	if (x>>8) // upper nibble
    	{
       		x = (UINT32)(unmap_table[x>>8] + 8);
    	}
	else
    	{
       		x = unmap_table[x & 0xff]; // lower nibble
	}
	
	prio = (y << 4) + x;

	//highestNode = os_rdy_q[prio].head;
	//highest_thread = (THREAD *) ((UINT32)highestNode - OFFSET_OF(_TCB, rdy_node));

	highest_thread = os_rdy_q[prio].head;
}

