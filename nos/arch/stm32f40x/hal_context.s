//===================================================================
//
// hal_sched.s (@sheart)
//This code was rewritten for cortex-m4 by phj. @phj
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================
#include "kconf.h"

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.global os_switch_context
.global os_load_context
.global os_zero_switch_context


// preserve GPRS. Push R7-R4, R11-R8, respectively.
// R0-R3 and R12 are scratch registers in IAR compiler. Use them to 'PUSH' for R8-R11
// PRIMASK is stored for the first context switching for a new thread

//================= os_ctx_sw_core() is in SVC mode ====================
os_switch_context:
//	LDR	SP, [R0]
	PUSH	{LR}
	PUSH	{LR}
	MRS	R12, xpsr
	PUSH	{R12}	
	MRS	R12, primask
	PUSH	{R12}
	PUSH {R0-R12}
	STR 	SP, [R0]
	MOV 	R0, R1			@ arg1 = arg2	

os_load_context:
	LDR  SP,  [R0]		 @ Restore SP
	POP	{R0-R12}
	POP	{R12}
 	MSR	primask, R12
	POP	{R12}
	MSR	xpsr_nzcvq, R12
	POP	{LR}
	CMP LR, #0xFFFFFFF9
	BEQ LR_EXC
	POP	{PC}
LR_EXC:	
	POP	{LR}
	BX	LR
	
	
os_zero_switch_context: @modify all
	LDR	SP, [R0]
	LDR	R12,	=thread_entry
	STR	R12, [SP, #-4]!		@ Store PC
	MOV	R12, #0xFFFFFFF9		@ LR for EXC_RETURN. Thread mode	
	STR	R12, [SP, #-4]!		@ Store LR
	MOV R12, #0x01000000
	STR	R12, [SP, #-4]!		@ Store xpsr
	MOV R12, #0x00000000
	STR	R12, [SP, #-4]!		@ Store primask
	
	MOV	R12, #0x00000000
	STR	R12, [SP, #-4]!		@ Store R12
	STR	R12, [SP, #-4]!		@ Store R11
	STR	R12, [SP, #-4]!		@ Store R10
	STR	R12, [SP, #-4]!		@ Store R9
	STR	R12, [SP, #-4]!		@ Store R8
	STR	R12, [SP, #-4]!		@ Store R7
	STR	R12, [SP, #-4]!		@ Store R6
	STR	R12, [SP, #-4]!		@ Store R5
	STR	R12, [SP, #-4]!		@ Store R4
	STR	R12, [SP, #-4]!		@ Store R3
	STR	R12, [SP, #-4]!		@ Store R2
	STR	R12, [SP, #-4]!		@ Store R1
	STR	R12, [SP, #-4]!		@ Store R0

	STR SP,  [R0]			@ Save SP(R13)
	MOV R0,	R1			@ arg1 = arg2
	
os_zero_load_context:		@ same as os_load_context
	LDR  SP,  [R0]		 @ Restore SP
	POP	{R0-R12}
	POP	{R12}
 	MSR	primask, R12
	POP	{R12}
	MSR	xpsr_nzcvq, R12
	POP	{LR}
	CMP LR, #0xFFFFFFF9
	BEQ LR_EXC2
	POP	{PC}
LR_EXC2:	
	POP	{LR}
	BX	LR
