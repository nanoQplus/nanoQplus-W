//===================================================================
//
// tick.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "queue_delta.h"
#include "nos_timer.h"
#include "error.h"

#include "lowpower.h"

extern UINT32 SysTick_Reload_OverFlow;
extern UINT32 __gcounter;
extern UINT32 __saved_alid;

DQUEUE	tick_q;

void tickq_Expired(void);
void tickq_Init(void)
{
	init_dqueue(&tick_q);
}

void tickq_Push(DNODE *new_node, UINT32 delta)
{
	DNODE *dnode = tick_q.head;
	UINT32 passed_tick;
	UINT32 i;

	passed_tick = os_timer_tick_get();
	os_timer_tick_stop();
		
	/* calibrate the current timer */
	if (dnode) {
		if (dnode->delta >= passed_tick) {	
			if (passed_tick == 1) {
				dnode->delta -= passed_tick;
			} // end if
			actual_idle_tick -= passed_tick;
		} // end if
	} // end if

	/* find the appropriate point to insert the new node */
	for (i = 0; (dnode != NULL) && (delta >= dnode->delta); dnode = dnode->next, i++) {
		delta -= dnode->delta;
	} // end for

	/* set the delta of the new node */
	new_node->delta = delta;

	/* insert the new node at the last */
	if (!dnode) {
		push_dnode(&tick_q, new_node);
	} else {
		/* subtract the delta value of the found node and insert the new node */
		dnode->delta -= delta;
		
		/* place the new node before the dnode */
		insert_dnode(&tick_q, dnode, new_node);		
	} // end if

	dnode = tick_q.head;

#ifdef PWM_M	
	actual_idle_tick = dnode->delta;
	//__gcounter += actual_idle_tick * 10;
	////__saved_alid = dnode->arg;
#endif	
	//uart_printf("tickq_Push :: dnode->delta = %d\r\n", dnode->delta);
	os_timer_tick_set(dnode->delta);
} // end func

void tickq_Remove(DNODE *old_node)
{
	DNODE *dnode_head = tick_q.head;
	DNODE *dnode_next = old_node->next;

	//uart_printf("tickq_Remove :: called!!\r\n");

	/* calibrate the current timer if old_node participates in tickling */
	if (dnode_head == old_node)
	{	
		UINT32 passed_tick;

		passed_tick = os_timer_tick_get();
		os_timer_tick_stop();

		if (old_node->delta >= passed_tick)
		{	
			old_node->delta -= passed_tick;
		}
	}

	/* update the delta value */
	if (dnode_next != NULL)
	{
		dnode_next->delta += old_node->delta; /* old_node->delta is updated above */
	}
	
	/* delete the old_node from the queue */
	delete_dnode(&tick_q, old_node);

	if (dnode_head == old_node)
	{
		os_timer_tick_set(dnode_next->delta);
	}
}

void tickq_Expired(void) {
	DNODE *dnode = tick_q.head;

	//uart_printf("tickq_Expired :: called!!\r\n");

	os_timer_tick_stop();
	
	if (dnode) {
		dnode->delta = 0;
		
		for (; (dnode != NULL); dnode=tick_q.head) {
			if (dnode->delta == 0) { /* dnode is expired */
				/* update the queue by removing the dnode processed */
				delete_first_dnode(&tick_q, dnode);

				/* dnode's handler is executed. */
				dnode->handler(dnode->arg);
			} else {
				break; /* escape the loop */
			} // end if
		} // end for
				
		/* set the next tick alarm */
		if ((dnode != NULL) & (SysTick_Reload_OverFlow == 0)) {
			os_timer_tick_set(dnode->delta);
		} // end if
	} // end if
} // end func
