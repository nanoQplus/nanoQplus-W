//===================================================================
//
// kernel.c (@sheart, @haekim)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include <nos.h>

int main(void)
{

	nos_init();

	/* 
	   os_start() creates idle thread and super thread.
	   it loads super thread at last.
     */
     
	os_start();

	return 0;
}

void nos_kernel_init()
{
   	os_sched_init();
	
	os_taskq_init();
}
