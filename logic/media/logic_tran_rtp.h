/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_tran_rtp.h
  Author:		罗发禄
  Version:		V2.0
  Date: 		2014-03-28
  Description:	RTP应用封装程序
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#ifndef _AU_TRAN_RTP_H_
#define _AU_TRAN_RTP_H_

#include "logic_rtp_session.h"
#include "logic_include.h"

typedef struct _RTP_OPEN_S
{
	unsigned int RecvIp;
    unsigned int Port;
	unsigned char Pt;
	char UserName[MAX_USER_NAME];
}RTP_OPEN_S;

typedef struct _RTP_ADDRESS_S
{
	unsigned int Ip;
    unsigned int Port;
}RTP_ADDRESS_S;

#define MS_RTP_SEND_V_OPEN		MS_MEDIA_METHOD(MS_RTP_SEND_V_ID, 0x00, RTP_OPEN_S)
#define MS_RTP_SEND_V_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_RTP_SEND_V_ID, 0x01)
#define MS_RTP_ADDRESS_V_ADD	MS_MEDIA_METHOD(MS_RTP_SEND_V_ID, 0x02, RTP_ADDRESS_S)
#define MS_RTP_ADDRESS_V_DEC	MS_MEDIA_METHOD(MS_RTP_SEND_V_ID, 0x03, RTP_ADDRESS_S)
#define MS_RTP_ADDRESS_V_COUNT	MS_MEDIA_METHOD_NO_ARG(MS_RTP_SEND_V_ID, 0x04)

#define MS_RTP_SEND_A_OPEN		MS_MEDIA_METHOD(MS_RTP_SEND_A_ID, 0x00, RTP_OPEN_S)
#define MS_RTP_SEND_A_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_RTP_SEND_A_ID, 0x01)
#define MS_RTP_ADDRESS_A_ADD	MS_MEDIA_METHOD(MS_RTP_SEND_A_ID, 0x02, RTP_ADDRESS_S)
#define MS_RTP_ADDRESS_A_DEC	MS_MEDIA_METHOD(MS_RTP_SEND_A_ID, 0x03, RTP_ADDRESS_S)
#define MS_RTP_ADDRESS_A_COUNT	MS_MEDIA_METHOD_NO_ARG(MS_RTP_SEND_A_ID, 0x04)

#define MS_RTP_RECV_A_OPEN		MS_MEDIA_METHOD(MS_RTP_RECV_A_ID, 0x00, RTP_OPEN_S)
#define MS_RTP_RECV_A_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_RTP_RECV_A_ID, 0x01)
#define MS_RTP_ADDR_A_RECV_ADD	MS_MEDIA_METHOD(MS_RTP_RECV_A_ID, 0x02, RTP_ADDRESS_S)
#define MS_RTP_ADDR_A_RECV_DEC	MS_MEDIA_METHOD(MS_RTP_RECV_A_ID, 0x03, RTP_ADDRESS_S)

#define MS_RTP_RECV_V_OPEN		MS_MEDIA_METHOD(MS_RTP_RECV_V_ID, 0x00, RTP_OPEN_S)
#define MS_RTP_RECV_V_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_RTP_RECV_V_ID, 0x01)
#define MS_RTP_ADDR_V_RECV_ADD	MS_MEDIA_METHOD(MS_RTP_RECV_V_ID, 0x02, RTP_ADDRESS_S)
#define MS_RTP_ADDR_V_RECV_DEC	MS_MEDIA_METHOD(MS_RTP_RECV_V_ID, 0x03, RTP_ADDRESS_S)
#endif

