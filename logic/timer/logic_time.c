/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_time.c
  Author:		chenbh
  Version:  	1.0
  Date: 		2014-12-11
  Description:  时钟处理
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include <stdio.h> 
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>

#include "logic_include.h"

#define MAX_RE_TIMER	(3*60*60)                // 重启最大时间跨度 2点到5点

static struct tm  g_reboot_timer;
static uint32 g_RebootTimerID = 0;              // 定时器ID

/*************************************************
  Function:		CouldBeReboot
  Description: 	判断是否可以重启
  Input:		
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static int32 CouldBeReboot(void)
{
	#if 0
	if (SYS_LCD_CLOSE == sys_get_lcd_state()) 		// 关屛下才可以重启
	{
		return TRUE;
	}
	#else
	if (SYS_MEDIA_NONE == sys_get_media_state())
	{
		return TRUE;
	}	
	#endif

	return FALSE;
}

/*************************************************
  Function:		set_reboot_timer
  Description: 	设置重启时间
  Input: 		
  	ptimer	:   重启时间 	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void set_reboot_timer(struct tm timer)
{
	g_reboot_timer.tm_hour = timer.tm_hour;
	g_reboot_timer.tm_min  = timer.tm_min;
	g_reboot_timer.tm_sec  = timer.tm_sec;
}

/*************************************************
  Function:			deal_reboot_timer_proc
  Description: 		定时重启
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void * deal_reboot_timer_proc(uint32 cmd, void * arg)
{
	// 判断是否可以重启
	if (CouldBeReboot())
	{
		//system("reboot");
		#ifdef _TIMER_REBOOT_NO_LCD_
        timer_reboot_control(TRUE);
		#else
		hw_stop_feet_dog();							// 让单片机来重启
		#endif
	}
	else
	{
		// 如果到点了,但是屏幕正在操作,则延迟20分钟在重启
		struct tm timer;
		timer.tm_sec = g_reboot_timer.tm_sec;
		timer.tm_min = (g_reboot_timer.tm_min + 20)%60;
		timer.tm_hour = g_reboot_timer.tm_hour + (g_reboot_timer.tm_min + 20)/60; 

		cancel_aurine_timer(g_RebootTimerID, NULL); // 取消原来定时器
		g_RebootTimerID = 0;
		
		// 凌晨5点后还没重启的,强制重启
		if (timer.tm_hour > 5 || (timer.tm_hour == 5 && timer.tm_min > 0) || timer.tm_hour < 2) 
		{			
			//init_reboot_timer();
			hw_stop_feet_dog();
		}
		else
		{
			set_reboot_timer(timer);
			g_RebootTimerID = add_aurine_abstimer(timer, deal_reboot_timer_proc, NULL);
		}
	}
	
	return NULL;
}

/*************************************************
  Function:			init_reboot_timer
  Description: 		定时重启初始化
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int32 init_reboot_timer(void)
{
	uint32 timerdiff;
	uint8 mac[6] = {0};
	uint64 MacInt64 = 0;
	uint32 ip = storage_get_netparam_bytype(HOST_IPADDR);
	
	memcpy(mac, storage_get_mac(HOUSE_MAC), 6);
	MacInt64 = mac[5]|((uint64)mac[4]<<8)|((uint64)mac[3]<<16)|
		((uint64)mac[2]<<24)|((uint64)mac[1]<<32)|((uint64)mac[0]<<40);
	srand((unsigned)ip|MacInt64);
	timerdiff = rand()%MAX_RE_TIMER;
	log_printf("init_reboot_timer  timerdiff : %d\n", timerdiff);

	//  重启时间为2 点到5 点随机时间
	memset(&g_reboot_timer, -1, sizeof(struct tm));  // -1 表示定时器中不做比较
	g_reboot_timer.tm_hour = 2 + timerdiff/(60*60);
	g_reboot_timer.tm_min  = (0 + timerdiff/60)%60;
	g_reboot_timer.tm_sec  = 0 + timerdiff%60;
	printf("reboot time : %02d:%02d:%02d\n", g_reboot_timer.tm_hour, g_reboot_timer.tm_min, g_reboot_timer.tm_sec);

	g_RebootTimerID = add_aurine_abstimer(g_reboot_timer, deal_reboot_timer_proc, NULL);
	if (g_RebootTimerID == 0)
	{
		printf("add_aurine_abstimer return false !!!!!!\n");
		return FALSE;
	}

	return TRUE;
}

/*************************************************
  Function:			init_reboot_timer
  Description: 		定时重启初始化
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int32 test_reboot_timer(void)
{
	//  测试调试用
	memset(&g_reboot_timer, -1, sizeof(struct tm));  // -1 表示定时器中不做比较
	g_reboot_timer.tm_hour = 5;
	g_reboot_timer.tm_min  = 5;
	g_reboot_timer.tm_sec  = 0;
	log_printf("reboot time : %02d:%02d:%02d\n", g_reboot_timer.tm_hour, g_reboot_timer.tm_min, g_reboot_timer.tm_sec);

	g_RebootTimerID = add_aurine_abstimer(g_reboot_timer, deal_reboot_timer_proc, NULL);
	if (g_RebootTimerID == 0)
	{
		log_printf("add_aurine_abstimer return false !!!!!!\n");
		return FALSE;
	}

	return TRUE;
}

