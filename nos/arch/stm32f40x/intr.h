//===================================================================
//
// intr.h (@sheart)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#ifndef INTR_H
#define INTR_H
#include "kconf.h"
#include "nos_common.h"
#include "hardware.h"

/*
#define ENABLE_IRQ() \
	do{ \
		asm volatile (\
			"MRS r7,CPSR\n\t"\
			"BIC r7,r7,#0x80\n\t"\
			"MSR CPSR_c,r7\n\t"\
		);\
	}while(0)

#define DISABLE_IRQ() \
	do{\
		asm volatile (\
			"MRS r7,CPSR\n\t"\
			"ORR r7,r7,#0x80\n\t"\
			"MSR CPSR_c, r7\n\t"\
		);\
	}while(0)

#define ENABLE_FIQ() \
	do{\
		asm volatile (\
			"MRS r7,CPSR\n\t"\
			"BIC r7,r7,#0x40\n\t"\
			"MSR CPSR_c,r7\n\t"\
		);\
	}while(0)

#define DISABLE_FIQ() \
	do{\
		asm volatile (\
			"MRS r7,CPSR\n\t"\
			"ORR r7,r7,#0x40\n\t"\
			"MSR CPSR_c, r7\n\t"\
		);\
	}while(0)

#define ENABLE_FIRQ() \
	do{\
		asm volatile (\
			"MRS r7,CPSR\n\t"\
			"BIC r7,r7,#0xC0\n\t"\
			"MSR CPSR_c,r7\n\t"\
		);\
	}while(0)

#define DIABLE_FIRQ() \
	do{\
		asm volatile (\
			"MRS r7,CPSR\n\t"\
			"ORR r7,r7,#0xC0\n\t"\
			"MSR CPSR_c, r7\n\t"\
		);\
	}while(0)


#define OS_DISABLE_GLOBAL_INTERRUPT()					\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_disable\n"	\
	"	orr	%0, %0, #128\n"					\
	"	msr	cpsr, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory", "cc");					\
	})
	

#define OS_ENABLE_GLOBAL_INTERRUPT()					\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_enable\n"	\
	"	bic	%0, %0, #128\n"					\
	"	msr	cpsr, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory", "cc");					\
	})

#define OS_INTR_LOCK(msrval) \
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_disable\n"	\
	"str	%0, [%1]\n" 						\
	"	orr	%0, %0, #128\n"					\
	"	msr	cpsr, %0"					\
	: 					\
	: "r" (temp), "r" (msrval)							\
	: "memory", "cc");					\
	})
	

#define OS_INTR_UNLOCK(msrval) \
	({							\
	__asm__ __volatile__(					\
	"	msr	cpsr, %0"					\
	:									\
	: "r" (msrval)							\
	: "memory", "cc");					\
	})
	*/
UINT32 nested_intr_cnt;		// the number of cli() or sli() that is called

// Functions and Variables
typedef struct _intr_status
{
	UINT32 cnt;
} INTR_STATUS;


#endif	// ~INTR_H
