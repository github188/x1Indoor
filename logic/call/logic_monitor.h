/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_monitor.h
  Author:     	chenbh
  Version:    	2.0
  Date: 		2014-12-11
  Description:  监视头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _LOGIC_MONITOR_H_
#define _LOGIC_MONITOR_H_
#include "logic_include.h"


extern void monitor_state_callbak(uint32 param1, uint32 param2);

// 监视返回值
typedef enum
{	 
	MONITOR_OK,										// 正常结束
	MONITOR_ERR,									// 一般错误
	MONITOR_ERR_ID,									// 设备索引错误
	MONITOR_SEARCH_ERR,								// 搜索设备失败
	MONITOR_REQUEST_TIMEOUT,						// 连接超时
	MONITOR_MONITORING_TIMEOUT,
	MONITOR_TALKING_TIMEOUT,
	MONITOR_HEART_TIMEOUT,							// 心跳超时
	MONITOR_BUSY									// 对方忙
}MONITOR_VALUE_E;

// 监视状态
typedef enum
{
	MONITOR_END,									// 监视结束
	MONITOR_SEARCH,									// 搜索设备中
	MONITOR_REQUEST,								// 连接中
	MONITOR_MONITORING,								// 监视中
	MONITOR_TALKING,								// 通话中
	MONITOR_GETLIST,								// 获取列表
}MONITOR_STATE_E;

#define MONITOR_TIMER		(MONITOR_END+10) 		// 监视计时

/*************************************************
  Function:			monitor_request
  Description:		监视请求
  Input: 	
   	1.ID			应用ID
  	2.DevType		设备类型
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_start(DEVICE_TYPE_E DevType, int8 index);

#define MONITOR_FAX_IP 1

#if MONITOR_FAX_IP
/*************************************************
  Function:			monitor_request
  Description:		监视请求
  Input: 	
   	1.ID			应用ID
  	2.DevType		设备类型
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 test_monitor_start(void);
#endif

/*************************************************
  Function:			monitor_stop
  Description:		结束监视或通话
  Input: 	
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_stop(void);

/*************************************************
  Function:				monitor_ini
  Description:			监视初始化
  Input: 	
  	1.GuiProc			GUI回调函数
  Output:				无
  Return:				
  Others:
*************************************************/
void monitor_ini(PFGuiNotify GuiProc);

/*************************************************
  Function:			monitor_distribute
  Description:		监视命令发送包处理函数
  Input: 	
  	1.recPacket		接受网络包
  Output:			无
  Return:			成功与否,true/fasle
  Others:
*************************************************/
int32 monitor_distribute(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			monitor_responsion
  Description:		监视命令应答包处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void monitor_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket);

/*************************************************
  Function:			monitor_talk
  Description:		监视转通话
  Input: 	
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_talk(void);

/*************************************************
  Function:			monitor_unlock
  Description:		室内机监视开锁
  Input: 			无
  Output:			无
  Return:			成功与否
  Others:
*************************************************/
int32 monitor_unlock(void);

/*************************************************
  Function:			monitor_next
  Description:		监视切换
  Input: 	
  	1.direct		方向: TRUE-下一个 FALSE-上一个
  Output:			无
  Return:			成功与否, TRUE / FALSE
  Others:
*************************************************/
int32 monitor_next(int32 direct);

/*************************************************
  Function:			monitor_video_snap
  Description:		抓拍
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 monitor_video_snap(void);

/*************************************************
  Function:		rtsp_monitor_sync_devlist
  Description: 	获取RTSP设备列表
  Input: 		无
  Output:		无
  Return:		PMonitorDeviceList 设备列表
  Others:		
*************************************************/
uint32 monitorlist_sync_devlist(PFGuiNotify GuiProc, DEVICE_TYPE_E DevType);

/*************************************************
  Function:		fill_devno_by_index
  Description: 	填充设备编号
  Input: 		
  	1.DevType   设备类型
  	2.index		设备索引
  Output:		无
  Return:		
  Others:		
*************************************************/
void fill_devno_by_index(DEVICE_TYPE_E DevType, int8 index, char *devno);

#endif

