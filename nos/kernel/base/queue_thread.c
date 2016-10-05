//===================================================================
//
// queue_thread.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "nos_common.h"
#include "queue_thread.h"

void init_tqueue(TQUEUE *q)
{
	q->head = q->tail = NULL;
	q->count = 0;
}

void init_tnode(THREAD *thread)
{
	thread->prev = thread->next = NULL;
}

void push_tnode(TQUEUE *q, THREAD *thread)
{
	if (q->head == NULL)
	{
		q->head = thread;
		q->tail = thread;
		thread->prev = NULL;
		thread->next = NULL;
	}
	else
	{
		thread->prev = q->tail;
		thread->next = NULL;
		q->tail->next = thread;
		q->tail = thread;
	}

	q->count++;
}

int add_tnode(TQUEUE *q, THREAD *p, THREAD *new_thread)
{
	if (p!=NULL) // thread is found
	{
		if (p->prev != NULL)
		{
			p->prev->next = new_thread;
			new_thread->prev = p->prev;
			p->prev = new_thread;
			new_thread->next = p;
		}
		else
		{
			new_thread->prev = NULL;
			p->prev = new_thread;
			new_thread->next = p;
			q->head = new_thread;
		}
	}

	q->count++;

	return 0;
}

int delete_tnode(TQUEUE *q, THREAD *p)
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

THREAD *pop_tnode(TQUEUE *q)
{
	THREAD *p=q->head;

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

int is_tnode_in_queue(THREAD *thread, TQUEUE *q)
{
    THREAD *p;

    for (p=q->head; p!=NULL; p = p->next)
    {
            if (p == thread)
                    return 1;
    }

    return 0;
}

#if 0
void print_tqueue(TQUEUE *q)
{
	THREAD *p;

	for (p=q->head; p!=NULL; p = p->next)
	{
		uart_printf("value = %d\n", p->value);
	}
}

void print_tnode(THREAD *thread)
{
	uart_printf("value = %d\n", thread->value);
}
#endif

