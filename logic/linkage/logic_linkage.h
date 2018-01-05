/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:  	logic_linkage.h
  Author:     	xiewr
  Version:    	1.0
  Date: 
  Description:  联动头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _SYS_LINKAGE_H_
#define _SYS_LINKAGE_H_

#if 0
#include "typedef.h"
#include "storage_types.h"
#include "logic_net_comm.h"
#endif
#include "logic_include.h"

typedef enum
{
	LVEWORD_TYPE_NONE = 0,							// 无留影留言功能
	LVEWORD_TYPE_TIMEOUT,							// 具有呼叫超时留影留言功能
	LVEWORD_TYPE_AUTO								// 自动进入留言功能
}LVEWORD_STATE_TYPE;

/*************************************************
  Function:			linkage_get_nodisturb_state
  Description: 		获取系统当前免打扰状态
  Input: 			无
  Output:			无
  Return:			TRUE / FALSE
  Others:			
*************************************************/
int32 linkage_get_nodisturb_state(void);

/*************************************************
  Function:			linkage_get_lvd_state
  Description: 		获取系统当前留影留言状态
  Input: 			无
  Output:			无
  Return:			留影留言状态
  Others:			
*************************************************/
LVEWORD_STATE_TYPE linkage_get_lvd_state(void);

/*************************************************
  Function:			linkage_alarm_card
  Description: 		安防刷卡处理
  Input: 			
  	1.DevType		设备类型 0-梯口 1-门前
  Output:			无
  Return:			TRUE/FALSE
  Others:			
*************************************************/
int32 linkage_alarm_card(uint8 DevType);

/*************************************************
  Function:			linkage_play_door_ring
  Description: 		播放门前铃
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:			
*************************************************/
void linkage_play_door_ring(void);

/*************************************************
  Function:			linkage_8area_door
  Description: 		第8防区联动门前铃
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:			
*************************************************/
int32 linkage_8area_door(void);

/*************************************************
  Function:			linkage_alarm_scene
  Description: 		布撤防联动情景
  Input: 			
  	1.state			布撤防操作
  Output:			无
  Return:			无
  Others:			
*************************************************/
void linkage_alarm_scene(DEFEND_STATE state);
#endif

