/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_getip.c
  Author:    	chenbh
  Version:   	2.0
  Date: 		2014-12-11
  Description:  获取IP地址信息
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "logic_getip.h"

#define ADDR_COUNT					100
#define USED_QUERYIP_FROM_DNS		1

static uint32		g_AreaCode = 0;					// 当前DNS服务器的小区号
static uint32 		g_AddressList[ADDR_COUNT]; 		// 地址列表
static DEVICE_NO	g_DevNoList[ADDR_COUNT]; 		// 设备号列表
static uint8 		g_EchoCount = 0;				// 应答的地址数量 ,搜索室内分机时用到
static uint8 		g_IsWaitEcho = FALSE;			// 是否等待应答

/*************************************************
  Function:    		get_ip_by_broadcast
  Description:		根据设备号广播获得IP地址
  Input: 
  	1.DeviceType	设备类型
	2.DeviceNo		设备编号
  Output:			无
  Return:			IP地址, 主机字节顺序，为0时为无返回值
  Others:
*************************************************/
static int get_ips_by_broadcast(uint8 DeviceType, char * DeviceNo)
{
	char RecData[30] = {0};
	int32 len = strlen(DeviceNo);
	char SendData[50]={0};
	uint8 EchoValue;
	int32 ReciSize;
	int32 IsMatch = FALSE; 				//是否含通配符
	int32 ret = FALSE;
	int32 n = 30;

	SendData[0] = DeviceType;
	memcpy(SendData+1, DeviceNo, len); 
	
	if (len==2)
	{
		if (DeviceNo[0]=='_' || DeviceNo[1]=='_')
		{
			IsMatch = TRUE;
		}	
	}
	else
	{
		if (DeviceNo[len-1]=='_' || DeviceNo[len-1]=='*')
		{
			IsMatch = TRUE;
		}	
	}

	// 广播发送, ip地址为广播地址
	g_EchoCount = 0;
	if (IsMatch)									// 有通配符的不等应答
	{
		g_IsWaitEcho = TRUE;
		set_nethead(G_BROADCAST_DEVNO, PRIRY_DEFAULT);
		net_direct_send(CMD_DNS_GETIP, SendData, len+1, INADDR_BROADCAST, NETCMD_UDP_PORT);
		
		// 延时30ms
		while (n)
		{
			DelayMs_nops(50);
			n--;
		}
		g_IsWaitEcho = FALSE;
	}
	else											// 无通配符的等应答
	{
		set_nethead(G_BROADCAST_DEVNO, PRIRY_DEFAULT);
		ret = net_send_command(CMD_DNS_GETIP, SendData, len+1, INADDR_BROADCAST, NETCMD_UDP_PORT, 3, &EchoValue, RecData, &ReciSize);
		if (TRUE == ret)
		{
			if (EchoValue == ECHO_OK)
			{
				memcpy(&g_DevNoList[0], RecData, sizeof(DEVICE_NO));
				memcpy(&g_AddressList[0], RecData+sizeof(DEVICE_NO), 4);
				g_EchoCount = 1;
			}
			//free(RecData);
		}
	}
	return g_EchoCount;
}

/*************************************************
  Function:    		net_set_dns_areacode
  Description:		设置DNS上的小区号
  Input: 
	1.AreaCode		DNS上的小区编号,通过命令获得
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_dns_areacode(uint32 AreaCode)
{
	g_AreaCode = AreaCode;
}

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
void net_recive_dns_echo(uint32 IP, char * ReciveData, int32 size)
{
	log_printf("IP:0x%x, size:%d\n", IP, size);
	int32 i = 0;
	if (size != sizeof(DEVICE_NO)+4)
	{
		return;
	}
	if (!g_IsWaitEcho || g_EchoCount >= ADDR_COUNT)
	{
		return;
	}

	// add by chenbh   加入重复IP 应答多次的判断 
	// ip模块应答后，如果移动设备再应答就会出错
	for (i = 0; i < g_EchoCount; i++)
	{
		if (g_AddressList[i] == (uint32)(ReciveData + sizeof(DEVICE_NO)))
		{
			log_printf(" echo ip have in list yet \n");
			return;
		}
	}
	
	memcpy(&g_DevNoList[g_EchoCount], ReciveData, sizeof(DEVICE_NO));
	memcpy(&g_AddressList[g_EchoCount], ReciveData+sizeof(DEVICE_NO), 4);
	if (g_AddressList[g_EchoCount] == 0)
	{
		g_AddressList[g_EchoCount] = IP;
		log_printf("g_EchoCount:%d, IP:%x\n", g_EchoCount, g_AddressList[g_EchoCount]);
	}
	g_EchoCount++;	
}

/*************************************************
  Function:    		net_is_lan_ip
  Description:		是否局域网IP
  Input: 
	1.ip			ip地址(主机字节顺序)
  Output:			无
  Return:			是否,true/false
  Others:
*************************************************/
int32 net_is_lan_ip(uint32 ip)
{
	/*
	a类网
	 10.0.0.0~10.255.255.255		1
	0x0A000000~0x0AFFFFFF
	b类网
	 172.16.0.0~172.31.255.255		16
	0xAC100000~0xAC1FFFFF
	c类网
	 192.168.0.0~192.168.255.255	255
	0xC0A80000~0xC0A8FFFF
	*/
	if (ip>=0x0A000000 && ip<0x0AFFFFFF)
	{
		return TRUE;
	}	
	if (ip>=0xAC100000 && ip<0xAC1FFFFF)
	{
		return TRUE;
	}	
	if (ip>=0xC0A80000 && ip<0xC0A8FFFF)
	{
		return TRUE;
	}	
	
	return FALSE;
}

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
int32 net_get_devices_and_ips(uint32 AreaCode, uint8 DeviceType, char * DeviceNo, PDEVICE_NO *DevNoList, uint32 **IPList)
{
	// 本地设备判断
	#if 0 	
	if (AreaCode!=g_AreaCode)
		return 0;
	#endif	
	
	int32 count = 0;
	int32 ret = FALSE; 

#if	USED_QUERYIP_FROM_DNS		
	// 先向中心服务器查询
	int32 len = strlen(DeviceNo);
	char SendData[50];
	uint8 EchoValue;
	char RecData[200];
	int32 ReciSize;
	uint32 ServerIP;
	int32 i = 0;
	
	SendData[0] = DeviceType;
	memcpy(SendData+1, DeviceNo, len); 
	ServerIP = net_get_center_ip();
	set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
	ret = net_send_command(CMD_SERVER_GETIP, SendData, len+1, ServerIP, NETCMD_UDP_PORT, 1, &EchoValue, RecData, &ReciSize);
	if (TRUE == ret)
	{
		if (EchoValue==ECHO_OK && ReciSize>=4)
		{
			memcpy(&count, RecData, 4);
			if (count>0 && ((sizeof(DEVICE_NO) + 4)*count+4) == ReciSize)
			{
				//数量4B＋设备编号＋IP地址4B 此处如果加上端口号则需要加上4B
				for (i=0; i<count; i++)
				{
					memcpy(&g_DevNoList[i], RecData+4+i*(sizeof(DEVICE_NO) + 4), sizeof(DEVICE_NO));
					memcpy(&g_AddressList[i], RecData+(i+1)*(sizeof(DEVICE_NO)+4), 4);
				}
			}
			else 
			{
				ret	= FALSE;
				count = 0;
			}
		}
		else 
		{
			ret	= FALSE;
		}	
	}
#endif	

	// 中心服务器查询不到再广播发送
	if (!ret)
	{
		count = get_ips_by_broadcast(DeviceType, DeviceNo);
	}
	
	if (count)
	{
		if (IPList)
		{
			*IPList = g_AddressList;
		}
		if (DevNoList)
		{
			*DevNoList = g_DevNoList;
		}
	}
	else
	{
		IPList = NULL;
	}
	
	log_printf("count:%d\n", count);
	return count;
}

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
int32 net_get_ips(uint32 AreaCode, uint8 DeviceType, char * DeviceNo, uint32 **IPList)
{
	return net_get_devices_and_ips(AreaCode, DeviceType, DeviceNo, NULL, IPList);
}

