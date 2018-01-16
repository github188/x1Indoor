/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	Logic_audio.c
  Author:		luofl
  Version:		2.0
  Date: 		2015-03-21
  Description:	音频处理程序
  
  History:		  
	1. Date:
	   Author:
	   Modification:
	2. ...
*************************************************/
#include "../logic_include.h"
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

#include "logic_audio_dec.h"
#include "logic_media.h"
#include "logic_tran_rtp.h"
#include "logic_audio.h"
#include "logic_audio_enc.h"
#include "logic_wav_play.h"
#include "logic_wav_record.h"
#include "logic_play_lyly_hit.h"

//#include "logic_alaw_agc.h"
#include "logic_audio_ai.h"
#include "logic_audio_ao.h"
#include "logic_cloud_itc.h"

static uint8 g_AudioIsPack = 0;
static AUDIO_PARAM AudioParam;
static AUDIO_STATE_E g_audio_mode = AS_NONE;

static pthread_mutex_t g_audio_mutex;
#define AUDIO_MUTEX_LOCK 	pthread_mutex_lock(&g_audio_mutex);
#define AUDIO_MUTEX_UNLOCK 	pthread_mutex_unlock(&g_audio_mutex);
#define AUDIO_MUTEX_INIT 	pthread_mutex_init(&g_audio_mutex, NULL);

#if (_AEC_TYPE_ == _SW_AEC_)
//by zxf
extern void echo_cancel_init(int size, int tail, int noiselevel, int echolevel);
extern void echo_cancel_free(void);
#endif


/*************************************************
  Function:    		set_audio_pack_mode
  Description: 		设置音频是否组包
  Input:			无
  Output:			无
  Return:			无		
  Others:
*************************************************/
void set_audio_pack_mode(uint8 IsPack)
{
	g_AudioIsPack = IsPack;
}

/*************************************************
  Function:    		set_audio_codec_param
  Description:		设置音频参数
  Input: 		
  Output:			AudioParam 音频参数变量
  Return:			无
  Others:
*************************************************/
static void set_audio_codec_param(AUDIO_PARAM *g_AudioParam)
{	
	memset(g_AudioParam, 0, sizeof(AUDIO_PARAM));
	g_AudioParam->isPack = g_AudioIsPack;
	g_AudioParam->AiAgc = 1.0;
	g_AudioParam->AoAgc = 1.0;
}

/*************************************************
  Function:    	audio_rtp_recv_open
  Description: 	打开音频RTP接收
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int audio_rtp_recv_open(int address)
{
	int ret = -1;
	RTP_OPEN_S rtps;
	
	rtps.RecvIp = 0;	
	rtps.Port = MEDIA_AUDIO_PORT;
	rtps.Pt = PAYLOAD_G711A;
	sprintf(rtps.UserName, "%s", "audio@aurine.cn");
	ret = ms_filter_call_method(mMediaStream.AudioRtpRecv, MS_RTP_RECV_A_OPEN, &rtps);
	RTP_ADDRESS_S rtpaddr;
	rtpaddr.Ip = address;
	rtpaddr.Port = MEDIA_AUDIO_PORT;
	ret = ms_filter_call_method(mMediaStream.AudioRtpRecv, MS_RTP_ADDR_A_RECV_ADD, &rtpaddr);
	return ret;
}

/*************************************************
  Function:    	audio_rtp_recv_close
  Description: 	关闭音频RTP接收
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int audio_rtp_recv_close(void)
{
	int ret = -1;
	RTP_ADDRESS_S rtpaddr;
	
	rtpaddr.Ip = 0;
	rtpaddr.Port = MEDIA_AUDIO_PORT;
	ret = ms_filter_call_method(mMediaStream.AudioRtpRecv, MS_RTP_ADDR_A_RECV_DEC, &rtpaddr);
	ret = ms_filter_call_method(mMediaStream.AudioRtpRecv, MS_RTP_RECV_A_CLOSE, NULL);
	
	return ret;
}

/*************************************************
  Function:    	audio_rtp_send_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
 static int audio_rtp_send_open(void)
{
	int ret = -1;
	RTP_OPEN_S rtps;
	
	rtps.RecvIp = 0;	
	rtps.Port = MEDIA_AUDIO_PORT;
	rtps.Pt = PAYLOAD_G711A;
	sprintf(rtps.UserName, "%s", "audio@aurine.cn");
	ret = ms_filter_call_method(mMediaStream.AudioRtpSend, MS_RTP_SEND_A_OPEN, &rtps);
	return ret;
}

/*************************************************
  Function:    	audio_rtp_send_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_rtp_send_close(void)
{
	int ret = -1;
	
	ret = ms_filter_call_method(mMediaStream.AudioRtpSend, MS_RTP_SEND_A_CLOSE, NULL);
	
	return ret;
}

/*************************************************
  Function:    		audio_sendaddr_add
  Description: 		增加音频发送地址
  Input: 			
  	1.IP			IP地址
  	2.AudioPort		音频端口
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int32 audio_sendaddr_add(uint32 IP, uint16 AudioPort)
{
	RTP_ADDRESS_S rtp_addr;
	
	rtp_addr.Ip = IP;
	rtp_addr.Port = MEDIA_AUDIO_PORT;
	ms_filter_call_method(mMediaStream.AudioRtpSend, MS_RTP_ADDRESS_A_ADD, &rtp_addr);
	
	return 0;
}

/*************************************************
  Function:    		audio_sendaddr_del
  Description: 		去除音频发送地址
  Input: 			
  	1.IP			IP地址
  	2.AudioPort		音频端口
  Output:			无
  Return:			无
  Others:
*************************************************/
void audio_sendaddr_del(uint32 IP, uint16 AudioPort)
{
	RTP_ADDRESS_S rtp_addr;
	
	rtp_addr.Ip = IP;
	rtp_addr.Port = AudioPort;
	ms_filter_call_method(mMediaStream.AudioRtpSend,  MS_RTP_ADDRESS_A_DEC, &rtp_addr);
}

/*************************************************
  Function:    	audio_enc_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_enc_open(void)
{
	ms_filter_call_method(mMediaStream.AudioEnc, MS_AUDIO_SF_ENC_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioEnc, MS_AUDIO_SF_ENC_OPEN, NULL);	
}

/*************************************************
  Function:    	audio_enc_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_enc_close(void)
{
	return ms_filter_call_method(mMediaStream.AudioEnc, MS_AUDIO_SF_ENC_CLOSE, NULL);	
}

/*************************************************
  Function:    	audio_dec_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int audio_dec_enable(void)
{
	int param = 1;
	
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_ENABLE, &param);	
}


/*************************************************
  Function:    		audio_dec_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int audio_dec_disable(void)
{
	int param = 0;
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_ENABLE, &param);	
}

/*************************************************
  Function:    	audio_dec_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
 static int audio_dec_open(void)
{
	ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_OPEN, NULL);	
}

/*************************************************
  Function:    		audio_dec_close
  Description: 		
  Input:			无
  Output:			无
  Return:			无		
  Others:
*************************************************/
static int audio_dec_close(void)
{
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_CLOSE, NULL);	
}

/*************************************************
  Function:    	audio_ao_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int audio_ao_enable(void)
{
	int param = 1;
	
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_ENABLE, &param);	
}

/*************************************************
  Function:    		audio_ao_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int audio_ao_disable(void)
{
	int param = 0;
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_ENABLE, &param);	
}

/*************************************************
  Function:    	audio_ao_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_ao_open(void)
{
	ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_OPEN, NULL);	
}

/*************************************************
  Function:    	audio_ao_close
  Description: 	
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_ao_close(void)
{
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_CLOSE, NULL);	
}

/*************************************************
  Function:    	audio_ai_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int audio_ai_enable(void)
{
	int param = 1;
	
	return ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_ENABLE, &param);	
}

/*************************************************
  Function:    		audio_ai_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int audio_ai_disable(void)
{
	int param = 0;
	return ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_ENABLE, &param);	
}

/*************************************************
  Function:    	audio_ai_open
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_ai_open(void)
{
	ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_OPEN, NULL);	
}

/*************************************************
  Function:    	audio_ai_close
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int audio_ai_close(void)
{
	return ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_CLOSE, NULL);	
}

/*************************************************
  Function:    		audio_aec_enable
  Description: 		
  Input:		
  Output:			无
  Return:			无		
  Others:
*************************************************/
int audio_aec_enable(void)
{
	unsigned char flg = TRUE;
	return ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_AEC_ENABLE, &flg);	
}

/*************************************************
  Function:    		audio_aec_disable
  Description: 		
  Input:		
  Output:			无
  Return:			无		
  Others:
*************************************************/
int audio_aec_disable(void)
{
	unsigned char flg = FALSE;
	return ms_filter_call_method(mMediaStream.AudioAi, MS_AUDIO_AI_AEC_ENABLE, &flg);	
}

/*************************************************
  Function:    		audio_lyly_hint_init
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_lyly_hint_init(AUDIO_HIT_PARAM *param, void * proc)
{
	int ret = ms_filter_call_method(mMediaStream.LylyHitPlay, MS_LYLY_HIT_PARAM, (void*)param);
	if (ret != HI_SUCCESS)
	{
		return ret;
	}

	ms_filter_set_notify_callback(mMediaStream.LylyHitPlay, proc);
	if (ret != HI_SUCCESS)
	{
		return ret;
	}
	
	ret = ms_filter_call_method(mMediaStream.LylyHitPlay, MS_LYLY_HIT_OPEN, NULL);
	if (ret != HI_SUCCESS)
	{
		return ret;
	}

	return ret;
}

/*************************************************
  Function:    		audio_lyly_hint_uninit
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int audio_lyly_hint_uninit(void)
{
	return ms_filter_call_method(mMediaStream.LylyHitPlay, MS_LYLY_HIT_CLOSE, NULL);		
}

/*************************************************
  Function:			audio_jrly_record_start
  Description:		家人留言
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_jrly_record_start(char * FileName)
{
	int ret = ms_filter_call_method(mMediaStream.WavRecord, MS_WAV_RECORD_PARAM, FileName);
	if (ret == RT_SUCCESS)
	{
		ret = ms_filter_call_method(mMediaStream.WavRecord, MS_WAV_RECORD_OPEN, NULL);
	}
	return ret;
}

/*************************************************
  Function:			audio_jrly_record_stop
  Description:		
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_jrly_record_stop(void)
{
	return ms_filter_call_method(mMediaStream.WavRecord, MS_WAV_RECORD_CLOSE, NULL);
}

/*************************************************
  Function:			audio_file_play_start
  Description:		播放文件
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_file_play_start(char * FileName, int IsRepeat, void * proc)
{
	int ret = RT_FAILURE;
	AUDIOPLAY_PARAM PlayParam;
	memset(&PlayParam, 0, sizeof(AUDIOPLAY_PARAM));

	// 判断文件格式是否支持
	if ((media_stream_FileExtCmp((const uint8*)FileName, ".wav") == 0)
		|| (media_stream_FileExtCmp((const uint8*)FileName, ".WAV")) == 0)
	{
		PlayParam.FileType = FILE_WAVE;
		PlayParam.callback = (AudioPlay_CallBack)proc;
		PlayParam.IsRepeat  = IsRepeat;
		//memcpy(PlayParam.filename, filename, sizeof(PlayParam.filename));
		strcpy(PlayParam.filename, FileName);
		
		ret = ms_filter_call_method(mMediaStream.FilePlayer, MS_AUDIO_PLAY_PARAM, &PlayParam);
		if (ret == RT_SUCCESS)
		{
			ret = ms_filter_call_method(mMediaStream.FilePlayer, MS_AUDIO_PLAY_OPEN, NULL);

		}
	}
	else							
	{
		return RT_FAILURE;
	}
	
	
	return ret;

	
}

/*************************************************
  Function:			send_audio_ao
  Description:		结束文件播放
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_file_play_stop(void)
{
	return ms_filter_call_method(mMediaStream.FilePlayer, MS_AUDIO_PLAY_CLOSE, NULL);
}

/*************************************************
  Function:			audio_lyly_hint_start
  Description:		播放留言提示音
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_lyly_hint_start(AUDIO_HIT_PARAM *param, void *callbak)
{
	int ret =  audio_rtp_send_open();	
    if (ret != HI_SUCCESS)
    {
   		goto erraudio0;
    }
	
	ret = audio_lyly_hint_init(param, callbak);
	if (ret != HI_SUCCESS)
	{
		goto erraudio1;
	}
	
	ms_media_link(mMediaStream.LylyHitPlay, mMediaStream.AudioRtpSend);	
	return ret;

erraudio1:
	audio_lyly_hint_uninit();
	
erraudio0:
	audio_rtp_send_close();
	
	return ret;
}

/*************************************************
  Function:			audio_lyly_hint_stop
  Description:		结束播放留言提示音
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_lyly_hint_stop(void)
{
	audio_lyly_hint_uninit();
	audio_rtp_send_close();

	ms_media_unlink(mMediaStream.LylyHitPlay, mMediaStream.AudioRtpSend);	
	return HI_SUCCESS;
}


/*************************************************
  Function:    	audio_net_talk_start
  Description:	开始播放网络音频
  Input: 			
	1.address	对端的IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_net_talk_start(int address)
{
	int ret = -1;
		
	ret = audio_rtp_send_open();
	if (ret != 0)
	{
		printf(" audio_rtp_send_open return error!!! \n");
		goto erraudio0;
	}

	ret = audio_enc_open();
	if (ret != 0)
	{
		printf(" audio_enc_open return error!!! \n");
		goto erraudio1;
	}
	
	ret = audio_ai_open();
	if (ret != 0)
	{
		printf("audio_ai_open return error!!! \n");
		goto erraudio3;
	}

	ret = audio_rtp_recv_open(address);
	if (ret != 0)
	{
		printf("audio_rtp_recv_open return error!!! \n");
		goto erraudio4;
	}
	
	ret = audio_dec_open();
	if (ret != 0)
	{
		printf("audio_dec_open return error!!! \n");
		goto erraudio5;
	}

	ret = audio_ao_open();
	if (ret != 0)
	{
		printf("audio_ao_open return error!!! \n");
		goto erraudio6;
	}
	
	#if (_AEC_TYPE_ == _SW_AEC_)
	//echo_cancel_init(160, 2048, -100, -200); //by zxf
	echo_cancel_init(160, 160*2, -100, -200); //by zxf
	#endif
	
	log_printf("audio_net_talk_start\n");
	ms_media_link(mMediaStream.AudioDec, mMediaStream.AudioAo);
	ms_media_link(mMediaStream.AudioRtpRecv, mMediaStream.AudioDec);	
	ms_media_link(mMediaStream.AudioEnc, mMediaStream.AudioRtpSend);
	ms_media_link(mMediaStream.AudioAi, mMediaStream.AudioEnc);
	
	return HI_SUCCESS;

	
erraudio6:
	audio_dec_close();
	
erraudio5:
	audio_rtp_recv_close();
	
erraudio4:
	audio_ai_close();
	
erraudio3:
erraudio2:	
	audio_enc_close();
	
erraudio1:
	audio_rtp_send_close;

erraudio0:
	return -1;
}

/*************************************************
  Function:    		audio_net_talk_stop
  Description:		停止播放网络音频
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_net_talk_stop(void)
{
	int ret;
	
	log_printf("audio_net_talk_stop\n");
	ret = audio_dec_close();
	log_printf("audio_dec_close return ==%d\n",ret);

	ret = audio_rtp_recv_close();
	log_printf("audio_rtp_recv_close return ==%d\n",ret);
	
	ret = audio_ai_close();
	log_printf("audio_ai_close return ==%d\n",ret);

	ret = audio_ao_close();
	log_printf("audio_ao_close return ==%d\n",ret);

	ret = audio_enc_close();
	log_printf("audio_enc_close return ==%d\n",ret);
	
	ret = audio_rtp_send_close();
	log_printf("audio_rtp_send_close return ==%d\n",ret);
	

	ms_media_unlink(mMediaStream.AudioAi,  mMediaStream.AudioEnc);
	ms_media_unlink(mMediaStream.AudioEnc, mMediaStream.AudioRtpSend);
	ms_media_unlink(mMediaStream.AudioRtpRecv, mMediaStream.AudioDec);
	ms_media_unlink(mMediaStream.AudioDec, mMediaStream.AudioAo);

	#if (_AEC_TYPE_ == _SW_AEC_)
	// by zxf
	// printf("Before echo_cancel_free\n");
	echo_cancel_free();//by zxf
	// printf("After echo_cancel_free\n");
	#endif

	return ret;
}

#ifdef _ENABLE_CLOUD_
/*************************************************
  Function:    	set_cloud_audio_send_funcs
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int set_cloud_audio_send_func(void *func)
{
	return ms_filter_call_method(mMediaStream.AudioCloudSend, MS_CLOUD_SEND_AUDIO_FUNC, func);
}

/*************************************************
  Function:    	start_play_cloud_audio
  Description:	云端对讲
  Input: 			
	1.address	对端的IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
int send_cloud_audio_data(char *data)
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudRecv, MS_CLOUD_RECV_AUDIO_DATA, data);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_send_enable
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int audio_cloud_send_enable()
{
	int ret = HI_FAILURE;
	HI_BOOL param = HI_TRUE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudSend, MS_CLOUD_SEND_AUDIO_ENABLE, &param);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_send_disable
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int audio_cloud_send_disable()
{
	int ret = HI_FAILURE;
	HI_BOOL param = HI_FALSE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudSend, MS_CLOUD_SEND_AUDIO_ENABLE, &param);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_recv_enable
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int audio_cloud_recv_enable()
{
	int ret = HI_FAILURE;
	HI_BOOL param = HI_TRUE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudRecv, MS_CLOUD_RECV_AUDIO_ENABLE, &param);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_recv_disable
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
int audio_cloud_recv_disable()
{
	int ret = HI_FAILURE;
	HI_BOOL param = HI_FALSE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudRecv, MS_CLOUD_RECV_AUDIO_ENABLE, &param);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_send_open
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_cloud_send_open()
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudSend, MS_CLOUD_SEND_AUDIO_OPEN, NULL);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_send_close
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_cloud_send_close()
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudSend, MS_CLOUD_SEND_AUDIO_CLOSE, NULL);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_recv_open
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_cloud_recv_open()
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudRecv, MS_CLOUD_RECV_AUDIO_OPEN, NULL);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_recv_close
  Description:	
  Input: 			
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_cloud_recv_close()
{
	int ret = HI_FAILURE;
	ret = ms_filter_call_method(mMediaStream.AudioCloudRecv, MS_CLOUD_RECV_AUDIO_CLOSE, NULL);
	return ret;
}

/*************************************************
  Function:    	audio_cloud_talk_start
  Description:	云端对讲
  Input: 			
	1.address	对端的IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
static int audio_cloud_talk_start(void)
{
	int ret = HI_FAILURE;	

	ret = audio_cloud_send_open();
	if (ret != 0)
	{
		printf(" open_audio_rtp_send return error!!! \n");
		goto erraudio0;
	}
	
	ret = audio_ai_open();
	if (ret != 0)
	{
		printf("open_audio_ai return error!!! \n");
		goto erraudio1;
	}

	ret = audio_cloud_recv_open();
	if (ret != 0)
	{
		printf("open_audio_rtp_recv return error!!! \n");
		goto erraudio2;
	}

	ret = audio_ao_open();
	if (ret != 0)
	{
		printf("open_audio_ao return error!!! \n");
		goto erraudio3;
	}

	#if (_AEC_TYPE_ == _SW_AEC_)
	//echo_cancel_init(160, 2048, -100, -200); //by zxf
	echo_cancel_init(160, 160*2, -100, -200); //by zxf
	#endif
	
	log_printf("start_play_net_audio\n");
	ms_media_link(mMediaStream.AudioCloudRecv, mMediaStream.AudioAo);	
	ms_media_link(mMediaStream.AudioAi, mMediaStream.AudioCloudSend);	
	return HI_SUCCESS;

	
erraudio3:
	audio_cloud_recv_close();
	
erraudio2:	
	audio_ai_close();
	
erraudio1:
	audio_cloud_send_close();

erraudio0:
	return HI_FAILURE;
}

/*************************************************
  Function:    		audio_cloud_talk_stop
  Description:		停止播放网络音频
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int audio_cloud_talk_stop(void)
{
	int ret;
	
	ret = audio_cloud_recv_close();
	log_printf("audio_cloud_recv_close return ==%d\n",ret);
	
	ret = audio_ai_close();
	log_printf("close_audio_ai return ==%d\n",ret);

	ret = audio_ao_close();
	log_printf("close_audio_ao return ==%d\n",ret);

	ret = audio_cloud_send_close();
	log_printf("audio_cloud_send_close return ==%d\n",ret);
	

	ms_media_unlink(mMediaStream.AudioAi,  mMediaStream.AudioCloudSend);
	ms_media_unlink(mMediaStream.AudioCloudRecv, mMediaStream.AudioAo);

	#if (_AEC_TYPE_ == _SW_AEC_)
	// by zxf
	// printf("Before echo_cancel_free\n");
	echo_cancel_free();//by zxf
	// printf("After echo_cancel_free\n");
	#endif
	
	return HI_SUCCESS;
}
#endif

/*************************************************
  Function:    		audio_mutex_init
  Description:		
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
void audio_mutex_init(void)
{
	pthread_mutex_init(&g_audio_mutex, NULL);
}

/*************************************************
  Function:    		open_audio_mode
  Description:		开启音频模式
  Input: 			
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_audio_mode(AUDIO_STATE_E mode, void* arg1, void *arg2, void *arg3)
{
	int ret = -1;
	AUDIO_MUTEX_LOCK;
	set_audio_codec_param(&AudioParam);		
	switch (mode)
	{
		case AS_NETTALK:							// 网络对讲模式
			{
				int address = *(int *)arg1;
				//AudioParam.AiAgc = 2.0;
				ret = audio_net_talk_start(address);
			}
			break;

		case AS_PLAY:			                    // 播放模式
			{
				char *filename = (char *)arg1;
				int IsRepeat = *(int *)arg2;
				void *callbak = arg3;
				if ((g_audio_mode & AS_NETTALK) || (g_audio_mode & AS_CLOUD_TALK))
				{
					ret = -1;
				}
				else
				{
	        		ret = audio_file_play_start(filename, IsRepeat, callbak);
				}
			}
	        break;
			
		case AS_HINT_LYLY:
			{
				AUDIO_HIT_PARAM *param = (AUDIO_HIT_PARAM *)arg1;
				void *callbak = arg2;
				ret = audio_lyly_hint_start(param, callbak);           // 播放留言提示音
			}
			break;

		case AS_JRLY_RECORD:
			{
				char *filename = (char *)arg1;
				ret = audio_jrly_record_start(filename);
			}
			break;
			
		#ifdef _ENABLE_CLOUD_
		case AS_CLOUD_TALK:								// 云端对讲模式
			//AudioParam.AiAgc = 6.0;
			ret = audio_cloud_talk_start();
			break;
		#endif
			
		default:
			break;			
	}

	if (0 == ret)
	{
		//driver_speak_on();
		g_audio_mode &= mode;
	}
	AUDIO_MUTEX_UNLOCK;
	return ret;
}

/*************************************************
  Function:    	close_audio_mode
  Description:		
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int close_audio_mode(AUDIO_STATE_E mode)
{
	AUDIO_MUTEX_LOCK
	int ret = -1;
	
	switch (mode)
	{
		case AS_NETTALK:
			ret = audio_net_talk_stop();
			break;

		case AS_PLAY:	
			ret = audio_file_play_stop();
			break;

		case AS_HINT_LYLY:
			ret = audio_lyly_hint_stop();           // 留言提示音
			break;

		case AS_JRLY_RECORD:
			{
				ret = audio_jrly_record_stop();
			}
			break;
			
		#ifdef _ENABLE_CLOUD_
		case AS_CLOUD_TALK:
			ret = audio_cloud_talk_stop();
			break;
		#endif

		default:
			AUDIO_MUTEX_UNLOCK
			return -1;			
	}

	g_audio_mode &= ~mode;
	AUDIO_MUTEX_UNLOCK
	return ret;
}

