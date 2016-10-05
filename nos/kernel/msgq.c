//===================================================================
//
// msgq.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "msgq.h"

#include "heap.h"
#include "critical_section.h"
#include "thread.h"
#include "sched.h"
#include "error.h"

STATUS msgq_create(UINT32 length, UINT32 *mqid)
{
	STATUS status = E_OK;
	MSGQ *msgq;

	msgq = nos_malloc(sizeof(struct _msgq));

	msgq->length = length;
	msgq->front  = 0;
	msgq->rear   = 0;
	msgq->nitem  = 0;
	msgq->queue = nos_malloc(sizeof(UINT32)*length);
	
	*mqid = (UINT32)msgq;

	service_error_check(S_MSGQ_CREATE, status);

	return status;
}

STATUS msgq_destroy(UINT32 mqid)
{
    STATUS status = E_OK;
	MSGQ *msgq = (MSGQ *)mqid;

    if (msgq == NULL)
	{		
        status = E_MSGQ_INVALID;
	}
    else
    {
        os_sched_lock();
		
        nos_free(msgq);

        os_sched_unlock();
    }

	service_error_check(S_MSGQ_DESTROY, status);

    return status;
}

STATUS msgq_send(UINT32 mqid, UINT32 *data)
{
	STATUS status = E_OK;
	MSGQ *msgq = (MSGQ *)mqid;

    if (msgq == NULL)
	{
        status = E_MSGQ_INVALID;
	}
	else if (msgq->nitem == msgq->length)
	{
		 status = E_MSGQ_FULL;

		 return status;
	}
	else  /* send a message into queue */
	{
		os_sched_lock();

		/* copy the data into kernel memory region */
		*(msgq->queue + msgq->rear) = *data;
		++msgq->nitem;
		msgq->rear = (msgq->rear+1)%msgq->length;
	
		os_sched_unlock();
	}
	
	service_error_check(S_MSGQ_SEND, status);

	return status;
}

STATUS msgq_recv(UINT32 mqid, UINT32 *data)
{
	STATUS status = E_OK;
	MSGQ *msgq = (MSGQ *)mqid;

    if (msgq == NULL)
	{
    	status = E_MSGQ_INVALID;
	}
	else if (msgq->nitem == 0)
	{
		status = E_MSGQ_EMPTY;
		return status;
	}
	else
	{
		os_sched_lock();

		*data = *(msgq->queue + msgq->front);

		msgq->front = (msgq->front+1)%msgq->length;
		--msgq->nitem;

		os_sched_unlock();
	}

	service_error_check(S_MSGQ_RECV, status);

	return status;
} 
