/********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_alarm_net.h
  Author:		wengjf
  Version:		V1.0
  Date:			2010-4-10
  Description:	网关的报警网络控制控制
  
  History:        
                  
    1. Date:
       Author:
       Modification:为防止索引的混乱，所有的防区号全部按0-7
       表示1-8防区号，不要更改。
    2. ... 
*********************************************************/
#ifndef __LOGIC_ALARM_NET_H__
#define __LOGIC_ALARM_NET_H__

//#include "net_comm_types.h"
#include "logic_include.h"

// 防区属性结构(上报给管理员机用)
typedef struct 
{
	uint8 area_amount;								// 防区数
	uint8 bypass[2];								// 启用参数
	uint8 area_type[16];							// 类型(16B)	
}ALARM_DEFINE_NET_DATA, *PALARM_DEFINE_NET_DATA;

// 安防状态结构(上报给管理员机用), 每一位代表一个防区
typedef struct
{
	uint8 defend_state;								// 布防状态,撤防状态,局防状态
	uint8 area_amount;								// 防区数1B
	uint8 area_state[2];							// 防区状态(2B)
	uint8 alarm_state[2];							// 防区报警状态(2B)
	uint8 fault_state[2];							// 防区故障状态(2B)	
}ALARM_STATUES_NET_DATA, *PALARM_STATUES_NET_DATA;

/*************************************************
  Function:    		alarm_define_report
  Description: 		属性上报
  Input:        
  Output:       	无
  Return:			上报成功与否
  Others:       	
*************************************************/
int32 alarm_define_report(void);

/*************************************************
  Function:    		alarm_status_report
  Description: 		安防状态上报
  Input:        
  Output:       	无
  Return:			上报成功与否
  Others:       	
*************************************************/
int32 alarm_status_report(void);

/*************************************************
  Function:    		set_event_report
  Description: 		布防操作事件上报
  Input:        	
  	1.controlby		控制执行者
  Output:      		无
  Return:			上报成功与否
  Others:
*************************************************/
int32 set_event_report(EXECUTOR_E controlby);

/*************************************************
  Function:    		unset_event_report
  Description: 		撤防操作事件上报
  Input:        	
  	1.controlby		控制执行者
  Output:       	无
  Return:			上报成功与否
  Others:
*************************************************/
int32 unset_event_report(EXECUTOR_E controlby);

/*************************************************
  Function:    		partset_event_report
  Description: 		局防操作事件上报
  Input:       
  	1.controlby		控制执行者
  Output:       	无
  Return:			上报成功与否
  Others:
*************************************************/
int32 partset_event_report(EXECUTOR_E controlby);

/*************************************************
  Function:    		bypass_event_report
  Description: 		旁路操作事件
  Input:        
  	1.bypass_num	旁路防区数
    2.bypass_area	旁路防区
  Output:       	无
  Return:			上报成功与否
  Others:       	bit0－15，1－旁路的防区
*************************************************/
int32 bypass_event_report(int8 bypass_num, uint16 bypass_area);

/*************************************************
  Function:    		force_alarm_report
  Description: 		挟持撤防报警事件上报
  Input:   	    	
  Output:       	无
  Return:			上报成功与否
  Others:
*************************************************/
int force_alarm_report(void);

/*************************************************
  Function:    		sos_alarm_report
  Description: 		紧急求救报警事件上报
  Input:   	   		
  Output:       	无
  Return:			上报成功与否
  Others:
*************************************************/
int sos_alarm_report(void);

/*************************************************
  Function:    		break_alarm_report
  Description: 		防区触发报警事件上报
  Input:   	    	
  	1. area_num     防区号码
  Output:       	无
  Return:			上报成功与否
  Others:
*************************************************/
int32 break_alarm_report(uint8 area_num);

/*************************************************
  Function:    		alarm_distribute
  Description: 		报警网络包的分发
  Input:       		
  	1.recPacket		接收包
  Output:
  Return: 	   
  Others:      		
*************************************************/
int32 alarm_distribute(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			alarm_responsion
  Description:		报警接收应答包处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void alarm_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket);

/*************************************************
  Function:    		sync_comm_event_send
  Description: 		同步安防事件上报通用函数
  Input:   	    
  	1.command		安防操作事件
    2.controlby		控制执行者
  Output:       	无
  Return:			上报成功与否
  Others:
*************************************************/
int32 sync_comm_event_send(uint16 command, EXECUTOR_E controlby);
#endif

