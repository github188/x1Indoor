/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_rtp_session.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#include "logic_media.h"
#include "logic_rtp_session.h"

#define SAVE_RECV_VIDEO			0
#define SAVE_RECV_AUDIO     	0
#define _STATIC_SEND_BUF_		0	

#define MAX_RTP_DATA_SIZE 		(1024*5)

#define MAX_SEND_SIZE			50
#define MAX_SENDONEBUF_SIZE		1460
#define MAX_RTP_SENDDATA_SIZE 	(MAX_SENDONEBUF_SIZE*MAX_SEND_SIZE)

#define tv2dbl(tv) ((tv).tv_sec + (tv).tv_usec / 1000000.0)

struct evt_queue_elt {
	context cid;
	rtp_opaque_t event_opaque;
	double event_time;
	struct evt_queue_elt *next;
};

typedef struct _rtp_session_recv
{
	MSThread mMSThread;
	int mMaxfds;
	pthread_mutex_t lockprocess;
	MSList * List;
	char pRecvBuff[MAX_RTP_DATA_SIZE];
}Rtp_Session_Recv;

static struct evt_queue_elt * evt_queue = NULL;
static RtpSession mRtpSession[3];
static Rtp_Session_Recv mRtpRecvSession;
#if _STATIC_SEND_BUF_
static char mRtpSessionSendBuf[MAX_RTP_SENDDATA_SIZE+10];
static int mRtpSessionSendbufIndex = 0;
#endif

#if SAVE_RECV_VIDEO
FILE *pH264File = NULL;
#endif

#if SAVE_RECV_AUDIO
FILE *pAlawRecFile = NULL;
#endif

#define MULT_VIDEO_BUF_SIZE  80000
static char g_multbuf[MULT_VIDEO_BUF_SIZE];

static uint32 g_PrePackTs = 0;			// 前一包时间戳
static char *g_PreBufPos = NULL;		// 已保存数据指针	
static uint32 g_PreBufLen = 0;			// 已保存数据长度
static uint8 g_PrePackPt = 0;			// 前一包数据帧类型			
static uint8 g_CurPackPt = 0;			// 该一包数据帧类型			

static uint8 g_startrecv = 0;
static uint8 g_endbit = 0;
static uint8 g_StartDeal = 0;   		// 开始视频解码

#if 0
/*************************************************
  Function:    		IPtoUlong
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
unsigned long IPtoUlong(char* ip)
{
	unsigned long uIp=0;
	char *p,*q;
	char temp[10];
	int num;
	p = ip;
	for(;;)
	{
		q = (char *)strchr(p,'.');
		if(q == NULL)
			break;
		num = q-p;
		memset(temp,0,10);
		memcpy(temp,p,num);
		uIp = uIp << 8;
		uIp += atoi(temp);
		p=q+1;
	}
	uIp = uIp << 8;
	uIp += atoi(p);
	return uIp;
}
/*************************************************
  Function:    		dbl2tv
  Description: 		 
  Input: 
	1.d			
  Output:			无
  Return:			无
  Others:
*************************************************/
static struct timeval dbl2tv(double d) 
{
	struct timeval tv;

	tv.tv_sec = (long) d;
	tv.tv_usec = (long) ((d - (long) d) * 1000000.0);
	
	return tv;
}
#endif

/*************************************************
  Function:    		insert_in_evt_queue
  Description: 		 
  Input: 
	1.elt
  Output:			无
  Return:			无
  Others:
*************************************************/
static void insert_in_evt_queue(struct evt_queue_elt *elt)
{
	if (evt_queue == NULL || elt->event_time < evt_queue->event_time) 
	{
		elt->next = evt_queue;
		evt_queue = elt;
	}
	else 
	{
		struct evt_queue_elt *s = evt_queue;
		while (s != NULL) 
		{
			if (s->next == NULL || elt->event_time < s->next->event_time) 
			{
				elt->next = s->next;
				s->next = elt;
				break;
			}
			s = s->next;
		}
	}
}

/*************************************************
  Function:    		RTPSchedule
  Description: 		 
  Input: 
	1.cid
	2.opaque
	3.tp
  Output:			无
  Return:			无
  Others:
*************************************************/
void RTPSchedule(context cid, rtp_opaque_t opaque, struct timeval *tp)
{
	struct evt_queue_elt *elt;

	elt = (struct evt_queue_elt *) ms_new(struct evt_queue_elt,1);
	if (elt == NULL)
		return;

	elt->cid = cid;
	elt->event_opaque = opaque;
	elt->event_time = tv2dbl(*tp);
	
	insert_in_evt_queue(elt);

	return;
}


/*************************************************
  Function:    		msleep
  Description: 		停止毫秒 
  Input: 
	1.ms			毫秒数
  Output:			无
  Return:			无
  Others:
*************************************************/
static void msleep(int ms)
{ 
	usleep(ms*1000);
}	

/*************************************************
  Function:    		get_bitclk
  Description: 		
  Input:
  	1.pt			rtp有效载荷类型
  Output:			无
  Return:			带宽
  Others:
*************************************************/
static int get_bitclk(int pt)
{
    switch (pt)
    {
	    case PAYLOAD_G711A:
	        return 8000;

	    case PAYLOAD_H264:
	        return 90000;

	    default:
	        return 8000;
    }
}

/*************************************************
  Function:    		get_bandwidth
  Description: 		获得带宽
  Input:
  	1.pt			rtp有效载荷类型
  Output:			无
  Return:			带宽
  Others:
*************************************************/
static double get_bandwidth(int pt)
{
    switch (pt)
    {
	    case PAYLOAD_G711A:
	        return 64*2;

	    case PAYLOAD_H264:
	        return 512;

	    default:
	        return 120;
    }
}

/*************************************************
  Function:    		is_multcast_address
  Description: 		判断是否组播地址
  Input:
  	1.IP			IP地址
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
static int is_multcast_address(unsigned int ip)
{
    return (((ip >> 28) & 0xf) == 0xe);
}

/*************************************************
  Function:    		calc_tsinc
  Description:
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int calc_tsinc(struct timeval * time0, double rate)
{
    if (NULL == time0)
    {
        return 80;
    }

    struct timeval tp, temp;
    gettimeofday(&tp, NULL);
    temp = tp;
    tp.tv_sec = tp.tv_sec - time0->tv_sec;
    if (tp.tv_usec>time0->tv_usec)
    {
        tp.tv_usec = tp.tv_usec-time0->tv_usec;
    }
    else
    {
        tp.tv_usec = 1000000 + tp.tv_usec-time0->tv_usec;
        tp.tv_sec--;
    }
    * time0 = temp;
    return (tp.tv_sec*1000000+tp.tv_usec) * rate/1000000.0;
}

/*************************************************
  Function:    		setup_connection
  Description: 	
  Input: 		
  	1.username	
  	2.LocalPort	
  	3.bandwidth 
  Output:		
  	1.cid			RTP ID
  Return:		
  Others:
*************************************************/
static int setup_connection(context *cid, char * username, char* RecvIp, uint16 LocalPort, int bandwidth)
{
    rtperror err;
    char cname[255];
    person uid, conid;

    err = RTPCreateExt(cid);
    if (err != RTP_OK)
    {
        goto error;
    }

    err = RTPSessionSetReceiveAddr(*cid, RecvIp, LocalPort);

    err = RTPMemberInfoSetSDES(*cid, 0, RTP_MI_CNAME, username);
    if (err != RTP_OK)
    {
        goto error;
    }

    err = RTPMemberInfoSetSDES(*cid, 0, RTP_MI_NAME, "冠林");
    if (err != RTP_OK)
    {
        goto error;
    }

    struct timeval tp;
    gettimeofday(&tp, NULL);
    srand(tp.tv_usec);
    conid = rand();
    err = RTPSessionAddToContributorList(*cid, (int) conid);
    if (err)
    {
        goto error;
    }
    err = RTPSessionGetUniqueIDForCSRC(*cid, conid, &uid);
    if (err)
    {
        goto error;
    }
    sprintf(cname, "Contributor %d", (int) conid);
    err = RTPMemberInfoSetSDES(*cid, uid, RTP_MI_CNAME, cname);
    if (err)
    {
        goto error;
    }
    err = RTPMemberInfoSetSDES(*cid, uid, RTP_MI_NAME, cname);
    if (err)
    {
        goto error;
    }

    RTPSessionSetBandwidth(*cid, bandwidth, 0.05);
    err = RTPOpenConnection(*cid);
    if (err)
    {
        goto error;
    }
    return true;

error:
    if (*cid != -1)
    {
        RTPDestroy(*cid);
        *cid = - 1;
    }

    return false;
}

/*************************************************
  Function:    		rtp_get_addr_count
  Description:
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int rtp_get_addr_count(context RtpCid)
{
    if (RtpCid == -1)
    {
        return 0;
    }
	
    return RTPSessionGetSendAddrCount(RtpCid);
}

/*************************************************
  Function:		rtp_session_disconnection
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int rtp_session_disconnection(RtpSession * session)
{
	ms_return_val_if_fail(session, -1);

    if (session->RtpCid != -1)
    {
        context cid = session->RtpCid;

        session->RtpCid = -1;
        session->Pt = 0;
        session->RtpSock = -1;
        session->RtcpSock = -1;
		session->SendIP  = 0;
		session->RecvIP  = 0;
		session->RecvPort = 0;
        session->TsTime.tv_sec = 0;
        session->TsTime.tv_usec = 0;
		msleep(100);
        RTPCloseConnection(cid, "Goodbye!");
        msleep(100);
        RTPDestroy(cid);
        msleep(100);
		return 0;
    }

	return -1;
}

/*************************************************
  Function:		rtp_session_connection
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static int rtp_session_connection(RtpSession * session)
{
	int ret = -1;
	
	ms_return_val_if_fail(session, -1);
    if (setup_connection(&session->RtpCid, session->UserName, NULL,  session->RecvPort, get_bandwidth(session->Pt)))
    {
        RTPSessionGetRTPSocket(session->RtpCid, &session->RtpSock);
        RTPSessionGetRTCPSocket(session->RtpCid, &session->RtcpSock);

		int vol;
		socklen_t len = sizeof(int);

		vol = 1024*512;
		setsockopt(session->RtpSock, SOL_SOCKET, SO_SNDBUF, &vol, (socklen_t)len);
		getsockopt(session->RtpSock, SOL_SOCKET, SO_SNDBUF, &vol, (socklen_t*)&len);
		getsockopt(session->RtpSock, SOL_SOCKET, SO_REUSEADDR, &vol, (socklen_t*)&len);
        session->TsTime.tv_sec = 0;
        session->TsTime.tv_usec = 0;
        ret = 0;
    }
	
    return ret;
}

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
int rtp_session_open(RtpSession * session, int type)
{
	int ret = 0;
	ms_return_val_if_fail(session, -1);
	
	ms_media_lock(session);
	if (session->RtpCid == -1)
	{
		session->Type = type;
		session->RecvIP = 0;
		session->RtpSock = -1;
		session->RtcpSock = -1;
		ret = rtp_session_connection(session);
	}
	if (0 == ret)
	{
		session->Count++;
	}	

	#if SAVE_RECV_VIDEO
	if (pH264File == NULL)
	{
    	pH264File = fopen("/mnt/nand1-2/LYLY/rtp_recv_video.h264", "wb");
	}
	#endif
	
	#if SAVE_RECV_AUDIO
	if (pAlawRecFile == NULL)
	{
		pAlawRecFile = fopen("/mnt/nand1-2/rtp_recv_audio.alaw", "wb");
	}
	#endif
	ms_media_unlock(session);

	return ret;
}

/*************************************************
  Function:    		rtp_session_close
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_close(RtpSession * session)
{
	int ret = -1;
	
	ms_return_val_if_fail(session, -1);
	ms_media_lock(session);
	if (session->Count > 0)
	{
		session->Count--;
	}

	if (0 == session->Count)
	{
		ret = rtp_session_disconnection(session);	

		#if SAVE_RECV_VIDEO
		if (pH264File != NULL)
		{
			FSFlush(pH264File);
			fclose(pH264File);
			pH264File = NULL;			
		}
		
		#endif

		#if SAVE_RECV_AUDIO
		if (pAlawRecFile != NULL)
		{
			FSFlush(pH264File);
			fclose(pAlawRecFile);
			pAlawRecFile = NULL;
		}
		#endif
	}
	ms_media_unlock(session);

	return ret;
}

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
int rtp_session_sendaddr_add(RtpSession * session, unsigned int IP, unsigned int Port)
{
    ms_return_val_if_fail(session, -1);

	if (session->RtpCid == -1)
	{
		return -1;
	}
	
	ms_media_lock(session);
    unsigned char ttl = UNICAST_TTL;
    if (is_multcast_address(IP))
    {
        ttl = MULTCAST_TTL;
    }

    struct in_addr in;
    in.s_addr = htonl(IP);
    char remote[16];

    sprintf(remote, "%s", inet_ntoa(in));
    int ret = RTPSessionAddSendAddr(session->RtpCid, remote, Port, ttl);
	if (ret == RTP_OK)
	{
		session->SendIP++;
	}
	else
	{
	}
	ms_media_unlock(session);
    return 0;
}

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
int rtp_session_sendaddr_dec(RtpSession * session, unsigned int IP, unsigned int Port)
{
    ms_return_val_if_fail(session, -1);

	if (session->RtpCid == -1)
	{
		return -1;
	}

	ms_media_lock(session);
	msleep(50);
    uint8 ttl = UNICAST_TTL;
    if (is_multcast_address(IP))
    {
        ttl = MULTCAST_TTL;
    }

    struct in_addr in;
    in.s_addr = htonl(IP);
    char remote[16];
    sprintf(remote, "%s", inet_ntoa(in));
    int ret = RTPSessionRemoveSendAddr(session->RtpCid, remote, Port, ttl);
	if (ret != RTP_OK)
	{
	}
	if (session->SendIP > 0)
	{
		session->SendIP--;
	}
	usleep(1000*500);
	ms_media_unlock(session);
	
	return 0;
}

/*************************************************
  Function:    		rcf8349_check_head	
  Description:		
  Input: 			无
  Output:			无
  Return:			0 不带头 3-类型1  4-类型2	
  Others:
*************************************************/
static int rcf8349_check_head(char* data)
{
	if (data)
	{
		if (*data != 0x00)
		{
			return 0;
		}

		if (*(data+1) != 0x00)
		{
			return 0;
		}

		if (*(data+2) == 0x01)
		{
			return 3;
		}
		else
		{
			if (*(data+2) == 0x00 && *(data+3) == 0x01)
			{
				return 4;
			}
		}
	}

	return 0;
}

#if _STATIC_SEND_BUF_
/*************************************************
  Function:		rtp_session_sendbuf_add
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
static void rtp_session_sendbuf_add(int size)
{
	#if 0
	if ((mRtpSessionSendbufIndex + size) > MAX_RTP_SENDDATA_SIZE)
	{
		mRtpSessionSendbufIndex = 0;
	}
	else
	{
		mRtpSessionSendbufIndex += size; 
	}
	#else
	mRtpSessionSendbufIndex++;
	if (mRtpSessionSendbufIndex >= MAX_SEND_SIZE)
	{
		mRtpSessionSendbufIndex = 0;
	}
	#endif
	
}
#endif

#if (SAVE_RECV_VIDEO || SAVE_RECV_AUDIO)
/*************************************************
  Function:    		rtp_session_set_private
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
static void RTP_Recv_SaveFie(FILE *pH264File, mblk_t *arg)
{

    if (!pH264File)
    {
        return ;
    }
    fwrite(arg->address, arg->len, 1, pH264File);
    fflush(pH264File);
}
#endif

#if 0  // del by chenbh  2015-06-15 解决发送数据错误
/*************************************************
  Function:		rtp_session_rcf8349
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void rtp_session_rcf8349(RtpSession * session, char* stream, int size, int maker, int tsinc, int delay)
{	
	int tmp = size;
	char* addr = stream;
	int forbidden_bit, nal_reference_idc, nal_unit_type;
	PFU_HEAD fu_head;
	PFU_INDICATOR fu_ind;
	int dec, index, pack = 0;
	
	int head = rcf8349_check_head(stream);
	if (0 == head)
	{
		return;
	}
	
	tmp -= head;
	
	forbidden_bit = addr[head] & 0x80; 
	nal_reference_idc = addr[head] & 0x60;
	nal_unit_type = (addr[head]) & 0x1f;

	index = head-1;
	
	while (tmp > 0)
	{	
		fu_ind = (PFU_INDICATOR)&addr[index];
		fu_head = (PFU_HEAD)&addr[index+1];


		fu_ind->F = forbidden_bit;
		fu_ind->NRI = nal_reference_idc>>5;
		fu_ind->Type = FU_A;
		
		if (tmp < RTP_H264_SIZE)
		{
			fu_head->E = 1;
			fu_head->R = 0;
			fu_head->S = 0;
			fu_head->Type = nal_unit_type;
			maker = 1;	
			RTPSend(session->RtpCid, tsinc, maker, session->Pt, (int8*)&(addr[index]), tmp+2);
			if (delay > 0)
			{
				usleep(delay);
			}
			tmp	= 0;
			break;
		}
		else
		{
			if (pack == 0)
			{
				pack = 1;
				fu_head->E = 0;
				fu_head->R = 0;
				fu_head->S = 1;
				fu_head->Type = nal_unit_type;
			}
			else
			{	
				fu_head->E = 0;
				fu_head->R = 0;
				fu_head->S = 0;
				fu_head->Type = nal_unit_type;
			}
			// 第一包本身带有nalu头 只要在加一个字节的PFU_INDICATOR 头即可
			if (fu_head->S == 1) 
			{
				dec = (RTP_H264_SIZE+1);
			}
			else // 其他必须在前面填充两字节 nalu和PFU_INDICATOR头
			{
				dec = (RTP_H264_SIZE+2);
			}	
			maker = 0;
			RTPSend(session->RtpCid, tsinc, maker, session->Pt, (int8*)&(addr[index]), dec);
			index = index + (dec-2);
			tmp -= RTP_H264_SIZE;
		}

		if (delay > 0)
		{
			usleep(delay);
		}
	}
}

#else // add by chenbh  2015-06-15 解决发送数据错误
/*************************************************
  Function:		rtp_session_rcf8349
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
void rtp_session_rcf8349(RtpSession * session, char* stream, int size, int maker, int tsinc, int delay)
{	
	char rtpBuf[RTP_H264_SIZE];
	int NalHeadByte,FUindicator,FUHeader;
	int payloadLength = 0, bytesToSend = 0;
	int FU_A_Type = 28;

	int head = rcf8349_check_head(stream);
	if (0 == head)
	{
		return;
	}

	char * pData = (char *)(stream+head);
	bytesToSend = size - head;
	NalHeadByte = pData[0];
	FUindicator = NalHeadByte &0xE0;
	FUindicator |= FU_A_Type;

	FUHeader = NalHeadByte & 0x1F;
	
	int firstPacket = 0;
	int lastPacket = 0;
	int count =1;
	int ret=0;
	while(bytesToSend)
	{

		if (count==1)
			firstPacket =1;
		else
			firstPacket =0;

		if (bytesToSend <= RTP_H264_SIZE-2)
		{
			payloadLength = bytesToSend;
			lastPacket =1;
		}
		else
		{
			payloadLength = RTP_H264_SIZE-2;
			lastPacket =0;
		}
		if (firstPacket ==1 )
		{
			FUHeader = (NalHeadByte &0x1F)|0x80; // 1000 000 
		}
		if (lastPacket == 1)
		{		
			FUHeader = (NalHeadByte &0x1F)|0x40;// 0100 0000
		}

		if (firstPacket==0 && lastPacket==0)
		{
			FUHeader = (NalHeadByte &0x1F);
		}
		
		rtpBuf[0] = FUindicator;
		rtpBuf[1] = FUHeader;
		if (firstPacket == 1)
		{
			memcpy(rtpBuf+2, pData+1, payloadLength-1);
		}
		else
		{
			memcpy(rtpBuf+2, pData, payloadLength);
		}

		if (firstPacket == 1)
		{
			maker =0;
			ret = RTPSend(session->RtpCid, tsinc, maker, session->Pt, rtpBuf, payloadLength-1+2);		
			if ( ret != RTP_OK )
			{
				while (ret != RTP_OK )
				{
					usleep(100*10);			
					ret =RTPSend(session->RtpCid, tsinc, maker, session->Pt, rtpBuf, payloadLength-1+2);		
					if ( ret ==RTP_BAD_ADDR || ret == RTP_BAD_PORT )
						break;
						
				}
			}
		}
		else
		{
			if (lastPacket ==1)
				maker =1;
			else
				maker =0;
			ret = RTPSend(session->RtpCid, 0, maker, session->Pt, rtpBuf, payloadLength+2);	
			if ( ret !=RTP_OK )
			{
				while (ret!=RTP_OK )
				{
					usleep(100*10);			
					ret =RTPSend(session->RtpCid, tsinc, maker, session->Pt, rtpBuf, payloadLength+2);	
					if ( ret ==RTP_BAD_ADDR || ret == RTP_BAD_PORT )
					break;
				}
			}
		}

		pData += payloadLength;
		bytesToSend -= payloadLength;
		count++;
	}
}
#endif

/*************************************************
  Function:		rtp_session_send
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int rtp_session_send(RtpSession * session, mblk_t * stream)
{ 	
	rtperror echo;
	ms_return_val_if_fail(session, -1);
	ms_return_val_if_fail(stream, -1);
	ms_return_val_if_fail(stream->address, -1);
	
    if ( session->RtpCid == -1)
    {
        return -1;
    }
	
    int tsinc, headlen;                                      // 时间增量
    int maker = stream->marker;

	if (session->TsTime.tv_sec==0 && session->TsTime.tv_usec==0)
    {
        gettimeofday(&session->TsTime, NULL);
		if (session->Pt == PAYLOAD_H264)
		{
			tsinc = 90000;	
		}
		else
		{
			maker = 1;
        	tsinc = 80;
		}
    }
    else
    {
        tsinc = calc_tsinc(&session->TsTime, get_bitclk(session->Pt)); 
    }

	//if (maker == 1)
	{
		session->tsinc = tsinc;
	}

	if (stream->len > 0)
	{		
		if (stream->len > RTP_H264_SIZE)
		{
			rtp_session_rcf8349(session, stream->address, stream->len, maker, tsinc, stream->delay);
		}
		else
		{
			if (MS_RTP_SEND_V_ID == stream->id)
			{
				headlen = rcf8349_check_head(stream->address);
			}
			else
				headlen = 4;
			
			#if _STATIC_SEND_BUF_
			memcpy(&mRtpSessionSendBuf[MAX_SENDONEBUF_SIZE*mRtpSessionSendbufIndex], (stream->address+headlen), (stream->len-headlen));
			RTPSend(session->RtpCid, session->tsinc, maker, session->Pt, (int8*)(&mRtpSessionSendBuf[MAX_SENDONEBUF_SIZE*mRtpSessionSendbufIndex]), (stream->len-headlen));
			rtp_session_sendbuf_add( (stream->len-headlen));
			#else
			echo = RTPSend(session->RtpCid, session->tsinc, maker, session->Pt, (int8*)(stream->address+headlen), stream->len-headlen);			
			#endif			

			if (stream->delay > 0)
			{
				usleep(stream->delay);
			}

		} 

	}
	
	return 0;
}

/*************************************************
  Function:    		rtp_session_set_payload_type
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_payload_type(RtpSession *session, unsigned char pt)
{
	ms_return_val_if_fail(session, -1);
	if (session->RtpCid == -1)
	{
		session->Pt = pt;
	}
	return 0;
}

/*************************************************
  Function:    		rtp_session_set_recv_addr
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_recv_port(RtpSession *session, unsigned int Port)
{
	ms_return_val_if_fail(session, -1);
	if (session->RtpCid == -1)
	{
		session->RecvPort = Port;
	}
	return 0;
}

/*************************************************
  Function:    		rtp_session_set_username
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_username(RtpSession *session, char* username)
{
	ms_return_val_if_fail(session, -1);
	ms_return_val_if_fail(username, -1); 
	if (session->RtpCid == -1)
	{
		memset(session->UserName, 0, sizeof(session->UserName));
		sprintf(session->UserName, "%s",username);
	}
	return 0;
}

/*************************************************
  Function:    		rtp_session_set_private
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
int rtp_session_set_private(RtpSession *session, void* private)
{
	ms_return_val_if_fail(session, -1);
	ms_return_val_if_fail(private, -1);
	session->private = private;
	return 0;
}

/*************************************************
  Function:    		rtp_check_h264_head	
  Description:		
  Input: 			无
  Output:			无
  Return:			0 不带头 3-类型1  4-类型2	
  Others:
*************************************************/
static int rtp_check_h264_head(char* data)
{
	if (data)
	{
		if (*data != 0x00)
		{
			return 0;
		}

		if(*(data+1) != 0x00)
		{
			return 0;
		}

		if(*(data+2) == 0x01)
		{
			return 3;
		}
		else
		{
			if(*(data+2) == 0x00 && *(data+3) == 0x01)
			{
				return 4;
			}
		}
	}

	return 0;
}

/*************************************************
  Function:    		recive_video_data
  Description:		接收UDP视频数据
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void rtp_recive_video_mult_data(char* data, int size, uint32 ts, uint8 index)
{
	uint8 startBit = data[index+1]&0x80;
    uint8 endBit = data[index+1]&0x40;

	uint8 FUIndicator = *(data+index);
	uint8 FUHeader = *(data+index+1);
	
	if (startBit)
	{		
		g_PreBufPos[g_PreBufLen]   = 0;
		g_PreBufPos[g_PreBufLen+1] = 0;
		g_PreBufPos[g_PreBufLen+2] = 0;
		g_PreBufPos[g_PreBufLen+3] = 1;
		g_PreBufPos[g_PreBufLen+4] = (FUIndicator&0xE0)+(FUHeader&0x1F);
		g_PreBufLen += 5;
		memcpy(g_PreBufPos+g_PreBufLen, &data[index+2], (size-(index+2)));
		g_PreBufLen += (size-(index+2));
 		g_startrecv = 1;
	}
	else
	{
		if (g_startrecv)
		{
			if ((g_PreBufLen+(size-(index+2)))< MULT_VIDEO_BUF_SIZE)
			{
				memcpy(g_PreBufPos+g_PreBufLen, &data[index+2], (size-(index+2)));
				g_PreBufLen += (size-(index+2));
			}
			else
			{
				g_startrecv = 0;
			}
		}
	}
	

	if (endBit && g_startrecv)
	{
		g_endbit = 1;
		g_startrecv = 0;
	}
}


/////////////
//by zxf

int tim_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
	int nsec;
	if ( x->tv_sec > y->tv_sec )
		return   -1;
	if ((x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec))
		return   -1;
	result->tv_sec = ( y->tv_sec-x->tv_sec );
	result->tv_usec = ( y->tv_usec-x->tv_usec );
	if (result->tv_usec<0)
	{
		result->tv_sec--;
		result->tv_usec+=1000000;
	}
	return 0;
}
struct  timeval  nowtime,runtime,lasttime;

/*************************************************
  Function:		rtp_session_recvprocess
  Description:	RTP接收线程 	
  Input: 		
  	1.ptr		接收会话参数	
  Output:		
  Return:		 
  Others:
*************************************************/
static void * rtp_session_recvprocess(void * ptr)
{
	mblk_t arg;
	Rtp_Session_Recv * f = (Rtp_Session_Recv*)ptr;
    rtperror err;
	MSList * mList;
	RtpSession * mRtp;
	int Address;
	uint8 tmp,index;
	char video_data[MAX_RTP_DATA_SIZE];
	ms_media_blk_init(&arg);
	
	int rtpcountperdelay = 20;		//by zxf
	static int32 frame_counts = 0;
	uint8 marker = 0;				// 结束包标志
	uint32 ts = 0;					// 当前包时间戳

	signed char TrtCounts = 3;
	g_PreBufPos = (char *)malloc(MULT_VIDEO_BUF_SIZE);
	if (g_PreBufPos == NULL)
	{
		while (TrtCounts)
		{
			g_PreBufPos = (char *)malloc(MULT_VIDEO_BUF_SIZE);
			if (g_PreBufPos != NULL)
			{
				break;
			}
			TrtCounts--;
		}
	}

	while (f->mMSThread.thread_run)
	{
		fd_set  sockets;
        int     event;
        int     retval;
        double  now;
        struct timeval timeout_tv, now_tv;
		
        if (evt_queue != NULL)
        {
            event = 0;
            timeout_tv.tv_sec = 0;
            timeout_tv.tv_usec = 2000;
        }
        else
        {
            event = 0;
            timeout_tv.tv_sec = 0;
            timeout_tv.tv_usec = 2000;
        }
        FD_ZERO(&sockets);

		ms_process_lock(f);
		mList = f->List;
		for (; mList != NULL; mList = mList->next)
		{
			mRtp = (RtpSession*)mList->data;
			if (mRtp && mRtp->RtpCid != -1)
        	{
	            if (mRtp->RtpSock != -1)
	            {
	                FD_SET(mRtp->RtpSock, &sockets);
	            }
        	}
		}
		ms_process_unlock(f);
		
		retval = select(f->mMaxfds, &sockets, NULL, NULL, &timeout_tv);
		if (retval > 0)
		{
			///////////////
			//by zxf
			//usleep(100); 
			rtpcountperdelay--;
			if(rtpcountperdelay<=0)
			{
				usleep(10000);
				rtpcountperdelay=20;
			}
			//continue;
			/////////////////

			int recbuflen = MAX_RTP_DATA_SIZE;
			
			ms_process_lock(f);
			mList = f->List;
			for (; mList != NULL; mList = mList->next)
			{
				mRtp = (RtpSession*)mList->data;
				if (mRtp && mRtp->RtpCid != -1)
	            {
	                if (FD_ISSET(mRtp->RtpSock, &sockets))
	                {
	                    recbuflen = MAX_RTP_DATA_SIZE;
						char source_addr[100];
						char port[100];
						
	                    err = RTPReceive(mRtp->RtpCid, mRtp->RtpSock, f->pRecvBuff, &recbuflen);
						RTPMostRecentAddr(mRtp->RtpCid, source_addr, port);
						Address = IPtoUlong(source_addr);
	                    if ((err == RTP_OK || err == RTP_PACKET_LOOPBACK) && Address == mRtp->RecvIP)
	                    {
	                        rtp_packet rtppkt = RTPGetRTPPacket(f->pRecvBuff, recbuflen);
	                        int pt = RTP_PT(*(rtppkt.RTP_header));
										
	                        if (pt == mRtp->Pt)
	                        {
	                            if (mRtp->func)
	                            {
	                            	if (PAYLOAD_H264 == pt)
	                            	{
										if (recbuflen <= 6)
						                {
						                	printf(" recbuflen <= 6 !!!!!!! \n");											 
											continue;
						                }  
										
	                            		index = rtp_check_h264_head(rtppkt.payload);										
										tmp = ((*(rtppkt.payload+index))&0x1f);
										marker = ((*(rtppkt.payload+index+1))&0x80);
										ts = rtppkt.RTP_header->ts;
										if (tmp == FU_A)
										{
											g_CurPackPt = (*(rtppkt.payload+index+1))&0x1f;
										}
										else
										{
											g_CurPackPt = tmp;
										}
										
										// 第一个SPS开始处理
										if (g_StartDeal == 0 && (tmp == SPS || (FU_A == tmp && SPS == ((*(rtppkt.payload+index+1))&0x1f))))
										{
											g_StartDeal = 1;
											g_startrecv = 0;
											g_PreBufLen = 0;
											g_PrePackPt = NONE;

											/*
											// add by chenbh 增加SPS数据帧解析
											uint8 spsbuf[100] = {0};
											memset(spsbuf, 0, sizeof(spsbuf));
											memcpy(spsbuf, rtppkt.payload + index, rtppkt.payload_len - index);
											h264_analyze(spsbuf, rtppkt.payload_len - index);
											*/
										}
										
										if (g_StartDeal == 1)
		                            	{
		                            		// add by chenbh 2015-09-24 为了兼容解析流式码流
		                            		#if 0 // debug log
											if (tmp == SPS)
											{
												frame_counts = 0;
											}
											else if (0x80 == marker)
											{
												frame_counts++;
												printf(" frame_counts: %d\n", frame_counts);
											}
											printf(" frame_counts: %d, marker: %02x, tmp: %02x, g_CurPackPt: %02x, rtppkt.payload[%d]:%02x\n", frame_counts, marker, tmp, g_CurPackPt, index+1, rtppkt.payload[index+1]);
											#endif
											// 每帧的分割标志marker == 0x80 
											if ((0x80 == marker || g_PrePackPt != g_CurPackPt) && 												
												(g_PrePackPt == IDR || g_PrePackPt == PPP || g_CurPackPt == PPP) &&
												(g_PreBufLen > 0 && g_PreBufPos != NULL))											
											{
												//printf("send to dec  g_PreBufLen: %d, g_PrePackPt: %02x\n", g_PreBufLen, g_PrePackPt);
												g_PreBufPos[g_PreBufLen] = 0;
												arg.address = g_PreBufPos;										
												arg.len = g_PreBufLen;
												arg.ts = g_PrePackTs;
												arg.id = MS_FILTER_NOT_SET_ID;
												
												#if SAVE_RECV_VIDEO
												RTP_Recv_SaveFie(pH264File, &arg);
												#endif
				                            	mRtp->func(mRtp, &arg);
												g_PreBufLen = 0;
												g_startrecv = 0;
											}
																						
											if (FU_A == tmp)
			                            	{				                            		
			                            		g_endbit = 0;	
												g_PrePackTs = ts;
												g_PrePackPt = *(uint8 *)(rtppkt.payload+index+1) & 0x1F;
												rtp_recive_video_mult_data(rtppkt.payload, rtppkt.payload_len, rtppkt.RTP_header->ts, index);												
											}
											else if (STAP_A == tmp || STAP_B == tmp || MTAP16 == tmp || MTAP24 == tmp)	// 组合包情况暂时不考虑
											{
												g_startrecv = 0;	
												g_PreBufLen = 0;
												printf(" tap mode is not support !!!\n");
											}
											else
											{
												g_startrecv = 0;												
												g_PrePackPt = tmp;
												g_PrePackTs = ts;
												g_PreBufPos[g_PreBufLen] = 0x00;
												g_PreBufPos[g_PreBufLen+1] = 0x00;
												g_PreBufPos[g_PreBufLen+2] = 0x00;
												g_PreBufPos[g_PreBufLen+3] = 0x01;
												g_PreBufLen += 4;

												memcpy(g_PreBufPos+g_PreBufLen, rtppkt.payload+index, rtppkt.payload_len-index);
												g_PreBufLen += rtppkt.payload_len-index;
											}	
										}
									}
									else if (PAYLOAD_G711A == pt)
									{
										//printf("3333333333333333333333 recv audio len : %d \n", rtppkt.payload_len);
									    arg.address = rtppkt.payload;
										arg.len = rtppkt.payload_len;
										arg.ts = rtppkt.RTP_header->ts;
										arg.id = MS_FILTER_NOT_SET_ID;
										#if SAVE_RECV_AUDIO
										RTP_Recv_SaveFie(pAlawRecFile, &arg);
										#endif
		                            	mRtp->func(mRtp, &arg);
									}
								}
	                        }
	                    }
						else
						{
							if (Address != mRtp->RecvIP)
							{
								log_printf("Address != mRtp->RecvIP\n");
							}
						}
	                }
				}
			}
			ms_process_unlock(f);
		}
		else if (retval == 0 && event)              
        {
            struct evt_queue_elt *next;

            gettimeofday(&now_tv, NULL);
            now = tv2dbl(now_tv);
            while (evt_queue != NULL && evt_queue->event_time <= now)
            {
                RTPExecute(evt_queue->cid, evt_queue->event_opaque);
                next = evt_queue->next;
                free(evt_queue);
                evt_queue = next;
            }
		}
	}
	
	g_StartDeal = 0;
	f->mMSThread.thread = -1;
	pthread_detach(pthread_self());
    pthread_exit(NULL);
	
    return NULL;
}


/*************************************************
  Function:    		rtp_set_videoflag
  Description: 		设置标志位
  Input:
  	1.flag			0: 停止视频处理
  Output:			无
  Return:			
  Others:
*************************************************/
void rtp_set_videoflag(int flag)
{
	g_StartDeal= flag;
	g_PrePackTs = 0;
	g_PreBufLen = 0;
	g_PrePackPt = 0;
}


/*************************************************
  Function:    		rtp_set_fds_max
  Description: 		增加接收地址
  Input:
  	1.IP			IP地址
  	2.Port			端口
  Output:			无
  Return:			
  Others:
*************************************************/
static void rtp_set_fds_max(Rtp_Session_Recv *f)
{
	MSList *mList;
	RtpSession* mRtp;
    f->mMaxfds = 0;

    mList = f->List;
	for (;mList != NULL; mList = mList->next)
	{
		mRtp = (RtpSession*)mList->data;
		if (mRtp && mRtp->RtpCid != -1)
		{
			if (f->mMaxfds < mRtp->RtpSock)
			{
				f->mMaxfds = mRtp->RtpSock;
			}
		}
		
	}
    f->mMaxfds++;
}

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
int rtp_session_recvaddr_add(RtpSession * session, unsigned int IP, MSRTPDataFunc func)
{
	Rtp_Session_Recv * f = &mRtpRecvSession;
	
	ms_return_val_if_fail(session, -1);
	if (0 != session->RecvIP)
	{
		return -1;
	}
	
	ms_process_lock(f);
	session->RecvIP = IP;
	session->func = func;
	f->List = ms_list_prepend(f->List, session);
	rtp_set_fds_max(f);
	ms_process_unlock(f);
	
	return 0;
}

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
int rtp_session_recvaddr_dec(RtpSession * session)
{
	Rtp_Session_Recv * f = &mRtpRecvSession;
	ms_return_val_if_fail(session, -1);

	if (0 == session->RecvIP)
	{
		return -1;
	}
	
	ms_process_lock(f);
	session->RecvIP = 0;
	session->func = NULL;
	f->List = ms_list_remove(f->List, session);
	ms_process_unlock(f);
	
	return 0;
}

/*************************************************
  Function:    	rtp_session_new
  Description: 	
  Input: 		
  Output:		
  Return:		
  Others:
*************************************************/
RtpSession * rtp_session_new(int port)
{
	RtpSession * session = NULL;
	
	switch (port)
	{			
		case MEDIA_VIDEOL_PORT:  
			session = &mRtpSession[2];
			break;
		
		case MEDIA_VIDEO_PORT:
			session = &mRtpSession[1];
			break;

		case MEDIA_AUDIO_PORT:
			session = &mRtpSession[0];
			break;
	}
	
	return session;
}

/*************************************************
  Function:		ms_rtp_session_Init
  Description: 	
  Input: 		
  Output:		
  Return:		 
  Others:
  备注: 函数命修改为了和芯唐庫里头函数不重名
*************************************************/
void ms_rtp_session_Init(void)
{
	int i;
	int port[3] = {MEDIA_AUDIO_PORT, MEDIA_VIDEO_PORT, MEDIA_VIDEOL_PORT};
		
	for (i = 0; i < 3; i++)
	{
		ms_mutex_init(&mRtpSession[i].lock, NULL);
		mRtpSession[i].Count = 0;
		mRtpSession[i].RtpCid = -1;
		mRtpSession[i].SendIP = 0;
		mRtpSession[i].RecvIP = 0;
		mRtpSession[i].RecvPort = port[i];
		mRtpSession[i].Pt = 0;
		mRtpSession[i].RtpSock = -1;
		mRtpSession[i].RtcpSock = -1;  
		memset(mRtpSession[i].UserName, 0, MAX_USER_NAME);
	}

	#if 0
	////////////
	//by zxf
	gettimeofday(&lasttime,0); 
	usleep(1000000);
	gettimeofday(&nowtime,0);
	tim_subtract(&runtime,  &lasttime, &nowtime);
	printf("usleep(1000000) duration tv_sec=%d tv_usec=%d \r\n", runtime.tv_sec,runtime.tv_usec);

	gettimeofday(&lasttime,0); 
	usleep(1);
	gettimeofday(&nowtime,0);
	tim_subtract(&runtime,  &lasttime, &nowtime);
	printf("usleep(1) duration tv_sec=%d tv_usec=%d \r\n", runtime.tv_sec,runtime.tv_usec);
	//////////////
	#endif
	
	ms_mutex_init(&mRtpRecvSession.lockprocess, NULL);
	ms_thread_create(&mRtpRecvSession.mMSThread, rtp_session_recvprocess, &mRtpRecvSession);
}

