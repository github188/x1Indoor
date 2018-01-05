/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_alarm.h
  Author:   	罗发禄
  Version:   	2.0
  Date: 		2010-4-29
  Description:  安防逻辑处理头文件
  
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#ifndef __LOGIC_ALARM_H__
#define __LOGIC_ALARM_H__

#include "logic_alarm_net.h"

typedef int32 (*ALARMGUI_CALLBACK)(int32 Param1, int32 Param2);
typedef void  (*SHOW_SYSEVENHIT)(uint16 EventType);
extern int32 af_callback_gui(int32 Param1,int32 Param2);

typedef enum 
{
	URGENCY_ALARM = 0x00,							// 紧急求救按钮
	GAS_DETECTOR,									// 瓦斯气体探测器
	DEF_DOOR_INBREAK_DETECTOR,						// 大门防入侵探测器
	DEF_SPACE_INBREAK_DETECTOR,						// 空间防护探测器
	DEF_WINDOW_INBREAK_DETECTOR1,					// 窗户防入侵探测器1
	DEF_WINDOW_INBREAK_DETECTOR2,					// 窗户防入侵探测器2
	DEF_BALCONY_INBREAK_DETECTOR,					// 阳台防入侵探测器
	USER_DEFINED									// 自定义类型	
}AREA_TYPE;

#if 0
// 安防状态
typedef enum
{
   DIS_DEFEND = 0x00,								// 撤防
   SET_DEFEND,								    	// 布防
   PART_DEFEND,										// 局防
   RESERVE                            				// 保留
}DEFEND_STATE;

// 按优先级排列，不能更改顺序,线路故障不针对对单个防区
typedef enum
{
   UNENABLE_SHOW_STATE = 0x00,						// 未启用
   ISO_SHOW_STATE,								   	// 隔离
   ALARM_SHOW_STATE,								// 报警
   WORK_SHOW_STATE,                            		// 单个防区布防状态
   NORM_SHOW_STATE									// 单个防区撤防状态
}SHOW_STATE;
#endif

// 安防按钮处理的返回值类型
typedef enum
{
	AF_SUCCESS = 0x00,								// 成功
	AF_NO_REMOTE_CTRL,								// 报警器不允许此类远程控制
	AF_SETTING_ERR,									// 主机不在撤防状态设置失败
	AF_BAOJINGNOBUFANG,								// 正在报警不能布防
	AF_FQCHUFA,										// 布防时有防区触发错误
	AF_FQGUZHANG,									// 布防时有防区故障错误
	AF_STATE_SAME,									// 当前的设置布防状态与整机当前状态相同
	AF_BAOJING,										// 当前处于报警状态
	AF_CANNOT_OVERLEAP								// 不能越过，夜间模式(局防)不能直接进入外出模式(布防)，外出模式不能直接进入在家模式(撤防)	
}AF_RETURN_TYPE; 

// 报警回调函数
typedef enum
{
	BAOJING_SHOW = 0x00,							// 强制显示报警界面
	BAOJING_FRESH,									// 当有状态显示界面时，需要刷新界面的内容
	BAOJING_DUIJIANG,								// 报警时正处于对讲状态
	BAOJING_KEY										// 按键的报警处理
}AF_CALLBACK_TYPE;

typedef enum 
{
	NO_SOUND_TYPE = 0x00,							// 无声音状态
	OUT_SOUND_TYPE,									// 退出预警声音状态
	IN_SOUND_TYPE,									// 进入预警声音状态
	ALARM_SOUND_TYPE,								// 报警声音状态
	STOP_ALL_SOUND_TYPE								// 关闭所有声音，撤防用
}SOUND_TYPE;

#define SET_ONE_VALUE(val,state,pos) ((0==state)?(val&=(~(1<<pos))):(val|=(1<<pos)))

/*************************************************
  Function:    		alarm_get_area_define
  Description: 		获得属性页面显示信息
  Input:
  Output:    		
  	1.define_info	属性页面显示的信息
  Return:				
  Others:    		       
*************************************************/
void alarm_get_area_define(uint8 * define_info);

/*************************************************
  Function:    		alarm_set_area_define
  Description: 		设置报警属性页面显示信息
  Input:
   	1.define_info	属性的数据
  Output:    		
  Return:			false:失败 true:成功				
  Others:           
*************************************************/
uint8 alarm_set_area_define(uint8 *define_info);

/*************************************************
  Function:    		alarm_get_area_isolation
  Description: 		获取防区隔离参数
  Input:
  Output:   
  Return:			暂时隔离参数
  Others:           
*************************************************/
uint8 alarm_get_area_isolation(void);

/*************************************************
  Function:    		alarm_set_area_isolation
  Description: 		设置报警暂时隔离显示信息
  Input:
  	1.iso_info		暂时隔离显示信息
  Output:    		
  Return:			false:失败 true:成功
  Others:           
*************************************************/
uint8 alarm_set_area_isolation(uint8 iso_info);

/*************************************************
  Function:    		alarm_get_part_valid
  Description: 		获取防区局防有效参数
  Input:
  Output:   
  Return:			局防有效参数	
  Others:           
*************************************************/
uint8 alarm_get_part_valid(void);

/*************************************************
  Function:    		alarm_set_area_part_valid
  Description: 		设置报警局防有效页面显示信息
  Input:
  	1.valid			局防有效页面显示信息
  Output:    		
  Return:			false:失败 true:成功
  Others:           
*************************************************/
uint8 alarm_set_area_part_valid(uint8 valid);

/*************************************************
  Function:    		center_get_area_define
  Description: 		中心管理机获取的防区属性
  Input:
  Output:  
   	1.define 		中心管理机获取的属性
  Return:				
  Others:         	
  					中心管理机获取的防区属性有
  					防区数、启用参数、防区类型
*************************************************/
void center_get_area_define(ALARM_DEFINE_NET_DATA * alarm_define);

/*************************************************
  Function:    		center_get_area_state
  Description: 		中心管理机的获取的安防状态
  Input:			无
  Output:		
  	1.status 		中心管理机获取的安防状态结构体
  Return:			无	
  Others:           
*************************************************/
void center_get_area_state(ALARM_STATUES_NET_DATA * status);

/*************************************************
  Function:    		ipad_get_area_state
  Description: 		ipad获取安防状态
  Input:			无
  Output:		
  	1.status 		ipad获取的安防状态结构体
  Return:			无	
  Others:           
*************************************************/
void ipad_get_area_state(ALARM_STATUES_NET_DATA * status);

/*************************************************
  Function:			check_defend_oper
  Description:		安防状态处理
  Input: 
  	1.state			安防状态
  	2.exec			执行者
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE check_defend_oper(DEFEND_STATE state, EXECUTOR_E exec);

/*************************************************
  Function:			alarm_clear_alerts_operator
  Description:		清除警示操作
  Input: 		
  Output:			无
  Return:			
  Others:			无
*************************************************/
void alarm_clear_alerts_operator(void);

/*************************************************
  Function:			alarm_set_operator
  Description:		布防操作
  Input: 
   	1.exec			执行者
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_set_operator(EXECUTOR_E exec);

/*************************************************
  Function:			alarm_single_set_operator
  Description:		单防区布防操作
  Input: 		
  	1.num			防区号
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_single_set_operator(uint8 num);

/*************************************************
  Function:			alarm_unset_operator
  Description:		撤防命令
  Input: 
  	1.exec			执行者
  	2.force_type    1:挟持 0:正常
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_unset_operator(EXECUTOR_E exec, uint8 force_type);

/*************************************************
  Function:			alarm_partset_operator
  Description:		局防操作
  Input: 	
  	1.exec			执行者
  Output:			无
  Return:			无
  Others:			无
*************************************************/
AF_RETURN_TYPE alarm_partset_operator(EXECUTOR_E exec);

/*************************************************
  Function:			alarm_logic_init
  Description:		报警逻辑初始化
  Input: 		
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void alarm_logic_init(void);

/*************************************************
  Function:    		alarm_init_gui_callback
  Description: 		安防GUI 回调函数
  Input:
  	1.func			回调函数
  Output:    		无
  Return:			无
  Others:           
*************************************************/
//void alarm_init_gui_callback(ALARMGUI_CALLBACK func);
void alarm_init_gui_callback(ALARMGUI_CALLBACK func, SHOW_SYSEVENHIT func1);

/*************************************************
  Function:    		logic_get_area_isolation
  Description: 		获取防区隔离参数
  Input:
  Output:   
  Return:			暂时隔离参数
  Others:           
*************************************************/
uint8 logic_get_area_isolation(void);

/*************************************************
  Function:    		logic_set_area_isolation
  Description: 		设置防区暂时隔离参数
  Input:
  	1.iso_info		1- 暂时隔离
  Output:    		
  Return:			false:失败 true:成功
  Others:           每一位代表一个防区
*************************************************/
uint8 logic_set_area_isolation(uint8 iso_info);

/*************************************************
  Function:    		logic_get_part_valid
  Description: 		获取防区局防有效参数
  Input:
  Output:   
  Return:			局防有效参数	
  Others:           
*************************************************/
uint8 logic_get_part_valid(void);

/*************************************************
  Function:    		logic_set_part_valid
  Description: 		设置防区局防有效参数
  Input:
  	1.valid			局防有效参数
  Output:    		
  Return:			false:失败 true:成功
  Others:           
*************************************************/
uint8 logic_set_part_valid(uint8 valid);

/*************************************************
  Function:    		logic_get_alarm_param
  Description: 		获得安防参数
  Input:
  Output:    		
  	1.param_info	安防参数
  Return:				
  Others:    		       
*************************************************/
void logic_get_alarm_param(uint8 *param_info);

/*************************************************
  Function:    		logic_set_alarm_param
  Description: 		设置安防参数
  Input:
   	1.param_info	安防参数
  Output:    		
  Return:			false:失败 true:成功				
  Others:           
*************************************************/
uint8 logic_set_alarm_param(uint8 *param_info);

/*************************************************
  Function:    		alarm_gui_callback
  Description: 		执行安防回调
  Input:
  	1.Param1		参数1		
  	2.Param2		参数2
  Output: 			无   		
  Return:			无
  Others:           
*************************************************/
void alarm_gui_callback(int32 Param1, int32 Param2);

/*************************************************
  Function:    		alarm_get_24clear_param  
  Description: 		获得非24小时防区清除后标志
  Input:
  Output:    		
  	1.param	非24小时防区清除后标志参数
  Return:				
  Others:    add by wufn		       
*************************************************/
uint8 alarm_get_no24touch_param(void);

/*************************************************
  Function:    		alarm_get_24clear_param  
  Description: 		获得非24小时防区触发
  Input:
  Output:    		
  	1.param	非24小时防区触发标志参数
  Return:				
  Others:    add by wufn		       
*************************************************/
uint8 alarm_get_no24touch_param(void);

/*************************************************
  Function:    	logic_get_g_whole_alarm_state_param  
  Description: 		获取整机报警标志
  Input:
  Output:    		
  	1.param	整机报警标志
  Return:				
  Others:    add by wufn		       
*************************************************/
uint8 logic_get_g_whole_alarm_state_param(void);

#endif

