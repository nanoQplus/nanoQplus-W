#include "nos.h"
#include "kconf.h"

extern UINT32 __cpu_context_backup_point;
extern volatile void *pjmp;

__attribute__ ((always_inline)) static __INLINE void __restore_cpu_context(UINT32 p) {
	__ASM volatile (
		"mov	r12, %[backup]		\n\t"
		"add	r12, r12, #48		\n\t"
		"ldmia	r12!, {r0-r6}		\n\t"
		"msr	apsr_nzcvq, r0		\n\t"
		"msr	msp, r1			\n\t"
		"msr	psp, r2			\n\t"
		"msr	control, r3		\n\t"
		"msr	faultmask, r4		\n\t"
		"msr	basepri, r5		\n\t"
		"msr	primask, r6		\n\t"
		"ldr	r3, [r7, #12]		\n\t"
		"mov	r12, %[backup]		\n\t"
		"ldmia	r12!, {r0-r6}		\n\t"
		"ldr	r3, [r7, #12]		\n\t"
		"mov	r12, %[backup]		\n\t"
		"add	r12, r12, #76		\n\t"
		"ldr	r13, [r12]		\n\t"
		"add	r12, r12, #4		\n\t"
		"ldr	r14, [r12]		\n\t"
		"mov	r0, %[backup]		\n\t"
		"add	r0, r0, #28		\n\t"
		"ldr	r7, [r0]		\n\t"
		:
		:[backup]"r"(p)
		:"r4", "r5", "r6", "r8", "r9", "r10", "r11"
	);
} // end func


__attribute__ ((always_inline)) static __INLINE void __use_private_stack() {
	__ASM volatile (
		"ldr	r3, =0x10010000	\n\t"
		"mov	sp, r3			\n\t"
		);
} // end func
