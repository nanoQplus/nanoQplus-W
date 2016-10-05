/*
 * Copyright (C) 2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @mainpage NanoQplus3 (Nano OS, NOS): an operating system for IoT devices
 * @version 3.0.0
 * @image html main.png
 
Copyright (C) 2015  Electronics and Telecommunications Research Institute (ETRI) 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 * @image html GPLv3_Logo_small.png
 * @see http://www.gnu.org/licenses/gpl-3.0.html
 * @see http://www.opensw-seed.org/

Contact
    - Embedded SW Research Department, ETRI. \n
    218 Gajeong-ro, Yuseong-gu, Daejeon, 305-700, KOREA
    - Seontae Kim (stkim10@etri.re.kr)
    - Haeyong Kim (haekim@etri.re.kr)
 */
 
/**
 * @file nos_init.c
 * @brief Initialize NanoQplus 
 * @author Haeyong Kim (haekim@etri.re.kr)
 * @date 2012. 2. 22.
 * @copyright GNU General Public License v3
 */

#define NDEBUG
#include "kconf.h"
#include "nos_debug.h"
#include "nos.h"
#include "critical_section.h"

void nos_init(void)
{
    nested_intr_cnt = 0;
    NOS_ENTER_CRITICAL_SECTION();

    NOS_DEBUG_NOTIFY;
    nos_arch_init();

    NOS_DEBUG_NOTIFY;
    nos_platform_init();
    
#ifdef KERNEL_M
    NOS_DEBUG_NOTIFY;
    nos_kernel_init();
#endif


    NOS_EXIT_CRITICAL_SECTION();
}
