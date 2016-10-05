//========================================================================
// File		: uart_put.c 
// Author	: Haeyong Kim (haekim@etri.re.kr), sheart@etri.re.kr
// Date		: 2006.06.01
// Description : Print charater on the terminal (STDOUT).
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================

#include "nos.h"
//#include "s5pc100.h"

// ASCII Characters
#define _BS (0x08)    // Back Space (ASCII)
#define _CR (0x0D)    // Carrage Return (ASCII)

void app_init(void)
{
	//nos_uart_putc(0, 'A');


	UINT8 i;
	UINT8 tilde = '~';
	const char *name = "Nano OS";
	const char *kor_name = "³ª³ë OS";

	//os_init();
	//os_uart_init();

	// print string
	uart_printf("\n*** %s (Korean: %s) ***\n", name, kor_name);

	// print 16-bit integer (-2147483647~2147483647, 2's complement)
	uart_printf("\nINT32 : %d %c %i",0x80000000, tilde, 0x7fffffff);	// %d = %i
	uart_printf("\nINT32 : %i ",0x7fffffff);	// %d = %i
	uart_puts("\nINT32 : ");	//print string
	//uart_puti(0x80000000);
	uart_putc(tilde);	// print character
	//uart_puti(0x7fffffff);

	// print 16-bit unsigned integer (0~65535)
	uart_printf("\nUINT32 : %u ~ %u", 0x00000000, 0x7fffffff);
	uart_puts("\nUINT32 : ");
	uart_putu(0);
	uart_putc('~');
	uart_putu(4294967295);

	// print 16-bit unsigned hexadecimal (0x0000~ 0xffff)
	uart_printf("\nHex32 : %x ~ %x", 0x00000000, 0xffffffff);

	// print 16-bit unsigned octal
	uart_printf("\nOctal32 : %o ~ %o\n\n", 0x00000000, 0xffffffff);

	while (1)
	{
		// print character
		for (i=0; i<5; i++)
		{
			uart_putc('-');
			delay_ms(200); // wait for 200 ms
		}
		for (i=0; i<5; i++)
		{
			// Backspace key handling
			uart_putc(_BS);	// Backspace character '_BS' = 0x08
			uart_putc(' ');	
			uart_putc(_BS);
			delay_ms(200); // wait for 200 ms
		}
	}

	//return 0;
}
