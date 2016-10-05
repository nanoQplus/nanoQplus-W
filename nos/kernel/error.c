//===================================================================
//
// error.c (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "error.h"
#include "kernel.h"
#include "uart.h"
#include "critical_section.h"

const char *service_name[] = 
{
      	"THREAD_CREATE",
        "THREAD_TERMINATE",
        "THREAD_EXIT",
        "THREAD_ACTIVATE",
        "THREAD_CHAIN",
        "THREAD_SLEEP",
        "THREAD_WAIT",
        "THREAD_RESUME",
        "THREAD_PRIORITY_CHANGE",
        "ALARM_CREATE",
        "ALARM_DESTROY",
        "ALARM_START",
        "ALARM_STOP",
        "EVENT_CLEAR",
        "EVENT_GET",
        "EVENT_SET",
        "EVENT_WAIT",
        "MSGQ_CREATE",
        "MSGQ_DESTROY",
        "MSGQ_SEND",
        "MSGQ_RECV",
        "MUTEX_CREATE",
        "MUTEX_DESTROY",
        "MUTEX_GET",
        "MUTEX_GET_TRY_COUNT_LIMIT",
        "MUTEX_RELEASE",
        "TASKQ_REGISTER"
};

const char *error_name[] = 
{
        "E_OK",
        "E_OS_PERMISSION",
        "E_ALARM_CREATE",
        "E_ALARM_ID",
        "E_ALARM_INVALID",
        "E_MUTEX_CREATE",
        "E_MUTEX_ID",
        "E_MUTEX_INVALID",
        "E_MUTEX_PRIORITY",
        "E_MUTEX_ACCESS",
        "E_MUTEX_OCCUPIED",
        "E_EVENT_MODE",
        "E_EVENT_STATE",
        "E_THREAD_PRIORITY",
        "E_THREAD_OVER",
        "E_THREAD_INVALID",
        "E_THREAD_ID",
        "E_SYS_MEMORY",
        "E_THREAD_OPTION",
        "E_THREAD_STATE",
        "E_THREADQ_FULL",
        "E_THREADQ_ENQUEUE",
        "E_THREADQ_DEQUEUE",
        "E_THREADQ_FETCH",
        "E_MSGQ_CREATE",
        "E_MSGQ_ID",
        "E_MSGQ_INVALID",
        "E_MSGQ_FULL",
        "E_MSGQ_EMPTY",
        "E_TASKQ_FULL"
};

void service_error_check(UINT32 service_id, STATUS status)
{
	if (status == E_OK) return;
	else
	{
	  //UINT32 msr;
		
		//os_sched_unlock();
		//OS_INTR_LOCK(&msr);
		//uart_printf("\n[ERROR] : Code(%s) by the function (%s) of thread id(%d).\n", error_name[status], service_name[service_id], current_thread->vid);
		//os_sched_unlock_switch();
        	//uart_printf("\n## Service Error Occurred. So, System Abort.\n\n");
        	//SWRESET_REG = 0xC100; // sw reset
       	//OS_INTR_UNLOCK(msr);
		system_abort(0); // sw reset
	}
}
