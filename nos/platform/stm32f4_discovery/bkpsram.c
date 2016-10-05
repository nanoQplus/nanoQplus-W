#include "lowpower.h"
#include "pwmgmt.h"

#include "bkpsram.h"

int __bkpsram_check_area(UINT32 addr, UINT32 size) {

	if (
		((UINT32)addr < BKPSRAM_START_ADDR) ||
		((UINT32)addr > BKPSRAM_START_ADDR + BKPSRAM_SIZE_LIMIT)
	   ) {
		return BKPSRAM_ERROR_ADDR;
	} // end if

	if (
		(size > BKPSRAM_SIZE_LIMIT) ||
		((((UINT32)addr) + size) > BKPSRAM_START_ADDR + BKPSRAM_SIZE_LIMIT)
	   ) {
		return BKPSRAM_ERROR_ADDR;
	} // end if

	return BKPSRAM_SUCCESS;
		
} // end func

void bkpsram_init() {

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		// set RCC->APB1ENR.pwren
	PWR_BackupAccessCmd(ENABLE);					// set PWR->CR.dbp = 1;
	PWR_BackupRegulatorCmd(ENABLE);					// set PWR->CSR.bre = 1;

} // end func

void bkpsram_disable() {

	PWR_BackupRegulatorCmd(DISABLE);				// set PWR->CSR.bre = 0;
	PWR_BackupAccessCmd(DISABLE);					// set PWR->CR.dbp = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);		// set RCC->APB1ENR.pwrdisable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, DISABLE);

} // end func

int bkpsram_write(UINT32 src, UINT32 dest, UINT32 size) {
	if (__bkpsram_check_area(dest, size) == BKPSRAM_ERROR_ADDR) {
		return BKPSRAM_ERROR_ADDR;
	} // end if

	memcpy((void *)dest, (void *)src, size);

	return BKPSRAM_SUCCESS;

} // end func

int bkpsram_read(UINT32 src, UINT32 dest, UINT32 size) {
	if (__bkpsram_check_area(src, size) == BKPSRAM_ERROR_ADDR) {
		return BKPSRAM_ERROR_ADDR;
	} // end if

	memcpy((void *)dest, (void *)src, size);

	return BKPSRAM_SUCCESS;

} // end func



