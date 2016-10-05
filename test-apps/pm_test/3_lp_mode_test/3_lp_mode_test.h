#ifndef __3_LP_MODE_TEST_H__
#define __3_LP_MODE_TEST_H__

#define MYCHIP_ID	0
#define MYCHIP_PART	0

#define	TASK_NUM		3

typedef struct _mytask {
	UINT32		tid;	// thread id
	UINT32		alid;	// alarm id
	UINT32		busywait;	// busywait time
	UINT32		period; // in msec
	UINT32		offset; // in msec
	UINT32		flag;
} mytask; // end struct



mytask my_task[TASK_NUM] = {
	{
		0,	// thread id
		0,	// alaram id
		20,	// busywait time (in msec)
		3700,	// period (in msec)
		100,	// offset (in msec)
		0,	// flag (0 = busywait, 1 = idlewait)
	},

#if TASK_NUM > 1
	{0, 0, 50, 4000, 400, 1},
#endif

#if TASK_NUM > 2
	{0, 0, 100, 4600, 900, 0}
#endif
};

typedef struct _mystruct {
	UINT32	bootcount;
} mystruct;

mystruct my_struct = {
	0
};


#endif // __3_LP_MODE_TEST__
