#ifndef __BKPSRAM_H__
#define __BKPSRAM_H__

#define BKPSRAM_START_ADDR	0x40024000
#define BKPSRAM_SIZE_LIMIT	4096

#define BKPSRAM_SUCCESS		0
#define BKPSRAM_ERROR_ADDR	1

// enable & init bkpsram, setup base address
void bkpsram_init();

// disable bkpsram
void bkpsram_disable();

// write bkpsram
int bkpsram_write(UINT32 src, UINT32 dest, UINT32 size);

// read bkpsram
int bkpsram_read(UINT32 src, UINT32 dest, UINT32 size);

#endif // __BKPSRAM_H__
