/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_timer.h
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

#ifndef _AU_TIMER_H
#define _AU_TIMER_H

#include <time.h>

#include "au_types.h"
//#include "au_config.h"

#ifdef  __cplusplus
extern "C"
{
#endif


/*定时器调试信息*/
/*定时器类型*/
#define AU_ABS_TIMER  0x1
#define AU_REL_TIMER  0x2

/*定时器运行状态*/
#define AU_TIMER_RUN  0x1
#define AU_TIMER_STOP 0x2


//执行动作函数定义接口
typedef void *(*FTIMER_FUNC)(uint32, void *);

/*timer type*/
typedef enum _AU_E_TimerAttr
{
    AU_TIMER_LOOP  = 0,       /* the timer start loop */
    AU_TIMER_ONE_SHOOT ,      /* the timer start once and then stop */
    AU_TIMER_AUTO_FREE        /* the timer start once and then free */
}AU_E_TimerAttr;

/*定时器维护信息数据结构*/
typedef struct _MY_TIMER
{
    uint32 ID;		
    uint32 ulTmFlag; 	    /*相对/绝对定时器标志，VTOP_ABS_TIMER为绝对定时器，VTOP_REL_TIMER为相对定时器 */
    struct _MY_TIMER *next;	/*定时器指针*/
    uint32 ulTmAttr;        /*定时器类型，周期：VTOP_TIMER_LOOP，自动释放：VTOP_TIMER_AUTO_FREE，停止但不释放：VTOP_TIMER_ONE_SHOOT */
    uint32 ulTmRunInfo;     /*定时器运行状态，运行：VTOP_TIMER_RUN，停止：VTOP_TIMER_STOP*/
    struct tm  stAbsTime;   /*绝对定时器超时时刻*/
    uint32 ulRelTmLen;		/*相对定时器时长*/
    uint32 uRelCount;       //计时计数
	FTIMER_FUNC func;		//定时执行动作
	void *param;			//执行参数
}MY_TIMER ,*PMY_TIMER;

/*************************************************
  Function:    		set_aurine_timer_count
  Description: 		初始化定时器任务
  Input: 			
	1.count			新数量
  Output:			无
  Return:			0: 失败;>0成功
  Others:
*************************************************/
int init_auTimer(void);


/*************************************************
  Function:    		set_aurine_timer_count
  Description: 		设置定时器数量, 默认值为2000
  Input: 			
	1.count			新数量
  Output:			无
  Return:			设置成功的数量
  Others:
*************************************************/
int set_aurine_timer_count(int count);


/*************************************************
  Function:    		add_aurine_timer
  Description: 		增加一个相对定时器
  Input: 			
	1.speed			相对定时时间,
	2.func			定时器执行动作
	3.param			定时器执行动作参数
  Output:			无
  Return:			返回定时器ID
  Others:
*************************************************/
uint32 add_aurine_realtimer(uint32 speed, FTIMER_FUNC func, void * param);


/*************************************************
  Function:    		add_aurine_timer
  Description: 		增加一个绝对定时器
  Input: 			
	1.abstime		绝对定时时间,
	2.func			定时器执行动作
	3.param			定时器执行动作参数
  Output:			无
  Return:			返回定时器ID
  Others:
*************************************************/
uint32 add_aurine_abstimer(struct tm abstime, FTIMER_FUNC func, void * param);
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
int cancel_aurine_timer(uint32 ID, FTIMER_FUNC func);


/*************************************************
  Function:    		free_aurine_timer
  Description: 		清除所有定时器, 可不执行
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void free_aurine_timer(void);

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
int cancel_aurine_timer(uint32 ID, FTIMER_FUNC func);


#ifdef __cplusplus
};
#endif

#endif /* !_AU_TIMER_H */

