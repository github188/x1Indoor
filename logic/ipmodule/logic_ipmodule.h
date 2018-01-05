/********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_ipmodule.h
  Author:		
  Version:		
  Date:			
  Description:	
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ... 
*********************************************************/
#ifndef __LOGIC_IPMODULE_H__
#define __LOGIC_IPMODULE_H__

#include "logic_include.h"
#include "storage_include.h"

// 家电厂商
typedef enum
{
	IP_MODULE_CODE_OK = 200,						// OK
	IP_MODULE_CODE_UNLINE = 408,					// 模块离线
	IP_MODULE_CODE_IP_ERR = 484,					// 模块下发IP错误
	IP_MODULE_CODE_CODE_ERR = 401,					// 绑定码错误
}IP_MODULE_ERR_CODE;


typedef struct
{
	unsigned char JdType;
	unsigned char Index;
	unsigned char IsUsed;
	unsigned char IsTune;
	unsigned char  Addr;
	unsigned char  Reserve1;
	unsigned short Reserve2;
	char Name[20];
}AU_JDDEV_INFO, *PAU_JDDEV_INFO;


/*************************************************
  Function:			public_distribute
  Description:		公共模块接收发送包回调函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否,true/fasle
  Others:
*************************************************/
int ipmodule_distribute(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			set_door_ip
  Description:		获取网络门前机IP1 
  Input: 	
  	1.type			EXT_MODE_TYPE
  	2.IP	
  Output:			无
  Return:			无
  Others:			
*************************************************/
void set_netdoor_ip(EXT_MODE_TYPE type, uint32 IP);

#ifdef _USE_ELEVATOR_
/*************************************************
  Function:			ipmodule_elevator_becken
  Description:		移动终端请求与室内机主机电梯召唤
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_elevator_becken(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			elevator_becken_unlock
  Description:		移动终端请求与室内主机开锁被动召唤电梯	
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_elevator_becken_unlock(const PRECIVE_PACKET recPacket);
#endif

/*************************************************
  Function:			ipmodule_get_extensionAddr
  Description:		室内分机向IP模块获取分机的IP地址
  Input: 			
  	1.extensionNo	分机号
  Output:			无
  Return:			分机IP地址4B
  Others:
*************************************************/
int ipmodule_get_extensionAddr(uint32 extensionNo);

/*************************************************
  Function:    		get_ipmodule
  Description:		
  Input: 			无
  Output:			无
  Return:			捆绑的状态
  Others:
*************************************************/
uint32 get_ipmodule(void);

/*************************************************
  Function:			ipmodule_get_room_param
  Description:		获取室内主机的相关参数信息内容
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_get_room_param(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			ipmodule_get_netdoor_list
  Description:		获取室内主机捆绑的网络门前机信息列表
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_get_netdoor_list(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			ipmodule_get_manager_list
  Description:		获取室内主机捆绑的管理中心列表	
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_get_manager_list(const PRECIVE_PACKET recPacket);

/*************************************************
  Function:			get_ipmodule_address
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
uint32 get_ipmodule_address(void);

/*************************************************
  Function:			ipmodule_init
  Description:		IP模块初始化
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void ipmodule_init(void);
#endif


