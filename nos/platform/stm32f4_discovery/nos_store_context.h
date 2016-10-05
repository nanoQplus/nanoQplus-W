//===================================================================
//
// nos_store_context.h (@phj)
//
//===================================================================
// Copyright 2016-2025, ETRI
//===================================================================

#include "stm32f4xx.h"
#include <stdio.h>


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NOS_STORE_CONTEXT_H
#define __NOS_STORE_CONTEXT_H


/* Define Functions. */

#define STORE_CONTEXT() do{\
	register  uint32_t R12 __asm("r12");\
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);\
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);\
	PWR_BackupAccessCmd(ENABLE);\
	PWR_BackupRegulatorCmd(ENABLE);\
	asm("ldr	r12, =0x40024004\n\t"\
		"add r12, r12, #32\n\t"\
		"stmia	r12!, {r0-r11}\n\t"\
		"add r8, r13, #0\n\t"\
		"add r12, r12, #4\n\t"\
		"stmia	r12!, {r8}\n\t"\
		"mrs r0, psr\n\t"\
		"mrs r1, msp\n\t"\
		"mrs r2, psp\n\t"\
		"mrs r3, control\n\t"\
		"mrs r4, faultmask\n\t"\
		"mrs r5, basepri\n\t"\
		"mrs r6, primask\n\t"\
		"ldr	r12, =0x40024004\n\t"\
		"stmia	r12!, {r0-r6}\n\t"\
		"add r0, r14, #0\n\t"\
		"stmia	r12!, {r0}\n\t"\
		"add r0, r15, #12\n\t"\
		"ldr	r12, =0x4002405c\n\t"\
		"stmia r12!, {r0}\n\t"\
		"ldr r0, =0x40024054\n\t"\
		"str %0, [r0]\n\t"::"r" (R12):);\
	Store_LocalStack_Value();\
	Store_GlobalStack_Value();\
	Store_SCB_Value();\
	Store_Systick_Value();\
	Store_RTC_Value();\
	Store_RCC_Value();\
	Store_EXTI_Value();\
	Store_NVIC_Value();\
	Store_GPIOD_Value();\
	Store_GPIOA_Value();\
	Store_SYSCFG_Value();\
	Select_Reset_Mode(1);\
	Store_CR_DBP_BB();\
	Store_CSR_LSION_BB();\
	Store_BDCR_RTCEN_BB();\
}while(0)
	/*
	RCC_LSICmd(ENABLE);\
	RCC_RTCCLKCmd(ENABLE);\
	Store_CSR_LSION_BB();\
	Store_BDCR_RTCEN_BB();\
	*/



/* Exported functions ------------------------------------------------------- */	
void Select_Reset_Mode(uint8_t mode);
uint32_t Find_EPDS(void);
void Store_DS_IN_BKSRAM(uint32_t ep_sram);
int8_t write_to_backup_sram(uint32_t *data, uint16_t bytes, uint16_t offset);
int8_t read_from_backup_sram(uint8_t *data, uint16_t bytes, uint16_t offset);
void clear_backup_sram();
void Store_LocalStack_Value(void);
void Store_GlobalStack_Value(void);
void Store_SCB_Value(void);
void Store_Systick_Value(void);
void Store_RTC_Value(void);
void Store_RCC_Value(void);
void Store_EXTI_Value(void);
void Store_NVIC_Value(void);
void Store_GPIOD_Value(void);
void Store_GPIOA_Value(void);
void Store_SYSCFG_Value(void);
void Store_CR_DBP_BB(void);
void Store_CSR_LSION_BB(void);
void Store_BDCR_RTCEN_BB(void);
#endif
