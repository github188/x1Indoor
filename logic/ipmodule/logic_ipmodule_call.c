/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_jiadian_call.c
  Author:   	txl
  Version:   	2.0
  Date: 		09.5.21
  Description:  家电函数
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#include "logic_ipmodule.h"

#ifdef _IP_MODULE_DJ_

//static NET_DOOR_PARAM g_doorparam;
//static uint8 g_NetDoor = 0;

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
void set_netdoor_ip(EXT_MODE_TYPE type, uint32 IP)
{
	#if 0
	if (EXT_MODE_NETDOOR1 == type)
	{
		g_doorparam.DoorIP1 = IP;
	}
	else
	{
		g_doorparam.DoorIP2 = IP;
	}
	#endif
}

/*************************************************
  Function:			get_door_ip
  Description:		获取网络门前机IP1 
  Input: 	
  	1.type			EXT_MODE_TYPE
  Output:			无
  Return:			无
  Others:			
*************************************************/
uint32 get_netdoor_ip(EXT_MODE_TYPE type)
{
	#if 0
	uint32 ipaddr;
	if (0 == g_NetDoor)
	{
		g_NetDoor = 1;
		ipaddr = get_door_address(1);
		set_netdoor_ip(EXT_MODE_NETDOOR1, ipaddr);
		
		ipaddr = get_door_address(2);
		set_netdoor_ip(EXT_MODE_NETDOOR2, ipaddr);
	}
	
	if (EXT_MODE_NETDOOR1 == type)
	{
		return g_doorparam.DoorIP1;
	}
	else
	{
		return g_doorparam.DoorIP2;
	}
	#else
	if (EXT_MODE_NETDOOR1 == type)
	{
		return storage_get_netdoor_ip(0);
	}
	else
	{
		return storage_get_netdoor_ip(1);
	}
	#endif
}

/*************************************************
  Function:			ipmodule_get_room_param
  Description:		获取室内主机的相关参数信息内容
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_get_room_param(const PRECIVE_PACKET recPacket)
{
	char des[100] = {0};
	char strsub[10];
	char data[200] = {0};
	int dataLen = 0;
	int sublen;
	int devnoLen;
	
	//通用梯口机启用状态
	int isused = storage_get_extmode(EXT_MODE_GENERAL_STAIR);	

	//中心服务器
	uint32 centerIP = storage_get_netparam_bytype(CENTER_IPADDR);

	//流媒体服务器
	uint32 RtspIP = storage_get_netparam_bytype(RTSP_IPADDR);
	
	//设备编号
	FULL_DEVICE_NO *pFullDevno = storage_get_devparam();

	log_printf("ipmodule_get_room_param\n");
	if (RtspIP  == 0)
	{
		RtspIP = g_Rtsp_ServerInfo.ip;
	}
	
	if ( NULL == pFullDevno )
	{
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}
	devnoLen = sizeof(FULL_DEVICE_NO);

	
    sprintf(strsub, "%d", pFullDevno->Rule.Subsection);
	sublen = strlen(strsub);

	memset(des, 0, sizeof(des));
	memcpy(des, storage_get_dev_desc(), 70);		// 分段描述符
	
	//组合发送数据
	memcpy(data, &isused, 4);
	memcpy(data+4, &centerIP, 4);
	memcpy(data+8, &RtspIP, 4);
	memcpy(data+12, pFullDevno, devnoLen);
	memcpy(data+12+devnoLen, des, sublen*10);
	dataLen = 12 + devnoLen + sublen*10;
	net_send_echo_packet(recPacket, ECHO_OK, data, dataLen);
}

/*************************************************
  Function:			ipmodule_get_netdoor_list
  Description:		获取室内主机捆绑的网络门前机信息列表
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_get_netdoor_list(const PRECIVE_PACKET recPacket)
{
	char data[50] = {0};
	int datalen = 0;
	int count = 0;

	int index = 0;
	uint32 netdoorIP = 0;
	uint8 mac[10] = {0};

	log_printf("ipmodule_get_netdoor_list\n");
	
	//网络门前机一
	if (storage_get_extmode(EXT_MODE_NETDOOR1))
	{
		memset(mac, 0, sizeof(mac));
		netdoorIP = 0;
		index = 0;

		memcpy(mac, storage_get_mac(DOOR1_MAC), 6);
		netdoorIP = get_netdoor_ip(EXT_MODE_NETDOOR1);
		if (netdoorIP > 0)
		{
			count = 1;

			memcpy(data+4, &index, 4);
			memcpy(data+8, &netdoorIP, 4);
			memcpy(data+12, mac, 6);
			datalen = 18;		
			log_printf(" netdoor1: index=%d,ip=0x%x, mac=%d-%d-%d-%d-%d-%d\n", 
							index, netdoorIP, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);		
		}
	}
	
	//网络门前机二
	if (storage_get_extmode(EXT_MODE_NETDOOR2))
	{
		memset(mac, 0, sizeof(mac));
		netdoorIP = 0;
		index = 1;

		memcpy(mac, storage_get_mac(DOOR2_MAC), 6);
		netdoorIP = get_netdoor_ip(EXT_MODE_NETDOOR2);
		if (netdoorIP > 0)
		{
			count++;

			memcpy(data+datalen, &index, 4);
			memcpy(data+datalen+4, &netdoorIP, 4);
			memcpy(data+datalen+8, mac, 6);
			datalen += 14;		

			log_printf(" netdoor2: index=%d,ip=0x%x, mac=%d-%d-%d-%d-%d-%d\n", 
							index, netdoorIP, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);	
		}
	}
	
	memcpy(data, &count, 4);
	net_send_echo_packet(recPacket, ECHO_OK, data, datalen);
}

/*************************************************
  Function:			ipmodule_get_manager_list
  Description:		获取室内主机捆绑的管理中心列表	
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_get_manager_list(const PRECIVE_PACKET recPacket)
{
	int count = 0;
	char data[100] = {0};
	int datalen = 4;
	int index = 0;
	uint32 ip;
	
	ip = storage_get_netparam_bytype(MANAGER1_IPADDR);
	if ( ip > 0 )
	{
		count++;
		index = 0;
		memcpy(data+4, &index, 4);
		memcpy(data+8, &ip, 4);
		datalen = 12;
	}

	ip = storage_get_netparam_bytype(MANAGER2_IPADDR);
	if ( ip > 0 )
	{
		count++;
		index = 1;
		memcpy(data+datalen, &index, 4);
		memcpy(data+datalen+4, &ip, 4);
		datalen += 8;
	}

	ip = storage_get_netparam_bytype(MANAGER3_IPADDR);
	if ( ip > 0 )
	{
		count++;
		index = 2;
		memcpy(data+datalen, &index, 4);
		memcpy(data+datalen+4, &ip, 4);
		datalen += 8;
	}
	
	memcpy(data, &count, 4);
	net_send_echo_packet(recPacket, ECHO_OK, data, datalen);
}

/*************************************************
  Function:			ipmodule_get_extensionAddr
  Description:		室内分机向IP模块获取分机的IP地址
  Input: 			
  	1.extensionNo	分机号
  Output:			无
  Return:			分机IP地址4B
  Others:
*************************************************/
int ipmodule_get_extensionAddr(uint32 extensionNo)
{
	uint8 EchoValue = ECHO_OK;
	int ret,ReciSize = 0;
	char ReciData[500];
	char bindCode[10] = {0};
	char data[10] = {0};
	uint32 ipModuleAddr = get_ipmodule_address();
	sprintf(bindCode, "%06d", get_ipmodule_bindcode());
	
	
	memcpy(data, bindCode, 6);
	memcpy(data+6, &extensionNo, 4);
	set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
	ret = net_send_command(CMD_GET_EXTENSION_ADDR, data, 10, ipModuleAddr, \
							NETCMD_UDP_PORT, 3, &EchoValue, ReciData, &ReciSize);
	if (ret == FALSE)
	{
		return 0;
	}
	
	if(EchoValue == ECHO_OK && ReciData != NULL && ReciSize > 0 )
	{
		uint32 address = 0;
		memcpy(&address, ReciData, 4);
		return address;
	}
	else
	{
		return 0;
	}
}
#endif

#ifdef _IP_MODULE_
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
void ipmodule_elevator_becken(const PRECIVE_PACKET recPacket)
{
	char *data = recPacket->data + NET_HEAD_SIZE;
	char verCode[10] = {0};
	uint32 action = 0;
	
	if ( NULL == data )
	{
		log_printf(">>>elevator_becken: data = NULL.\n");
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}

	// 判断验证码是否匹配
	memcpy(verCode, data, 6);
	if ( get_ipmodule_bindcode() != atoi(verCode) )
	{
		log_printf(">>>elevator_becken: bindcode = %s, error.\n", verCode);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return ;
	}
	log_printf(">>>elevator_becken: bindcode=%s.\n", verCode);
	
	//进行电梯召唤动作
	memcpy(&action, data+6, 4);
	if ( action == 0 ) 		//向上召唤
	{
		dianti_set_cmd(ELEVATOR_UP);
	}
	else if ( action == 1 ) //向下召唤
	{
		dianti_set_cmd(ELEVATOR_DOWN);
	}
	
	net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
}

/*************************************************
  Function:			elevator_becken_unlock
  Description:		移动终端请求与室内主机开锁被动召唤电梯	
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:			
*************************************************/
void ipmodule_elevator_becken_unlock(const PRECIVE_PACKET recPacket)
{
	char *data = recPacket->data + NET_HEAD_SIZE;
	char verCode[10] = {0};
	if ( NULL == data )
	{
		log_printf(">>>elevator_becken_unlock: data = NULL.\n");
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}

	//判断验证码是否匹配
	memcpy(verCode, data, 6);
	if ( get_ipmodule_bindcode() != atoi(verCode) )
	{
		log_printf(">>>elevator_becken_unlock: bindcode = %s, error.\n", verCode);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return ;
	}
	log_printf(">>>elevator_becken_unlock: bindcode=%s.\n", verCode);

	dianti_set_cmd(ELEVATOR_CALL);
	net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
}
#endif
#endif

