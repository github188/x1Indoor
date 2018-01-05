/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_ipmodule_alarm.c
  Author:   	txl
  Version:   	2.0
  Date: 		12.10.21
  Description:  报警函数
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#if 0
#include "include.h"
#include "logic_alarm.h"
#include "storage_include.h"
#endif

#include "storage_include.h"
#include "logic_include.h"

#ifdef _IP_MODULE_ALARM
/*************************************************
  Function:			ipmodule_transmit_alarmrecord
  Description:		往移动分机转发报警记录
  Input: 			
  	1.type			报警类型
  	2.areano		防区号
  	3.timestr		报警时间字符串
  Output:			无
  Return:			成功失败 true/false
  Others:
*************************************************/
int ipmodule_transmit_alarmrecord(uint32 type, uint32 areano, char *timestr)
{
	int i = 0;
	uint32 address = 0;
	char data[30] = {0};
	//IPAD_EXTENSION_LIST ipadList;
	
	if (NULL == timestr)
	{
		log_printf("ipmodule_transmit_alarmrecord: input argument timestr = NULL.\n");
		return FALSE;
	}
	log_printf("ipmodule_transmit_alarmrecord: type=%d, areano=%d, timestr=%s\n", type, areano, timestr);
	memcpy(data, &type, 4);
	memcpy(data+4, &areano, 4);
	memcpy(data+8, timestr, 20);

	// 2014-03-10 luofl modi 为配合IOS的推送功能,将原来报警命令发给移动终端的改为发给IP模块
	#if 0
	get_ipad_extension(&ipadList);
	for (i = 0; i < ipadList.count; i++)
	{
		address = ipadList.ipadData[i].ipAddr;
		if (address > 0 && ipadList.ipadData[i].state == 1)
		{		
			log_printf("ipmodule_transmit_alarmrecord: address=0x%x\n", address);
			net_direct_send(CMD_TRANSMIT_ALARMRECORD, data, 28, address, NETCMD_UDP_PORT);
		}
	}
	#else
	address = get_ipmodule_address();				// 获取IP模块地址
	set_nethead(G_CENTER_DEVNO, PRIRY_HIGHEST);
	net_direct_send(CMD_TRANSMIT_ALARMRECORD, data, 28, address, NETCMD_UDP_PORT);
	#endif
	
	return TRUE;
}

/*************************************************
  Function:			ipmodule_query_alarmrecord
  Description:		查询当前报警记录
  Input: 			
  	1.recPacket		接收包
  Output:			无
  Return:			0:失败 >0: 成功
  Others:
*************************************************/
int ipmodule_query_alarmrecord(PRECIVE_PACKET recPacket)
{
	uint8 excutor = 0;
	char verCode[10] = {0};
	PALARM_TOUCH_INFO_LIST pList = NULL;
	int i, count = 0;
	int len = 0;
	char data1[512] = {0};	
	char * data = recPacket->data + NET_HEAD_SIZE;

	// 未收到验证码
	if (NULL == data)
	{
		log_printf(">>>ipmodule_query_alarmrecord: data = NULL.\n");
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}

	// 判断验证码是否匹配
	memcpy(verCode, data, 6);
	if (get_ipmodule_bindcode() != atoi(verCode))
	{
		log_printf(">>>ipmodule_query_alarmrecord: bindcode = %s, error.\n", verCode);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return ;
	}
	log_printf(">>>ipmodule_query_alarmrecord: bindCode=%s.\n", verCode);
	log_printf(">>>address=0x%x.size=%d\n", recPacket->address, sizeof(ALARM_TOUCH_INFO));

	pList = storage_get_afbj_unread_record();
	pList->pAlarmRec[1].bReaded = 1;
	if (pList)
	{
		// 只上传最新的最多10条记录
		int j, startIndex = 0;

		count = pList->nCount;
		if (count > 10)
		{
			count = 10;
			pList->nCount = 10;
		}
		
		// 组合数据应答
		memcpy(data1, &count, 4);
		memcpy(data1+4, (char*)&(pList->pAlarmRec[startIndex]), count*sizeof(ALARM_TOUCH_INFO));
		len  = count*sizeof(ALARM_TOUCH_INFO) + 4;
		log_printf("count %d len %d\n",count,len);
		net_send_echo_packet(recPacket, ECHO_OK, data1, len);
		
		for (i = startIndex; i < pList->nCount; i++)
		{
			log_printf("i:%d, type=%d, no=%d, read=%d, time1=%s\n", i, pList->pAlarmRec[i].type, pList->pAlarmRec[i].TouchNum, \
						pList->pAlarmRec[i].bReaded, pList->pAlarmRec[i].time);
		}
		free(pList);
		pList = NULL;
	}
	else
	{
		count = 0;
		net_send_echo_packet(recPacket, ECHO_OK, &count, 4);
		log_printf("ipmodule_query_alarmrecord: count = 0.\n");
	}
}

/*************************************************
  Function:			ipmodule_clear_alarm
  Description:		清除警示
  Input: 			
  	1.recPacket		接收包
  Output:			无
  Return:			0:失败 >0: 成功
  Others:
*************************************************/
void ipmodule_clear_alarm(PRECIVE_PACKET recPacket)
{
	uint8 excutor = 0;
	char verCode[10] = {0};
	char * data = recPacket->data + NET_HEAD_SIZE;

	// 未收到验证码
	if (NULL == data)
	{
		log_printf(">>>ipmodule_clear_alarm: data = NULL.\n");
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}

	// 判断执行者
	memcpy(&excutor, data, 1);
	if (EXECUTOR_LOCAL_FJ != excutor)
	{
		log_printf(">>>ipmodule_clear_alarm: excutor = 0x%x, error.\n", excutor);
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}

	// 判断验证码是否匹配
	memcpy(verCode, data+1, 6);
	if (get_ipmodule_bindcode() != atoi(verCode))
	{
		log_printf(">>>ipmodule_clear_alarm: bindcode = %s, error.\n", verCode);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return ;
	}

	// 清除警示
	net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
	alarm_clear_alerts_operator();
	//af_gui_reaload();
}

/*************************************************
  Function:			ipmodule_sos_remotectrl
  Description:		远程SOS报警控制
  Input: 			
  	1.recPacket		接收包
  Output:			无
  Return:			0:失败 >0: 成功
  Others:
*************************************************/
void ipmodule_sos_remotectrl(PRECIVE_PACKET recPacket)
{
	uint8 excutor = 0;
	char verCode[10] = {0};
	char * data = recPacket->data + NET_HEAD_SIZE;

	// 未收到验证码
	if (NULL == data)
	{
		log_printf(">>>ipmodule_sos_remotectrl: data = NULL.\n");
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return;
	}

	// 判断执行者
	memcpy(&excutor, data, 1);
	if (EXECUTOR_LOCAL_FJ != excutor)
	{
		log_printf(">>>ipmodule_sos_remotectrl: excutor = 0x%x, error.\n", excutor);
		net_send_echo_packet(recPacket, ECHO_ERROR, NULL, 0);
		return ;
	}
	
	// 判断验证码是否匹配
	memcpy(verCode, data+1, 6);
	if (get_ipmodule_bindcode() != atoi(verCode))
	{
		log_printf(">>>ipmodule_sos_remotectrl: bindcode = %s, error.\n", verCode);
		net_send_echo_packet(recPacket, ECHO_MAC, NULL, 0);
		return;
	}
	log_printf(">>>ipmodule_sos_remotectrl: bindCode=%s.\n", verCode);

	// sos处理
	sos_alarm_report();
	//RockOSSendMsg(MBGUI, AS_ALARM_PROC, (void *)0);
	net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);
}
#endif

