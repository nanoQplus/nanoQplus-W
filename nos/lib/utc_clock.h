/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file utc_clock.h
 * @brief UTC (Coordinated Universal Time)
 * @author Haeyong Kim (ETRI)
 * @date 2014. 8. 14.
 * @ingroup 
 * @copyright GNU General Public License v3
 */


#ifndef _UTC_CLOCK_H_
#define _UTC_CLOCK_H_

#include "nos_common.h"

/*
 * Reference UTC time  (base: 1970.01.01 00:00:00, GMT)
 * Korea: GMT+0900,  +32400 seconds
 * 2014.01.01 00:00:00, GMT - 1388534400 sec (+31536000)
 * 2015.01.01 00:00:00, GMT - 1420070400 sec (+31536000)
 * 2016.01.01 00:00:00, GMT - 1451606400 sec (+31536000)
 * 2017.01.01 00:00:00, GMT - 1483228800 sec (+31622400, 366days)
 */

/**
 * Set the UTC seconds count.
 *
 * @param sec: New UTC seconds.
 */
int nos_set_utc_time(uint32_t sec);

/**
 * Get the UTC seconds count.
 *
 * @return: Current UTC seconds.
 */
uint32_t nos_get_utc_time(void);


typedef struct nos_date_time
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;    
} NOS_DATE_TIME_T;

uint32_t nos_set_time(NOS_DATE_TIME_T *date_time_p);
int nos_get_time(NOS_DATE_TIME_T *date_time_wp);
int nos_get_compile_time(NOS_DATE_TIME_T *date_time_wp);
void nos_print_date(void);
void nos_print_time(void);
int nos_convert_utc_to_time(uint32_t utc_sec, NOS_DATE_TIME_T *date_time_wp);

int nos_utc_init(void);


#endif /* _UTC_CLOCK_H_ */
