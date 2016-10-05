/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */

/**
 * @file utc_clock.c
 * @brief UTC (Coordinated Universal Time)
 * @author Haeyong Kim (ETRI)
 * @date 2014. 8. 14.
 * @ingroup 
 * @copyright GNU General Public License v3
 */

#include <stdio.h>

#define NDEBUG
#include "kconf.h"
#include "nos_debug.h"

#include "utc_clock.h"
#include "nos_rtc.h"
#include "user_timer.h"

#ifdef SIXLO_M
#include "6lo_netman.h"
#endif
#ifdef RPL_M
#include "rpl.h"
#endif

enum
{
    NOS_COMMON_YEAR_TOTAL_SECONDS = 31536000,
    NOS_LEAP_YEAR_TOTAL_SECONDS = 31622400,
    NOS_DAY_TOTAL_SECONDS = 86400,
    NOS_HOUR_TOTAL_SECONDS = 3600,
    NOS_MIN_TOTAL_SECONDS = 60,
};
const uint8_t days_of_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


uint32_t prev_utc_time;
void utc_second_tick(void* args)
{
    uint32_t time;

    // precise 1sec calibration using UTC   
    time = nos_get_utc_time();
    if (prev_utc_time == time)
        return;
    prev_utc_time = time;
    
    #ifdef SIXLO_M
    sixlo_lifetime_tick(NULL);
    #endif

    #ifdef RPL_M
    rpl_second_tick(NULL);
    #endif
}


int nos_utc_init(void)
{
    NOS_DATE_TIME_T curr_date_time;
    
    /* Set Date and Time */
    nos_get_compile_time(&curr_date_time);
    nos_set_time(&curr_date_time);
    prev_utc_time = nos_get_utc_time();

    nos_user_timer_create_ms(utc_second_tick, NULL, 100, TRUE);
    return EXIT_SUCCESS;
}



int nos_set_utc_time(uint32_t sec)
{
    NOS_DEBUG_START;
    nos_rtc_set_time(sec);
    return EXIT_SUCCESS;
}


uint32_t nos_get_utc_time(void)
{
    NOS_DEBUG_START;
    return nos_rtc_get_time();
}


static bool nos_is_leap_year(uint16_t curr_year)
{
    NOS_DEBUG_START;
    if (curr_year%4 != 0)
        return FALSE;
    else if (curr_year%100 != 0)
        return TRUE;
    else if (curr_year%400 != 0)
        return FALSE;
    else
        return TRUE;    
}

uint32_t nos_set_time(NOS_DATE_TIME_T *date_time_p)
{
    NOS_DEBUG_START;
    int i;
    uint32_t utc_sec;

    if (date_time_p->year < 1970)
        return 0;

    utc_sec = 0;
    //year
    for (i=1970; i<date_time_p->year; i++)
    {
        if (nos_is_leap_year(i))
            utc_sec += NOS_LEAP_YEAR_TOTAL_SECONDS;
        else
            utc_sec += NOS_COMMON_YEAR_TOTAL_SECONDS;
    }
    // month
    for (i=1; i<date_time_p->month; i++)
    {
        utc_sec += days_of_month[i]*NOS_DAY_TOTAL_SECONDS;
        if (i == 2 && nos_is_leap_year(date_time_p->year))
            utc_sec += NOS_DAY_TOTAL_SECONDS;
    }
    //day
    utc_sec += (date_time_p->day-1)*NOS_DAY_TOTAL_SECONDS;
    //hour
    utc_sec += date_time_p->hour*NOS_HOUR_TOTAL_SECONDS;
    //min
    utc_sec += date_time_p->min*NOS_MIN_TOTAL_SECONDS;
    // sec
    utc_sec += date_time_p->sec;

    nos_set_utc_time(utc_sec);
    return utc_sec;
}


int nos_convert_utc_to_time(uint32_t utc_sec, NOS_DATE_TIME_T *date_time_wp)
{
    NOS_DEBUG_START;
    int i;

    date_time_wp->year = 1970;
    date_time_wp->month = 1;
    date_time_wp->day = 1;
    date_time_wp->hour = 0;
    date_time_wp->min = 0;
    date_time_wp->sec = 0;    
    
    //year
    for (i=1970; ; i++)
    {
        if (nos_is_leap_year(i))
        {
            if (utc_sec >= NOS_LEAP_YEAR_TOTAL_SECONDS)
                utc_sec -= NOS_LEAP_YEAR_TOTAL_SECONDS;
            else
                break;
        }
        else
        {
            if (utc_sec >= NOS_COMMON_YEAR_TOTAL_SECONDS)
                utc_sec -= NOS_COMMON_YEAR_TOTAL_SECONDS;
            else
                break;

        }
        date_time_wp->year++;
    }

    
    // month
    for (i=1; ; i++)
    {
        if (utc_sec >= days_of_month[i]*NOS_DAY_TOTAL_SECONDS)
            utc_sec -= days_of_month[i]*NOS_DAY_TOTAL_SECONDS;
        else
            break;
        if (i == 2 && nos_is_leap_year(date_time_wp->year))
        {
            if (utc_sec >= NOS_DAY_TOTAL_SECONDS)
                utc_sec -= NOS_DAY_TOTAL_SECONDS;
            else
                break;
        }
        date_time_wp->month++;
    }

    //day
    for (i=1; ; i++)
    {
        if (utc_sec >= NOS_DAY_TOTAL_SECONDS)
            utc_sec -= NOS_DAY_TOTAL_SECONDS;
        else
            break;
        date_time_wp->day++;
    }

    //hour
    for (i=0; ; i++)
    {
        if (utc_sec >= NOS_HOUR_TOTAL_SECONDS)
            utc_sec -= NOS_HOUR_TOTAL_SECONDS;
        else
            break;
        date_time_wp->hour++;
    }

    //min
    for (i=0; ; i++)
    {
        if (utc_sec >= NOS_MIN_TOTAL_SECONDS)
            utc_sec -= NOS_MIN_TOTAL_SECONDS;
        else
            break;
        date_time_wp->min++;
    }

    // sec
    date_time_wp->sec = utc_sec;
    return EXIT_SUCCESS;
}


int nos_get_time(NOS_DATE_TIME_T *date_time_wp)
{
    NOS_DEBUG_START;
    nos_convert_utc_to_time(nos_get_utc_time(), date_time_wp);
    return EXIT_SUCCESS;
}


int nos_get_compile_time(NOS_DATE_TIME_T *date_time_wp)
{
    NOS_DEBUG_START;
    char month[4];
    volatile int t1,t2,t3;
    sscanf(__DATE__, "%s %d %d", month, &t1, &t2);
    date_time_wp->day = (uint8_t)t1;
    date_time_wp->year = (uint16_t)t2;
    sscanf(__TIME__, "%d:%d:%d", &t1, &t2, &t3);
    date_time_wp->hour = (uint8_t)t1;
    date_time_wp->min = (uint8_t)t2;
    date_time_wp->sec = (uint8_t)t3;
    switch (month[0])
    {
        case 'J': 
            if (month[1] == 'a')
                date_time_wp->month = 1;    //Jan
            else
            {
                if (month[2] == 'n')
                    date_time_wp->month = 6;    //Jun
                else
                    date_time_wp->month = 7;    //Jul
            }
            break;
        case 'F': 
            date_time_wp->month = 2;
            break;            
        case 'M': 
            if (month[2] == 'r')
                date_time_wp->month = 3;    //Mar
            else
                date_time_wp->month = 5;    //May
            break;
        case 'A': 
            if (month[1] == 'p')
                date_time_wp->month = 4;    //Apr
            if (month[1] == 'u')
                date_time_wp->month = 8;    //Aug
            break;
        case 'S': 
            date_time_wp->month = 9;
            break;
        case 'O': 
            date_time_wp->month = 10;
            break;
        case 'N': 
            date_time_wp->month = 11;
            break;
        case 'D': 
            date_time_wp->month = 12;
            break;
    }
    return EXIT_SUCCESS;
}


void nos_print_date(void)
{
    NOS_DEBUG_START;
    NOS_DATE_TIME_T date_time;
    nos_get_time(&date_time);
    printf("%u/%02u/%02u", date_time.year, date_time.month, date_time.day);
}


void nos_print_time(void)
{
    NOS_DEBUG_START;
    NOS_DATE_TIME_T date_time;
    nos_get_time(&date_time);
    printf("%02u:%02u:%02u", date_time.hour, date_time.min, date_time.sec);
}

