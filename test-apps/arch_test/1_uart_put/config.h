/*
 * config.h - system configuration header
 * 
 * Created on: Nov 20, 2012
 *      by WBSIDE Kernel Configurator
 * Copyright(c) 2012 MDS Technology Co.,Ltd.
 * All rights reserved.
 * 
 * DO NOT EDIT THIS CONFIGURATION HEADER FILE!
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_



/* Included Components */

#define CONFIG_BSP_CONSOLE_INIT
#define CONFIG_BSP_EARLY_INIT
#define CONFIG_CPLUS_RUNTIME
#define CONFIG_ELF_MODULE
#define CONFIG_FAULTLIB
#define CONFIG_IOSYS
#define CONFIG_KERNEL_MODULE
#define CONFIG_MESSAGE_QUEUE
#define CONFIG_MMU
#define CONFIG_MSGPORTLIB
#define CONFIG_PCI_SHOW
#define CONFIG_POSIX_LIBRARY
#define CONFIG_POSIX_PTHREAD
#define CONFIG_POSIX_SIGNAL
#define CONFIG_PROCESS
#define CONFIG_PROCESS_SHOW
#define CONFIG_SEM_SHOW
#define CONFIG_SHELL_CORE
#define CONFIG_SHELL_LOGO
#define CONFIG_SYMBOL_SHOW
#define CONFIG_SYMBOL_TABLE
#define CONFIG_SYSCALL_FAULTLIB
#define CONFIG_SYSCALL_IOSLIB
#define CONFIG_SYSCALL_MMANLIB
#define CONFIG_SYSCALL_MQLIB
#define CONFIG_SYSCALL_MSGPORTLIB
#define CONFIG_SYSCALL_SEMLIB
#define CONFIG_SYSCALL_THREADLIB
#define CONFIG_SYSLOG
#define CONFIG_SYS_CACHE_SUPPORT
#define CONFIG_SYS_EXC_INIT
#define CONFIG_SYS_LWIPNET
#define CONFIG_TERMIO
#define CONFIG_THREAD_HOOK
#define CONFIG_THREAD_SHOW
#define CONFIG_TIMER
#define CONFIG_VFS_CORE
#define CONFIG_VMS
#define CONFIG_VMS_ELF_LOADER
#define CONFIG_VXWORKSPORT_LIBRARY
#define SUBSYS_FS_EXT2FS
#define SUBSYS_FS_MSDOSFS



/* Included Parameters */

#define CONFIG_BSP_CONSOLE_BAUD_RATE              115200
#define CONFIG_DCACHE_MODE                        0x01
#define CONFIG_ICACHE_MODE                        0x01
#define CONFIG_IOSYS_MAX_DEVICE                   100
#define CONFIG_IOSYS_MAX_FILE                     100
#define CONFIG_IO_TTY_CONSOLE                     "/dev/tty0"
#define CONFIG_IO_TTY_NUM                         1
#define CONFIG_SHELL_PRIORITY                     245
#define CONFIG_SHELL_PROMPT                       "-> "
#define CONFIG_SHELL_STACK_SIZE                   32768 * 8
#define CONFIG_SYSLOG_MAX_LOGMSG                  32
#define LWIPNET_GATEWAY                           "192.168.10.1"
#define LWIPNET_IP                                "192.168.10.123"
#define LWIPNET_MAC                               "00:55:53:55:55:00"
#define LWIPNET_SUBNET_MASK                       "255.255.255.0"



// dkwu
#undef CONFIG_CPLUS_RUNTIME
#undef CONFIG_PCI_SHOW
#undef CONFIG_POSIX_LIBRARY
#undef CONFIG_POSIX_PTHREAD
#undef CONFIG_POSIX_SIGNAL
#undef CONFIG_PROCESS
#undef CONFIG_PROCESS_SHOW
#undef CONFIG_SYS_LWIPNET
#undef CONFIG_VXWORKSPORT_LIBRARY
#undef SUBSYS_FS_EXT2FS
#undef SUBSYS_FS_MSDOSFS
#undef CONFIG_THREAD_HOOK
//#undef CONFIG_FAULTLIB
//#undef CONFIG_SYSCALL_FAULTLIB
#undef CONFIG_VMS_ELF_LOADER
//#undef CONFIG_SYSLOG_MAX_LOGMSG
//#undef CONFIG_SYSLOG
//#undef CONFIG_MSGPORTLIB
#undef CONFIG_SYSCALL_IOSLIB
#undef CONFIG_SYSCALL_MSGPORTLIB
#undef CONFIG_VMS
#undef CONFIG_SYSCALL_THREADLIB
#undef CONFIG_SYSCALL_SEMLIB
#undef CONFIG_SYSCALL_MQLIB
#undef CONFIG_SYSCALL_MMANLIB
#undef CONFIG_MESSAGE_QUEUE
//#undef CONFIG_TIMER
#undef CONFIG_MMU
#undef CONFIG_SYS_CACHE_SUPPORT


#endif /* CONFIG_H_ */

