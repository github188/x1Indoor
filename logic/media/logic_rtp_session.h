/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_rtp_session.h
  Author:   	陈本惠
  Version:  	1.0
  Date: 		2014-12-11
  Description:  媒体rtp 处理
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _RTP_SESSION_H_
#define _RTP_SESSION_H_

#include "logic_include.h"
#include "logic_media_core.h"
#include "rtp/rtp_api.h"
#include "rtp/rtp_highlevel.h"

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN			1234
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN				4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER				LITTLE_ENDIAN
#endif

// RTP PAGE
#define RTP_H264_SIZE			1400
#define RTP_FU_A_SIZE			14
#define RTP_NAUL_SIZE			12

// RTP 分包方式
#define NONE					0
#define PPP						1
#define IDR						5
#define SEI						6
#define SPS						7
#define PPS						8

#define STAP_A					24
#define STAP_B					25
#define MTAP16					26
#define MTAP24					27
#define FU_A					28
#define FU_B					29

//海思格式定义
#define PAYLOAD_G711A			8//rtpPayloadPCMA
#define PAYLOAD_G711U			0//rtpPayloadPCMU
#define PAYLOAD_G728			15//rtpPayloadG728
#define PAYLOAD_G729			18//rtpPayloadG729


#define PAYLOAD_H261			31//rtpPayloadH261 
#define PAYLOAD_H263			34//rtpPayloadH263 
#define PAYLOAD_H264			98//rtpPayLoadDyn98
#define PAYLOAD_MPEG4			100//rtpPayLoadDyn100


#if (BYTE_ORDER == BIG_ENDIAN)
typedef struct 
{
	unsigned char F:1;				
	unsigned char NRI:2;
	unsigned char Type:5;
}NALU_HEAD,*PNALU_HEAD;

typedef struct
{
	unsigned char F:1;				
	unsigned char NRI:2;
	unsigned char Type:5;
}FU_INDICATOR,*PFU_INDICATOR;

typedef struct 
{
	unsigned char S:1;				
	unsigned char E:1;
	unsigned char R:1;
	unsigned char Type:5;
}FU_HEAD,*PFU_HEAD;
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN)
typedef struct 
{	
	unsigned char Type:5;
	unsigned char NRI:2;
	unsigned char F:1;				
}NALU_HEAD,*PNALU_HEAD;

typedef struct
{
	unsigned char Type:5;
	unsigned char NRI:2;
	unsigned char F:1;				
}FU_INDICATOR,*PFU_INDICATOR;

typedef struct 
{
	unsigned char Type:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;				
}FU_HEAD,*PFU_HEAD;
#endif

#define MULTCAST_TTL			20
#define UNICAST_TTL				0
#define MAX_USER_NAME			20	

#if 0
#ifdef _AU_PROTOCOL_
#define MEDIA_AUDIO_PORT		24020				//音频端口
#define MEDIA_VIDEO_PORT		24010				//视频端口
#else
#define MEDIA_AUDIO_PORT		31420				//音频端口
#define MEDIA_VIDEO_PORT		31410				//视频端口
#endif
#define MEDIA_VIDEOL_PORT		31430
#endif


#define RTP_SESSION_SEND		0x0001
#define RTP_SESSION_RECV		0x0010

struct _RtpSession;

typedef void (*MSRTPDataFunc)(struct _RtpSession *f, mblk_t * arg);

typedef struct _RtpSession 
{	
	unsigned int Type;
	pthread_mutex_t lock;	
	unsigned int Count;
	context RtpCid;
	unsigned int SendIP;
	unsigned int RecvIP;
	unsigned int RecvPort;
	unsigned char Pt;
 	socktype RtpSock;
	socktype RtcpSock;  
	char UserName[MAX_USER_NAME];
	struct timeval TsTime;
	unsigned long long tsinc;
	MSRTPDataFunc func;
	void* private;
}RtpSession;

#if 0
/*************************************************
  Function:    		msleep
  Description: 		停止毫秒 
  Input: 
	1.ms			毫秒数
  Output:			无
  Return:			无
  Others:
*************************************************/
void msleep(int ms);
#endif
/*************************************************
  Function:    		rtp_get_addr_count
  Description:
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int rtp_get_addr_count(context RtpCid);

/*************************************************
  Function:    		rtp_session_open
  Description: 	
  Input: 		
  	1.session
  	2.type
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_open(RtpSession * session, int type);

/*************************************************
  Function:    		rtp_session_close
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_close(RtpSession * session);

/*************************************************
  Function:    		rtp_session_sendaddr_add
  Description: 		增加发送地址
  Input:
  	1.IP			IP地址
  	2.Port			端口
  Output:			无
  Return:			
  Others:
*************************************************/
int rtp_session_sendaddr_add(RtpSession * session, unsigned int IP, unsigned int Port);

/*************************************************
  Function:    		rtp_session_sendaddr_dec
  Description: 		去除发送地址
  Input:
  	1.IP			IP地址
  	2.Port			端口
  Output:			无
  Return:			无
  Others:
*************************************************/
int rtp_session_sendaddr_dec(RtpSession * session, unsigned int IP, unsigned int Port);

/*************************************************
  Function:		rtp_session_send
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int rtp_session_send(RtpSession * session, mblk_t * stream);

/*************************************************
  Function:    		rtp_session_set_payload_type
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_payload_type(RtpSession *session, unsigned char pt);

/*************************************************
  Function:    		rtp_session_set_recv_addr
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_recv_port(RtpSession *session, unsigned int Port);

/*************************************************
  Function:    		rtp_session_set_username
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_username(RtpSession *session, char* username);

/*************************************************
  Function:    		rtp_session_set_private
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_private(RtpSession *session, void* private);

/*************************************************
  Function:    		rtp_session_recvaddr_add
  Description: 		增加接收地址
  Input:
  	1.IP			IP地址
  	2.Port			端口
  Output:			无
  Return:			
  Others:
*************************************************/
int rtp_session_recvaddr_add(RtpSession * session, unsigned int IP, MSRTPDataFunc func);

/*************************************************
  Function:    		rtp_session_recvaddr_dec
  Description: 		删除接收地址
  Input:
  	1.IP			IP地址
  	2.Port			端口
  Output:			无
  Return:			
  Others:
*************************************************/
int rtp_session_recvaddr_dec(RtpSession * session);

/*************************************************
  Function:    		rtp_session_new
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
RtpSession *rtp_session_new(int port);

/*************************************************
  Function:    		ms_rtp_session_Init
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
void ms_rtp_session_Init(void);

/*************************************************
  Function:    		rtp_set_naluflag
  Description: 		设置标志位
  Input:
  	1.IP			IP地址
  	2.Port			端口
  Output:			无
  Return:			
  Others:
*************************************************/
void rtp_set_naluflag(int flag);

/*************************************************
  Function:    		rtp_set_naluflag
  Description: 		设置标志位
  Input:
  	1.flag			0: 停止视频处理
  Output:			无
  Return:			
  Others:
*************************************************/
void rtp_set_videoflag(int flag);

#endif

