/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:  	logic_interphone.h
  Author:     	xiewr
  Version:    	1.0
  Date: 
  Description:  网络分发模块头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _LOGIC_INTERPHONE_H_
#define _LOGIC_INTERPHONE_H_

#include "logic_include.h"

#define DOOR_DEVICE_NUM			2					// 门前机最大数
#define STAIR_DEVICE_NUM		20					// 梯口分机最大数
#define AREA_DEVICE_NUM			99					// 区口机最大数 

#define CALL_AUDIO_PT			PT_G711A			// 默认呼叫音频格式
#define CALL_VIDEO_PT			PT_H264				// 默认呼叫视频格式

#define CALL_RTP_AUDIO_PT		8					// RTP音频类型
#define CALL_RTP_VIDEO_PT		98					// RTP视频类型

#if (_LCD_DPI_ == _LCD_800480_)
#define SNAP_PIC_WIDTH			640
#define SNAP_PIC_HEIGHT			480
#elif (_LCD_DPI_ == _LCD_1024600_)
#define SNAP_PIC_WIDTH			640
#define SNAP_PIC_HEIGHT			480
#endif

#if 0
#define	INTER_TPRI				TPRI_NORMAL		// 对讲监视线程优先级
#define	INTER_MB				MBGUI			// 对讲监视线程邮箱
#endif

// 音频或视频发送模式
typedef enum
{
    _NONE 		= 0x00,
	_SENDONLY	= 0x01,
	_RECVONLY	= 0x02,
	_SENDRECV	= 0x03
}INTER_MEDIA_SEND_STATE;

// 应答码
typedef enum
{
	TRC_UNKNOWN		=	0,							// 未知
	TRC_TRYING		=	100,						// 请求已经接收，请等待 
	TRC_RINGING		=	180,						// 已经振铃
	TRC_QUEUED		=	182,						// 呼叫进入等待队列
	TRC_OK			=	200,						// 请求确定
	TRC_MOVE_TEMP	=	302,						// 临时转向
	TRC_NOT_FOUND	=	404,						// 找不到该用户
	TRC_BUSY		=	486,						// 忙 
	TRC_TERMINATED	=	487 						// 请求被终止 
}INTER_RESPONSE_CODE_E;

/*************************************************
  Function:			is_fenji1_DeviceNo
  Description:		判断是否为主室内分机
  Input: 			无
  Output:			无
  Return:			成功与否
  Others:			分机号为0 室内主机
*************************************************/
uint8 is_fenji1_DeviceNo(void);

/*************************************************
  Function:			is_main_DeviceNo
  Description:		判断是否为主室内分机
  Input: 			无
  Output:			无
  Return:			成功与否
  Others:			分机号为0 室内主机
*************************************************/
uint8 is_main_DeviceNo(void);

/*************************************************
  Function:			get_stair_fullno
  Description:		获得梯号
  Input: 			
  	1.StairNo		梯口号
  	2.ExtNo			分机号
  Output:			无
  	1.StairNo		梯口号 
  Return:			梯号字符串
  Others:
*************************************************/
char * get_stair_fullno(char * StairNo, char* ExtNo);

/*************************************************
  Function:			set_stair_address
  Description:		设置梯口机地址
  Input: 			
  	1.no			编号 0-9
  	2.address		地址
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_stair_address(int32 no, uint32 address);

/*************************************************
  Function:			get_stair_address
  Description:		从列表中获得梯口机地址,未找到不广播获取
  Input: 			
  	1.no			编号 0-9, 0xFF为所有
  Output:			无
  Return:			地址
  Others:
*************************************************/
uint32 get_stair_address_inlist(int32 no);

/*************************************************
  Function:			get_stair_address
  Description:		获得指定梯口机地址
  Input: 			
  	1.no			编号 0-9, 0xFF为所有
  Output:			无
  Return:			地址
  Others:
*************************************************/
uint32 get_stair_address(int32 no);

/*************************************************
  Function:			get_stair_list
  Description:		获得梯口机地址列表
  Input: 			无
  Output:			
  	1.FirstIndex	第一个设备索引: 0-~
  	2.ipaddr		第一个设备IP
  Return:			是否获取到设备IP: TRUE / FALSE
  Others:
*************************************************/
int32 get_stair_list(uint32 *index, uint32 *ipaddr);

/*************************************************
  Function:			set_stair0_addr
  Description:		获得梯口机0地址
  Input: 			无
  Output:			
  Return:			IPaddr
  Others:
*************************************************/
void set_stair0_addr(void);

/*************************************************
  Function:			get_stair0_addr
  Description:		获得梯口机0地址
  Input: 			无
  Output:			
  Return:			IPaddr
  Others:
*************************************************/
uint32 get_stair0_addr(void);

/*************************************************
  Function:			get_door_address
  Description:		获得门前机地址
  Input: 			
  	1.ID			编号 1－2 
  Output:			无
  Return:			成功－地址，失败－0
  Others:			
*************************************************/
uint32 get_door_address(int32 ID);

/*************************************************
  Function:			get_netdoor_list
  Description:		获得网络门前机地址列表
  Input: 			无
  Output:			
  	1.index			第一个连接设备的索引:1-2
  	2.ipaddr		第一个连接设备的IP
  Return:			TRUE / FALSE
  Others:
*************************************************/
int32 get_netdoor_list(uint32 *index, uint32 *ipaddr);

/*************************************************
  Function:			get_netdoor_address_inlist
  Description:		获得门前机地址
  Input: 			
  	1.no			编号 1-2
  Output:			无
  Return:			地址
  Others:
*************************************************/
uint32 get_netdoor_address_inlist(int32 no);

/*************************************************
  Function:			get_phonedoor_address_inlist
  Description:		获得门前机地址
  Input: 			
  	1.no			编号 1-2
  Output:			无
  Return:			地址
  Others:
*************************************************/
uint32 get_phonedoor_address_inlist(int32 no);

/*************************************************
  Function:			set_area_address
  Description:		获得区口机地址
  Input: 			
  	1.no			编号: 1-40
  	2.address		地址
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_area_address(int32 no, uint32 address);

/*************************************************
  Function:			get_stair_address
  Description:		获得区口机地址,未找到不广播获取
  Input: 			
  	1.no			编号 1-40
  Output:			无
  Return:			地址
  Others:
*************************************************/
uint32 get_area_address_inlist(int32 no);

/*************************************************
  Function:			get_area_address
  Description:		获得指定区口机地址
  Input: 			
  	1.no			编号
  Output:			无
  Return:			地址
  Others:
*************************************************/
uint32 get_area_address(int32 no);

/*************************************************
  Function:			search_stair_list
  Description:		获得梯口机地址列表
  Input: 			无
  Output:			
  Return:			是否获取到设备IP: TRUE / FALSE
  Others:
*************************************************/
int32 search_stair_list(void);

/*************************************************
  Function:			search_area_list
  Description:		获得区口机地址列表
  Input: 			无
  Output:			
  Return:			TRUE / FALSE
  Others:
*************************************************/
int32 search_area_list(void);

/*************************************************
  Function:			search_door_list
  Description:		获得网络门前机地址列表
  Input: 			无
  Output:			
  Return:			TRUE / FALSE
  Others:
*************************************************/
int32 search_door_list(void);

/*************************************************
  Function:			get_phonedoor_address
  Description:		获得门前机地址
  Input: 			
  	1.ID			编号 1－2 
  Output:			无
  Return:			成功－地址，失败－0
  Others:			
*************************************************/
uint32 get_phonedoor_address(int32 ID);

/*************************************************
  Function:			init_list_address
  Description:		初始化设备列表
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 init_list_address(void);

/*************************************************
  Function:			inter_distribute
  Description:		对讲接收发送包回调函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 inter_distribute(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			inter_responsion
  Description:		对讲接收应答包处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void inter_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket);
#endif

