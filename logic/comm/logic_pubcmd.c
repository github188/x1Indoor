/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_pubcmd.c
  Author:    	xiewr
  Version:   	1.0
  Date: 
  Description:  公共命令
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
//#include "logic_pubcmd.h"
//#include "logic_ipmodule.h"
#include "logic_include.h"

RTSP_SERVER_INFO	 g_Rtsp_ServerInfo = {{0}, 0, 0, 0};
RP_EVENT_SERVER_INFO g_Event_Server = {0, 0};

#ifdef _AURINE_REG_
/***********************************************************/
/*							平台注册					   */
/***********************************************************/
static uint32 g_FjIP[10];							// 注册保持的分机数
static uint8 g_aurineSvrOnLine  = 0;				// 如果此值>5则不在线
static uint8 g_fjip_flag = 0;

/*************************************************
  Function:    		get_local_fjIP
  Description:		获得分机
  Input:			
  Output:			
  Return:			
  Others:			
*************************************************/
static void get_local_fjIP(void)
{
	uint8 i;
	uint8 count = 0;
	char no[50];
	uint32 *IPs = NULL;	
	PFULL_DEVICE_NO dev = storage_get_devparam();
	uint32 LocalAreaCode = dev->AreaNo;
	strncpy(no, dev->DeviceNoStr, dev->DevNoLen-1);
	no[dev->DevNoLen-1] = '_';
	no[dev->DevNoLen] = 0;
	count = net_get_ips(LocalAreaCode, DEVICE_TYPE_ROOM, no, &IPs);
	memset(g_FjIP, 0, sizeof(g_FjIP));
	for (i = 0; i < count; i++)
	{
		g_FjIP[i] = IPs[i];
	}
}

/*************************************************
  Function:    		logic_reg_aurine_state
  Description:		获得平台状态
  Input:			
  Output:			
  Return:			0:在线；1:冠林平台未注册；2:平台离线
  Others:			
*************************************************/
uint8 logic_reg_aurine_state(void)
{
	if (g_aurineSvrOnLine >= 5) 
	{
		return 2;
	}
	
	if (storage_get_regcode() == 0)
	{
		return 1;
	}
	
	return 0;
}

/*************************************************
  Function:    		logic_reg_aurine_ontimer
  Description:		获得平台定时保持
  Input:			
  Output:			
  Return:			
  Others:			
*************************************************/
void logic_reg_aurine_ontimer(void)
{
	uint8 sMac[60]; 
	uint8 tmp[100];
	uint8 i;
	uint32 Aurineip;
	uint32 regCode = 0;

	#ifdef _IP_MODULE_RTSP_
	if (is_main_DeviceNo())
	{
		Aurineip = storage_get_netparam_bytype(RTSP_IPADDR);
		if (Aurineip == 0)
		{
			Aurineip = g_Rtsp_ServerInfo.ip;
		}
		if (Aurineip != 0)
		{	
			get_houseno_desc(storage_get_devno_str(),tmp);
			memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);
			memcpy(sMac+6, tmp, 32);
			memset(tmp, 0, 100);
			sprintf(tmp, "%d", get_ipmodule_bindcode());
			memcpy(sMac+38, tmp, 6);
			// 向流媒体服务器发送终端通讯保持	
			set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
			//net_send_by_list(CMD_RTSP_KEEP, sMac, 44, Aurineip, NETCMD_UDP_PORT, 1, 2);
			net_send_by_list(CMD_REGISTER, sMac, 44, Aurineip, NETCMD_UDP_PORT, 1, 2);
		}

		g_aurineSvrOnLine++;
	}
	#endif
	regCode = storage_get_regcode();
	if (regCode && is_main_DeviceNo())
	{
		Aurineip = storage_get_netparam_bytype(AURINE_IPADDR);
		if (Aurineip != 0)
		{
			// 主机向冠林平台保持
			memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);
			set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
			net_send_by_list(CMD_REGISTER, sMac, 6, Aurineip, NETCMD_UDP_PORT, 1, 2);
		}

		// 分机IP只获取一次
		if (!g_fjip_flag)
		{
			g_fjip_flag = 1;
			get_local_fjIP();
		}

		// 通知分机向冠林平台保持
		for (i = 0; i < 10; i++)
		{
			if (g_FjIP[i] > 0)
			{
				net_send_by_list(CMD_REGISTER, sMac, 6, g_FjIP[i], NETCMD_UDP_PORT, 1, 2);
			}
		}
		g_aurineSvrOnLine++;
	}
	
}

/*************************************************
  Function:    		logic_reg_aurine
  Description:		获得平台注册码
  Input:			
  Output:			
  Return:			
  Others:			
*************************************************/
uint32 logic_reg_aurine(void)
{
	uint32 ret = 0;
	uint8 EchoValue = 1;
	char RecData[200];
	uint32 ReciSize = 0;
	uint32 Aurineip = storage_get_netparam_bytype(AURINE_IPADDR);
	
	if (Aurineip != 0)
	{		
		unsigned char sMac[30]; 
		
		memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);	// 取出mac码
		set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
		ret = net_send_command(CMD_REGISTER_AURINE, sMac, 6, Aurineip, NETCMD_UDP_PORT, 3, &EchoValue, RecData, &ReciSize);
		if (TRUE == ret)
		{
			if (EchoValue == ECHO_OK)
			{
				memcpy(&ret, RecData, 4);
				storage_set_regcode(ret);
				get_local_fjIP();
				return ret;
			}
			else
			{
				return 0;
			}
		}	
		else
		{
			return 0;
		}
	}
	return 0;
}

/*************************************************
  Function:    		logic_reg_aurine_ini
  Description:		初始化平台注册码
  Input:			
  Output:			
  Return:			
  Others:			
*************************************************/
void logic_reg_aurine_ini(void)
{
	get_local_fjIP();
}
#endif

#ifdef _RTSP_REG_KEEP_
/*************************************************
  Function:    	logic_rtsp_ontimer
  Description:	与流媒体服务器定时保持
  Input:		无	
  Output:		无
  Return:		无
  Others:		ipad与iPhone才能获取到室内摄像头	
*************************************************/
void logic_rtsp_ontimer(void)
{
	uint8 sMac[60]; 
	uint8 tmp[100];
	uint32 Rtspip = 0;

	if (is_main_DeviceNo())
	{
		Rtspip = storage_get_netparam_bytype(RTSP_IPADDR);
		if (Rtspip == 0)
		{
			//log_printf(" rtsp server is not set !!! \n");
			return;
		}
		
		if (Rtspip != 0)
		{	
			memset(sMac, 0, sizeof(sMac));
			memset(tmp, 0, sizeof(tmp));
			get_houseno_desc(storage_get_devno_str(), tmp);
			memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);
			memcpy(sMac+6, tmp, 32);
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%d", get_ipmodule_bindcode());
			memcpy(sMac+38, tmp, 6);
			
			// 向流媒体服务器发送终端通讯保持	
			set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
			net_direct_send(CMD_REGISTER, sMac, 44, Rtspip, NETCMD_UDP_PORT);
		}
	}
}
#endif

#ifdef _USE_NEW_CENTER_
static uint8 g_regflag = 0;				// 终端注册状态 0未注册 1注册
static uint8 g_regkeeptimes = 0;		// 终端保持命令无应答次数

/*************************************************
  Function:			requst_event_ip
  Description:		请求事件上报服务器
  Input: 	
  Output:			无
  Return:			
  Others:
*************************************************/
void logic_requst_event_ip(void)
{
	uint32 ServerIP = 0;
	ServerIP = net_get_center_ip();
	set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
	net_direct_send(CMD_GET_EVENT_SERVER, NULL, 0, ServerIP, NETCMD_UDP_PORT);
}

/*************************************************
  Function:    		logic_reg_center_ini
  Description:		终端注册
  Input:			
  Output:			
  Return:			
  Others:		MAC 码+旧设备编号+硬件版本标识（64B）+软件版本标识（64B）+本机设备名称（32字节，’\0’结尾					
*************************************************/
void logic_reg_center_ini(void)
{
	uint8 sMac[10] = {0}; 							// mac 码
	uint8 DevStr[50] = {0};							// 本机设备名称
	uint8 HardVer[64] = {0};						// 硬件版本标示
	uint8 SoftVer[64] = {0};						// 软件版本标示
	uint8 SendData[200] = {0};
	uint32 CenterIp = 0;
	DEVICE_NO devno;

	CenterIp = net_get_center_ip();
	if (CenterIp == 0)
	{
		return;
	}
	memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);		// mac 码
	memset(&devno, 0, sizeof(DEVICE_NO));
	devno = storage_get_predevno();						// 旧设备编号
	memcpy(HardVer, HARD_VER, 64);						// 硬件版本标示
	memcpy(SoftVer, SOFT_VER, 64);						// 软件版本标示
	get_houseno_desc(storage_get_devno_str(),DevStr);	// 本机设备编号描述

	memset(SendData, 0, sizeof(SendData));
	memcpy(SendData, sMac, 6);
	memcpy(SendData+6, &devno, sizeof(DEVICE_NO));
	memcpy(SendData+6+sizeof(DEVICE_NO), HardVer, 64);
	memcpy(SendData+70+sizeof(DEVICE_NO), SoftVer, 64);
	memcpy(SendData+134+sizeof(DEVICE_NO), DevStr, 32);

	set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
	net_direct_send(CMD_REGISTER_AURINE, SendData, 178, CenterIp, NETCMD_UDP_PORT);
}


/*************************************************
  Function:    		logic_reg_center_ontimer
  Description:		与服务器保持
  Input:			
  Output:			
  Return:			
  Others:			MAC 码 6B+本机设备名称（32字节，’\0’结尾
*************************************************/
void logic_reg_center_ontimer(void)
{
	uint8 tmp[50];
	uint8 sMac[50]; 
	uint8 senddata[100];
	uint32 CenterIp = 0;
	g_regkeeptimes++;

	CenterIp = net_get_center_ip();
	get_houseno_desc(storage_get_devno_str(),tmp);	
	memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);	
	memcpy(sMac+6, tmp, 32);
	memset(senddata, 0, sizeof(senddata));
	memcpy(senddata, sMac, 38);
	senddata[38] = 0;

	log_printf("send CMD_REGISTER\n");
	set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
	net_direct_send(CMD_REGISTER, senddata, 38, CenterIp, NETCMD_UDP_PORT);

	if (g_Event_Server.ip == 0)
	{
		logic_requst_event_ip();
	}
	log_printf("g_regkeeptimes : %d\n", g_regkeeptimes);
}

/*************************************************
  Function:    		logic_reg_center_state
  Description:		获取终端注册状态
  Input:			
  Output:			
  Return:			0未注册 1注册成功保持超时 2保持正常
  Others:			
*************************************************/
int32 logic_reg_center_state(void)
{
	if (g_regflag == 0)
	{
		return 0;
	}
	else if (g_regkeeptimes >= 3)
	{
		return 1;
	}	
	else
	{
		return 2;
	}
}

/*************************************************
  Function:    		logic_reg_center_ini_again
  Description:		重新向服务器注册
  Input:			
  Output:			
  Return:			
  Others:			
*************************************************/
void logic_reg_center_ini_again(void)
{
	g_regflag = 0;
	logic_reg_center_ini();
}
#endif

#ifdef _APP_CALL_ELEVATOR_
/*************************************************
  Function:    		logic_elevator
  Description:		召唤电梯功能
  Input:			
  Output:			
  Return:			
  Others:			
*************************************************/
void logic_elevator(const PRECIVE_PACKET recPacket)
{
	if (NULL == recPacket)
	{
		return;
	}
	
	char sendata[1024];
	uint32 echo;	
	
	//应答的数据是四个字节刚好一个指针变量也是4个字节
	uint32 datalen = sizeof(TunnelReplayData);
	char *data = recPacket->data+NET_HEAD_SIZE;;
	
	TunnelSendData tsdata;
	memset(&tsdata, 0, sizeof(TunnelSendData));
	
	memcpy(&tsdata, data+4, 20);
	tsdata.dataparam = data+24;
	
	uint8 elevator_type;
	memcpy(&elevator_type, tsdata.dataparam + 20, 1);
	
	TunnelReplayData trdata;				
	memset(&trdata, 0, sizeof(TunnelReplayData));
	
	log_printf("==systype: %d==\n",tsdata.systype);	
	log_printf("==seq: %d==\n",tsdata.seq);
	log_printf("==cmdtype: %d==\n",tsdata.cmdtype);
	log_printf("==elevator_type:%d==\n",elevator_type);
	log_printf("==dataSize:%d==\n",tsdata.dataSize);
	
	trdata.systype = 6;
	trdata.cmdtype = tsdata.cmdtype;
	trdata.dire = DIR_ACK;
	trdata.seq = tsdata.seq;
	trdata.dataSize = 4;
	
	memcpy(sendata, &datalen, 4);
		
	if(tsdata.cmdtype == CMDTYPE_ELEVATOR)
	{	
		echo = 0;
		trdata.dataparam = &echo;
		memcpy(sendata+4, (char *)&trdata, 20);
		memcpy(sendata+24, trdata.dataparam, 4);
		net_send_echo_packet(recPacket, ECHO_OK, sendata, 4 + sizeof(TunnelReplayData));
	
		if(elevator_type)
		{
			log_printf("===becall dianti===\n");
			//被动召梯
			dianti_set_cmd(ELEVATOR_CALL);
		}
		else
		{
			log_printf("===call dianti===\n");
			//主动召梯
			dianti_set_cmd(ELEVATOR_DOWN);
		}
	}
	else
	{
		echo = 1;
		trdata.dataparam = &echo;
		memcpy(sendata+4, (char *)&trdata, 20);
		memcpy(sendata+24, trdata.dataparam, 4);
		net_send_echo_packet(recPacket, ECHO_ERROR, sendata, 4 + sizeof(TunnelReplayData));
	}
}
#endif

/*************************************************
  Function:			public_distribute
  Description:		公共命令接收包回调函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 public_distribute(const PRECIVE_PACKET recPacket)
{
	PNET_HEAD head;
	int cmd;
	
	#ifdef _AURINE_REG_
	if (recPacket == NULL)
	{
		return TRUE;
	}
	#endif

	head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	cmd = head->command | SSC_PUBLIC << 8;
	switch (cmd)
	{		
		case CMD_SOFT_SET_PARAM:
		case CMD_SOFT_GET_PARAM:
		case CMD_SOFT_TERMINAL_CMD:
		{
			#ifndef _AU_PROTOCOL_
				uint32 U32Encrypt = 0;
				uint8 hwEncrypt[16] = {0};
				
				PMAIN_NET_HEAD PMainNethead = (PMAIN_NET_HEAD)(recPacket->data);
				memcpy(hwEncrypt, PMainNethead->hwEncrypt, sizeof(PMainNethead->hwEncrypt));
				memcpy(&U32Encrypt, &hwEncrypt[12], 4);
				log_printf("U32Encrypt : %08x\n", U32Encrypt);
												
				if (U32Encrypt != _TYSET_HWENCRYPT_)
				{
					log_printf(" Encrypt is wrong !!! \n");
					return FALSE;
				}

				switch (cmd)
				{
					case CMD_SOFT_SET_PARAM:
						set_param_cmd_deal(recPacket);
						break;

					case CMD_SOFT_GET_PARAM:
						get_param_cmd_deal(recPacket);
						break;

					case CMD_SOFT_TERMINAL_CMD:
						terminal_cmd_deal(recPacket);
						break;

					default:
						break;								
				}							
				return TRUE;
			#else
				return FALSE;
			#endif
		}
	
		case CMD_DEVNO_RULE_CMD:							
		{
			char sub_des[70] = {0};
			char ssub[10] = {0};
			DEVICENO_RULE *Rule = (DEVICENO_RULE *)(recPacket->data+NET_HEAD_SIZE);
			storage_set_devno_rule(TRUE, *Rule);
			
			sprintf(ssub, "%d",	Rule->Subsection);
			memcpy(sub_des, (char *)(recPacket->data+NET_HEAD_SIZE+8), 10*strlen(ssub));
			storage_set_dev_desc(sub_des);					
			log_printf("public distribute : set devno rule : StiarNoLen:%d,RoomNoLen:%d,CellNoLen:%d,UseCellNo:%d,SubSection:%d, sublen:%s\n", 
							Rule->StairNoLen, Rule->RoomNoLen, Rule->CellNoLen, Rule->UseCellNo, Rule->Subsection, ssub);
			return TRUE;
		}	
		
		case CMD_SET_SYS_PASS:						// 设置管理员密码
		{
			char *data = NULL;
			data = recPacket->data+NET_HEAD_SIZE;
			data[head->DataLen] = 0;
			log_printf("public_distribute : CMD_SET_SYS_PASS : set admin pass:%s %d %d\n", data, strlen(data), head->DataLen);
			storage_set_pass(PASS_TYPE_ADMIN, data);
			net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
			return TRUE;
		}

		case CMD_SET_AREACODEAM:					// 设置区号
		{
			//uint32 AreaNo = *((uint32 *)(recPacket->data+NET_HEAD_SIZE));
			// 高2B数据预留但不做处理
			uint16 AreaNo = *((uint16 *)(recPacket->data+NET_HEAD_SIZE));

			// 设置区号
			storage_set_areano(AreaNo);
			
			net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
			net_change_comm_deviceno();
			log_printf("public_distribute : CMD_SET_AREACODEAM : AreaNo:%d\n", AreaNo);
			return TRUE;
		}	

		case CMD_TERMINAL_CMD:
		{
			char *data = recPacket->data+NET_HEAD_SIZE;
			data[head->DataLen] = 0;
			if (strlen(data))
			{
				//执行终端命令
				log_printf("CMD_TERMINAL_CMD:%s\n", data);
				system(data);
				net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
			}
			else
			{
				net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
			}
			return TRUE;
		}		

		case CMD_SYN_TIME:							// 时间同步
		{
			ZONE_DATE_TIME LocalDateTime = {0};
			DATE_TIME *datetime = (DATE_TIME *)(recPacket->data+NET_HEAD_SIZE);
			log_printf("public_distribute : CMD_SYN_TIME! \nyear: %d, month: %d, day: %d, hour: %d, min: %d, sec: %d\n",
					datetime->year, datetime->month, datetime->day, datetime->hour, datetime->min, datetime->sec);
					
			LocalDateTime.year = datetime->year;
			LocalDateTime.month = datetime->month;
			LocalDateTime.day = datetime->day;
			LocalDateTime.hour = datetime->hour;
			LocalDateTime.min = datetime->min;
			LocalDateTime.sec = datetime->sec;
			LocalDateTime.week = datetime->week;
			
			get_local_time(&LocalDateTime, 8);
			set_rtc_timer(&LocalDateTime);
			#ifdef _ADD_BLUETOOTH_DOOR_
			logic_set_bluetooth_door_time();
			#endif
			return TRUE;
		}

		#ifdef _AURINE_REG_
		case CMD_REGISTER_AURINE:
			break;

		case CMD_REGISTER:
		{
			uint8 sMac[30];  	
			uint32 Aurineip = storage_get_netparam_bytype(AURINE_IPADDR);
			
			if (is_main_DeviceNo()) 
			{
				break;
			}
			
			if (Aurineip != 0)
			{
				memcpy(sMac, storage_get_mac(HOUSE_MAC), 6);
				set_nethead(G_CENTER_DEVNO, PRIRY_DEFAULT);
				net_send_by_list(CMD_REGISTER, sMac, 6, Aurineip, NETCMD_UDP_PORT, 1, 2);
			}
			return TRUE;
			break;
		}
		#endif

		#ifdef _NEW_SELF_IPC_
		case CMD_REGISTER:
			{				
				if (TRUE == is_main_DeviceNo())
				{
					#ifdef _AU_PROTOCOL_
					uint8 index = head->DeviceNo.DeviceNo2%10;
					#else
					uint8 index = head->SrcDeviceNo.DeviceNo2%10;
					#endif
					uint32 ip = recPacket->address;

					// add by chenbh 2016-03-09 如果该设备号有变化则改变存储
					PMonitorDeviceList Devlist = NULL;
					storage_free_monitordev_memory(&Devlist);
					storage_get_home_monitordev(&Devlist);
					if (Devlist->Homedev[index].EnableOpen &&
						(0 == strcmp(Devlist->Homedev[index].FactoryName, "SELFIPC") ||
						0 == strcmp(Devlist->Homedev[index].FactoryName, "SelfIPC")))
					{
						// 目前只有比较IP 地址
						if (ip != Devlist->Homedev[index].DeviceIP)
						{
							Devlist->Homedev[index].DeviceIP = ip;
							storage_save_monitordev(HOME_CAMERA, Devlist);
						}
					}
					storage_free_monitordev_memory(&Devlist);
					set_ipc_online(index, recPacket->address);
				}
			}
			break;
		#endif
		
		#ifdef _APP_CALL_ELEVATOR_
		case CMD_TUNNEL:
			{
				logic_elevator(recPacket);
				return TRUE;
			}
		#endif
			
		default:
			#ifdef _IP_MODULE_
			return ipmodule_distribute(recPacket);
			#endif
			break;
	}
	return FALSE;
}

/*************************************************
  Function:			public_responsion
  Description:		公共命令接收应答包处理函数
  Input: 	
  	1.recPacket		应答包
  	2.SendPacket	发送包
  Output:			无
  Return:			无
  Others:
*************************************************/
void public_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket)
{
	PNET_HEAD head;
	int cmd;
	
	#ifdef _AURINE_REG_
	if (recPacket == NULL)
	{
		g_aurineSvrOnLine = 0;
		return;
	}
	#endif
	
	head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);
	cmd = head->command | SSC_PUBLIC << 8;
	switch (cmd)
	{	
		#ifdef _AURINE_REG_
		case CMD_REGISTER:							// 室内终端注册	0x62
		{
			g_aurineSvrOnLine = 0;
			break;
		}
		#endif

		#ifdef _USE_NEW_CENTER_
		case CMD_REGISTER:
			{
				if (head->EchoValue == ECHO_OK)		
				{
					g_regkeeptimes = 0;				// 清空发送次数
				}
			}
			break;
			
		case CMD_REGISTER_AURINE:
			{
				log_printf("CMD_REGISTER_AURINE head->EchoValue : %d \n", head->EchoValue);
				if (head->EchoValue == ECHO_OK)		// 终端注册成功
				{
					PFULL_DEVICE_NO pDevNo;
					
					// 应答参数处理  编号规则+分段描述
					/*
					char sub_des[70] = {0};
					char ssub[10] = {0};
					DEVICENO_RULE *Rule = (DEVICENO_RULE *)(recPacket->data+NET_HEAD_SIZE);
					storage_set_devno_rule(TRUE, *Rule);
					
					sprintf(ssub, "%d",	Rule->Subsection);
					memcpy(sub_des, (char *)(recPacket->data+NET_HEAD_SIZE+8), 10*strlen(ssub));
					storage_set_dev_desc(sub_des);	
					*/
					
					g_regflag = 1;
					// 注册成功保存注册设备编号
					pDevNo = storage_get_devparam();					
					storage_set_predevno(&(pDevNo->DeviceNo));
					
					// 注册成功事件上报IP获取
					logic_requst_event_ip();
				}
				else
				{
					g_regflag = 0;
				}
			}
			break;

		case CMD_GET_SUIT_SERVER:
			{
				if (head->EchoValue == ECHO_OK)
				{
					char *RecData = (recPacket->data+NET_HEAD_SIZE);
					memcpy(&g_Rtsp_ServerInfo.deviceno, (PDEVICE_NO)RecData, sizeof(DEVICE_NO));
					g_Rtsp_ServerInfo.ip = *(uint32 *)(RecData+12);
					g_Rtsp_ServerInfo.comm_port = *(uint32 *)(RecData+16);
					g_Rtsp_ServerInfo.rtsp_port = *(uint32 *)(RecData+20);
					
					log_printf("g_Rtsp_ServerInfo.ip: %x \n ", g_Rtsp_ServerInfo.ip);
					log_printf("g_Rtsp_ServerInfo.comm_port: %d \n ", g_Rtsp_ServerInfo.comm_port);
					log_printf("g_Rtsp_ServerInfo.rtsp_port: %d \n ", g_Rtsp_ServerInfo.rtsp_port);
				}
				else
				{
					memset(&g_Rtsp_ServerInfo, 0, sizeof(RTSP_SERVER_INFO));
					log_printf("CMD_GET_SUIT_SERVER return error!!!! \n ");
				}
			}
			break;

		case CMD_GET_EVENT_SERVER:
			{
				if (head->EchoValue == ECHO_OK)
				{
					char *RecData = (recPacket->data+NET_HEAD_SIZE); 
					memcpy(&g_Event_Server.ip, RecData, 4);
					memcpy(&g_Event_Server.port, RecData+4, 4);
					log_printf("g_Event_Server.ip : %x, g_Event_Server.port: %d \n", g_Event_Server.ip, g_Event_Server.port);
				}
				else
				{
					memset(&g_Event_Server, 0, sizeof(RP_EVENT_SERVER_INFO));
					log_printf("CMD_GET_EVENT_SERVER return error!!! \n");
				}
			}
			break;
		#endif

		case CMD_REQ_SYN_TIME:
		{
			if (head->EchoValue == ECHO_OK)
			{
				ZONE_DATE_TIME LocalDateTime = {0};
				DATE_TIME *datetime = (DATE_TIME *)(recPacket->data+NET_HEAD_SIZE);
				log_printf("public_distribute : CMD_SYN_TIME! \nyear: %d, month: %d, day: %d, hour: %d, min: %d, sec: %d\n",
						datetime->year, datetime->month, datetime->day, datetime->hour, datetime->min, datetime->sec);
						
				LocalDateTime.year = datetime->year;
				LocalDateTime.month = datetime->month;
				LocalDateTime.day = datetime->day;
				LocalDateTime.hour = datetime->hour;
				LocalDateTime.min = datetime->min;
				LocalDateTime.sec = datetime->sec;
				LocalDateTime.week = datetime->week;
				
				get_local_time(&LocalDateTime, 8);  // 上位机下来时间没有加上时区
				set_rtc_timer(&LocalDateTime);
				#ifdef _ADD_BLUETOOTH_DOOR_
				logic_set_bluetooth_door_time();
				#endif
			}
			break;
		}
		
		default:
			break;
	}
}

