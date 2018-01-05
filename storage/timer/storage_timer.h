/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	storage_timer.c
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-06
  Description:  时间函数实现头文件

  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/

#ifndef __STORAGE_TIMER_H__
#define __STORAGE_TIMER_H__

#include "storage_include.h"

#define RTC_USE				0
#define IsLeapYear(AYear)   (((AYear) % 4 == 0) && (((AYear) % 100 != 0) || ((AYear) % 400 == 0)))

#if 0
typedef struct
{
	uint32 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;	
	uint8 week;
}ZONE_DATE_TIME, * PZONE_DATE_TIME;					// 时间结构体
#endif

/*************************************************
  Function:		WeekDay
  Description: 	特定年月日对应的星期数
  Input: 		
  	1.AYear		年份
  	2.AMonth	月
  	3.ADay		日
  Output:		无
  Return:		无
  Others:
*************************************************/
uint16 WeekDay(int16 AYear, int16 AMonth, int16 ADay);

/*************************************************
  Function:		MonthDays
  Description: 	获取当前年份月份的天数
  Input: 		
  	1.iYear		年份
  	2.iMonth	月
  Output:		无
  Return:		无
  Others:
*************************************************/
uint16 MonthDays(int16 iYear, int16 iMonth);

/*************************************************
  Function:		YearDays
  Description: 	获取当前年份的天数
  Input: 		
  	1.iYear		年份
  Output:		无
  Return:		无
  Others:
*************************************************/
uint16 YearDays(int16 iYear);

/*************************************************
  Function:		init_rtc_timer
  Description: 	获取RTC时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void init_rtc_timer(void);

/*************************************************
  Function:		set_rtc_timer
  Description: 	设置RTC时间
  Input: 		
  	1.datatime	获取当前时间
  Output:		无
  Return:		ECHO_VALUE
  Others:
*************************************************/
uint32 set_rtc_timer(PZONE_DATE_TIME datatime);

/*************************************************
  Function:		get_timer
  Description: 	获取当前时间
  Input: 		
  	1.datatime	获取当前时间
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_timer(PZONE_DATE_TIME datatime);

/*************************************************
  Function:		get_timer
  Description: 	获取格林时间 没有加上时区
  Input: 		
  	1.datatime	获取当前时间
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_gmtime(PZONE_DATE_TIME datatime);

/*************************************************
  Function:		get_timeofday
  Description: 	获取当前时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_timeofday(void);

/*************************************************
  Function:		add_sys_timer
  Description: 	定时时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void add_sys_timer(void);

/*************************************************
  Function:		init_timer
  Description: 	初始化时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void init_timer(void);

/*************************************************
  Function:			get_utc_time
  Description: 		获取utc时间
   1.DateTime		本地时间
   2.TimeZone		时区 (0-12)
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void get_utc_time(PZONE_DATE_TIME DateTime, uint8 TimeZone);

/*************************************************
  Function:			get_local_time
  Description: 		获取本地时间
   1.DateTime		UTC时间
   2.TimeZone		时区 (0-12)
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void get_local_time(PZONE_DATE_TIME DateTime, uint8 TimeZone);
#endif

