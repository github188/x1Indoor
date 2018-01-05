/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_video.c
  Author:   	陈本惠
  Version:  	1.0
  Date: 		2014-12-11
  Description:  视频处理
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/uio.h>
#include <errno.h>
#include <sys/time.h>
#include <semaphore.h>

#include "logic_video.h"
#include "logic_h264_dec.h"
#include "logic_jpeg_dec.h"
#include "logic_jpeg_enc.h"
#include "logic_avi_play.h"
#include "logic_avi_record.h"
#include "logic_mp3_play.h"
#include "logic_rtsp_play.h"
#include "logic_tran_rtp.h"

#define _SNAP_WAIT_TIME_		1000

#ifdef _TFT_7_
#define _JPEG_ENC_WIDTH_		640
#define _JPEG_ENC_HEIGHT_		480

#define _JPEG_DEC_WIDTH_		640
#define _JPEG_DEC_HEIGHT_		480

#define _H264_DEC_FULL_WIDTH_	1024
#define _H264_DEC_FULL_HEIGHT_	600
#define _H264_DEC_WIDTH_		820
#define _H264_DEC_HEIGHT_		600
#else
#define _JPEG_ENC_WIDTH_		380
#define _JPEG_ENC_HEIGHT_		272

#define _JPEG_DEC_WIDTH_		380
#define _JPEG_DEC_HEIGHT_		272

#define _H264_DEC_FULL_WIDTH_	640
#define _H264_DEC_FULL_HEIGHT_	480
#define _H264_DEC_WIDTH_		380
#define _H264_DEC_HEIGHT_		272
#endif

static JpegDecParam g_JpegDecParam;
static JpegEncParam g_JpegEncParam;
static RecordParam  g_RecordParam;

/*************************************************
  Function:			inter_full_screen
  Description:		全频操作
  Input: 	
  	1.flg			是否全屏		
  Output:			无
  Return:			无
  Others:
*************************************************/
void set_full_screen(uint8 flg)
{
	V_RECT_S vorect;
	
	vorect.x = 0;
	vorect.y = 0;
	log_printf("flg : %d\n", flg);
	if (flg)
	{
		vorect.width  = _H264_DEC_FULL_WIDTH_;
		vorect.height = _H264_DEC_FULL_HEIGHT_;
		ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_LEVEL, &vorect);
	}
	else
	{
		vorect.width  = _H264_DEC_WIDTH_;
		vorect.height = _H264_DEC_HEIGHT_;
		ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_LEVEL, &vorect);
	}
}

/*************************************************
  Function:			rtsp_set_full_screen
  Description:		全频操作
  Input: 	
  	1.flg			是否全屏		
  Output:			无
  Return:			无
  Others:
*************************************************/
int rtsp_set_full_screen(uint8 flg)
{
	uint8 ret = flg;
	return ms_filter_call_method(mMediaStream.RtspPlay, MS_RTSP_PLAY_LEVEL, &ret);
}

/*************************************************
  Function:    	set_jpg_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
void set_jpg_enc_param(char *filename, uint16 width, uint16 heigh, DEVICE_TYPE_E DevType)
{
	memset(&g_JpegEncParam, 0, sizeof(JpegEncParam));
	memcpy(g_JpegEncParam.mName, filename, sizeof(g_JpegEncParam.mName));
	g_JpegEncParam.vorect.x = 0;  // 默认设置为0
	g_JpegEncParam.vorect.y = 0;
	if (width == 0 || heigh == 0)
	{
		g_JpegEncParam.vorect.width = _JPEG_ENC_WIDTH_;
		g_JpegEncParam.vorect.height = _JPEG_ENC_HEIGHT_;
	}
	else
	{
		g_JpegEncParam.vorect.width = width;
		g_JpegEncParam.vorect.height = heigh;
	}

	// add by chenbh 增加监视社区
	if (DevType == DEVICE_TYPE_DOOR_PHONE ||
		DevType == DEVICE_TYPE_AURINE_SERVER_STREAMINGSERVER)		
	{
		g_JpegEncParam.FB_Mode = FB_DISPLAY_MODE_YCBYCR;
		g_JpegEncParam.src_mode = STREAM_SRC_ANOLOG;
	}
	else
	{
		g_JpegEncParam.FB_Mode = FB_DISPLAY_MODE_RGB565;
		g_JpegEncParam.src_mode = STREAM_SRC_H264_DEC;
	}
}

/*************************************************
  Function:    	get_jpgdec_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
int32 get_jpg_enc_param(JpegEncParam *jpgParam)
{
	memset(jpgParam, 0, sizeof(JpegEncParam));
	memcpy(jpgParam->mName, g_JpegEncParam.mName, sizeof(g_JpegEncParam.mName));
	jpgParam->vorect.x = g_JpegEncParam.vorect.x;
	jpgParam->vorect.y = g_JpegEncParam.vorect.y;
	jpgParam->vorect.width = g_JpegEncParam.vorect.width;
	jpgParam->vorect.height = g_JpegEncParam.vorect.height;	
	jpgParam->FB_Mode = g_JpegEncParam.FB_Mode;
	jpgParam->src_mode = g_JpegEncParam.src_mode;
	return TRUE;
}

/*************************************************
  Function:    	set_jpg_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
void set_jpg_dec_param(char *filename, uint16 pos_x, uint16 pos_y, uint16 width, uint16 heigh)
{
	memset(&g_JpegDecParam, 0, sizeof(g_JpegDecParam));
	memcpy(g_JpegDecParam.mName, filename, sizeof(g_JpegDecParam.mName));
	g_JpegDecParam.vorect.x = pos_x;
	g_JpegDecParam.vorect.y = pos_y;
	if (width == 0 || heigh == 0)
	{
		g_JpegDecParam.vorect.width = _JPEG_DEC_WIDTH_;
		g_JpegDecParam.vorect.height = _JPEG_DEC_HEIGHT_;
	}
	else
	{
		g_JpegDecParam.vorect.width = width;
		g_JpegDecParam.vorect.height = heigh;
	}
}

/*************************************************
  Function:    	get_jpgdec_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
int32 get_jpg_dec_param(JpegDecParam *jpgParam)
{
	memset(jpgParam, 0, sizeof(JpegDecParam));
	memcpy(jpgParam->mName, g_JpegDecParam.mName, sizeof(g_JpegDecParam.mName));
	jpgParam->vorect.x = g_JpegDecParam.vorect.x;
	jpgParam->vorect.y = g_JpegDecParam.vorect.y;
	jpgParam->vorect.width = g_JpegDecParam.vorect.width;
	jpgParam->vorect.height = g_JpegDecParam.vorect.height;	
	return TRUE;
}

/*************************************************
  Function:    	get_avi_record_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
static int32 get_avi_record_param(RecordParam *PParam)
{
	memset(PParam, 0, sizeof(PParam));
	PParam->RecordMode = g_RecordParam.RecordMode;
	PParam->AudioFormat = g_RecordParam.AudioFormat;
	PParam->VideoFormat = g_RecordParam.VideoFormat;
	memcpy(PParam->filename, g_RecordParam.filename, sizeof(g_RecordParam.filename));
	return TRUE;
}

/*************************************************
  Function:    	get_avi_record_param
  Description: 		
  Input:		
  Output:		
  Return:		
  Others:
*************************************************/
int32 set_avi_record_param(uint8 mode, uint8 atp, uint8 vtp, char * filename)
{
	if (filename == NULL)
	{
		printf("%s error: file is null \n", __FUNCTION__);
		return FALSE;
	}
	memset(&g_RecordParam, 0, sizeof(g_RecordParam));
	g_RecordParam.RecordMode = mode;
	g_RecordParam.AudioFormat = atp;
	g_RecordParam.VideoFormat = vtp;
	memcpy(g_RecordParam.filename, filename, sizeof(g_RecordParam.filename));
	return TRUE;
}
/*************************************************
  Function:    	open_video_rtp_send
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int open_video_rtp_send(void)
{
	int ret = -1;
	RTP_OPEN_S rtps;
	
	rtps.RecvIp = 0;	
	rtps.Port = MEDIA_VIDEO_PORT;
	rtps.Pt = PAYLOAD_H264;
	sprintf(rtps.UserName, "%s", "video@aurine.cn");
	ret = ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_SEND_V_OPEN, &rtps);
	
	return ret;
}

/*************************************************
  Function:    	close_video_rtp_send
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int close_video_rtp_send(int level)
{
	int ret = -1;
	
	ret = ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_SEND_V_CLOSE, NULL);
	
	return ret;
}

/*************************************************
  Function:    	open_video_rtp_recv
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
static int open_video_rtp_recv(int address)
{
	int ret = -1;
	RTP_OPEN_S rtps;
	
	rtps.RecvIp = 0;	
	rtps.Port = MEDIA_VIDEO_PORT;
	rtps.Pt = PAYLOAD_H264;
	sprintf(rtps.UserName, "%s", "video@aurine.cn");
	ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_RECV_V_OPEN, &rtps);
	
	// 室内呼中心情况特殊 需要做判断
	if (0 != address)
	{
		RTP_ADDRESS_S rtpaddr;
		rtpaddr.Ip = address;
		rtpaddr.Port = MEDIA_VIDEO_PORT;
		ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_ADDR_V_RECV_ADD, &rtpaddr);
	}
	
	return ret;
}

/*************************************************
  Function:    		close_video_rtp_recv
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int close_video_rtp_recv(void)
{
	int ret = -1;
	RTP_ADDRESS_S rtpaddr;
	
	rtpaddr.Ip = 0;
	rtpaddr.Port = MEDIA_AUDIO_PORT;
	ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_ADDR_V_RECV_DEC, &rtpaddr);
	ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_RECV_V_CLOSE, NULL);

	return ret;
}

/*************************************************
  Function:    	open_video_rtp_recv
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int add_video_recvaddr(int address)
{
	int ret = -1;
	RTP_ADDRESS_S rtpaddr;
	rtpaddr.Ip = address;
	rtpaddr.Port = MEDIA_VIDEO_PORT;
	ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_ADDR_V_RECV_ADD, &rtpaddr);
	
	return ret;
}

/*************************************************
  Function:    	open_video_rtp_recv
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
static int add_video_recvport(void)
{
	int ret = -1;
	RTP_OPEN_S rtps;
	
	rtps.RecvIp = 0;	
	rtps.Port = MEDIA_VIDEO_PORT;
	rtps.Pt = PAYLOAD_H264;
	sprintf(rtps.UserName, "%s", "video@aurine.cn");
	ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_RECV_V_OPEN, &rtps);
	return ret;
}


/*************************************************
  Function:    		add_video_sendaddr
  Description: 		增加视频发送地址
  Input: 			
  	1.IP			IP地址
  	2.VideoPort		视频端口
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int add_video_sendaddr(uint32 IP, uint16 VideoPort) 
{
	RTP_ADDRESS_S rtp_addr;

	rtp_addr.Ip = IP;
	rtp_addr.Port = VideoPort;
	int ret = ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_ADDRESS_V_ADD, &rtp_addr);
	usleep(5000);
	
	return ret;
}

/*************************************************
  Function:    		del_video_sendaddr
  Description: 		去除视频发送地址
  Input: 			
  	1.IP			IP地址
  	2.VideoPort		视频端口
  Output:			无
  Return:			无
  Others:
*************************************************/
void del_video_sendaddr(uint32 IP, uint16 VideoPort)
{
	RTP_ADDRESS_S rtp_addr;

	rtp_addr.Ip = IP;
	rtp_addr.Port = VideoPort;

	ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_ADDRESS_V_DEC, &rtp_addr);
	usleep(5000);
}



/*************************************************
  Function:    		get_video_addr_count
  Description: 		获取RTP视频连接数
  Input:			无
  Output:			无
  Return:			已连接的数量		
  Others:
*************************************************/
int get_video_addr_count(void)
{
	int ret = -1;

	ret = ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_ADDRESS_V_COUNT, NULL);
	return ret;
}

/*************************************************
  Function:			rtsp_play_close
  Description:		关闭rtsp播放
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 rtsp_play_close(void)
{
	return ms_filter_call_method(mMediaStream.RtspPlay, MS_RTSP_PLAY_CLOSE, NULL);
}

/*************************************************
  Function:			rtsp_play_open
  Description:		开启rtsp播放
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 rtsp_play_open(V_RECT_S rect, char *Url, void *callback_func)
{
	int ret = -1;
	RtspPlayParam param;
	memset(&param, 0, sizeof(RtspPlayParam));
	param.callback = callback_func;
	param.val = Url;
	memcpy(&param.rect, &rect, sizeof(V_RECT_S));
	
	ret = ms_filter_call_method(mMediaStream.RtspPlay, MS_RTSP_PLAY_PARAM, &param);
	if (ret == 0)
	{
		return ms_filter_call_method(mMediaStream.RtspPlay, MS_RTSP_PLAY_OPEN, NULL);
	}

	return -1;
}

/*************************************************
  Function:			lyly_play_pause
  Description:		暂停/播放切换
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 lyly_play_pause(void)
{
	AviPlayState avistate;
	memset(&avistate, 0, sizeof(avistate));
	avistate.cmd = AVI_CMD_PAUSE;

	int ret = ms_filter_call_method(mMediaStream.AviPlay, MS_AVI_PLAY_STATE, &avistate);
	if (ret == RT_SUCCESS)
	{
		return ret;
	}

	return RT_FAILURE;
}

/*************************************************
  Function:			lyly_play_start
  Description:		启动留影留言
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int32 lyly_play_start(char *filename, void * proc)
{
	AviPlayState avistate;
/*	if (filename == NULL || proc == NULL)
	{		
		return FALSE;
	}*/
	memset(&avistate, 0, sizeof(avistate));
	memcpy(avistate.filename, filename, sizeof(avistate.filename));
	log_printf("avistate.filename : %s\n", avistate.filename);
	avistate.callback = (PMEDIA_CALLBACK)proc;
	avistate.cmd = AVI_CMD_PLAY;

	int ret = ms_filter_call_method(mMediaStream.AviPlay, MS_AVI_PLAY_STATE, &avistate);
	if (ret == RT_SUCCESS)
	{
		ret = ms_filter_call_method(mMediaStream.AviPlay, MS_AVI_PLAY_OPEN, NULL);
		return TRUE;
	}
	return FALSE;
}

/*************************************************
  Function:			lyly_play_stop
  Description:		最后退出再调用
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
void lyly_play_stop(void)
{
	ms_filter_call_method(mMediaStream.AviPlay, MS_AVI_PLAY_CLOSE, NULL);
}

/*************************************************
  Function:    	lyly_record_start
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int32 lyly_record_start(void)
{
	RecordParam RecParam;
	
	if (TRUE == get_avi_record_param(&RecParam))
	{
		ms_filter_call_method(mMediaStream.AviRecord, MS_AVI_RECORD_PARAM, &RecParam);
	}
	return ms_filter_call_method(mMediaStream.AviRecord, MS_AVI_RECORD_OPEN, NULL);
}

/*************************************************
  Function:    	lyly_record_stop
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int32 lyly_record_stop(void)
{
	return ms_filter_call_method(mMediaStream.AviRecord, MS_AVI_RECORD_CLOSE, NULL);	
}

/*************************************************
  Function:			mp3_play_pause
  Description:		暂停/播放切换
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
int32 mp3_play_pause(void)
{
	Mp3PlayState mp3state;
	memset(&mp3state, 0, sizeof(mp3state));
	mp3state.cmd = MP3_CMD_PAUSE;

	int ret = ms_filter_call_method(mMediaStream.Mp3Play, MS_MP3_PLAY_STATE, &mp3state);
	if (ret == RT_SUCCESS)
	{
		return ret;
	}

	return RT_FAILURE;
}

/*************************************************
  Function:			lyly_play_start
  Description:		启动留影留言
  Input: 			无
  Output:			无
  Return:			TRUE/FALSE
  Others:
*************************************************/
int32 mp3_play_start(char *filename, void * proc)
{
	Mp3PlayState mp3state;
/*	if (filename == NULL || proc == NULL)
	{		
		return FALSE;
	}*/
	memset(&mp3state, 0, sizeof(mp3state));
	memcpy(mp3state.filename, filename, sizeof(mp3state.filename));
	log_printf("avistate.filename : %s\n", mp3state.filename);
	mp3state.callback = (PMEDIA_CALLBACK)proc;
	mp3state.cmd = MP3_CMD_PLAY;

	int ret = ms_filter_call_method(mMediaStream.Mp3Play, MS_MP3_PLAY_STATE, &mp3state);
	if (ret == RT_SUCCESS)
	{
		ret = ms_filter_call_method(mMediaStream.Mp3Play, MS_MP3_PLAY_OPEN, NULL);
		return ret;
	}
	return RT_FAILURE;
}

/*************************************************
  Function:			mp3_play_stop
  Description:		主动调用，播放完一首会自己反初始化
  Input: 			无
  Output:			无
  Return:		
  Others:
*************************************************/
void mp3_play_stop(void)
{
	ms_filter_call_method(mMediaStream.Mp3Play, MS_MP3_PLAY_CLOSE, NULL);
}

/*************************************************
  Function:    	open_jpeg_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_jpeg_enc(void)
{
	int ret = -1;
	JpegEncParam JpegParam;
	if (TRUE == get_jpg_enc_param(&JpegParam))
	{
		if (JpegParam.src_mode == STREAM_SRC_H264_DEC)
		{
			ret = ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_SNAP, NULL);
		}

		if (0 == ret)
		{
			ret = ms_filter_call_method(mMediaStream.JpegEnc, MS_JPEG_ENC_PARAM, &JpegParam);
		}
	}

	if (0 == ret)
	{
		ret = ms_filter_call_method(mMediaStream.JpegEnc, MS_JPEG_ENC_OPEN, NULL);
	}
	return ret;
}

/*************************************************
  Function:    	close_jpeg_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_jpeg_enc(void)
{
	return ms_filter_call_method(mMediaStream.JpegEnc, MS_JPEG_ENC_CLOSE, NULL);
}

/*************************************************
  Function:    	open_jpeg_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_jpeg_dec(void)
{
	JpegDecParam JpegParam;
	if (TRUE == get_jpg_dec_param(&JpegParam))
	{
		ms_filter_call_method(mMediaStream.JpegDec, MS_JPEG_DEC_PARAM, &JpegParam);
	}
	return ms_filter_call_method(mMediaStream.JpegDec, MS_JPEG_DEC_OPEN, NULL);
}

/*************************************************
  Function:    	close_jpeg_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_jpeg_dec(void)
{
	return ms_filter_call_method(mMediaStream.JpegDec, MS_JPEG_DEC_CLOSE, NULL);	
}

/*************************************************
  Function:    	open_video_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int open_video_dec(void)
{
	V_RECT_S  vorect;
	vorect.x = 0;
	vorect.y = 0;
	vorect.width  = _H264_DEC_WIDTH_;
	vorect.height = _H264_DEC_HEIGHT_;	
	
	int32 ret = ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_LEVEL, &vorect);	
	if (ret == -1)
	{
		printf(" MS_H264_DEC_LEVEL return error\n");
	}
	
	ret = ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_OPEN, NULL);	
	return ret;
}

/*************************************************
  Function:    	close_video_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int close_video_dec(void)
{
	rtp_set_videoflag(0);
	return ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_CLOSE, NULL);	
}

/*************************************************
  Function:    		stop_video_send_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void stop_video_send_mode(int net)
{
	return;
}

/*************************************************
  Function:    		start_video_send_mode
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int start_video_send_mode(int net)
{
	return -1;
}

/*************************************************
  Function:    		stop_video_recv_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void stop_video_recv_mode(void)
{
	int ret = close_video_rtp_recv();
		
	usleep(20000);
	ret = close_video_dec();

	ms_media_unlink(mMediaStream.VideoRtpRecv, mMediaStream.VideoDec);
}

/*************************************************
  Function:    		start_video_send_mode
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int start_video_recv_mode(int32 addr)
{
	log_printf("open_video_rtp_recv addr : 0X%x\n", addr);
	rtp_set_videoflag(0);
	int ret = open_video_rtp_recv(addr);
	if (ret != 0)
	{
		return -1;
	}
	
	log_printf("ms_media_link \n");
	ms_media_link(mMediaStream.VideoRtpRecv, mMediaStream.VideoDec);

	log_printf("open_video_dec \n");
	ret = open_video_dec();
	if (ret != 0)
	{
		goto errvideo1;
	}
	return 0;

errvideo1:	
	log_printf("return err !!!\n");
	close_video_rtp_recv();
	
	return -1;
}

/*************************************************
  Function:    		stop_video_both_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void stop_video_both_mode(int net)
{
	return;		
}

/*************************************************
  Function:    		start_video_both_mode
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int start_video_both_mode(int net, int32 addr)
{
	return 0;	
}

/*************************************************
  Function:    		open_video_mode
  Description:		
  Input: 			
    1.addres		对端地址
  	2.mode			视频模式 发送、接收
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_video_mode(VIDEO_STATE_E mode, int addr)
{
	int ret = -1;
	switch (mode)
	{
		case VS_NETTALK_BOTH:						// 双向视频
			ret = start_video_both_mode(1, addr);
			break;
			
		case VS_NETTALK_SEND:						// 视频发送
			ret = start_video_send_mode(1);
			break;

		case VS_NETTALK_RECIVE:						// 视频接收
			ret = start_video_recv_mode(addr);
			break;
			
		case VS_NETSNAP:							// 视频抓拍
			ret = start_video_send_mode(0);
			break;

		default:
			break;			
	}

	return ret;
}

/*************************************************
  Function:    		close_video_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int close_video_mode(VIDEO_STATE_E mode)
{
	int ret = -1;

	switch (mode)
	{
		case VS_NETTALK_SEND:
			stop_video_send_mode(1);
			ret = 0;
			break;

		case VS_NETSNAP:
			stop_video_send_mode(0);
			ret = 0;
			break;	

		case VS_NETTALK_RECIVE:
			stop_video_recv_mode();
			break;
			
		case VS_NETTALK_BOTH:
			stop_video_both_mode(1);
			break;
			
		default:
			break;			
	}

	return ret;
}


