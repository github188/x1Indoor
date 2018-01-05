/*********************************************************************
  COPYRIGHT (C), 2006-2010, AURINE
  FILE NAME: 		logic_wlmqj_set.c
  AUTHOR:   		xiewr
  VERSION:   		1.0 
  DATE:     		2010-06-04
  DESCRIPTION:		网络门前机设置

  History:        
    1. Date:
       Author:
       Modification:
    2. ...     
*********************************************************************/
#include "logic_wlmqj_set.h"

#define NET_DOOR_LEN      		188
#define ND_WAIT_TIME	  		3
static uint32 g_NetdoorIp = 0;						// 门前机的IP	
static unsigned char g_NetdoorMAC[6] = {0};			// 门前机的MAC
#ifdef _ADD_BLUETOOTH_DOOR_
static char BtDoorIndex = 0;      //no="8":网络门前机1
#endif

/*************************************************
  Function:    		send_netdoor_cmd_echodata
  Description:		往网络门前及发送命令
  					此接口主要用有应答数据的
  					带MAC码,MAC在数据前面
  					应答的数据 指针需由调用者释放
  Input: 
  	1.cmd:			发送的命令
  	2.data: 		数据内容
  	3.datelen:		数据的长度
  Output:			无
	1.RecData		指向接受数据的指针的地址
	2.ReciSize		收到的数据长度
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发送错误:0x030000
  					ECHO_REPEAT:0x03000A/196618
  Others:
*************************************************/
static uint32 send_netdoor_cmd_echodata(NET_COMMAND cmd, unsigned char * SendData,uint32 len, char * RecData, uint32 * ReciSize)
{
	uint8 EchoValue = 0;
	uint32 SendIp = 0;
	uint32 ret = FALSE;
	char data[200] = {0};
	
	SendIp = g_NetdoorIp;
	memcpy(data, g_NetdoorMAC, 6);
	
	if (SendData)
	{
		memcpy(data + 6, SendData, len);
	}

	ret = net_send_command(cmd, data, len + 6, SendIp, NETCMD_UDP_PORT, ND_WAIT_TIME, &EchoValue, RecData, ReciSize);
	log_printf("send_netdoor_cmd_echodata : cmd = 0x%x , SendIp = 0x%x, ret = %d EchoValue = %d, len = %d \n", cmd, SendIp, ret, EchoValue, len + 6);
	//DelayMs_nops(10);
	if (TRUE == ret)
	{
		if (EchoValue == ECHO_OK)
		{
			return NETDOOR_ECHO_SUCESS;
		}
		else
		{
			return NETDOOR_ECHO_COMM | EchoValue;
		}
	}	
	else
	{
		return NETDOOR_ERR_SEND;
	}
}

/*************************************************
  Function:    		send_netdoor_cmd_noecho
  Description:		往网络门前机发送设置命令
  					此接口主要用于无应答数据的发送
  					带MAC,MAC在数据前面
  Input: 
  	1.cmd:			发送的命令
  	2.data: 		数据内容
  	3.datelen:		数据的长度
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
uint32 send_netdoor_cmd_noecho(NET_COMMAND cmd ,unsigned char * SendData, uint32 len)
{
	uint32 ret = send_netdoor_cmd_echodata(cmd, SendData, len, NULL, NULL);
	return ret;
}

/*************************************************
  Function:			logic_set_netdoor_netparam
  Description:		设置网络门前机网络参数
  Input: 				
    1.netparam		网络参数
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发生错误:0x030000
  Others:
*************************************************/
int32 logic_set_netdoor_netparam(PNET_PARAM netparam)
{
	int32 ret;
	uint32 ip;
	int32 len = sizeof(NET_PARAM_SEND);
	char data[250] = {0};
	NET_PARAM_SEND param;

	memset(data, 0, sizeof(data));
	memset(&param, 0, sizeof(NET_PARAM_SEND));

	param.IP = netparam->IP;									
	param.SubNet = netparam->SubNet;								
	param.DefaultGateway = netparam->DefaultGateway;						
	param.DNS1 = netparam->DNS1;									
	param.DNS2 = netparam->DNS2;									
	param.CenterIP = netparam->CenterIP;								
	param.ManagerIP = netparam->ManagerIP;							
	sprintf(param.SipProxyServer, "%s", UlongtoIP(netparam->SipProxyServer));
	sprintf(param.AurineServer, "%s", UlongtoIP(netparam->AurineServer));
	#ifdef _LIVEMEDIA_
	ip = (storage_get_netparam_bytype(RTSP_IPADDR));
	if ( ip == 0)
	{
		ip = g_Rtsp_ServerInfo.ip;
	}
	sprintf(param.StunServer, "%s", UlongtoIP(ip));
	#else
	sprintf(param.StunServer, "%s", UlongtoIP(netparam->StunServer));
	#endif
	param.IP1 = netparam->IP1;          		
	param.SubNet1 =netparam->SubNet1;        		
	
	memcpy(data, &param,  sizeof(NET_PARAM_SEND));
	log_printf("logic_set_netdoor_netparam:\n IP:\t0x%x\nSubNet:\t0x%x\nDefaultGateWay:\t0x%x\n", \
		netparam->IP, netparam->SubNet, netparam->DefaultGateway);
	ret = send_netdoor_cmd_noecho(CMD_GATEWAY_SET_NET_PARAM, data, len);
	if (NETDOOR_ECHO_SUCESS == ret)
	{
		g_NetdoorIp = netparam->IP;
	}
	
	return ret;
}

/*************************************************
  Function:			logic_get_netdevice_netparam
  Description:		获得网络设备网络参数
  Input: 				
  Output:			
  	1.netparam		网络参数
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:			
*************************************************/
int32 logic_get_netdevice_netparam(PNET_PARAM netparam)
{
	char RecData[NET_DOOR_LEN+2] = {0};	
	uint32 ReciSize;
	uint32 ret;
	NET_PARAM_SEND param;
	
	memset(&param, 0, sizeof(NET_PARAM_SEND));	
	ret = send_netdoor_cmd_echodata(CMD_GATEWAY_GET_NET_PARAM, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if (ReciSize == NET_DOOR_LEN)
		{
			if (RecData) 
			{
				memcpy(&param, RecData, sizeof(NET_PARAM_SEND));
				netparam->IP = param.IP ;									
				netparam->SubNet = param.SubNet ;									
				netparam->DefaultGateway = param.DefaultGateway ;									
				netparam->DNS1 = param.DNS1 ;									
				netparam->DNS2 = param.DNS2 ;									
				netparam->CenterIP = param.CenterIP ;			
				netparam->ManagerIP = param.ManagerIP;		
				netparam->SipProxyServer = IPtoUlong(param.SipProxyServer);
				netparam->AurineServer = IPtoUlong(param.AurineServer);
				netparam->StunServer = IPtoUlong(param.StunServer);
				netparam->IP1 = param.IP1;          	
				netparam->SubNet1 = param.SubNet1;
				log_printf("logic_get_netdevice_netparam:\n IP:\t0x%x\nSubNet:\t0x%x\nDefaultGateWay:\t0x%x\n", \
					netparam->IP, netparam->SubNet, netparam->DefaultGateway);
			}
		}
	}

	return ret;
}

/*************************************************
  Function:			logic_set_netdoor_devnoinfo
  Description:		设置网络门前机设备编号以及规则
  Input: 				
    1.devno			设备编号
    2.rule 			编号规则
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_set_netdoor_devnoinfo(PFULL_DEVICE_NO devno, DEVICENO_RULE * rule)
{
	int32 ret;
	int32 devicenolen = sizeof(FULL_DEVICE_NO);
	int32 devicerulllen = sizeof(DEVICENO_RULE);
	int32 datalen = devicenolen + devicerulllen;
	char data[100] = {0};
		
	if (devno == NULL &&  rule == NULL)							// 参数不能都为空
	{
		return NETDOOR_ERR_ECHO;
	}
	else if(devno == NULL)										// 不设置设备编号
	{
		memcpy(data, rule, devicerulllen);
		datalen = devicerulllen;
	}
	else if(rule == NULL)										// 不设置规则
	{
		memcpy(data, devno, devicenolen);
		datalen = devicenolen;
		log_printf("logic_set_netdoor_devnoinfo : devno->devicenostr:%s \n", devno->DeviceNoStr);
	}	
	else														// 同时设置编号和规则
	{
		memcpy(data, devno, devicenolen);
		memcpy(data + devicenolen, rule, devicerulllen);
		datalen = devicenolen + devicerulllen;
		log_printf("logic_set_netdoor_devnoinfo : devno->devicenostr:%s \n", devno->DeviceNoStr);
	}
	
	ret = send_netdoor_cmd_noecho(CMD_GATEWAY_SET_DEV_INFO, data, datalen);
	return ret;

}

/*************************************************
  Function:			logic_get_netdevice_devnoinfo
  Description:		获取网络门前机设备编号以及规则
  Input: 			
  Output:			
    1.devno			设备编号
    2.rule 			编号规则
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_netdevice_devnoinfo(PFULL_DEVICE_NO devno, DEVICENO_RULE * rule)
{
	int32 devicenolen = sizeof(FULL_DEVICE_NO);
	int32 devicerulllen = sizeof(DEVICENO_RULE);
	char RecData[100] = {0};									
	uint32 ReciSize;
	uint32 ret;
	
	ret = send_netdoor_cmd_echodata(CMD_GATEWAY_GET_DEV_INFO, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if (RecData) 
		{
			if (devno)
			{
				memcpy(devno, RecData, devicenolen);
				log_printf("logic_get_netdevice_devnoinfo : devno->devicenostr:%s \n", devno->DeviceNoStr);
			}
			if (rule)
			{
				memcpy(rule, RecData + devicenolen, devicerulllen);
			}
		}	
	}

	return ret;
}

/*************************************************
  Function:			logic_get_netdoor_version
  Description:		获得网络门前机内核和程序版本号
  Input: 			无
  Output:			
    1.KerVer		内核版本号
  	2.BuildTime		编译时间
  	3.ver			程序版本号
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_netdoor_version(FWM_VERSION_T * KerVer, char * BuildTime, char * ver)
{
	uint32 len = sizeof(FWM_VERSION_T);
	char RecData[200] = {0};									
	uint32 ReciSize;
	uint32 ret;
	
	ret = send_netdoor_cmd_echodata(CMD_GATEWAY_GET_VER, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if (RecData)
		{
			if (KerVer)
			{
				memcpy(KerVer, RecData, len);
			}
			
			if (BuildTime)
			{
				memcpy(BuildTime, RecData + len, 80);
			}
		
			if (ver)
			{
				memcpy(ver, RecData + len + 80, ReciSize - len - 80);
			}
			
			log_printf("logic_get_netdoor_version : len = %d ,kervel->u32algver = %d, BuildTime = %s ver = %s \n", len, KerVer->u32AlgVer, BuildTime, ver);
		}
	}
	return ret;
}

/*************************************************
  Function:			logic_mng_netdoor_card
  Description:		管理卡信息
  Input: 	
  	1.OperType		操作类型
    2.CardNo		卡号指针
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001/196609
  					ECHO_MAC:0x030009/196617
  					NETDOOR_ERR_SEND:0x030000/196608		
  Others:
*************************************************/
int32 logic_mng_netdoor_card(uint8 OperType, char * CardNo)
{
	int32 ret = 0;
	unsigned char SendData[4] = {0};
	SendData[0] = OperType;
	if (CardNo == NULL)
	{
		memset(SendData + 1, 0, 3);
	}
	else
	{
		memcpy(SendData + 1, CardNo, 3);
	}
	ret = send_netdoor_cmd_noecho(CMD_NETDOOR_MNG_CARD, SendData, 4);
	log_printf("logic_mng_netdoor_card : ret = %d \n", ret);
	return ret;
}

/*************************************************
  Function:			logic_get_netdevice_lockinfo
  Description:		获得锁信息
  Input: 			无
  Output:			
  	1.LockType		锁类型
  	2.LockTime		锁时长
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001/196609
  					ECHO_MAC:0x030009/196617
  					NETDOOR_ERR_SEND:0x030000/196608		
  Others:
*************************************************/
int32 logic_get_netdevice_lockinfo(uint8 * LockType, int32 * LockTime)
{
	char RecData[100] = {0};									
	uint32 ReciSize;
	uint32 ret;
	
	ret = send_netdoor_cmd_echodata(CMD_NETDOOR_GET_LOCK, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if (ReciSize == 5)
		{
			if (RecData) 
			{
				memcpy(LockType, RecData, 1);
				memcpy(LockTime, RecData + 1, 4);
				log_printf("logic_get_netdevice_lockinfo: LockType = %d , LockTime = %d \n", *LockType, *LockTime);
			}
		}
	}

	return ret;
}

/*************************************************
  Function:			logic_set_netdevice_lockinfo
  Description:		设置锁信息
  Input: 	
  	1.LockType		锁类型
    2.LockTime		锁时长
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001/196609
  					ECHO_MAC:0x030009/196617
  					NETDOOR_ERR_SEND:0x030000/196608		
  Others:
*************************************************/
int32 logic_set_netdevice_lockinfo(uint8 LockType, int32 LockTime)
{
	int32 ret = 0;
	unsigned char SendData[5] = {0};
	SendData[0] = LockType;
	memcpy(SendData + 1, &LockTime, 4);
	log_printf("logic_set_netdevice_lockinfo: LockType = %d , LockTime = %d \n", LockType, LockTime);
	ret = send_netdoor_cmd_noecho(CMD_NETDOOR_SET_LOCK, SendData, 5);
	return ret;
}

/*************************************************
  Function:    		get_netdevice_mac
  Description:		获得指定网络设备的硬件地址
  Input: 			
  	1.ip			设备IP地址
  Output:			
	1.HWaddr		获得的硬件地址
  Return:			成功与否, TRUE/FALSE
  Others:			可获得网关mac和网络门前机mac,根据IP地址的
  					不同
*************************************************/
static uint32 get_netdevice_mac(uint32 IP, unsigned char * HWaddr)
{
	int32 ret = NETDOOR_ERR_ECHO;
	uint8 EchoValue;
	int32 RecSize;
	char RecData[100] = {0};

	if (HWaddr == NULL)
	{
		return NETDOOR_ERR_ECHO;
	}
	if (IP == 0)
	{
		return NETDOOR_ERR_ECHO;
	}

	ret = net_send_command(CMD_GATEWAY_GET_MAC, NULL, 0, IP, NETCMD_UDP_PORT, ND_WAIT_TIME, &EchoValue, RecData, &RecSize);
	if (ret)
	{
		if (!EchoValue)
		{
			memcpy(HWaddr, RecData, 6);
			log_printf("get_netdevice_mac : EchoValue = %d \n netdeviceMAC = %02X-%02X-%02X-%02X-%02X-%02X\n", \
				EchoValue, HWaddr[0], HWaddr[1], HWaddr[2], HWaddr[3], HWaddr[4], HWaddr[5]);
			ret = NETDOOR_ECHO_SUCESS;
		}
		else
		{
			ret = NETDOOR_ERR_ECHO;
		}
	}
	else
	{
		ret = NETDOOR_ERR_ECHO;
	}
	log_printf("get_netdevice_ma ret : %x \n", ret );
	
	return ret;	
}

/*************************************************
  Function:    		get_netdoor_mac
  Description:		获得网络门前机的MAC地址
  Input: 			
    1.index			网络门前机的ID
  Output:			
	1.HWaddr		获得的硬件地址
  Return:			成功与否, TRUE/FALSE
  Others:			在进入门前机设置时首先调用此函数
*************************************************/
uint32 logic_get_netdoor_mac(uint8 index, unsigned char * HWaddr)
{
	uint32 ret = NETDOOR_ERR_ECHO;
	unsigned char mac[6] = {0};
	
	ret = get_netdevice_mac(NETDOOR_FAULT_IP, mac);
	log_printf("logic_get_netdoor_mac : netdoor default ip = 0x%x, ret = %d\n", NETDOOR_FAULT_IP, ret);	
	if (NETDOOR_ECHO_SUCESS == ret)
	{
		if (HWaddr)
		{
			memcpy(HWaddr, mac, 6);
		}
		log_printf("index: %d \n", index);
		g_NetdoorIp = NETDOOR_FAULT_IP;
		memcpy(g_NetdoorMAC, mac, 6);

		// 将MAC保存到文件中
		if (index == 1)
		{
			storage_set_mac(DOOR1_MAC, mac);
			log_printf("storage_set_mac(DOOR1_MAC, mac):%x \n", mac[5]);
		}
		else if (index == 2)
		{
			storage_set_mac(DOOR2_MAC, mac);
			log_printf("storage_set_mac(DOOR2_MAC, mac): %x \n", mac[5]);

		}
		return NETDOOR_ECHO_SUCESS;
	}
	
	return NETDOOR_ERR_ECHO;
}

/*************************************************
  Function:			logic_set_netdoor_other_settings
  Description:		网络设备其他设置
  Input: 				
    1.OtherSettings	设置值 0:不启用,1:启用
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发生错误:0x030000
  Others:			数组大小为10,数组第一个元素为拍照设置的值,
  					第二个元素为监视设置的值,第三个元素为语言
  					第四个元素为视频格式
*************************************************/
uint32 logic_set_netdoor_other_settings(unsigned char * OtherSettings)
{
	int32 ret;
	unsigned char OtherSettingData[10] = {0};

	if (OtherSettings == NULL)
	{
		return NETDOOR_ERR_SEND;
	}
	
	memcpy(OtherSettingData, OtherSettings, sizeof(OtherSettingData));
	ret = send_netdoor_cmd_noecho(CMD_GATEWAY_SET_OTHERS, OtherSettingData, sizeof(OtherSettingData));
	return ret;	
}

/*************************************************
  Function:			logic_get_netdoor_other_settings
  Description:		获得网络设备其他设置
  Input: 				
  Output:			
  	1.OtherSettings	其他设置值
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发生错误:0x030000
  					其他:0x0300**
  Others:			
*************************************************/
uint32 logic_get_netdoor_other_settings(unsigned char * OtherSettings)
{
	char RecData[100] = {0};									
	uint32 ReciSize;
	uint32 ret;
	int32 len = 10;
	
	ret = send_netdoor_cmd_echodata(CMD_GATEWAY_GET_OTHERS, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if (ReciSize == len)
		{
			if (RecData) 
			{
				memcpy(OtherSettings, RecData, len);	
				log_printf("logic_get_netdoor_other_settings : IsShoot = %d, IsMonitored = %d \n", OtherSettings[0], OtherSettings[1]);
			} 
		}
	}

	return ret;	
}

/*************************************************
  Function:    		get_netdoor_ip
  Description:		获得网络门前机IP地址
  Input:			网络门前机的索引
  Output:			
  Return:			FALSE / IP地址
  Others:			
*************************************************/
static uint32 get_netdoor_ip(uint8 index)
{
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint32 LocalAreaCode = dev->AreaNo;
	char DeviceNo[20] = {0};
	uint32 * IPs = NULL;
	int32 count = 0;

	// 获得网关设备号
	strncpy(DeviceNo, dev->DeviceNoStr, dev->DevNoLen - 1);
	if (index == 1)
	{
		DeviceNo[dev->DevNoLen-1] = '8';
	}
	else if(index == 2)
	{
		DeviceNo[dev->DevNoLen-1] = '9';
	}
	else
	{
		return FALSE;
	}
	DeviceNo[dev->DevNoLen] = 0;

	// 获取IP地址
	count = net_get_ips(LocalAreaCode, DEVICE_TYPE_ROOM, DeviceNo, &IPs);
	if (0 == count)
	{
		printf("get_netdoor_ip : cann't netdoor ip!\n");
		return FALSE;														
	}
	else
	{
		log_printf("get_netdoor_ip : ip = %d \n", IPs[0]);
		return IPs[0];
	}
}

/*************************************************
  Function:    		logic_netdoor_pub_ini
  Description:		初始化:获得网络门前机地址和MAC地址
  Input: 			无
  Output:			
  Return:			成功与否, TRUE/FALSE
  Others:			无线终端与网关交互操作之前要调用此函数
*************************************************/
uint32 logic_netdoor_pub_ini(uint8 index)
{
	uint32 ret = 0;
	uint32 NetdoorIp = 0;
	unsigned char NetdoorMac[10] = {0};
	
	NetdoorIp = get_netdoor_ip(index);
	if (index == 1)
	{
		memcpy(NetdoorMac, storage_get_mac(DOOR1_MAC), 6);
	}
	else
	{
		memcpy(NetdoorMac, storage_get_mac(DOOR2_MAC), 6);
	}
	g_NetdoorIp = NetdoorIp;
	memcpy(g_NetdoorMAC, NetdoorMac, 6);
	
	if (!NetdoorIp)
	{
		log_printf("logic_netdoor_pub_ini : can not get NetdoorIp  !!\n");
		return NETDOOR_ERR_ECHO;
	}
	return NETDOOR_ECHO_SUCESS;	
}

#ifdef _ADD_BLUETOOTH_DOOR_

/*************************************************
  Function:    		logic_netdoor_pub_ini
  Description:		初始化:获得网络门前机地址和MAC地址
  Input: 			无
  Output:			
  Return:			成功与否, TRUE/FALSE
  Others:			无线终端与网关交互操作之前要调用此函数
*************************************************/
uint32 logic_bluetoothdoor_pub_ini(uint8 index, uint32 NetdoorIp)
{
	uint32 ret = 0;
	unsigned char NetdoorMac[10] = {0};
	
	if (index == 1)
	{
		memcpy(NetdoorMac, storage_get_mac(DOOR1_MAC), 6);
	}
	else
	{
		memcpy(NetdoorMac, storage_get_mac(DOOR2_MAC), 6);
	}
	g_NetdoorIp = NetdoorIp;
	memcpy(g_NetdoorMAC, NetdoorMac, 6);
	
	if (!NetdoorIp)
	{
		log_printf("logic_netdoor_pub_ini : can not get NetdoorIp  !!\n");
		return NETDOOR_ERR_ECHO;
	}
	return NETDOOR_ECHO_SUCESS;	
}

/*************************************************
  Function:			logic_get_bluetooth_door_sysinfo
  Description:		获取蓝牙门前系统信息
  Input: 	
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_bluetooth_door_sysinfo(char * data)
{
	char RecData[200] = {0};									
	uint32 ReciSize;
	uint32 ret;
	uint32 len = 96;
	
	ret = send_netdoor_cmd_echodata(CMD_NETDOOR_SYSINFO, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if ((RecData) && (len == ReciSize))
		{
			if (data)
			{
				memcpy(data, RecData, len);
			}
		}
	}

	return ret;
}

/*************************************************
  Function:			logic_get_bluetooth_door_sysinfo
  Description:		获取蓝牙门前绑定室内列表
  Input: 	
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_bluetooth_door_room_list(PNET_BING_ROOM_LIST *prec_data)
{
	char RecData[NET_PACKBUF_SIZE] = {0};									
	uint32 ReciSize;
	uint32 ret;

	*prec_data = (PNET_BING_ROOM_LIST)malloc(sizeof(NET_BING_ROOM_LIST));
	if(*prec_data)
	{
		(*prec_data)->counts = 0;
		(*prec_data)->info = NULL;
	}
	
	ret = send_netdoor_cmd_echodata(CMD_NETDOOR_ROOMLIST, NULL, 0, RecData, &ReciSize);
	if (!ret)
	{
		if (RecData)
		{			
			(*prec_data)->counts = *(uint32 *)RecData;
			(*prec_data)->info = (PNET_BING_ROOM)malloc(sizeof(NET_BING_ROOM) * (*prec_data)->counts);
			if ((*prec_data)->info && ((sizeof(NET_BING_ROOM) * (*prec_data)->counts) == (ReciSize - 4)))
			{
				memset((*prec_data)->info, 0, sizeof(NET_BING_ROOM) * (*prec_data)->counts);
				memcpy((*prec_data)->info, RecData + 4, sizeof(NET_BING_ROOM) * (*prec_data)->counts);
			}
		}
	}

	return ret;
}

/*************************************************
  Function:			BledoorSetTime_proc
  Description:		设置蓝牙门前时间线程
  Input:
  Output:			无
  Return:			成功或失败
  Others:
*************************************************/
static void* BledoorSetTime_proc(void *param)
{
	uint8 i = 0;
	ZONE_DATE_TIME LocalDateTime = {0};
	DATE_TIME SendTime = {0};
	uint16 bindstatus = 0;
	uint32 nd_ip = 0;
	
	// 设置分离线程
	pthread_detach(pthread_self());

	for(i=0; i<2; i++)
	{
		if (TRUE == storage_get_extmode(EXT_MODE_BLUETOOTHDOOR1 + i))
		{
			nd_ip = get_netdoor_ip(i + 1);

			get_timer(&LocalDateTime);
			get_utc_time(&LocalDateTime, 8);

			SendTime.year = LocalDateTime.year;
			SendTime.month = LocalDateTime.month;
			SendTime.day = LocalDateTime.day;
			SendTime.hour = LocalDateTime.hour;
			SendTime.min = LocalDateTime.min;
			SendTime.sec = LocalDateTime.sec;
			SendTime.week = LocalDateTime.week;
	
			net_direct_send(CMD_SYN_TIME, &SendTime, sizeof(DATE_TIME), nd_ip, NETCMD_UDP_PORT);	
		}
	}	

	pthread_exit(NULL);
	return NULL;             // 返回后资源由系统释放
}

/*************************************************
  Function:			logic_set_bluetooth_door_time
  Description:		设置蓝牙门前时间
  Input: 	
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_set_bluetooth_door_time()
{
	struct ThreadInfo BledoorSetTime;
	if (inter_start_thread(&BledoorSetTime, BledoorSetTime_proc, NULL, 0) != 0)
	{
		return FALSE;
	}	

	return TRUE;
}

/*************************************************
  Function:			logic_get_bluetooth_door_record
  Description:		查看门前最近24小时记录和历史记录
  Input: 	
    1.cmd			命令
    2.index			获取第几台蓝牙门前记录 1:蓝牙门前1;   2:蓝牙门前2
    3.type 			记录类型
    4.start			开始记录标志
    5.num			获取记录条数
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_bluetooth_door_record(NET_COMMAND cmd, uint8 index, uint8 type, uint32 start, uint32 num, PNET_RECORD_LIST *prec_data)
{
	char data[100] = {0};
	uint32 ret = 0;								
	uint32 ReciSize;
	char RecData[NET_PACKBUF_SIZE] = {0};

	g_NetdoorIp = get_netdoor_ip(index);
	if (index == 1)
	{
		memcpy(g_NetdoorMAC, storage_get_mac(DOOR1_MAC), 6);
	}
	else
	{
		memcpy(g_NetdoorMAC, storage_get_mac(DOOR2_MAC), 6);
	}

	memset(data, 0, sizeof(data));
	data[0] = type;
	memcpy(data+1, &start, 4);
	memcpy(data+5, &num, 4);

	ret = send_netdoor_cmd_echodata(cmd, data, 9, RecData, &ReciSize);
	if (!ret)
	{
		if (4+48*num >= ReciSize)
		{
			if (RecData) 
			{
				*prec_data = (PNET_RECORD_LIST)malloc(sizeof(NET_RECORD_LIST));
				if(*prec_data)
				{
					(*prec_data)->counts = 0;
					(*prec_data)->info = NULL;
				}
				
				(*prec_data)->counts = *(uint32 *)RecData;
				(*prec_data)->info = (PNET_RECORD_INFO)malloc(sizeof(NET_RECORD_INFO) * (*prec_data)->counts);
				if (((*prec_data)->info) && ((sizeof(NET_RECORD_INFO) * (*prec_data)->counts) == (ReciSize - 4)))
				{
					memset((*prec_data)->info, 0, sizeof(NET_RECORD_INFO) * (*prec_data)->counts);
					memcpy((*prec_data)->info, RecData + 4, sizeof(NET_RECORD_INFO) * (*prec_data)->counts);
				}
			}
		}
	}

	return ret;
}

/*************************************************
  Function:			save_bluetoothdoor_pic
  Description: 		保存图片
  Input: 			
  	1.ID			会话ID
  	2.data			数据
  	3.size			大小
  Output:			无
  Return:			无
*************************************************/
static void save_bluetoothdoor_pic(uint32 ID, int8 * data, int size)
{	
	if (NULL == data || 0 == size)
	{
		return;
	}
	
	storage_write_bledoor_pic(ID, (char*)data, size, BtDoorIndex);
	//show_sys_event_hint(AS_NEW_MSG);
	//sys_sync_hint_state();
}

/*************************************************
  Function:			update_msg_recv_state
  Description: 		更新信息的状态
  Input: 			
  	1.ID			会话ID
  	2.Total			总包数
  	3.CurNum		当前包数
  	4.size			当前接收数据大小
  Output:			无
  Return:			无
*************************************************/
static void* update_bluetoothdoor_recv_state(uint32 ID, int Total, int CurNum, int size)
{	
	if (Total == CurNum)
	{
		//set_public_state(PUB_STATE_XINXI, 1);
	}
}

/*************************************************
  Function:			recv_bluetoothdoor_pic_start
  Description:		接收蓝牙门前抓拍图片开始(采用多包接收)
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
void recv_bluetoothdoor_pic_start(const PRECIVE_PACKET recPacket)
{
	uint32 ID = 0;									// 会话ID
	char * data = (char*)&ID;						// 会话ID
	uint32 remoteDevno2 = 0;

	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	BtDoorIndex = -1;
	#ifdef _AU_PROTOCOL_
	remoteDevno2 = head->DeviceNo.DeviceNo2 % 10;
	#else
	remoteDevno2 = head->SrcDeviceNo.DeviceNo2 % 10;
	#endif
	BtDoorIndex = (remoteDevno2 == 8) ? 1 : 2;	
	
	ID = net_start_multipacket_recive(recPacket->address, recPacket->port, 0, save_bluetoothdoor_pic, update_bluetoothdoor_recv_state);
	if (ID == 0)
	{
		log_printf("net_start_multipacket_recive = 0\n");
		net_send_echo_packet(recPacket, ECHO_BUSY, data, 4);	
		return;
	} 
	log_printf("net_start_multipacket_recive = id:%d\n", ID);
	net_send_echo_packet(recPacket, ECHO_OK, data, 4);
	return;
}

#endif

