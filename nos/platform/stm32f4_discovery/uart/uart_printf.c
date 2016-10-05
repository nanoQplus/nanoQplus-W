//===================================================================
//                                      
// uart_printf.c (@sheart)      
//                              
//===================================================================
// Copyright 2016-2025, ETRI            
//===================================================================
#include "uart.h"
#include "strlib.h"

#include "platform.h"
#include "arch.h"
#include "critical_section.h"
#include <stdarg.h>

////////// os_uart_printf(..) currently supported format 
//                              
// d or i : signed 16bit decimal integer
// u : unsigned 16bit decimal integer
// o : unsigned 16bit octal     
// x : unsigned 16bit hexadecimal integer
// c : character        
// s : string                   
// % : '%'                      
////////////////////////////////////////////////////

void uart_printf(const char *msg, ...)
{                  
	char buf[20];
		
//        os_sched_unlock();
		
        va_list ap;
        const char *p;
	UINT8 niter = 6; // default %.6f
        
        va_start(ap, msg);
        for (p=msg; *p; ++p)
        {
                switch(*p)
                {
                        case '%' :
                                p++;
                                if (*p == 'd' || *p == 'i')     // signed decimal integer
                                {
                                        //os_uart_puti(STDOUT, va_arg(ap, UINT16));
                                        itoa(va_arg(ap, int), buf, 10); // convert argument into string with a radix of 10
                                        nos_uart_puts(STDOUT, buf);
                                }
                                else if (*p == 'u')
                                {
                                        //os_uart_putu(STDOUT, va_arg(ap, UINT16));
                                        utoa(va_arg(ap, int), buf, 10); // convert argument into string with a radix of 10
                                        nos_uart_puts(STDOUT, buf);
                                }
                                else if (*p == 'x')     // unsigned hexadecimal integer
                                {
                                        utoa(va_arg(ap, int), buf, 16); // convert argument into string with a radix of 16
                                        nos_uart_puts(STDOUT, buf);
                                }
                                else if (*p == 'o')             // signed octal
                                {
                                        utoa(va_arg(ap, int), buf, 8); // convert argument into string with a radix of 8
                                        nos_uart_puts(STDOUT, buf);
                                }
				else if (*p == 'f')
				{
					//ftoa((float) 123.24676, buf);
					ftoa(va_arg(ap, double), buf, 4);
					//ftoa(32.1234, buf, 4);
					nos_uart_puts(STDOUT, buf);
				}
				else if (*p == '.')
				{
					p++;
					if (*p >= 48 && *p <= 57)
					{
						niter = *p - 48;
					}
					p++;
					if (*p == 'f')
					{
						ftoa(va_arg(ap, double), buf, niter);
						//ftoa(32.1234, buf, niter);
						nos_uart_puts(STDOUT, buf);
					}
				}
                                else if (*p == 'c')     // character
                                {
                                        nos_uart_putc(STDOUT, va_arg(ap, int));
                                }
                                else if (*p == 's')     // string of characters
                                {
                                        nos_uart_puts(STDOUT, va_arg(ap, const char *));
                                }
                                else if (*p =='%')      // write '%' to STDOUT
                                {
                                        nos_uart_putc(STDOUT, '%');
                                }
                                else
                                {
                                        nos_uart_putc(STDOUT, *p);
                                }
                                break;

                        case '\n' :
                                nos_uart_putc(STDOUT, *p);
                                nos_uart_putc(STDOUT, '\r');
                                break;

                        default :
                                nos_uart_putc(STDOUT, *p);
                                break;
                        }
        }
        va_end(ap);
        
//	os_sched_unlock_switch();
        return ;
}
