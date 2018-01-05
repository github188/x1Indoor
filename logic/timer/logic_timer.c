/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_timer.c
  Author:		chenbh
  Version:  	2.0
  Date: 
  Description:  定时器模块
				
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

#include "logic_timer.h"
#include "logic_include.h"

//将秒转换为年、月、日、时、分、秒,是Local时间
//#define au_localtime(timep) gmtime(timep);   // 和当前时间差个时区 8
#define au_localtime(timep) localtime(timep);  // 当前时间

#define	_THREAD_VERSIOR_		

#define	ALARM_TIME_MAX				60				//最长定时时间
#define	DEFAULT_TIMER_COUNT_MAX		1000			//默认最大定时器数量
#define MSEC_10MS       			47				//定时时间用

static PMY_TIMER g_timers;
static PMY_TIMER g_FreeTimers;

static int g_IsWaitCheck = 0;
static int g_CurTimerCount = 0;							//当前定时任务数
static int g_CurFreeCount = 0;							//当前需要释放的节点数
static int g_MaxTimerCount =DEFAULT_TIMER_COUNT_MAX;	//最大定时器数量控制

static sem_t g_checktimerSem;		
static pthread_t g_timerThread;			//发送线程
static pthread_mutex_t timerLock;

#define TIMER_LOCK()   pthread_mutex_lock(&timerLock)
#define TIMER_UNLOCK() pthread_mutex_unlock(&timerLock)  

static void free_node(PMY_TIMER node);
static int check_realtimer(PMY_TIMER curtimer);
static int check_abstimer(PMY_TIMER curtimer);
static int check_timer(PMY_TIMER curtimer);
static void *check_proc(void);
static uint32 get_id(void);

/*************************************************
  Function:    		free_node
  Description: 		放入释放列表中等待释放
  Input: 			
	1.node			要释放的节点
  Output:			无
  Return:			无
  Others:
*************************************************/
static void free_node(PMY_TIMER node)
{
	if ((g_CurFreeCount+g_CurTimerCount)<g_MaxTimerCount)
	{
		node->next = g_FreeTimers;
		g_FreeTimers = node;
		g_CurFreeCount++;
	}
	else
	{
		free(node);
	}
}

/*************************************************
  Function:    		check_realtimer
  Description: 		查看相对定时器定时时间是否到达
  Input: 
  	1.curtimer      定时器定时时间
  Output:			无
  Return:			无
  Others:
*************************************************/
static int check_realtimer(PMY_TIMER curtimer)
{
	curtimer->uRelCount++;
	int ret =0;
	if ( curtimer->uRelCount*MSEC_10MS >= curtimer->ulRelTmLen)
	{
		ret = 1;
	}
	if ( ret )
	{
		curtimer->uRelCount =0;
	}
	return ret;
}

/*************************************************
  Function:    		check_abstimer
  Description: 		查看绝对定时时间是否到达
  Input: 
  	1.curtimer      定时器定时时间
  Output:			无
  Return:			无
  Others:
*************************************************/
static int check_abstimer(PMY_TIMER curtimer)
{	
	time_t timep;
	time(&timep);
	struct tm *p = au_localtime(&timep);

	NT_BOOL ret  = FALSE;
	if ( (curtimer->stAbsTime.tm_year !=-1 && curtimer->stAbsTime.tm_year == p->tm_year)
		||  curtimer->stAbsTime.tm_year == -1)
	{
		ret =  TRUE;		
	}	
	else
		ret = FALSE;
	
	if ( ret && ( ( curtimer->stAbsTime.tm_yday !=-1 && curtimer->stAbsTime.tm_yday == p->tm_yday )
		||curtimer->stAbsTime.tm_yday ==-1 ) ) 
	{
		ret = TRUE;
	}
	else
		ret = FALSE;
	if ( ret && ( (curtimer->stAbsTime.tm_mon !=-1 && curtimer->stAbsTime.tm_mon == p->tm_mon)
		|| curtimer->stAbsTime.tm_mon ==-1 ) ) 
	{
		ret =  TRUE;
	}
	else
		ret = FALSE;

	if ( ret && ((curtimer->stAbsTime.tm_wday !=-1 && curtimer->stAbsTime.tm_wday == p->tm_wday ) 
		|| curtimer->stAbsTime.tm_wday ==-1 )) 
	{	
		ret = TRUE;
	}
	else
		ret = FALSE;
	
	if ( ret && (( curtimer->stAbsTime.tm_mday !=-1 && curtimer->stAbsTime.tm_mday == p->tm_mday )
		|| curtimer->stAbsTime.tm_mday==-1 ))
	{	
		ret =  TRUE;
	}	
	else
		ret = FALSE;
		
	if ( ret && (( curtimer->stAbsTime.tm_hour !=-1 && curtimer->stAbsTime.tm_hour == p->tm_hour )
		||  curtimer->stAbsTime.tm_hour ==-1 ))
	{
		ret =  TRUE;
	}	
	else 
		ret = FALSE;
		
	if ( ret && ((curtimer->stAbsTime.tm_min !=-1 && curtimer->stAbsTime.tm_min == p->tm_min )
		|| curtimer->stAbsTime.tm_min ==-1 )) 
	{
		ret =  TRUE;
	}
	else
		ret = FALSE;
		
	if ( ret && ( ( curtimer->stAbsTime.tm_sec !=-1 && curtimer->stAbsTime.tm_sec == p->tm_sec )
		|| curtimer->stAbsTime.tm_sec==-1 )) 
	{	
		ret =  TRUE;
	}
	else
		ret = FALSE;

	return ret;
}

/*************************************************
  Function:    		check_timer
  Description: 		查看定时器时间是否到达
  Input: 
  	1.curtimer      定时器定时时间
  Output:			无
  Return:			无
  Others:
*************************************************/
static int check_timer(PMY_TIMER curtimer)
{
	int ret =0;
	switch ( curtimer->ulTmFlag )
	{
		case AU_REL_TIMER:
			ret = check_realtimer(curtimer);
			break;
		case AU_ABS_TIMER:
		{	
			ret = check_abstimer(curtimer);
			/*
			if (ret)            
			{					
				if ( curtimer->uRelCount!=0)   // 想实现什么功能 ??????????
				{	
					ret = FALSE;
					curtimer->uRelCount++;
				}	
				else 
				{
					if ( ++curtimer->uRelCount > (1000/MSEC_100MS))
						curtimer->uRelCount = 0;
				}	
			}
			*/
			break;
		}	
	}
	return ret;
}

#ifdef _THREAD_VERSIOR_
/*************************************************
  Function:    		can_check
  Description: 		设置可访问信号量
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void can_check(void)
{
	//点灯操作将信号灯值原子地加1，表示增加一个可访问的资源
	if (g_IsWaitCheck)
	{	
		sem_post(&g_checktimerSem); 
	}	
}

/*************************************************
  Function:    		check_proc
  Description: 		定时执行函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void *check_proc(void)
{
	while (1)
	{
		PMY_TIMER q = g_timers;
		if (!q)
		{
			g_IsWaitCheck = TRUE ;
			sem_wait(&g_checktimerSem);  			// 阻塞操作，等待条件满足返回  sem_trywait(sem_t * sem)非阻塞
			g_IsWaitCheck = FALSE;	
		}	
		else			
		{		
			while (q)
			{
				if ( q->func && q->ulTmRunInfo == AU_TIMER_RUN  && check_timer(q)) 
				{
					(*(q->func)) (q->ID, q->param);
				}				
				q = q -> next;
			}	
		}	
		usleep(MSEC_10MS*1000);
	}
	pthread_exit(NULL);
}
#else
/*************************************************
  Function:    		check_proc
  Description: 		定时执行含数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void *check_proc(void)
{
	if (!g_timers)
	{	
		g_NextTime = 0; //没有任务了
		return NULL;
	}	

	if ( g_checktimering ) return NULL;
	
	g_checktimering = TRUE;
	
	PMY_TIMER q = g_timers;
	while (q)
	{
		if ( q->func && q->ulTmRunInfo == AU_TIMER_RUN  && check_timer(q)) 
		{
			(*(q->func)) (q->ID, q->param);
		}				
		
		q = q -> next;
	}
	
	g_checktimering = FALSE;
	return NULL;
}
#endif 

/*************************************************
  Function:    		get_id
  Description: 		获得下一个定时器ID
  Input: 			无
  Output:			无
  Return:			定时器ID, 失败返回0
  Others:
*************************************************/
static uint32 get_id(void)
{
	static uint32 id = 0;
	
	while(1)
	{
		if (id==0xFFFFFFFF)
		{
			id = 1;
		}
		else	
		{
			id++;
		}
		PMY_TIMER q = g_timers;
		int repeat = 0;
		//判断是否重复
		while (q)                             
		{
			if (q->ID == id) 
			{	
				repeat = 1;
				break;
			}	
			else
			{
				q = q -> next;
			}
		}
		if (!repeat)
			break;
	}
	return id;
}


/*************************************************
  Function:    		init_auTimer
  Description: 		初始化定时器任务
  Input: 			
  Output:			无
  Return:			
  Others:
*************************************************/
int init_auTimer(void)
{
	#ifdef _THREAD_VERSIOR_
	pthread_mutex_init (&timerLock, NULL); 	
	sem_init(&g_checktimerSem, 0, 0) ;
	pthread_create(&g_timerThread, NULL, (void*)check_proc, NULL);
	#endif 
    return TRUE;
}

/*************************************************
  Function:    		set_aurine_timer_count
  Description: 		设置定时器最大数量
  Input: 			
	1.count			新数量
  Output:			无
  Return:			设置成功的数量
  Others:
*************************************************/
int set_aurine_timer_count(int count)
{
	if (count<0)
		return -1;
	//释放剩余的节点	
	if (count<g_MaxTimerCount && (g_CurTimerCount+g_CurFreeCount)>count)	
	{
		while(g_CurFreeCount > 0)
		{
			PMY_TIMER node = g_FreeTimers;
			g_FreeTimers = g_FreeTimers->next;
			free(node);
			g_CurFreeCount--;
		}
	}
	g_MaxTimerCount = count;
	return count;
}

/*************************************************
  Function:    		add_aurine_realtimer
  Description: 		增加一个定时器
  Input: 			
	1.when			定时时间, time_t值
	2.func			定时器执行动作
	3.param			定时器执行动作参数
  Output:			无
  Return:			返回定时器ID
  Others:
*************************************************/
uint32 add_aurine_realtimer(uint32 speed, FTIMER_FUNC func, void * param)  
{
	if ( !g_timerThread ) 
	{
		return 0;
	}
	//超过最大数量
	if (g_CurTimerCount >= g_MaxTimerCount)
	{
		return 0;
	}
	//时间非法
	if(speed <= 0)
	{
		return 0;
	}
	PMY_TIMER q;

	#ifdef _THREAD_VERSIOR_
		TIMER_LOCK();                             
	#else
		while (g_checktimering)
		{
			usleep(10);
		}	
		g_checktimering = TRUE;	
	#endif 
	
	if (g_FreeTimers)
	{
		q = g_FreeTimers;
		g_FreeTimers = g_FreeTimers->next;
	}
	else
	{
		//增加一个节点
		q = malloc(sizeof(MY_TIMER));
		if (NULL==q)
		{
			#ifdef _THREAD_VERSIOR_
			TIMER_UNLOCK();
			#else		
			g_checktimering = FALSE;
			#endif 	
			log_printf("add_aurine_realtimer  NULL g_FreeTimers \n ");
			return 0;
		}	
	}
	q->ulTmFlag = AU_REL_TIMER;
	q->ID = get_id();
	q->ulRelTmLen = speed;
	q->func = func;
	q->param = param;
	q->ulTmAttr = AU_TIMER_LOOP;
	q->ulTmRunInfo = AU_TIMER_RUN;
	q->uRelCount= 0;
	g_CurTimerCount++;

	can_check();
	//找个位置插入
	//第一位
	q->next = g_timers;
	g_timers = q;
	#ifdef _THREAD_VERSIOR_
		TIMER_UNLOCK();
	#else		
		g_checktimering = FALSE;
	#endif 	
	return q->ID;

}

/*************************************************
  Function:    		add_aurine_abstimer
  Description: 		增加一个定时器
  Input: 			
	1.when			定时时间, time_t值
	2.func			定时器执行动作
	3.param			定时器执行动作参数
  Output:			无
  Return:			返回定时器ID
  Others:
*************************************************/
uint32 add_aurine_abstimer(struct tm abstime, FTIMER_FUNC func, void * param)
{
	if ( !g_timerThread ) 
	{
		return 0;
	}
	
	// 超过最大数量
	if (g_CurTimerCount >= g_MaxTimerCount)
	{
		return 0;
	}
	
	// 时间非法
	PMY_TIMER q;

	#ifdef _THREAD_VERSIOR_
		TIMER_LOCK();
	#else	
		while (g_checktimering)
		{
			usleep(10);
		}
		
		g_checktimering = TRUE;
	#endif 
	
	if (g_FreeTimers)
	{
		q = g_FreeTimers;
		g_FreeTimers = g_FreeTimers->next;
	}
	else
	{
		// 增加一个节点
		q = malloc(sizeof(MY_TIMER));
		if (NULL==q)
		{
		#ifdef _THREAD_VERSIOR_
			TIMER_UNLOCK();
		#else		
			g_checktimering = FALSE;
		#endif 	
		return 0;
		}
	}
	q->ulTmFlag = AU_ABS_TIMER;
	q->ID = get_id();
	q->ulRelTmLen = 0;
	q->stAbsTime = abstime;
	q->func = func;
	q->param = param;
	q->ulTmAttr=AU_TIMER_LOOP;
	q->ulTmRunInfo = AU_TIMER_RUN;
	q->uRelCount= 0;
	g_CurTimerCount++;
	can_check();
	// 找个位置插入
	// 第一位
	q->next = g_timers;
	g_timers = q;
	#ifdef _THREAD_VERSIOR_
		TIMER_UNLOCK();
	#else		
		g_checktimering = FALSE;
	#endif 	
	return q->ID;
}

/*************************************************
  Function:    		cancel_aurine_timer
  Description: 		取消一个定时器
  Input: 			
	1.when			定时器ID
	2.func			定时器设置要执行的动作
  Output:			无
  Return:			是否成功 TRUE/FALSE
  Others:
*************************************************/
int cancel_aurine_timer(uint32 ID, FTIMER_FUNC func)
{	
	#ifdef _THREAD_VERSIOR_
		TIMER_LOCK();
	#else	
		while(g_checktimering)
		{
			usleep(10);
		}		
		g_checktimering = TRUE;
	#endif 	

	PMY_TIMER t, q;
	t = (PMY_TIMER)0;
	
	for (q = g_timers; q; q = q->next) 
	{
		if ( q->ID == ID )//&& q->func == func) 
		{
			if (t)
				t -> next = q -> next;
			else
				g_timers = q -> next;
			break;
		}
		t = q;
	}
	int ret = FALSE;
	//找到
	if (q) 
	{
		g_CurTimerCount--;
		free_node(q);
		ret = TRUE; 
	}
		
	#ifdef _THREAD_VERSIOR_
		TIMER_UNLOCK();
	#else
		g_checktimering = FALSE;
	#endif		
	return ret; 
}

/*************************************************
  Function:    		free_aurine_timer
  Description: 		清除所有定时器, 可不执行
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void free_aurine_timer(void)
{
	PMY_TIMER t;
	
	while (g_timers)
	{
		t = g_timers; 
		g_timers = g_timers->next;
		free(t);
	}
	g_CurTimerCount = 0;
	while (g_FreeTimers)
	{
		t = g_FreeTimers; 
		g_FreeTimers = g_FreeTimers->next;
		free(t);
	}
	g_CurFreeCount = 0;
}

