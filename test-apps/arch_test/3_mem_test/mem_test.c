//========================================================================
// File		: mem_test.c 
// Author	: Haeyong Kim (haekim@etri.re.kr), sheart@etri.re.kr
// Date		: 2006.06.01
// Description : Memory allocation test
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================

#include "nos.h"

#define BLKSIZE 10000 // 40KB
//#define BLKSIZE 1000000 // 4MB

void app_init(void)
{
	int *ptr;	
	int i;

	ptr = nos_malloc(BLKSIZE*sizeof(int)); 

	for (i=0; i<BLKSIZE; i++)
		*(ptr+i) = i;

	//for (i=0; i<BLKSIZE; i++)
		//uart_printf("i=%d\n", *(ptr+i));

	//return 0;
}
