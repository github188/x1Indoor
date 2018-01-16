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
#include "logic_h264_enc.h"
#include "logic_jpeg_dec.h"
#include "logic_jpeg_enc.h"
#include "logic_avi_play.h"
#include "logic_avi_record.h"
#include "logic_mp3_play.h"
#include "logic_tran_rtp.h"
#include "logic_cloud_itc.h"

#define _SNAP_WAIT_TIME_		1000


static pthread_mutex_t g_video_mutex;
#define VIDEO_MUTEX_LOCK 	pthread_mutex_lock(&g_video_mutex);
#define VIDEO_MUTEX_UNLOCK 	pthread_mutex_unlock(&g_video_mutex);
#define VIDEO_MUTEX_INIT 	pthread_mutex_init(&g_video_mutex, NULL);

static int g_VideoErrTimes = 0;
static VIDEO_STATE_E g_video_mode = VS_NONE;
static JpegDecParam g_JpegDecParam;
static JpegEncParam g_JpegEncParam;
static RecordParam  g_RecordParam;

static video_params g_VideoEnc_Param = {
	.bit_rate = DEFAULT_BIT_RATE,
    .width = DEFAULT_WIDTH,
    .height = DEFAULT_HEIGHT,
    .framerate = DEFAULT_FRAMERATE,
};

static video_params g_CloudVideoEnc_Param = {
	.bit_rate = DEFAULT_CLOUD_BIT_RATE,
    .width = DEFAULT_CLOUD_WIDTH,
    .height = DEFAULT_CLOUD_HEIGHT,
    .framerate = DEFAULT_CLOUD_FRAMERATE,
    .gop_size = 15,
};

// 复位时使用参数
const video_params g_Init_VideoEnc_Param = {
	.bit_rate = DEFAULT_BIT_RATE,
    .width = DEFAULT_WIDTH,
    .height = DEFAULT_HEIGHT,
    .framerate = DEFAULT_FRAMERATE,
};

const video_params g_Init_CloudVideoEnc_Param = {
	.bit_rate = DEFAULT_CLOUD_BIT_RATE,
    .width = DEFAULT_CLOUD_WIDTH,
    .height = DEFAULT_CLOUD_HEIGHT,
    .framerate = DEFAULT_CLOUD_FRAMERATE,
    .gop_size = 15,
};


/*************************************************
  Function:    		set_video_param
  Description: 		设置视频参数
  Input: 			
  	1.param			视频参数
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int set_video_param(video_params *param)
{
	if (param == NULL)
	{
		return -1;
	}

	log_printf("param.bit_rate: %d, param.framerate: %d, param.width: %d, param.height: %d \n", \
		param->bit_rate, param->framerate, param->width, param->height);
	
	memset(&g_VideoEnc_Param, 0, sizeof(video_params));	
	memcpy(&g_VideoEnc_Param, param, sizeof(video_params));

	return 0;
}

/*************************************************
  Function:    		get_video_param
  Description: 		获取视频参数
  Input: 			
  	1.param			视频参数
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int get_video_param(video_params * videoParam)
{
	if (videoParam)
	{
		memset(videoParam, 0, sizeof(video_params));
		memcpy(videoParam, &g_VideoEnc_Param, sizeof(video_params));
	}
	return FALSE;
}	

/*************************************************
  Function:    		set_cloud_video_param
  Description: 		设置视频参数
  Input: 			
  	1.param			视频参数
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int set_cloud_video_param(video_params *param)
{
	if (param == NULL)
	{
		return -1;
	}

	log_printf("param.bit_rate: %d, param.framerate: %d, param.width: %d, param.height: %d \n", \
		param->bit_rate, param->framerate, param->width, param->height);
	
	memset(&g_CloudVideoEnc_Param, 0, sizeof(video_params));	
	memcpy(&g_CloudVideoEnc_Param, param, sizeof(video_params));

	return 0;
}

/*************************************************
  Function:    		get_cloud_video_param
  Description: 		获取视频参数
  Input: 			
  	1.param			视频参数
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int get_cloud_video_param(video_params * videoParam)
{
	if (videoParam)
	{
		memset(videoParam, 0, sizeof(video_params));
		memcpy(videoParam, &g_CloudVideoEnc_Param, sizeof(video_params));
	}
	return FALSE;
}

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
	
	vorect.x = H264_DISPLAY_X;
	vorect.y = H264_DISPLAY_Y;
	log_printf("flg : %d\n", flg);
	if (flg)
	{
		vorect.width  = H264_DISPLAY_W_FULL;
		vorect.height = H264_DISPLAY_H_FULL;
		ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_LEVEL, &vorect);
	}
	else
	{
		vorect.width  = H264_DISPLAY_W;
		vorect.height = H264_DISPLAY_H;
		ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_LEVEL, &vorect);
	}
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
		g_JpegEncParam.vorect.width = JPEG_ENC_W;
		g_JpegEncParam.vorect.height = JPEG_ENC_H;
	}
	else
	{
		g_JpegEncParam.vorect.width = width;
		g_JpegEncParam.vorect.height = heigh;
	}

	if (DevType == DEVICE_TYPE_DOOR_PHONE)
	{
		g_JpegEncParam.FB_Mode = FB_DISPLAY_MODE_YCBYCR;
	}
	else
	{
		g_JpegEncParam.FB_Mode = FB_DISPLAY_MODE_RGB565;
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
		g_JpegDecParam.vorect.width = JPEG_DEC_W;
		g_JpegDecParam.vorect.height = JPEG_DEC_H;
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
  Function:    	video_rtp_send_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_rtp_send_open(void)
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
  Function:    	video_rtp_send_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int video_rtp_send_close(void)
{
	int ret = -1;
	
	ret = ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_SEND_V_CLOSE, NULL);
	
	return ret;
}

/*************************************************
  Function:    	video_rtp_recv_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
static int video_rtp_recv_open(int address)
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
  Function:    		video_rtp_recv_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int video_rtp_recv_close(void)
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
  Function:    	video_recvaddr_add
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
static int video_recvaddr_add(int address)
{
	int ret = -1;
	RTP_ADDRESS_S rtpaddr;
	rtpaddr.Ip = address;
	rtpaddr.Port = MEDIA_VIDEO_PORT;
	ret = ms_filter_call_method(mMediaStream.VideoRtpRecv, MS_RTP_ADDR_V_RECV_ADD, &rtpaddr);
	
	return ret;
}

/*************************************************
  Function:    	video_recvport_add
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
static int video_recvport_add(void)
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
  Function:    		video_sendaddr_add
  Description: 		增加视频发送地址
  Input: 			
  	1.IP			IP地址
  	2.VideoPort		视频端口
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int video_sendaddr_add(uint32 IP, uint16 VideoPort) 
{
	RTP_ADDRESS_S rtp_addr;

	rtp_addr.Ip = IP;
	rtp_addr.Port = VideoPort;
	int ret = ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_ADDRESS_V_ADD, &rtp_addr);
	usleep(5000);
	
	return ret;
}

/*************************************************
  Function:    		video_sendaddr_del
  Description: 		去除视频发送地址
  Input: 			
  	1.IP			IP地址
  	2.VideoPort		视频端口
  Output:			无
  Return:			无
  Others:
*************************************************/
void video_sendaddr_del(uint32 IP, uint16 VideoPort)
{
	RTP_ADDRESS_S rtp_addr;

	rtp_addr.Ip = IP;
	rtp_addr.Port = VideoPort;

	ms_filter_call_method(mMediaStream.VideoRtpSend, MS_RTP_ADDRESS_V_DEC, &rtp_addr);
	usleep(5000);
}


/*************************************************
  Function:    	video_dec_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int video_dec_open(void)
{
	V_RECT_S  vorect;
	vorect.x = H264_DISPLAY_X;
	vorect.y = H264_DISPLAY_Y;
	vorect.width  = H264_DISPLAY_W;
	vorect.height = H264_DISPLAY_H;

	rtp_set_videoflag(0);
	int32 ret = ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_PARAM, &vorect);	
	if (ret == -1)
	{
		printf(" MS_H264_DEC_PARAM return error\n");
	}
	return ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_OPEN, NULL);	
}

/*************************************************
  Function:    	video_dec_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int video_dec_close(void)
{
	rtp_set_videoflag(0);
	return ms_filter_call_method(mMediaStream.VideoDec, MS_H264_DEC_CLOSE, NULL);	
}

/*************************************************
  Function:    		video_enc_open
  Description: 		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
static int video_enc_open(void)
{
	video_params param;
	
	memset(&param, 0, sizeof(video_params));
	memcpy(&param, &g_VideoEnc_Param, sizeof(video_params));
	log_printf("param.bit_rate: %d, param.framerate: %d, param.width: %d, param.height: %d \n", \
		param.bit_rate, param.framerate, param.width, param.height);

	int ret  = ms_filter_call_method(mMediaStream.VideoEnc, MS_H264_ENC_PARAM, &param);
	if (RT_SUCCESS == ret)
	{
		ret = ms_filter_call_method(mMediaStream.VideoEnc, MS_H264_ENC_OPEN, NULL);
	}
	return ret;
}

/*************************************************
  Function:    		video_enc_close
  Description: 		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
static int video_enc_close(void)
{
	return ms_filter_call_method(mMediaStream.VideoEnc, MS_H264_ENC_CLOSE, NULL);
}

/*************************************************
  Function:    		request_video_IFrame
  Description: 		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
int video_request_IFrame(void)
{
	return ms_filter_call_method(mMediaStream.VideoEnc, MS_H264_ENC_IDR, NULL);
}

/*************************************************
  Function:    		video_send_mode_stop
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void video_send_mode_stop()
{
	int ret = -1;

	// 将编码参数复位成默认值
	video_params Init_VideoEnc_Param = g_Init_VideoEnc_Param;
	set_video_param(&Init_VideoEnc_Param);
		
	ret = video_enc_close();
	log_printf(" video_enc_close return %d !!!!\n", ret);

	ret = video_rtp_send_close();
	log_printf(" video_rtp_send_close return %d !!!!\n", ret);
	
	ms_media_unlink(mMediaStream.VideoEnc, mMediaStream.VideoRtpSend);

	return;
}

/*************************************************
  Function:    		video_send_mode_start
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_send_mode_start(void)
{
	int ret = -1;
	
	ret = video_rtp_send_open();
	if (ret == -1)
	{
		printf("video_rtp_send_open return error!!! \n");
		goto error0;
	}

	ret = video_enc_open();
	if (ret == -1)
	{
		printf("video_enc_open return error!!! \n");
		goto error1;
	}	
	

	ms_media_link(mMediaStream.VideoEnc, mMediaStream.VideoRtpSend);		
	return 0;

	
error1:
	video_rtp_send_close();
	
error0:
	printf("start_video_mode_sendonly return error!!! \n");
	return -1;
}

/*************************************************
  Function:    		video_recv_mode_start
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_recv_mode_start(uint32 addr)
{
	log_printf("open_video_rtp_recv addr : 0X%x\n", addr);
	
	int ret = video_rtp_recv_open(addr);
	if (ret != 0)
	{
		return -1;
	}
	
	log_printf("ms_media_link \n");
	ms_media_link(mMediaStream.VideoRtpRecv, mMediaStream.VideoDec);

	log_printf("open_video_dec \n");
	ret = video_dec_open();
	if (ret != 0)
	{
		goto errvideo1;
	}
	return 0;

errvideo1:	
	log_printf("return err !!!\n");
	video_rtp_recv_close();
	
	return -1;
}

/*************************************************
  Function:    		video_recv_mode_stop
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void video_recv_mode_stop(void)
{
	int ret = video_rtp_recv_close();
		
	usleep(20000);
	ret = video_dec_close();

	ms_media_unlink(mMediaStream.VideoRtpRecv, mMediaStream.VideoDec);
}

/*************************************************
  Function:    		video_both_mode_start
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_both_mode_start(uint32 addr)
{
	return -1;	
}

/*************************************************
  Function:    		video_both_mode_stop
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void video_both_mode_stop()
{
	// 将编码参数复位成默认值
	video_params Init_VideoEnc_Param = g_Init_VideoEnc_Param;
	set_video_param(&Init_VideoEnc_Param);
	return;		
}

#ifdef _ENABLE_CLOUD_
/*************************************************
  Function:    	video_cloud_send_open
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_cloud_send_open()
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.VideoCloudSend, MS_CLOUD_SEND_VIDEO_OPEN, NULL);
	return ret;
}

/*************************************************
  Function:    	video_cloud_send_close
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_cloud_send_close()
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.VideoCloudSend, MS_CLOUD_SEND_VIDEO_CLOSE, NULL);
	return ret;
}

/*************************************************
  Function:    		video_cloud_enc_open
  Description: 		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
static int video_cloud_enc_open(void)
{		
	int ret  = ms_filter_call_method(mMediaStream.VideoCloudEnc, MS_CLOUD_H264_ENC_PARAM, &g_CloudVideoEnc_Param);
	if (RT_SUCCESS == ret)
	{
		ret = ms_filter_call_method(mMediaStream.VideoCloudEnc, MS_CLOUD_H264_ENC_OPEN, NULL);
	}
	return ret;
}

/*************************************************
  Function:    		video_cloud_enc_close
  Description: 		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
static int video_cloud_enc_close(void)
{
	return ms_filter_call_method(mMediaStream.VideoCloudEnc, MS_CLOUD_H264_ENC_CLOSE, NULL);
}

/*************************************************
  Function:    	video_cloud_set_send_func
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int video_cloud_set_send_func(void *func)
{
	return ms_filter_call_method(mMediaStream.VideoCloudSend, MS_CLOUD_SEND_VIDEO_FUNC, func);
}

/*************************************************
  Function:    	video_cloud_enable_send
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int video_cloud_enable_send()
{
	int ret = HI_FAILURE;
	HI_BOOL param = HI_TRUE;
	ret = ms_filter_call_method(mMediaStream.VideoCloudSend, MS_CLOUD_SEND_VIDEO_ENABLE, &param);
	return ret;
}

/*************************************************
  Function:    	video_cloud_disable_send
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int video_cloud_disable_send()
{
	int ret = HI_FAILURE;
	HI_BOOL param = HI_FALSE;
	ret = ms_filter_call_method(mMediaStream.VideoCloudSend, MS_CLOUD_SEND_VIDEO_ENABLE, &param);
	return ret;
}

/*************************************************
  Function:    		video_cloud_request_IFrame
  Description: 		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
int video_cloud_request_IFrame(void)
{
	return ms_filter_call_method(mMediaStream.VideoCloudEnc, MS_CLOUD_H264_ENC_IDR, NULL);
}

/*************************************************
  Function:    		video_cloud_mode_start
  Description:		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int video_cloud_mode_start(void)
{
	int ret = -1;
	
	ret = video_cloud_send_open();
	if (ret == -1)
	{
		err_printf("video_cloud_send_open return error!!! \n");
		goto error0;
	}

	ret = video_cloud_enc_open();
	if (ret == -1)
	{
		err_printf("video_cloud_enc_open return error!!! \n");
		goto error1;
	}	
	

	ms_media_link(mMediaStream.VideoCloudEnc, mMediaStream.VideoCloudSend);		
	return 0;

	
error1:
	video_cloud_send_close();
	
error0:
	err_printf("video_cloud_mode_start return error!!! \n");
	return -1;
}

/*************************************************
  Function:    		video_both_mode_stop
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void video_cloud_mode_stop(void)
{
	int ret = -1;
	video_params Init_VideoEnc_Param = g_Init_CloudVideoEnc_Param;
	set_cloud_video_param(&Init_VideoEnc_Param);
	
	ret = video_cloud_enc_close();
	log_printf(" video_cloud_enc_close return %d !!!!\n", ret);

	ret = video_cloud_send_close();
	log_printf(" video_cloud_send_close return %d !!!!\n", ret);
	
	ms_media_unlink(mMediaStream.VideoCloudEnc, mMediaStream.VideoCloudSend);

	return;
}

#endif


/*************************************************
  Function:    		video_mutex_init
  Description:		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
void video_mutex_init(void)
{
	pthread_mutex_init(&g_video_mutex, NULL);
}

/*************************************************
  Function:    		open_video_mode
  Description:		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_video_mode(VIDEO_STATE_E mode, void* arg1, void *arg2, void *arg3)
{
	VIDEO_MUTEX_LOCK
	int ret = -1;
	switch (mode)
	{
		case VS_NETTALK_BOTH:						// 双向视频
			{
				uint32 addr  = *(uint32 *)arg1;
				ret = video_both_mode_start(addr);
			}
			break;
			
		case VS_NETTALK_SEND:						// 视频发送
			{
				ret = video_send_mode_start();
				if (0 == ret)
				{
					//video_request_IFrame();		// 不用这个 会导致视频刚出来马赛克
				}
			}
			break;

		case VS_NETTALK_RECIVE:						// 视频接收
			{
				uint32 addr  = *(uint32 *)arg1;
				ret = video_recv_mode_start(addr);
			}
			break;

		#ifdef _ENABLE_CLOUD_
		case VS_CLOUD_SEND:							// 云对讲视频发送
			{
				ret = video_cloud_mode_start();
				if (0 == ret)
				{
					//video_cloud_request_IFrame();	// 不用这个 会导致视频刚出来马赛克
				}
			}
			break;
		#endif

		case VS_LOCAL_RECORD:						// 本地录制
			{
				char *filename = (char *)arg1;
				//ret = rk_media_start_mp4_record(filename);
			}
			break;
			
		case VS_NET_SNAP:							// 视频抓拍	
			{
				char *filename = (char *)arg1;
				void *func = (void *)arg2;
				
			}
			break;

		case VS_LOCAL_SNAP:							// 本地抓拍 
			{
				char *filename = (char *)arg1;
				void *func = (void *)arg2;
			 	//ret = rk_media_takephoto(filename, func);			
			}
			break;
						
		case VS_LYLY_PLAY:
			{
				char *filename = (char *)arg1;
				void *func = (void *)arg2;
			 			
			}
			break;
			
		default:
			break;			
	}

	if (0 == ret)
	{
		g_video_mode |= mode;
		g_VideoErrTimes = 0;
	}
	else
	{
		g_VideoErrTimes++;
		if (g_VideoErrTimes > 3)
		{
			err_printf("video err  g_VideoErrTimes :%d\n", g_VideoErrTimes);
			//storage_sys_set_nightReboot(1);
			//#ifdef USE_COM_FEETDOG
			//hw_set_lcd_pwm0(TRUE);
			//#else
			//drviver_dog_stopfeat();
			//#endif
		}
	}
	VIDEO_MUTEX_UNLOCK
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
	VIDEO_MUTEX_LOCK
	int ret = -1;
	
	switch (mode)
	{
		case VS_NETTALK_SEND:
			video_send_mode_stop();
			ret = 0;
			break;

		case VS_NETTALK_RECIVE:
			video_recv_mode_stop();
			break;
			
		case VS_NETTALK_BOTH:
			video_both_mode_stop();
			break;

		#ifdef _ENABLE_CLOUD_
		case VS_CLOUD_SEND:	
			video_cloud_mode_stop();
			break;
		#endif

		case VS_LOCAL_RECORD:
			//rk_media_stop_mp4_record();
			break;
			
		case VS_NET_SNAP:			
			ret = 0;
			break;

		case VS_LOCAL_SNAP:							
			ret = 0;
			break;

		case VS_LYLY_PLAY:
			{
				//rk_media_stop_mp4_play();
			}
			break;
			
		default:
			VIDEO_MUTEX_UNLOCK
			return -1;
	}

	g_video_mode &= ~mode;
	if (VS_NONE == g_video_mode)
	{
		
	}
	VIDEO_MUTEX_UNLOCK
	return ret;
}

