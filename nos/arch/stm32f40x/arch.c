/*
 * Copyright (C) 2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file arch.c
 * @brief STM32F10x MCU basic library
 * @author Haeyong Kim (ETRI)
 * @date 2014. 5. 8.
 * @ingroup noslib_stm32f10x
 * @copyright GNU General Public License v3
 */

#include "arch.h"
#include "platform.h"
#include "nos_rtc.h"
#include "nos_timer.h"


#ifdef UART_M
    #include <stdio.h>
#endif
#ifdef LED_M
    #include "led.h"
#endif


#ifndef SYSCLK
#error "SYSCLK should be defined in 'platform.h'."
#endif




//volatile uint8_t _nested_intr_cnt = 0;

void nos_arch_init(void)
{
    //Alternate Functions (remap, event control and EXTI configuration) registers
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	nested_intr_cnt=0; // added by phj. @160229
    nos_rtc_init();
  
    nos_timer_init();

#if defined (UART_M) && defined (__GNUC__)
    // I/O buffer initialization not to use buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#endif
}

void system_abort(UINT8 ecode)
{
        //uart_printf("\n## System Abort (%d) : Application Terminates\n\n", ecode);
        NVIC_SystemReset(); 	// sw reset @added by phj. @160302.
}


uint16_t ntohs(uint16_t a)
{
    return (((a << 8) & 0xff00) | ((a >> 8) & 0xff));
}

uint32_t ntohl(uint32_t a)
{
    return (((a << 24) & 0xff000000) |
            ((a << 8) & 0xff0000) |
            ((a >> 8) & 0xff00) |
            ((a >> 24) & 0xff));
}



#ifdef __GNUC__
/**
 * Minimal implementations  for newlib
 */
#include <sys/stat.h>

int _close(int fd)
{
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    return 1;
}

int _lseek(int fd, int ptr, int dir)
{
    return 0;
}

//register uint32_t * stack_ptr asm ("sp");
extern int _write (int fd, char *ptr, int len);
caddr_t _sbrk(int incr)
{
    extern MEMADDR_T _end; /* Defined by the linker */
    static MEMENTRY_T *heap_end;
    MEMENTRY_T *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;

    if ((MEMADDR_T)heap_end + incr > KERNEL_STACK_TOP_ADDR)
    {
#ifdef HEAP_DEBUG
        _write(1, "Heap and stack collision\n\r", 25);
        while(1);
#endif
        return (caddr_t)-1;
    }
    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int _stat(const char *filepath, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}
#endif //__GNUC__

