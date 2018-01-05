/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:  	logic_packet_list.h
  Author:     	xiewr
  Version:    	1.0
  Date: 
  Description:  网络包发送接收链表头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _NET_PACKET_LIST_H_
#define _NET_PACKET_LIST_H_

#if 0
#include "typedef.h"
#include "net_comm_types.h"
#include "storage_types.h"
#include "logic_udp_comm.h"
#include "logic_distribute.h"
#endif
#include "logic_include.h"

#define		PACKET_NODE_MAX_NUM			100				// 链表节点最大个数
#define		PACKET_NODE_DATA_SIZE		NET_DATA_MAX	// 包节点数据大小

// 超时发送检测间隔时间
#if !USE_RECIVE_THREAD
#define		PACKET_OUTTIME_INTERVAL		(1000/(UDP_RECV_TIMER_TICKS*5))	
#elif !USE_DISTRIBUTE_THREAD
#define		PACKET_OUTTIME_INTERVAL		(1000/(NET_DIST_TIMER_TICKS*5))	
#endif
#define 	PACKET_TIMEOUT_VALUE		3				// 默认超时判断时间, 发送者可以指定(秒)

// 发送超时处理函数, 参数:发送包
typedef void (*PFSEND_TIMEOUT_ECHO)(const PSEND_PACKET SendPacket);

/*************************************************
  Function:    		net_packet_list_init
  Description: 		初始化链表,分配空间
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_packet_list_init(void);

/*************************************************
  Function:    		net_add_send_packet
  Description: 		增加一个发送包到链表
  Input: 
	1.data			发送数据
	2.size			发送数据大小
	3.address		发送地址
	4.port			发送端口
	5.SendTimes		发送次数, 默认发送次数为1
	6.TimeOut		发送超时时间, 默认为TIMEOUT_VALUE
  Output:			无
  Return:			发送包
  Others:
*************************************************/
PSEND_PACKET net_add_send_packet(char * data, int32 size, uint32 address, uint16 port, 
	int32 SendTimes, int32 TimeOut);

/*************************************************
  Function:    		net_del_send_packet
  Description: 		从发送链表移除一个网络包, 加到空闲链表
  Input: 
	1.node			发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_del_send_packet(PSEND_PACKET node);

/*************************************************
  Function:    		net_send_proc
  Description:		发送线程处理函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* net_send_proc(void *arg);

/*************************************************
  Function:    		net_timeout_proc
  Description:		超时线程处理函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void* net_timeout_proc(void *arg);

/*************************************************
  Function:    		net_add_recive_packet
  Description:		接收网络数据包线程执行函数
  Input: 			
  	1.data			数据
  	2.size			大小
  	3.address		地址
  	4.port			端口号
  Output:			无
  Return:			无
  Others:			添加一个接收包到接收链表
*************************************************/
void net_add_recive_packet(char *data, uint32 size, uint32 address, uint16 port);

/*************************************************
  Function:    		net_get_recive_packet
  Description:		读取一个接收包, 从接收链表中移除该节点
  Input: 			无
  Output:			无
  Return:			接收包
  Others:
*************************************************/
PRECIVE_PACKET net_get_recive_packet(void);

/*************************************************
  Function:    		net_del_recive_packet
  Description:		把一个接收包添加到空闲链表
  Input: 			
	1.node			包节点
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_del_recive_packet(PRECIVE_PACKET node);

/*************************************************
  Function:    		net_get_sendpacket_by_responsion
  Description:		根据应答包头,获得发送包
  Input: 
	1.netHead		应答包头
  Output:			无
  Return:			发送包
  Others:
*************************************************/
PSEND_PACKET net_get_sendpacket_by_responsion(PNET_HEAD netHead);

/*************************************************
  Function:    		net_set_send_timeout_echofunc
  Description:		设置发送超时应答回调函数
  Input: 
	1.func			发送超时应答函数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_send_timeout_echofunc(PFSEND_TIMEOUT_ECHO func);

/*************************************************
  Function:    		start_comm_thread
  Description:		启动网络线程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_start_comm_thread(void);

/*************************************************
  Function:    		stop_comm_thread
  Description:		停止网络线程
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_stop_comm_thread(void);

#endif
