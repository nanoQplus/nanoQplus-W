//===================================================================
//
// msgq.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef MSGQ_H
#define MSGQ_H
#include "kconf.h"
#include "nos_common.h"

typedef struct _msgq
{
	 UINT32 length; 		// length of queue
	 UINT32 type;
	 UINT32 front, rear, nitem; // front, rear of the queue, the number of items in the queue
	 UINT32 *queue;
} MSGQ;

#define MSGQ_IS_FULL(mq) (mq->nitem == mq->len)
#define MSGQ_IS_EMPTY(mq) (mq->nitem == 0)

UINT32 msgq_create(UINT32 length, UINT32 *mqid);
UINT32 msgq_destroy(UINT32 mqid);
UINT32 msgq_send(UINT32 id, UINT32 *data);
UINT32 msgq_recv(UINT32 id, UINT32 *data);

#endif // ~MSGQ_H
