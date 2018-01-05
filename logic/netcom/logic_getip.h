/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_getip.h
  Author:     	chenbh
  Version:    	2.0
  Date: 
  Description:  获取网络IP信息头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _NET_GET_IP_H_
#define _NET_GET_IP_H_

#if 0
#include "typedef.h"
#include "storage_types.h"
#endif
#include "logic_include.h"
#include "storage_include.h"

/*************************************************
  Function:    		net_set_dns_areacode
  Description:		设置DNS上的小区号
  Input: 
	1.AreaCode		DNS上的小区编号,通过命令获得
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_dns_areacode(uint32 AreaCode);

/*************************************************
  Function:    		net_recive_dns_echo
  Description:		接收到DNS应答
  Input: 
	1.IP			IP地址
	2.ReciveData	接收数据地址
	3.size			接收数据大小
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_recive_dns_echo(uint32 IP, char * ReciveData, int32 size);

/*************************************************
  Function:    		net_is_lan_ip
  Description:		是否局域网IP
  Input: 
	1.ip			ip地址(主机字节顺序)
  Output:			无
  Return:			是否,true/false
  Others:
*************************************************/
int32 net_is_lan_ip(uint32 ip);

/*************************************************
  Function:    		net_get_devices_and_ips
  Description:		查询设备及IP地址列表
  Input: 
	1.AreaCode		本地区号
	2.DeviceType	设备类型
	3.DeviceNo		设备号字符串
  Output:			无
  	1.DevNoList		设备号列表
  	2.IPList		IP地址列表指针
  Return:			个数
  Others:
*************************************************/
int32 net_get_devices_and_ips(uint32 AreaCode, uint8 DeviceType, char * DeviceNo, PDEVICE_NO *DevNoList, uint32 **IPList);

/*************************************************
  Function:    		net_get_ips
  Description:		查询IP地址列表
  Input: 
	1.AreaCode		本地区号
	2.DeviceType	设备类型
	3.DeviceNo		设备号字符串
  Output:			无
  	1.IPList		IP地址列表指针
  Return:			IP地址个数
  Others:
*************************************************/
int32 net_get_ips(uint32 AreaCode, uint8 DeviceType, char * DeviceNo, uint32 **IPList);

#endif

