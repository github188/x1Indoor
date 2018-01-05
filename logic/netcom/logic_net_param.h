/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:  	logic_net_param.h
  Author:     	xiewr
  Version:    	1.0
  Date: 
  Description:  获取本地IP信息头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _NET_GET_PARAM_H_
#define _NET_GET_PARAM_H_

#if 0
#include "typedef.h"
#include "net_comm_types.h"
#include "storage_types.h"
#endif
#include "logic_include.h"

/*************************************************
  Function:    		net_set_net_param
  Description:		设置通讯网络参数
  Input: 			
  	1.NetParam		网络参数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_net_param(PNET_PARAM NetParam);

/*************************************************
  Function:    		net_set_local_param
  Description:		设置本地网络参数
  Input: 			
  	1.NetParam		网络参数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_local_param(PNET_PARAM NetParam);

/*************************************************
  Function:    		net_set_local_mac
  Description:		设置MAC
  Input: 			
  	1.mac			MAC
  Output:			无
  Return:			无
  Others:			TRUE / FALSE
*************************************************/
uint32 net_set_local_mac(uint8 *mac);

/*************************************************
  Function:    		net_get_local_ipaddress
  Description:		获得IP地址
  Input: 			无
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
uint32 net_get_local_ipaddress(void);

/*************************************************
  Function:    		net_get_netmask_address
  Description:		获得掩码地址
  Input: 			无
  Output:			无
  Return:			掩码地址
  Others:
*************************************************/
uint32 net_get_netmask_address(void);

/*************************************************
  Function:    		net_get_local_mac
  Description:		获得网卡硬件地址
  Input: 
  Output:			无
	1.HWaddr		获得的硬件地址
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int32 net_get_local_mac(unsigned char *HWaddr);

/*************************************************
  Function:    		net_get_net_status
  Description:		获得指定网络设备的物理网络状态
  Input: 
	1.ifname		设备名称, ifname=null获取eth0状态
  Output:			无
  Return:			0－连接成功 1－连接断开 2－未知
  Others:
*************************************************/
int net_get_net_status(char * ifname);

/*************************************************
  Function:    		net_get_center_ip
  Description:		获得中心服务器IP
  Input: 			无
  Output:			无
  Return:			中心服务器IP地址
  Others:
*************************************************/
uint32 net_get_center_ip(void);

/*************************************************
  Function:    		net_get_manager1_ip
  Description:		获得管理员机IP
  Input: 			
  	1.NO			管理员机序号 1-3
  Output:			无
  Return:			理员机IP
  Others:
*************************************************/
uint32 net_get_manager_ip(uint8 NO);

/*************************************************
  Function:    		net_get_aurine_ip
  Description:		获得冠林服务器IP
  Input: 			无
  Output:			无
  Return:			冠林IP
  Others:
*************************************************/
uint32 net_get_aurine_ip(void);

/*************************************************
  Function:    		net_get_aurine_stun_ip
  Description:		获得冠林StunIP
  Input: 			无
  Output:			无
  Return:			冠林Stun服务器IP
  Others:
*************************************************/
uint32 net_get_aurine_stun_ip(void);

/*************************************************
  Function:    		net_get_sip_proxy_ip
  Description:		获得sip代理服务器IP
  Input: 			无
  Output:			无
  Return:			sip代理服务器IP
  Others:
*************************************************/
uint32 net_get_sip_proxy_ip(void);

/*************************************************
  Function:    		IPtoUlong
  Description:		将IP地址字符串转化为long型IP地址
  Input:
  	1.ip			IP地址字符串
  Output:			
  Return:			转化后的unsigned long型IP地址
  Others:			
*************************************************/
uint32 IPtoUlong(char* ip);

#endif

