/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_ipmodule_set.c
  Author:     	luofl
  Version:    	1.0
  Date: 		2011-05-20
  Description:  
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "logic_include.h"
#include "storage_include.h"

#ifdef _IP_MODULE_
#define IPMODULE_ONLINE_TIMEOUT			20 	// IP模块在线超时时间
#define IPMODULETIME					2	// 定时器间隔

static uint32 g_ipmoduleID = 0;			    // 定时器ID			
static uint8 g_ipModule_online = 0;
static ZONE_DATE_TIME g_Timer;
static IP_MODULE_ERR_CODE g_errcode = IP_MODULE_CODE_UNLINE;
static int g_count = 10;				    // 在线时计数

/*************************************************
  Function:			get_ipmodule_online
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
uint8 get_ipmodule_bind(void)
{
	return get_ipmodule_bindcode();
}


/*************************************************
  Function:			get_ipmodule_online
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
uint8 get_ipmodule_online(void)
{
	return g_ipModule_online;
}

/*************************************************
  Function:			set_ipmodule_online
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
void set_ipmodule_online(uint8 bind)
{
	g_ipModule_online = bind;
}

/*************************************************
  Function:			get_ipmodule_errcode
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
IP_MODULE_ERR_CODE get_ipmodule_errcode(void)
{
	return g_errcode;
}

/*************************************************
  Function:			set_ipmodule_errcode
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
void set_ipmodule_errcode(IP_MODULE_ERR_CODE code)
{
	g_errcode = code;
}

/*************************************************
  Function:			get_ipmodule_address
  Description:		获取IP模块状态
  Input: 			无
  Output:			无
  Return:			IP地址
  Others:
*************************************************/
uint32 get_ipmodule_address(void)
{
	#ifdef _IP_MODULE_
	return get_ipmodule_addr();
	#else
	return 0;
	#endif
}

/*************************************************
  Function:    		get_ipmodule
  Description:		
  Input: 			无
  Output:			无
  Return:			捆绑的状态
  Others:
*************************************************/
uint32 get_ipmodule(void)
{
	uint32 state = get_ipmodule_bindstate();

	#if 0
	if (FALSE == is_main_DeviceNo())
	{
		return state;
	}
	else
	{
		if (state)
		{
			return get_ipmodule_online();
		}
		else
		{
			return state;
		}
	}
	#else
	return state;
	#endif
}

/*************************************************
  Function:			ipmodule_request_bind
  Description:		室内主机向绑定IP模块
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 ipmodule_request_bind(void)
{
	uint8 EchoValue = ECHO_OK;
	int ReciSize = 0, ret = 0;
	char ReciData[2000];
	char data[6] = {0};
	uint32 bindCode = get_ipmodule_bindcode();
	uint32 ipModuleAddr = get_ipmodule_address();

	//set_ipmodule_bindstate(0);
	set_ipmodule_online(0);
	set_ipmodule_errcode(IP_MODULE_CODE_UNLINE);	
	
	if (ipModuleAddr > 0)
	{
		sprintf(data, "%06d", bindCode);
		set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
		ret = net_send_command(CMD_IPMODULE_BIND_QUEST, data, 6, ipModuleAddr, NETCMD_UDP_PORT, 3, &EchoValue, ReciData, &ReciSize);
		if (ret == FALSE)
		{	
			set_ipmodule_bindstate(0);
			return 0;
		}
		
		if (EchoValue == ECHO_OK)
		{
			set_ipmodule_errcode(IP_MODULE_CODE_OK);
			set_ipmodule_bindstate(1);
			set_ipmodule_online(1);
			return TRUE;
		}
		else
		{
			if (EchoValue == ECHO_MAC)
			{
				set_ipmodule_errcode(IP_MODULE_CODE_CODE_ERR);
			}
			set_ipmodule_bindstate(0);
		}
	}
	return 0;
}

/*************************************************
  Function:			ipmodule_bind
  Description:		IP模块向室内主机绑定
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void ipmodule_bind(const PRECIVE_PACKET recPacket)
{
	char verCode[10] = {0};
	IPMODULE_INFO ipmoduleInfo;
	char *data = recPacket->data + NET_HEAD_SIZE;
	FULL_DEVICE_NO *pFulldevno = storage_get_devparam();

	//非主机则不进行绑定
	if (FALSE == is_main_DeviceNo())
	{
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}
	
	if (NULL == data)
	{
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}
	set_ipmodule_online(0);
	set_ipmodule_errcode(IP_MODULE_CODE_UNLINE);
	
	//判断验证码是否匹配
	memcpy(verCode, data, 6);
	
	if (get_ipmodule_bindcode() != atoi(verCode))
	{
		set_ipmodule_errcode(IP_MODULE_CODE_CODE_ERR);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return ;
	}

	if (pFulldevno)
	{
		set_ipmodule_errcode(IP_MODULE_CODE_OK);
		net_send_echo_packet(recPacket, ECHO_OK, (char *)pFulldevno, sizeof(FULL_DEVICE_NO));
		ipmoduleInfo.State = 1;
		ipmoduleInfo.IpAddr = recPacket->address;
		ipmoduleInfo.BindCode = atoi(verCode);
		set_ipmodule_info(&ipmoduleInfo);
	}
	else
	{
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
	}	
}

/*************************************************
  Function:			ipmodule_heart
  Description:		IP模块向室内机主机保持通讯
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void ipmodule_heart(const PRECIVE_PACKET recPacket)
{
	int ret;
	char verCode[10] = {0};
	IPAD_EXTENSION_LIST ipadList;
	
	char *data = recPacket->data + NET_HEAD_SIZE;
	if (NULL == data)
	{
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}

	//判断验证码是否匹配
	memcpy(verCode, data, 6);
	
	if (get_ipmodule_bindcode() != atoi(verCode))
	{
		set_ipmodule_errcode(IP_MODULE_CODE_CODE_ERR);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return ;
	}

	if (get_ipmodule_addr() != recPacket->address)
	{
		set_ipmodule_errcode(IP_MODULE_CODE_IP_ERR);
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}
	//Ipad分机数量4B
	
	memcpy(&(ipadList.count), data+6, 4);

	if (ipadList.count > 0)
	{
		memcpy(ipadList.ipadData, data+10, sizeof(IPAD_DATA)*ipadList.count);
	}
	
	//保存ipad分机信息
	ret = set_ipad_extension(ipadList);
	set_ipmodule_online(1);
	get_timer(&g_Timer);
	g_count = 10;
	
	if (ret)
	{
		net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
	}
	else
	{
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
	}
}

/*************************************************
  Function:			ipmodule_online
  Description:		IP模块在线状态处理函数
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void ipmodule_online(uint32 cmd, void * arg)
{
	if (g_count)
	{
		g_count--;
	}
	else
	{
		set_ipmodule_online(0);
		set_ipmodule_errcode(IP_MODULE_CODE_UNLINE);
	}
}

/*************************************************
  Function:			public_distribute
  Description:		公共模块接收发送包回调函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否,true/fasle
  Others:
*************************************************/
int ipmodule_distribute(const PRECIVE_PACKET recPacket)
{
	int ret = TRUE;
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	int cmd = head->command | SSC_PUBLIC<<8;
	
	switch (cmd)
	{
		case CMD_IPMODULE_BIND:						// IP模块向室内主机绑定
			ipmodule_bind(recPacket);
			break;
			
		case CMD_IPMODULE_HEART:					// IP模块向室内机主机保持通讯
			ipmodule_heart(recPacket);
			break;

		case CMD_IPMODULE_BIND_QUEST:
			log_printf("CMD_IPMODULE_BIND_QUEST head->EchoValue = %d\n",head->EchoValue);
			if (head->EchoValue == ECHO_OK)
			{
				log_printf("CMD_IPMODULE_BIND_QUEST111111111111\n");
				set_ipmodule_bindstate(1);
			}
			else
			{
				set_ipmodule_bindstate(0);
				log_printf("CMD_IPMODULE_BIND_QUEST00000000000000\n");
			}
			break;

		#ifdef _IP_MODULE_DJ_
		case CMD_GET_PARAM:							// 获取室内主机的相关参数信息内容
			ipmodule_get_room_param(recPacket);
			break;
		
		case CMD_GET_NETDOOR_LIST:					// 获取室内主机捆绑的网络门前机信息列表
			ipmodule_get_netdoor_list(recPacket);
			break;
		
		case CMD_GET_MANAGER_LIST:					// 获取室内主机捆绑的管理中心列表	
			ipmodule_get_manager_list(recPacket);
			break;
		#endif	

		#ifdef _USE_ELEVATOR_
		case CMD_ELEVATOR_BECKON:					// 移动终端请求与室内机主机电梯召唤
			ipmodule_elevator_becken(recPacket);
			break;
			
		case CMD_ELEVATOR_BECKON_UNLOCK:			// 移动终端请求与室内主机开锁被动召唤电梯
			ipmodule_elevator_becken_unlock(recPacket);
			break;
		#endif
			
		default:
			return FALSE;
	}
	
	return ret;
}

/*************************************************
  Function:			ipmodule_init
  Description:		IP模块初始化
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void ipmodule_init(void)
{
	// 不是主机时不进行在线状态处理
	if (is_main_DeviceNo() == FALSE)
	{
		int ret = ipmodule_request_bind();
		if (ret)
		{
			set_ipmodule_bindstate(1);
		}
		else
		{	
			set_ipmodule_bindstate(0);
		}
	}
	else if (!g_ipmoduleID)
	{
		g_ipmoduleID = add_aurine_realtimer(1000*IPMODULETIME, ipmodule_online, NULL);
	}
}
#endif

