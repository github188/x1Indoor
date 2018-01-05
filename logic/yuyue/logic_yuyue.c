/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_yuyue.c
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-06
  Description:  
				预约逻辑实现
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "storage_include.h"
#include "logic_include.h"

#ifdef _YUYUE_MODE_
extern PBE_COMM_LIST JdYuyueList;					// 家电预约列表

static uint8 TimeCount = 0;

/*************************************************
  Function:		yuyue_time
  Description: 	判断预约时间是否到
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
static uint8 yuyue_time(BE_DATETIME BeTime)
{
	uint32 year, BeYear;
	uint8 ret = FALSE;
	ZONE_DATE_TIME g_time;
	uint8 mon, day, week, hour, min, BeMon, BeDay, BeWeek, BeHour, BeMin;
	
	get_timer(&g_time);
	year = g_time.year;
	mon = g_time.month;
	day = g_time.day;
	//week = g_time.week;
	week = WeekDay(year, mon, day);
	hour = g_time.hour;
	min = g_time.min;
	
	BeYear = BeTime.tm_year;
	BeMon = BeTime.tm_mon;
	BeDay = BeTime.tm_mday + 1;
	BeWeek = (BeTime.tm_wday+1) % 7;
	BeHour = BeTime.tm_hour;
	BeMin = BeTime.tm_min;

	//log_printf("year: %d     %d\n", year, BeYear);
	//log_printf("mon: %d     %d\n", mon, BeMon);
	//log_printf("day: %d     %d\n", day, BeDay);
	//log_printf("week: %d     %d\n", week, BeWeek);
	//log_printf("hour: %d     %d\n", hour, BeHour);
	//log_printf("min: %d     %d\n", min, BeMin);
	//log_printf("BeTime.BeType = %d\n", BeTime.BeType);
	switch (BeTime.BeType)
	{
		case BE_DAY:
			if (hour == BeHour && min == BeMin)
			{
				ret = TRUE;		
			}
			break;

		case BE_WEEK:
			if (week == BeWeek && hour == BeHour && min == BeMin)
			{
				ret = TRUE;
			}
			break;

		case BE_MONTH:
			if (day == BeDay && hour == BeHour && min == BeMin)
			{
				ret = TRUE;
			}
			break;

		#if 0
		case BE_YEAR:
			if (mon == BeMon && day == BeDay && hour == BeHour && min == BeMin)
			{
				ret = TRUE;
			}
			else
			{
				ret = FALSE;
			}
			break;

		case BE_MON_FRI:
			if ((week > 0 && week < 6) && hour == BeHour && min == BeMin)
			{
				ret = TRUE;
			}
			else
			{
				ret = FALSE;
			}
			break;

		case BE_MON_SAT:
			if ((week > 0 && week <= 6) && hour == BeHour && min == BeMin)
			{
				ret = TRUE;
			}
			else
			{
				ret = FALSE;
			}
			break;

		case BE_ONE_DAY:
			if (year == BeYear && mon == BeMon && day == BeDay && hour == BeHour && min == BeMin)
			{
				ret = TRUE;
			}
			else
			{
				ret = FALSE;
			}
			break;
		#endif
		
		default:
			ret = FALSE;
			break;
	}
	// 防止多次执行
	if (TRUE == ret && g_time.sec > 30)
	{
		return FALSE;
	}
	log_printf("yuyue_time is same: %d\n", ret);
	
	return ret;
}

/*************************************************
  Function:		yuyue_ontimer
  Description: 	预约定时器
  Input:		无
  Output:		无
  Return:		
  Others:		
*************************************************/
void yuyue_ontimer(void)
{
	uint8 i;
	uint16 type, betype, addr, degree, act, index, istune;
	
	//if (++TimeCount >= 60)
	if (++TimeCount >= 30)                        // 容易出现开两次的现象
	{
		TimeCount = 0;
		if (JdYuyueList)
		{
			for (i = 0; i < JdYuyueList->nCount; i++)
			{
				if (JdYuyueList->be_comm[i].Used)
				{
					//log_printf("yuyue_ontimer: i = %d\n", i);
					if (TRUE == yuyue_time(JdYuyueList->be_comm[i].BeTime))
					{
						log_printf("yuyue_time: index = %d\n", i);
						betype = JdYuyueList->be_comm[i].BeType;	// 预约类型
						type = JdYuyueList->be_comm[i].Type;		// 设备类型
						index = JdYuyueList->be_comm[i].Index;		// 设备索引
						addr = JdYuyueList->be_comm[i].Address;		// 设备地址
						act = JdYuyueList->be_comm[i].Action;		// 执行动作
						degree = JdYuyueList->be_comm[i].Degree;	// 调节级数
                        istune = JdYuyueList->be_comm[i].IsTune;
						log_printf("yuyue betype: %d\n", betype);
						switch (betype)
						{
							case EV_BE_SM:	
							    #ifdef _AURINE_ELEC_NEW_
								if (JD_FACTORY_ACBUS == storage_get_extmode(EXT_MODE_JD_FACTORY) )
								{
								    if (index < 5)
								    {
								        exec_jd_scene_mode(index);
								    }
								    else
								    {
								        jd_aurine_scene_open(JdYuyueList->be_comm[i].Address,JdYuyueList->be_comm[i].Address,JdYuyueList->be_comm[i].Index+1);
								    }
								}
								else
								{
								    exec_jd_scene_mode(index);
								}
								#else
								exec_jd_scene_mode(index);
								#endif
								break;
								
							case EV_BE_JD:
								if (act)
								{
									log_printf("485send  device_on: addr = %d, degree = %d\n", addr, degree);
									#ifdef _AURINE_ELEC_NEW_
									if (JD_FACTORY_ACBUS == storage_get_extmode(EXT_MODE_JD_FACTORY) )
									{
									    if (type == JD_TYPE_DENGGUANG)
									    {
									        if (istune)
									        {
									            jd_aurine_light_open(index, addr, degree*10);
									        }
									        else
									        {
									            jd_aurine_light_open(index, addr, 100);
									        }
									    }
									    else
									    {
									        jd_aurine_yuyue_oper(JD_ON, type-1, index, addr, 100);
									    }
									}
									else
									{
									    device_on(addr, degree);
									}
									#else
									device_on(addr, degree);
									#endif
								}
								else
								{
								    #ifdef _AURINE_ELEC_NEW_
									if (JD_FACTORY_ACBUS == storage_get_extmode(EXT_MODE_JD_FACTORY))
									{
									    jd_aurine_yuyue_oper(JD_OFF,type-1,index,addr,100);
									}
									else
									{
									    device_off(addr);
									}
									#else
									device_off(addr);
									#endif
									log_printf("485send  device_off: addr = %d\n", addr);
								}
								break;

							default:
								break;
						}
					}
				}
			}
		}
	}
}
#endif

