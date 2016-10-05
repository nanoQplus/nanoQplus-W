//===================================================================
//
// queue_delta.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "nos_common.h"
#include "queue_delta.h"

void init_dqueue(DQUEUE *q)
{
	q->head = q->tail = NULL;
	q->count = 0;
}

void init_dnode(DNODE *node, void (*handler)(UINT32), UINT32 arg)
{
	node->prev = node->next = NULL;
	node->delta = 0;
	node->handler = handler;
	node->arg = arg;
}

void push_dnode(DQUEUE *q, DNODE *node)
{
	if (q->head == NULL)
	{
		q->head = node;
		q->tail = node;
		node->prev = NULL;
		node->next = NULL;
	}
	else
	{
		node->prev = q->tail;
		node->next = NULL;
		q->tail->next = node;
		q->tail = node;
	}

	q->count++;
}

/* insert new_node before the node in the linked list */
void insert_dnode(DQUEUE *q, DNODE *node, DNODE *new_node)
{
	if (node != NULL) /* if node is NULL, it is error */
	{
		if (node->prev != NULL)
		{
			node->prev->next = new_node;
			new_node->prev = node->prev;
			node->prev = new_node;
			new_node->next = node;
		}
		else
		{
			new_node->prev = NULL;
			node->prev = new_node;
			new_node->next = node;
			q->head = new_node;
		}
		
		q->count++;
	}
}

#if 0
int add_dnode(DQUEUE *q, DNODE *node, DNODE *new_node)
{
	DNODE *p;

	for (p=q->head; p!=NULL; p = p->next)
	{
		if (p == node)
			break;
	}

	if (p!=NULL) // node is found
	{
		if (p->prev != NULL)
		{
			p->prev->next = new_node;
			new_node->prev = p->prev;
			p->prev = new_node;
			new_node->next = p;
		}
		else
		{
			new_node->prev = NULL;
			p->prev = new_node;
			new_node->next = p;
			q->head = new_node;
		}
	}

	q->count++;

	return 0;
}
#endif

void delete_first_dnode(DQUEUE *q, DNODE *node)
{
	if (node->next != NULL)
	{
		q->head = node->next;

		node->next->prev = NULL;
		node->next = NULL;
	}
	else /* node->prev == NULL; node->next == NULL */
	{
		q->head = q->tail = NULL;
	}

	q->count--;
}

void delete_dnode(DQUEUE *q, DNODE *node)
{
	if (node!=NULL) // found
	{
		if (node->prev != NULL && node->next != NULL)
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = NULL;
			node->prev = NULL;
		}
		else if (node->prev == NULL && node->next != NULL)
		{
			q->head = node->next;

			node->next->prev = NULL;
			node->next = NULL;
		}
		else if (node->prev != NULL && node->next == NULL)
		{
			q->tail = node->prev;

			node->prev->next = NULL;
			node->prev = NULL;
		}
		else /* p->prev == NULL; p->next == NULL */
		{
			q->head = q->tail = NULL;
		}

		q->count--;
	}
}

DNODE *pop_dnode(DQUEUE *q)
{
	DNODE *p=q->head;

	if (p!=NULL) // found
	{
		q->head = p->next;

		if (p->next != NULL)
		{
			p->next->prev = p->prev;
		}
		else
		{
			q->tail = NULL;
		}

		q->count--;

		return p;
	}
	else
		return NULL;
		
}

int is_dnode_in_dqueue(DNODE *node, DQUEUE *q)
{
    DNODE *p;

    for (p=q->head; p!=NULL; p = p->next)
    {
            if (p == node)
                    return 1;
    }

    return 0;
}

#if 0
void print_dqueue(DQUEUE *q)
{
	DNODE *p;

	for (p=q->head; p!=NULL; p = p->next)
	{
		uart_printf("delta = %d\n", p->delta);
	}
}

void print_dnode(DNODE *node)
{
	uart_printf("delta = %d\n", node->delta);
}
#endif
