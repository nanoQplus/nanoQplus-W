//===================================================================
//
// queue.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "nos_common.h"
#include "queue.h"

void init_queue(QUEUE *q)
{
	q->head = q->tail = NULL;
	q->count = 0;
}

void init_node(NODE *node)
{
	node->prev = node->next = NULL;
}
//Just Insert End Point of Queue.
void push_node(QUEUE *q, NODE *node)
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
// Insert Front of p node.
int add_node(QUEUE *q, NODE *p, NODE *new_node)
{
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

int delete_node(QUEUE *q, NODE *p)
{
	if (p!=NULL) // found
	{
		if (p->prev != NULL && p->next != NULL)
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
			p->next = NULL;
			p->prev = NULL;
		}
		else if (p->prev == NULL && p->next != NULL)
		{
			q->head = p->next;

			p->next->prev = NULL;
			p->next = NULL;
		}
		else if (p->prev != NULL && p->next == NULL)
		{
			q->tail = p->prev;

			p->prev->next = NULL;
			p->prev = NULL;
		}
		else /* p->prev == NULL; p->next == NULL */
		{
			q->head = q->tail = NULL;
		}

		q->count--;
	}

	return 0;
}

NODE *pop_node(QUEUE *q)
{
	NODE *p=q->head;

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

int is_node_in_queue(NODE *node, QUEUE *q)
{
    NODE *p;

    for (p=q->head; p!=NULL; p = p->next)
    {
            if (p == node)
                    return 1;
    }

    return 0;
}

#if 0
void print_queue(QUEUE *q)
{
	NODE *p;

	for (p=q->head; p!=NULL; p = p->next)
	{
		uart_printf("value = %d\n", p->value);
	}
}

void print_node(NODE *node)
{
	uart_printf("value = %d\n", node->value);
}
#endif

