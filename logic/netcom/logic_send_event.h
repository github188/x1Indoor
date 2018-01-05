/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_send_event.h
  Author:   	xiewr
  Version:   	1.0
  Date: 		
  Description:  未发送成功事件头文件
  
  History:        
                  
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef __NET_SEND_EVENT_H__
#define __NET_SEND_EVENT_H__

#if 0
#include "typedef.h"
#include "net_command.h"
#endif
#include "logic_include.h"

#define SEND_RECORD_MAXLEN       	20 													// 每条记录的长度
#define SEND_RECORD_HEAD_LEN       	(1 + sizeof(NET_COMMAND)) 							// 每条记录头的长度
#define SEND_RECORD_DATA_LEN       	(SEND_RECORD_MAXLEN - SEND_RECORD_HEAD_LEN) 		// 每条记录有效载荷
#define SEND_RECORD_STOR_COUNT		2000												// 存储记录数

/*************************************************
  Function:			net_ini_unsend_file
  Description: 		启动时,初始化事件文件
  Input: 			无
  Output:			无
  Return:			无
*************************************************/
void net_ini_unsend_file(void);

/*************************************************
  Function:    		net_close_unsend_file
  Description:		关闭未发送成功文件(在程序结束时关闭)
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_close_unsend_file(void);

/*************************************************
  Function:			net_send_event_from_file
  Description: 		从存储的文件中发送指定发送方的事件
  Input: 
	1.cmd			命令
	2.data			发送数据
	3.size			发送数据大小
	4.SendTo		发送方, 冠林平台/中心服务器/管理员机 
  Output:			无
  Return:			是否成功
*************************************************/
int32 net_send_event_from_file(NET_COMMAND cmd, char * data, int32 size, EVENT_SENDTO_E SendTo);

/*************************************************
  Function:			net_check_is_report_event_echo
  Description: 		判断该应答包是否上报事件的应当包
  Input: 
	1.ID			应答命令包
  Output:			无
  Return:			是否成功
*************************************************/
int32 net_check_is_report_event_echo(NET_COMMAND cmd, uint16 ID);

/*************************************************
  Function:			net_update_event_file
  Description: 		定时更新事件发送文件
  Input: 			无
  Output:			无
  Return:			无
*************************************************/
void net_update_event_file(void);

#endif 

