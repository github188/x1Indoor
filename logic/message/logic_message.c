/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_message.c
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-11-06
  Description:	信息处理逻辑
  				中心发给住户的称为信息（类型包括文字、图片、声音或组合）
  				用户通过手机发给住户的称为短信, 类型为纯文字, 一次不超过70个汉字。
  History:                    
    1. Date:		 
       Author:		 
       Modification: 
    2. ...
*********************************************************/
#if 0
#include "include.h"
#include "storage_include.h"
#include "net_comm_types.h"
#include "net_command.h"
#endif

#include "logic_include.h"

#define REC_SHORT_MSG_SIZE		(8 + 1 + 20 + 200)

/*************************************************
  Function:			save_msg
  Description: 		保存信息(由中心发出)
  Input: 			
  	1.ID			会话ID
  	2.data			数据
  	3.size			大小
  Output:			无
  Return:			无
*************************************************/
static void save_msg(uint32 ID, int8 * data, int size)
{
	uint32 Len;	
	REC_BULLETIN_HEAD 	rec_bulletin_head;			// 信息头	
	char des[14] = {0xff,0xff};
	char tmp_des[14] = {0};
	static uint32 sta_id = 0;
	
	if (NULL == data || 0 == size)
	{
		return;
	}
	memset(&rec_bulletin_head, 0, sizeof(REC_BULLETIN_HEAD));

	// 信息ID号
	memcpy(&rec_bulletin_head.ID, data, 4);
	if (sta_id == rec_bulletin_head.ID)
	{
		log_printf("sta_id == rec_bulletin_head.ID: %d \n", sta_id);
		return;
	}
	sta_id = rec_bulletin_head.ID;
	log_printf("rec_bulletin_head.ID : %d\n", rec_bulletin_head.ID);

	// 类型
	rec_bulletin_head.BulletinType = *(int8 *)((int8*)data + 4);
	log_printf("rec_bulletin_head.BulletinType : %d\n", rec_bulletin_head.BulletinType);
	
	// 标题
	memcpy(rec_bulletin_head.title, (int8*)data + 4 + 4, 40);
	//log_printf("rec_bulletin_head.title : %s\n", rec_bulletin_head.title);
	
	// 文字部分长度
	memcpy(&Len, (int8*)data+ 4 + 4 + 40, 4);	
	rec_bulletin_head.CharLen = Len;
	//log_printf("rec_bulletin_head.CharLen : %d\n", rec_bulletin_head.CharLen);
	
	// 图片部分长度
	memcpy(&Len, (int8*)data+ 4 + 4 + 40 + 4, 4);
	rec_bulletin_head.ImageLen = Len;
    //log_printf("rec_bulletin_head.ImageLen : %d\n", rec_bulletin_head.ImageLen);

	// 声音部分长度
	memcpy(&Len, (int8*)data+ 4 + 4 + 40 + 4 + 4, 4);
	rec_bulletin_head.VoiceLen = Len;
	
	// 繁体字不行
	#if 0
	GuiDspGetIDString(SID_Info_Centre_Des, tmp_des, 10);
	Unicode2GBKtemp(tmp_des, des);
	#endif
	//log_printf("save_msg end \n");
	storage_write_msg(&rec_bulletin_head, (char*)data + sizeof(REC_BULLETIN_HEAD), des);
	show_sys_event_hint(AS_NEW_MSG);
	sys_sync_hint_state();
}

/*************************************************
  Function:			save_short_msg
  Description: 		保存短信(用户发往中心,由中心转发给住户)
  Input: 			
  	1.rec_short_msg	短信结构
  Output:			无
  Return:			无
*************************************************/
static void save_short_msg(REC_SHORT_MSG *rec_short_msg)
{
	uint8 des[14];
	REC_BULLETIN_HEAD 	rec_bulletin_head;
	
	if (NULL == rec_short_msg)
	{
		return;
	}
	
	memset(&rec_bulletin_head, 0, sizeof(REC_BULLETIN_HEAD));
	rec_bulletin_head.CharLen = strlen(rec_short_msg->Content);
	memset(des, 0, sizeof(des));
	// 繁体字不行
	rec_bulletin_head.title[0] = 0xff;
	#if 0
	GuiDspGetIDString(SID_Info_Sms, des, 6);
	Unicode2GBKtemp(des, rec_bulletin_head.title);
	#endif
	memset(des, 0, sizeof(des));
	rec_bulletin_head.BulletinType = 0;
	memcpy(des, rec_short_msg->SendNo, 14);
	//Gbk2Unicode(des, rec_short_msg->SendNo);

	log_printf("des: %s, rec_short_msg->Content: %s,rec_short_msg->SendNo: %s\n", des, rec_short_msg->Content, rec_short_msg->SendNo);
	storage_write_msg(&rec_bulletin_head, rec_short_msg->Content, des);
	show_sys_event_hint(AS_NEW_MSG);
	sys_sync_hint_state();
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
static void* update_msg_recv_state(uint32 ID, int Total, int CurNum, int size)
{	
	if (Total == CurNum)
	{
		//set_public_state(PUB_STATE_XINXI, 1);
	}
}

/*************************************************
  Function:			recv_msg_start
  Description:		接收信息开始(采用多包接收)
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
static void recv_msg_start(const PRECIVE_PACKET recPacket)
{
	uint32 ID = 0;									// 会话ID
	char * data = (char*)&ID;						// 会话ID
	ID = net_start_multipacket_recive(recPacket->address, recPacket->port, 0, save_msg, update_msg_recv_state);
	if (ID == 0)
	{
		//log_printf("net_start_multipacket_recive = 0\n");
		net_send_echo_packet(recPacket, ECHO_BUSY, data, 4);	
		return;
	} 
	//log_printf("net_start_multipacket_recive = id:%d\n", ID);
	net_send_echo_packet(recPacket, ECHO_OK, data, 4);
	return;
}

/*************************************************
  Function:			msg_distribute
  Description:		接收短信发送命令应答处理函数
  Input:
  	1.packet		接收包
  Output:			无
  Return:			合法命令TRUE, 非法命令FALSE
  Others:
**************************************************/
int msg_distribute(const PRECIVE_PACKET recPacket)
{
	char net_cmd[2];								// 短信转发网络命令
	REC_SHORT_MSG 		rec_short_msg;				// 接受短信结构
	uint32 				validate_code;				// 验证码
	
	PNET_HEAD head = (PNET_HEAD)(recPacket->data + MAIN_NET_HEAD_SIZE);	// 包数据
	int cmd = head->command | SSC_INFO << 8;
	net_cmd[0] = SSC_INFO;							// 网络命令(子系统号+命令)
	net_cmd[1] = head->command;
	
	log_printf("msg_distribute cmd: %x \n", cmd);
	switch (cmd)
	{
		case CMD_SEND_INFO:							// 发送信息				
			recv_msg_start(recPacket);
			break;				
			
		case CMD_SEND_SHORT_MSG:					// 短信发送，无应答			
			memcpy(&rec_short_msg, recPacket->data + NET_HEAD_SIZE, REC_SHORT_MSG_SIZE);
			net_send_echo_packet(recPacket, ECHO_OK, NULL, 0);	
			save_short_msg(&rec_short_msg);
			break;	
			
		case CMD_SEND_MSG_VALIDATE:					// 信息功能验证码，无应答
			memcpy(&validate_code, recPacket->data + NET_HEAD_SIZE, 4);
			log_printf("validate_code : 0x%x\n", validate_code);
			net_send_echo_packet(recPacket, ECHO_OK, NULL,0);
			//save_validate_code(validate_code);
			storage_set_validate_code(validate_code);
			break;	
			
		default:
			return FALSE;							// 无效命令
	}
	
	return TRUE;
}

/*************************************************
  Function:			msg_responsion
  Description:		接收短信应答命令应答处理函数
  Input:
  	1.recPacket		网络接收包
  	2.SendPacket	网络发送包
  Output:			无
  Return:			无
  Others:
**************************************************/
void  msg_responsion(const PRECIVE_PACKET recPacket, const PSEND_PACKET SendPacket)
{
	return;
}

