/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_distribute.h
  Author:     	chenbh
  Version:    	1.0
  Date: 		2014-12-11
  Description:  网络分发模块头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _NET_DISTRIBUTE_H_
#define _NET_DISTRIBUTE_H_

#include "storage_include.h"
#include "logic_include.h"

#define	USE_DISTRIBUTE_THREAD	0					// 使用线程分发数据
#if USE_DISTRIBUTE_THREAD
#define	MB_NETDIST				MBCom				
#define TPRI_NETDIST			TPRI_COM
#else
#define	NET_DIST_TIMER_TICKS	100					// 使用定时器分发数据
#endif

// 多包接收超时检测间隔
#if !USE_RECIVE_THREAD
#define	NET_MULTI_RECV_CHECK_TICKS	UDP_RECV_TIMER_TICKS
#elif !USE_DISTRIBUTE_THREAD
#define	NET_MULTI_RECV_CHECK_TICKS	NET_DIST_TIMER_TICKS
#endif

extern DEVICE_NO G_CENTER_DEVNO;
extern DEVICE_NO G_MANAGER_DEVNO;
extern DEVICE_NO G_BROADCAST_DEVNO;
extern DEVICE_NO G_MULTCMD_DEVNO;

/*********************网络接收回调***********************************/
//定义接收数据发送包回调函数类型, 参数:接收包, 返回值:合法命令TRUE, 非法命令FALSE,调用者不用应答
typedef int32 (*PFDISTRIBUTE)(const PRECIVE_PACKET recPacket);
//定义接收数据应答包回调函数类型, 参数:接收包, 发送包
typedef void (*PFRESPONSION)(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket);

/**********************多包发送*********************************/
//多包发送结果应答回调函数类型, 参数:会话ID，应答结果
typedef void (*PFMAX_SEND_ECHO)(uint32 ID, int sendEcho);
//多包接收结果回调函数类型, 参数:会话ID, 数据, 数据长度
typedef void (*PFMAX_RECIVE)(uint32 ID, char * data, int size);
//多包发送或接收状态, 参数:会话ID, 总共包数, 已经接收或发送成功包数, 已经接收或发送成功数据长度
typedef void (*PFMAX_SEND_REC_STATE)(uint32 ID, int Total, int CurNum, int size);

/*************************************************
  Function:    		net_direct_send
  Description:		网络直接发送，不加到链表中
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
  Output:			无
  Return:			包ID
  Others:
*************************************************/
uint16 net_direct_send(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port);

/*************************************************
  Function:    		net_send_by_list
  Description:		采用链表发送网络命令包,不阻塞
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
	6.SendTimes		发送次数
	7.TimeOut		单次发送超时时间
  Output:			无
  Return:			包ID
  Others:
*************************************************/
uint16 net_send_by_list(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port, 
				int32 SendTimes, int32 TimeOut);

/*************************************************
  Function:    		net_send_command
  Description:		网络命令阻塞发送，该命令在主线程中等待应答
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.address		对方IP地址
	5.port			对方端口号
	6.TimeOut		发送超时时间
  Output:			
	1.EchoValue		应答码
	2.ReciData		接收应答数据, 注意:需外部分配空间
	3.ReciSize		接收应答数据大小
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int32 net_send_command(NET_COMMAND cmd, char * data, int32 size, uint32 address, uint16 port, int32 TimeOut,
				 uint8 * EchoValue, char * ReciData, int32 * ReciSize);

/*************************************************
  Function:    		net_send_echo_packet
  Description:		发送应答包
  Input: 
	1.recPacket		接收包
	2.echoValue		应答码
	3.data			附加数据
	4.size			附加数据大小
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_send_echo_packet(PRECIVE_PACKET recPacket, ECHO_VALUE_E echoValue, char * data, int32 size);

/*************************************************
  Function:    		net_send_event
  Description:		上报事件，含报警事件
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.SendTo		发送方, 冠林平台/中心服务器/管理员机 
  Output:			
  Return:			是否可发送, TRUE/FALSE， FALSE－该命令为非定义的事件命令或数据长度参数错误
  Others:
*************************************************/
int32 net_send_event(NET_COMMAND cmd, char * data, int32 size, EVENT_SENDTO_E SendTo);

/*************************************************
  Function:    		net_set_recivedata_func
  Description:		设置函数回调函数
  Input: 
	1.subSysCode	子系统代号
	2.distribute	接收发送命令应答处理函数
	3.responsion	接收应答命令应答处理函数
  Output:			无
  Return:			无
  Others:
*************************************************/
int32 net_set_recivedata_func(SUB_SYS_CODE_E SubSysCode, PFDISTRIBUTE distribute, PFRESPONSION responsion);

/*************************************************
  Function:    		net_start_comm
  Description:		启动通讯模块
  Input: 
	1.FullDeviceNo	设备编号
	2.NetParam		传入网络参数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_start_comm(PFULL_DEVICE_NO FullDeviceNo, PNET_PARAM NetParam);

/*************************************************
  Function:    		net_stop_comm
  Description:		停止通讯模块
  Input: 			无		
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_comm(void);

/*************************************************
  Function:    		net_get_comm_device_type
  Description:		获得网络模块设备类型
  Input: 			无
  Output:			无
  Return:			设备类型
  Others:
*************************************************/
DEVICE_TYPE_E net_get_comm_device_type(void);

/*************************************************
  Function:    		net_change_comm_deviceno
  Description:		更新网络模块设备号信息，含小区号
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_change_comm_deviceno(void);

/*************************************************
  Function:    		net_start_multipacket_recive
  Description:		开始分包接收数据
  Input: 
	1.address		对方IP地址
	2.port			对方端口号
	3.TimeOut		超时时间, 秒, 0时,为默认超时时间
	4.func			接收成功后应答回调函数
	5.state			接收状态回调函数
  Output:			无
  Return:			接收会话ID, 0为忙应答
  Others:
*************************************************/
uint32 net_start_multipacket_recive(uint32 address, uint16 port, uint8 TimeOut, PFMAX_RECIVE func, PFMAX_SEND_REC_STATE state);

/*************************************************
  Function:    		net_stop_multipacket_recive
  Description:		中断分包接收数据, (用户可以中断分包接收)
  Input: 
	1.ID			接收会话ID
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_multipacket_recive(uint32 ID);

/*************************************************
  Function:    		net_recive_mult_timeout_check
  Description:		多包接收超时检查
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* net_recive_mult_timeout_proc(void *arg);

/*************************************************
  Function:    		net_multi_send_check
  Description:		检查是否有多包发送任务需要处理
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_multi_send_check(void);

/*************************************************
  Function:    		net_start_multi_packet_send
  Description:		分包数据发送
  Input: 
	1.ID			会话ID
	2.data			数据
	3.size			数据大小
	4.address		对方IP地址
	5.port			对方端口号
	6.func			发送成功后应答回调函数
	7.state			发送状态回调函数
  Output:			无
  Return:			是否成功, TRUE / FALSE
  Others:
*************************************************/
int32 net_start_multi_packet_send(uint32 ID, char * data, int32 size, uint32 address, uint16 port, PFMAX_SEND_ECHO func, PFMAX_SEND_REC_STATE state);

/*************************************************
  Function:    		net_stop_multipacket_send
  Description:		中断分包发送数据, (用户可以中断分包发送)
  Input: 
	1.ID			发送会话ID
	2.address		发送地址
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_multipacket_send(uint32 ID, uint32 address);

/*************************************************
  Function:    		net_get_utc_time
  Description:		获取UTC时间
  Input: 
  Output:			
  	1.DateTime		时间结构体
  Return:			无
  Others:			默认时区为8
*************************************************/
void net_get_utc_time(NET_DATA_TIME * DateTime);

/*************************************************
  Function:    		set_nethead
  Description: 		设置目标设备的区号和设备号, 和优先级
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_nethead(DEVICE_NO DeviceNo, uint16 Priry);
#endif

