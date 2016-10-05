//===================================================================
//
// stack.c (@sheart)
//
//===================================================================
// Copyright 2004-2010, ETRI
//===================================================================

#include <nos.h>

static UINT32 stack_check(STACK_PTR mem_s, STACK_PTR mem_e, UINT32 stack_size)
{
    STACK_PTR p = mem_s;
    UINT32 cnt = 0;
    UINT32 used;

    while (p != mem_e && *p == 0x00000000)
    {
            cnt++;
            p++;
    }
	
    used =  stack_size - cnt*4;

    return used;
}

void stack_printf(UINT32 tid)
{
    UINT32 used;
	THREAD *thread = (THREAD *)tid;

    used = stack_check(thread->stack_start, thread->stack_bottom, thread->stack_size);
    uart_printf("Stack Usage (thread %d) : Total(%d Bytes) Used(%d Bytes, %d%%)\n", (UINT32)thread, thread->stack_size, used, used*100/thread->stack_size);
}

