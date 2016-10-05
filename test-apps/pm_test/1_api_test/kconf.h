/*
 * Automatically generated C config: don't edit
 */
#define AUTOCONF_INCLUDED
#define CONFIG_MCU_NAME "stm32f40x"
#define GCC_TOOLCHAIN 1

/*
 * Platform: STM32F4 Discovery Kit
 */
#define CONFIG_PLATFORM_NAME "stm32f4_discovery"
#define UART_M 1
#define UART1_STDIO 1
#undef UART1_SLIPIO
#undef UART1_DISABLED
#define UART1 1
#define CONFIG_UART1_BAUDRATE 115200
#undef UART2_STDIO
#undef UART2_SLIPIO
#define UART2_DISABLED 1
#undef UART2
#undef LED_M
#undef BUTTON_M
#undef NAND_M
#define PWM_M 1

/*
 * Kernel
 */
#define KERNEL_M 1
#define SCHED_PERIOD_10 1
#undef SCHED_PERIOD_32
#undef SCHED_PERIOD_100
#undef TASKQ_LEN_8
#undef TASKQ_LEN_16
#define TASKQ_LEN_32 1
#undef TASKQ_LEN_64
#undef TASKQ_LEN_128
#define USER_TIMER_M 1
#define THREAD_M 1
#define ENABLE_SCHEDULING 1
#undef THREAD_EXT_M
#undef SEM_M
#undef MSGQ_M

/*
 * Debugging
 */
#undef NOS_DEBUG_M
#undef HEAP_DEBUG

/*
 * Storage
 */
#undef CFD_M
