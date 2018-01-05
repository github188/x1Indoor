/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_message.c
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-11-06
  Description:	信息处理逻辑
  				中心发给住户的称为信息（类型包括文字、图片、声音或组合）
  				用户通过手机发给住户的称为短信, 类型为纯文字, 一次不超过70个汉字。
  History:                    
    1. Date:		 
       Author:		 
       Modification: 
    2. ...
*********************************************************/
#ifndef __LOGIC_MESSAGE_H__
#define __LOGIC_MESSAGE_H__

//#include "net_comm_types.h"
#include "logic_include.h"

/*************************************************
  Function:			msg_distribute
  Description:		接收短信发送命令应答处理函数
  Input:
  	1.packet		接收包
  Output:			无
  Return:			合法命令TRUE, 非法命令FALSE
  Others:
**************************************************/
int msg_distribute(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			message_responsion
  Description:		接收短信应答命令应答处理函数
  Input:
  	1.recPacket		网络接收包
  	2.SendPacket	网络发送包
  Output:			无
  Return:			无
  Others:
**************************************************/
void  msg_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket);
#endif

