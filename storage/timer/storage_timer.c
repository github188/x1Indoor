/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	storage_timer.c
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-06
  Description:  时间函数实现

  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#include "storage_include.h"

static ZONE_DATE_TIME g_timer;
#ifdef _LIVEMEDIA_
static unsigned long long  g_tick = 0;
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
uint16 WeekDay(int16 AYear, int16 AMonth, int16 ADay)
{

    uint8 monthday[] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
    uint16 iDays = (AYear - 1) % 7 + (AYear - 1) / 4 - (AYear - 1) / 100 + (AYear - 1) / 400;
    iDays += monthday[AMonth-1] + ADay;

    if (IsLeapYear(AYear) && AMonth > 2)
    {
    	iDays++;
    }
    return iDays % 7;
}

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
uint16 MonthDays(int16 iYear, int16 iMonth)
{
    switch (iMonth)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
            break;
        case 2:
            if (IsLeapYear(iYear))
                return 29;
            else
                return 28;
            break;
    }
    return 0;
}

/*************************************************
  Function:		YearDays
  Description: 	获取当前年份的天数
  Input: 		
  	1.iYear		年份
  Output:		无
  Return:		无
  Others:
*************************************************/
uint16 YearDays(int16 iYear)
{
	uint16 Days = MonthDays(iYear,2)+30*4+31*7;
	return Days;
}

/*************************************************
  Function:    	au_settimeofday
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int au_settimeofday(int year, int month, int day, int hour, int min, int sec)
{
	char cmd[50];

	sprintf(cmd, "date -s \" %04d-%02d-%02d %02d:%02d:%02d\"",year, month, day, hour, min, sec);
	log_printf("au_settimeofday:%s\n", cmd);
	system(cmd);

	return 0;
}

/*************************************************
  Function:		init_rtc_timer
  Description: 	获取RTC时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void init_rtc_timer(void)
{
	g_timer.year  = 2014;
	g_timer.month = 10;		
	g_timer.day   = 1;
	g_timer.hour  = 5;
	g_timer.min   = 1;		
	g_timer.sec   = 1;
	g_timer.week  = WeekDay(g_timer.year,g_timer.month,g_timer.day);

	au_settimeofday(g_timer.year, g_timer.month, g_timer.day, g_timer.hour, g_timer.min, g_timer.sec);
}

/*************************************************
  Function:		set_rtc_timer
  Description: 	设置RTC时间
  Input: 		
  	1.datatime	获取当前时间
  Output:		无
  Return:		ECHO_VALUE
  Others:
*************************************************/
uint32 set_rtc_timer(PZONE_DATE_TIME datatime)
{
	uint32 ret = FALSE;
	
	if (datatime)
	{
		if (datatime->month > 12) datatime->month = 1;
        if (datatime->day > 31) datatime->day = 1;
        if (datatime->hour >= 24) datatime->hour = 0;
        if (datatime->min >= 60) datatime->min = 0;
        if (datatime->sec >= 60) datatime->sec = 0;
		memcpy(&g_timer,datatime,sizeof(ZONE_DATE_TIME));
		g_timer.week  = WeekDay(g_timer.year,g_timer.month,g_timer.day);

		au_settimeofday(datatime->year, datatime->month, datatime->day, datatime->hour, datatime->min, datatime->sec);
		ret = TRUE;
	}

	return ret;
}

/*************************************************
  Function:		get_timer
  Description: 	获取本地当前时间
  Input: 		
  	1.datatime	获取当前时间
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_timer(PZONE_DATE_TIME datatime)
{
	if (datatime)
	{
		struct tm *tm;
		time_t t;		
		time(&t);
		tm = localtime(&t);
		
		datatime->year = tm->tm_year+1900;
		datatime->month = (uint8)tm->tm_mon+1;
		datatime->day = (uint8)tm->tm_mday;
		datatime->hour = (uint8)tm->tm_hour;
		datatime->min = (uint8)tm->tm_min;
		datatime->sec = (uint8)tm->tm_sec;
		datatime->week = (uint8)tm->tm_wday;
	}
}

/*************************************************
  Function:		get_timer
  Description: 	获取格林时间 没有加上时区
  Input: 		
  	1.datatime	获取当前时间
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_gmtime(PZONE_DATE_TIME datatime)
{
	if (datatime)
	{
		struct tm *tm;
		time_t t;		
		time(&t);
		tm = gmtime(&t);
		
		datatime->year = tm->tm_year+1900;
		datatime->month = (uint8)tm->tm_mon+1;
		datatime->day = (uint8)tm->tm_mday;
		datatime->hour = (uint8)tm->tm_hour;
		datatime->min = (uint8)tm->tm_min;
		datatime->sec = (uint8)tm->tm_sec;
	}
}

/*************************************************
  Function:		get_timeofday
  Description: 	获取当前时间,可以精确到毫秒
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void get_timeofday(void)
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday (&tv , &tz);  // 系统函数
	log_printf("tv.tv_sec ,tv.tv_usec : %d:%ld \n", tv.tv_sec, tv.tv_usec);
	//log_printf("tz.tz_minuteswest : %d , tz.tz_dsttime : %d \n", tz.tz_minuteswest, tz.tz_dsttime);
}

/*************************************************
  Function:		set_sys_timer
  Description: 	定时时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void set_sys_timer(void)
{
	int year = g_timer.year;
	int month = g_timer.month;
	int maxday = MonthDays(year, month);
	
	g_timer.day++;
	g_timer.week  = WeekDay(g_timer.year,g_timer.month,g_timer.day);
	if (g_timer.day > maxday)
	{
		g_timer.day = 1;
		g_timer.month++;
		if (g_timer.month > 12)
		{
			g_timer.month = 1;
			g_timer.year++;
		}
		g_timer.week  = WeekDay(g_timer.year,g_timer.month,g_timer.day);
	}
}

/*************************************************
  Function:		add_sys_timer
  Description: 	定时时间 
  				// 暂时不用 直接取实时时间而不要定时器计时
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void add_sys_timer(void)
{
	g_timer.sec++;
	if (g_timer.sec >= 60)
	{
		g_timer.sec = 0;
		g_timer.min++;
		if (g_timer.min >= 60)
		{
			g_timer.min = 0;
			g_timer.hour++;
			if (g_timer.hour >= 24)
			{
				g_timer.hour = 0;
				set_sys_timer();
			}
		}
	}
}

/*************************************************
  Function:		init_timer
  Description: 	初始化时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void init_timer(void)
{
	init_rtc_timer();
}

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
void get_utc_time(PZONE_DATE_TIME DateTime, uint8 TimeZone)
{
	// 只改变时钟
	if (DateTime->hour >= TimeZone)
	{
		DateTime->hour -= TimeZone;
	}
	else
	{
		// 时钟
		DateTime->hour = (DateTime->hour+24) - TimeZone;

		// day > 1
		if (DateTime->day > 1)
		{
			DateTime->day--;
		}
		// day == 1
		else
		{
			// month > 1
			if (DateTime->month > 1)
			{
				DateTime->month--;
				DateTime->day = MonthDays(DateTime->year, DateTime->month);
			}
			// month == 1
			else
			{
				DateTime->year--;
				DateTime->month = 12;
				DateTime->day = MonthDays(DateTime->year, DateTime->month);
			}
		}
	}
}

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
void get_local_time(PZONE_DATE_TIME DateTime, uint8 TimeZone)
{
	// 增加TimeZone个时区
	if (DateTime->hour < (24-TimeZone))
	{
		DateTime->hour += TimeZone;
	}
	else
	{
		// 时钟
		DateTime->hour = (DateTime->hour+TimeZone) - 24;

		// 只增加日
		if (DateTime->day != MonthDays(DateTime->year, DateTime->month))
		{
			DateTime->day++;
		}
		// 增加月或年月
		else
		{
			DateTime->day = 1;
			// 只增加月
			if (DateTime->month != 12)
			{
				DateTime->month++;
			}
			// 增加年月
			else
			{
				DateTime->month = 1;
				DateTime->year++;
			}
		}
	}	
}

#ifdef _LIVEMEDIA_
/*************************************************
  Function:		get_rtc_timer_rtsp
  Description: 	获取RTC时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
unsigned long long get_rtc_timer_rtsp(void)
{
	return (((g_timer.year)*365+g_timer.month*30+g_timer.day)*24+g_timer.hour)*3600+g_timer.min*60+g_timer.sec;;	
}

/*************************************************
  Function:		get_rtc_tick_rtsp
  Description: 	获取RTC时间
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
unsigned long long get_rtc_tick_rtsp(void)
{
	g_tick += 10;
	return g_tick;	
}
#endif

