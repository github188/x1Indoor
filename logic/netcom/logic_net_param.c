/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_net_param.c
  Author:    	xiewr
  Version:   	1.0
  Date: 
  Description:  获取IP地址信息
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include <netinet/in.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/sockios.h> 
#include <linux/types.h>
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned long long	u64;	
#include  <linux/sockios.h>
#include  <linux/ethtool.h>

#include "logic_net_comm.h"

static PNET_PARAM g_NetParam;						// 网络参数

/*************************************************
  Function:    		IPtoUlong
  Description:		将IP地址字符串转化为long型IP地址
  Input:
  	1.ip			IP地址字符串
  Output:			
  Return:			转化后的unsigned long型IP地址
  Others:			
*************************************************/
uint32 IPtoUlong(char* ip)
{
	uint32 uIp=0;
	char *p, *q;
	char temp[10];
	int32 num;
	p = ip;
	for( ; ; )
	{
		q = (char *)strchr(p,'.');
		if (q == NULL)
		{
			break;
		}
		num = q - p;
		memset(temp, 0, 10);
		memcpy(temp, p, num);
		uIp = uIp << 8;
		uIp += atoi(temp);
		p=q+1;
	}
	uIp = uIp << 8;
	uIp += atoi(p);
	return uIp;
}

/*************************************************
  Function:    		UlongtoIP
  Description:		将long型IP地址转化为IP地址字符串
  Input:
  	1.uIp			unsigned long型IP地址
  Output:			
  Return:			转化后的字符串指针
  Others:			
*************************************************/
char* UlongtoIP(uint32 uIp)
{
	static char strIp[20];
	char temp[20];
	int32 i;
	uint32 t;
	memset(strIp, 0, 20);
	for(i = 3; i >= 0; i--)
	{
		t = uIp >> (i * 8);
		t = t & 0xFF;
		sprintf(temp, "%d", (uint32)t);
		strcat(strIp, temp);
		if(i != 0)
		{
			strcat(strIp, ".");
		}
	}
	return strIp;
}

/*************************************************
  Function:    		net_set_net_param
  Description:		设置通讯网络参数
  Input: 			
  	1.NetParam		网络参数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_net_param(PNET_PARAM NetParam)
{
	g_NetParam = NetParam;
}

/*************************************************
  Function:    		net_set_local_param
  Description:		设置本地网络参数
  Input: 			
  	1.NetParam		网络参数指针
  Output:			无
  Return:			无
  Others:
*************************************************/
void net_set_local_param(PNET_PARAM NetParam)
{
	log_printf("net_set_local_param : ipaddr:0x%x, netmask:0x%x, gateway:0x%x\n", NetParam->IP, NetParam->SubNet, NetParam->DefaultGateway);
	net_set_net_param(NetParam);
	storage_set_ipaddr();
}

/*************************************************
  Function:    		net_set_local_mac
  Description:		设置MAC
  Input: 			
  	1.mac			MAC
  Output:			无
  Return:			无
  Others:			TRUE / FALSE
*************************************************/
uint32 net_set_local_mac(uint8 *mac)
{	
	uint32 ret = FALSE;

	// 存储设置MAC
	ret = storage_set_mac(HOUSE_MAC, (char *)mac);
	if (TRUE == ret)
	{
		storage_set_ipaddr();
	}
	return ret;
}

/*************************************************
  Function:    		net_get_local_ipaddress
  Description:		获得IP地址
  Input: 			无
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
uint32 net_get_local_ipaddress(void)
{
	return g_NetParam->IP;
}

/*************************************************
  Function:    		net_get_netmask_address
  Description:		获得掩码地址
  Input: 			无
  Output:			无
  Return:			掩码地址
  Others:
*************************************************/
uint32 net_get_netmask_address(void)
{
	return g_NetParam->SubNet;
}

/*************************************************
  Function:    		net_get_local_mac
  Description:		获得网卡硬件地址
  Input: 
  Output:			无
	1.HWaddr		获得的硬件地址
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int32 net_get_local_mac(unsigned char *HWaddr)
{
	uint8 NetMac[6] = {0};
	if (HWaddr == NULL)
	{
		return FALSE;
	}
	memcpy(HWaddr, NetMac, sizeof(NetMac));
	return TRUE;
}

#if 1
/*************************************************
  Function:    		net_get_net_status
  Description:		获得指定网络设备的物理网络状态
  Input: 
	1.ifname		设备名称, ifname=null获取eth0状态
  Output:			无
  Return:			0－连接成功 1－连接断开 2－未知
  Others:
*************************************************/
int net_get_net_status(char * ifname)
{
	struct ifreq ifr;
	struct ethtool_value edata;
	
	memset(&ifr, 0, sizeof(ifr));
	if (ifname == NULL)
	{
		strcpy(ifr.ifr_name, "eth0");
	}
	else
	{
		strcpy(ifr.ifr_name, ifname);
	}
	
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0)
	{
		return -1;
	}
	
	edata.cmd = 0x0000000a;
	ifr.ifr_data = (caddr_t)&edata;
	int retval = 2;
	if (ioctl(skfd, SIOCETHTOOL, &ifr) >= 0)
	{
		retval = (edata.data ? 0 : 1);
	}
	close(skfd);
	
	return retval;
}
#else
/*************************************************
  Function:    		net_get_net_status
  Description:		获得指定网络设备的物理网络状态
  Input: 
	1.ifname		设备名称, 如eth0, ifname=null 获得任意地址
  Output:			无
  Return:			0－连接成功 1－连接断开 2－未知
  Others:
*************************************************/
int net_get_net_status(char *ifname)
{
	int retval;
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0)
		return -1;
	struct ifreq ifr;
	struct ethtool_value edata;
	memset(&ifr, 0, sizeof(ifr));
	edata.cmd = ETHTOOL_GLINK;
	if (ifname==NULL)
		strcpy(ifr.ifr_name, "eth0");
	else
		strcpy(ifr.ifr_name, ifname);
	ifr.ifr_data = (char *) &edata;
	if (ioctl(skfd, SIOCETHTOOL, &ifr) < 0)
	{
		retval = 2;
		uint16 *data, mii_val;
		unsigned phy_id;
		if (ioctl(skfd, SIOCGMIIPHY, &ifr) >= 0)
		{
			data = (uint16 *)(&ifr.ifr_data);
			phy_id = data[0];
			data[1] = 1;
			if (ioctl(skfd, SIOCGMIIREG, &ifr) >= 0)
			{
				mii_val = data[3];
				retval = (((mii_val & 0x0016) == 0x0004) ? 0 : 1);
			}
		}
	}
	else
		retval = (edata.data ? 0 : 1);
	close(skfd);
	#if 0
	if (retval == 2)
		printf("Could not determine status \n");
	if (retval == 1)
		printf("Link down \n");
	if (retval == 0)
		printf("Link up \n");
	#endif		
	return retval;
}
#endif

/*************************************************
  Function:    		net_get_center_ip
  Description:		获得中心服务器IP
  Input: 			无
  Output:			无
  Return:			中心服务器IP地址
  Others:
*************************************************/
uint32 net_get_center_ip(void)
{
	return g_NetParam->CenterIP;
}

/*************************************************
  Function:    		net_get_manager_ip
  Description:		获得管理员机IP
  Input: 			
  	1.NO			管理员机序号 1-3
  Output:			无
  Return:			理员机IP
  Others:
*************************************************/
uint32 net_get_manager_ip(uint8 NO)
{
	switch (NO)
	{
		case 1:
			return g_NetParam->ManagerIP;
		case 2:
			return g_NetParam->ManagerIP1;
		case 3:
			return g_NetParam->ManagerIP2;
		default:
			return 0;
	}
}

/*************************************************
  Function:    		net_get_aurine_ip
  Description:		获得冠林服务器IP
  Input: 			无
  Output:			无
  Return:			冠林IP
  Others:
*************************************************/
uint32 net_get_aurine_ip(void)
{
	//return IPtoUlong(g_NetParam->AurineServer);
	return (g_NetParam->AurineServer);
}

/*************************************************
  Function:    		net_get_aurine_stun_ip
  Description:		获得冠林StunIP
  Input: 			无
  Output:			无
  Return:			冠林Stun服务器IP
  Others:
*************************************************/
uint32 net_get_aurine_stun_ip(void)
{
	//return IPtoUlong(g_NetParam->StunServer);
	return (g_NetParam->StunServer);
}

/*************************************************
  Function:    		net_get_sip_proxy_ip
  Description:		获得sip代理服务器IP
  Input: 			无
  Output:			无
  Return:			sip代理服务器IP
  Others:
*************************************************/
uint32 net_get_sip_proxy_ip(void)
{
	//return IPtoUlong(g_NetParam->SipProxyServer);
	return (g_NetParam->SipProxyServer);
}

