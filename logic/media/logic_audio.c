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
#include "storage_include.h"
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
#include "logic_audio_ai.h"
#include "logic_audio_ao.h"
#include "logic_play_lyly_hit.h"

static uint8 g_AudioIsPack = 0;

#if (_AEC_TYPE_ == _SW_AEC_)
//by zxf
void echo_cancel_init(int size, int tail, int noiselevel, int echolevel);
void echo_cancel_free(void);
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
  Function:    	open_audio_rtp_send
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_rtp_send(void)
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
  Function:    	close_audio_rtp_send
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_rtp_send(void)
{
	int ret = -1;
	
	ret = ms_filter_call_method(mMediaStream.AudioRtpSend, MS_RTP_SEND_A_CLOSE, NULL);
	
	return ret;
}

/*************************************************
  Function:    	open_audio_rtp_recv
  Description: 	打开音频RTP接收
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
static int open_audio_rtp_recv(int address)
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
  Function:    	close_audio_rtp_recv
  Description: 	关闭音频RTP接收
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int close_audio_rtp_recv(void)
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
  Function:    		add_audio_sendaddr
  Description: 		增加音频发送地址
  Input: 			
  	1.IP			IP地址
  	2.AudioPort		音频端口
  Output:			无
  Return:			成功与否true/false
  Others:
*************************************************/
int32 add_audio_sendaddr(uint32 IP, uint16 AudioPort)
{
	RTP_ADDRESS_S rtp_addr;
	
	rtp_addr.Ip = IP;
	rtp_addr.Port = AudioPort;
	ms_filter_call_method(mMediaStream.AudioRtpSend, MS_RTP_ADDRESS_A_ADD, &rtp_addr);
	
	return 0;
}

/*************************************************
  Function:    		del_audio_sendaddr
  Description: 		去除音频发送地址
  Input: 			
  	1.IP			IP地址
  	2.AudioPort		音频端口
  Output:			无
  Return:			无
  Others:
*************************************************/
void del_audio_sendaddr(uint32 IP, uint16 AudioPort)
{
	RTP_ADDRESS_S rtp_addr;
	
	rtp_addr.Ip = IP;
	rtp_addr.Port = AudioPort;
	ms_filter_call_method(mMediaStream.AudioRtpSend,  MS_RTP_ADDRESS_A_DEC, &rtp_addr);
}

/*************************************************
  Function:    		get_audio_addr_count
  Description: 		获取RTP音频连接数
  Input:			无
  Output:			无
  Return:			已连接的数量		
  Others:
*************************************************/
int get_audio_addr_count(void)
{
	int ret = -1;
	
	ret = ms_filter_call_method(mMediaStream.AudioRtpSend, MS_RTP_ADDRESS_A_ADD, NULL);
	
	return ret;
}

/*************************************************
  Function:    		set_audio_aec_enable
  Description: 		设置是否开启消回声
  Input:		
	  enable		0 不开启 1 开启
  Output:			无
  Return:			无		
  Others:
*************************************************/
int set_audio_aec_enable(unsigned char enable)
{
	unsigned char flg = enable;
	return ms_filter_call_method(mMediaStream.AudioAI, MS_AUDIO_AI_AEC_ENABLE, &flg);	
}

/*************************************************
  Function:    	open_audio_ai
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_ai(void)
{
	ms_filter_call_method(mMediaStream.AudioAI, MS_AUDIO_AI_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioAI, MS_AUDIO_AI_OPEN, NULL);	
}

/*************************************************
  Function:    	close_audio_ai
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_ai(void)
{
	return ms_filter_call_method(mMediaStream.AudioAI, MS_AUDIO_AI_CLOSE, NULL);	
}

/*************************************************
  Function:    	open_audio_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_enc(void)
{
	ms_filter_call_method(mMediaStream.AudioSfEnc, MS_AUDIO_SF_ENC_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioSfEnc, MS_AUDIO_SF_ENC_OPEN, NULL);	
}

/*************************************************
  Function:    	close_audio_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_enc(void)
{
	return ms_filter_call_method(mMediaStream.AudioSfEnc, MS_AUDIO_SF_ENC_CLOSE, NULL);	
}

/*************************************************
  Function:    	open_audio_local_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_local_enc(void)
{
	uint8 flg = TRUE;
	return ms_filter_call_method(mMediaStream.AudioSfEnc, MS_AUDIO_SF_ENC_LOCAL, &flg);	
}

/*************************************************
  Function:    	close_audio_local_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_local_enc(void)
{
	uint8 flg = FALSE;
	return ms_filter_call_method(mMediaStream.AudioSfEnc, MS_AUDIO_SF_ENC_LOCAL, &flg);	
}

/*************************************************
  Function:    	open_dec_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_dec_enable(void)
{
	int param = 1;
	
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_ENABLE, &param);	
}


/*************************************************
  Function:    		open_dec_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int open_dec_disable(void)
{
	int param = 0;
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_ENABLE, &param);	
}

/*************************************************
  Function:    	open_audio_dec
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
 static int open_audio_dec(void)
{
	ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_OPEN, NULL);	
}

/*************************************************
  Function:    		close_audio_dec
  Description: 		
  Input:			无
  Output:			无
  Return:			无		
  Others:
*************************************************/
static int close_audio_dec(void)
{
	return ms_filter_call_method(mMediaStream.AudioDec, MS_AUDIO_DEC_CLOSE, NULL);	
}

/*************************************************
  Function:    	open_ao_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_ao_enable(void)
{
	int param = 1;
	
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_ENABLE, &param);	
}

/*************************************************
  Function:    		open_ao_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int open_ao_disable(void)
{
	int param = 0;
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_ENABLE, &param);	
}

/*************************************************
  Function:    	open_audio_ao
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
 static int open_audio_ao(void)
{
	ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_PARAM, &AudioParam);
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_OPEN, NULL);	
}

/*************************************************
  Function:    	close_audio_ao
  Description: 	
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
static int close_audio_ao(void)
{
	return ms_filter_call_method(mMediaStream.AudioAo, MS_AUDIO_AO_CLOSE, NULL);	
}

/*************************************************
  Function:    	start_play_net_audio
  Description:	开始播放网络音频
  Input: 			
	1.address	对端的IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
static int start_play_net_audio(int address)
{
	int ret = -1;
		
	ret = open_audio_rtp_send();
	if (ret != 0)
	{
		printf(" open_audio_rtp_send return error!!! \n");
		goto erraudio0;
	}

	ret = open_audio_enc();
	if (ret != 0)
	{
		printf(" open_audio_enc return error!!! \n");
		goto erraudio1;
	}
	
	ret = open_audio_ai();
	if (ret != 0)
	{
		printf("open_audio_ai return error!!! \n");
		goto erraudio3;
	}

	ret = open_audio_rtp_recv(address);
	if (ret != 0)
	{
		printf("open_audio_rtp_recv return error!!! \n");
		goto erraudio4;
	}
	
	ret = open_audio_dec();
	if (ret != 0)
	{
		printf("open_audio_dec return error!!! \n");
		goto erraudio5;
	}

	ret = open_audio_ao();
		if (ret != 0)
	{
		printf("open_audio_ao return error!!! \n");
		goto erraudio6;
	}
	

	#if (_AEC_TYPE_ == _SW_AEC_)
	//echo_cancel_init(160, 2048, -100, -200); //by zxf
	echo_cancel_init(160, 160*2, -100, -200); //by zxf
	#endif
	
	log_printf("start_play_net_audio\n");
	ms_media_link(mMediaStream.AudioDec, mMediaStream.AudioAo);
	ms_media_link(mMediaStream.AudioRtpRecv, mMediaStream.AudioDec);	
	ms_media_link(mMediaStream.AudioSfEnc, mMediaStream.AudioRtpSend);
	ms_media_link(mMediaStream.AudioAI, mMediaStream.AudioSfEnc);
	
	return HI_SUCCESS;

erraudio6:
	close_audio_dec();
	
erraudio5:
	close_audio_rtp_recv();
	
erraudio4:
	close_audio_ai();
	
erraudio3:	
erraudio2:	
	close_audio_enc();
	
erraudio1:
	close_audio_rtp_send;

erraudio0:
	return -1;
}

/*************************************************
  Function:    		stop_play_net_audio
  Description:		停止播放网络音频
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static void stop_play_net_audio(void)
{
	int ret;
	
	log_printf("stop_play_net_audio\n");
	ret = close_audio_dec();
	log_printf("close_audio_dec return ==%d\n",ret);

	ret = close_audio_rtp_recv();
	log_printf("close_audio_rtp_recv return ==%d\n",ret);
	
	ret = close_audio_ai();
	log_printf("close_audio_ai return ==%d\n",ret);

	ret = close_audio_ao();
	log_printf("close_audio_ao return ==%d\n",ret);

	ret = close_audio_enc();
	log_printf("close_audio_enc return ==%d\n",ret);
	
	//ret = close_audio_agc();
	//log_printf("close_audio_agc return ==%d\n",ret);
	
	ret = close_audio_rtp_send();
	log_printf("close_audio_rtp_send return ==%d\n",ret);
	

	ms_media_unlink(mMediaStream.AudioAI,  mMediaStream.AudioSfEnc);
	ms_media_unlink(mMediaStream.AudioSfEnc, mMediaStream.AudioRtpSend);
	ms_media_unlink(mMediaStream.AudioRtpRecv, mMediaStream.AudioDec);
	ms_media_unlink(mMediaStream.AudioDec, mMediaStream.AudioAo);

	#if (_AEC_TYPE_ == _SW_AEC_)
	// by zxf
	// printf("Before echo_cancel_free\n");
	echo_cancel_free();//by zxf
	// printf("After echo_cancel_free\n");
	#endif
}

/*************************************************
  Function:			start_play_file
  Description:		播放文件
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int start_play_file(void)
{
	
}

/*************************************************
  Function:			send_audio_ao
  Description:		结束文件播放
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
 int stop_play_file(void)
{
	return ms_filter_call_method(mMediaStream.FilePlayer, MS_AUDIO_PLAY_CLOSE, NULL);
}

/*************************************************
  Function:			start_hint_lyly_play
  Description:		播放留言提示音
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int start_hint_lyly_play(void)
{	
 	int ret =  open_audio_rtp_send();	
    if (ret != HI_SUCCESS)
    {
   		goto erraudio1;
    }

	ms_media_link(mMediaStream.LylyFilePlayer, mMediaStream.AudioRtpSend);	
	return ret;

erraudio1:
	close_audio_rtp_send();
	
    return ret;
}

/*************************************************
  Function:			stop_hint_lyly_play
  Description:		结束播放留言提示音
  Input:			无
  Output:			无
  Return:			无
  Others:
*************************************************/
static int stop_hint_lyly_play(void)
{
	int ret = close_audio_rtp_send();
	if (0 == ret)
	{
		ms_media_unlink(mMediaStream.LylyFilePlayer, mMediaStream.AudioRtpSend);
	}
	
	return HI_SUCCESS;
}

/*************************************************
  Function:    		set_audio_codec_param
  Description:		设置音频参数
  Input: 		
  Output:			AudioParam 音频参数变量
  Return:			无
  Others:
*************************************************/
void set_audio_codec_param(AUDIO_PARAM *g_AudioParam, int len)
{	
	memset(g_AudioParam, 0, sizeof(AUDIO_PARAM));
	g_AudioParam->MicValue = 95;
	g_AudioParam->SpkValue = 95;
	g_AudioParam->IsAec = 1;
	g_AudioParam->IsPack = g_AudioIsPack;
	//g_AudioParam->PackNum = 2;
	g_AudioParam->PackNum = AUDIO_NUM;
	g_AudioParam->byte_per_packet = len;
	g_AudioParam->AgcType = 0;
	g_AudioParam->MultAgcH = 3.0;
	g_AudioParam->MultAgcL = 1.0;
	g_AudioParam->AIAgcH = 0.8;
	g_AudioParam->AIAgcL = 1.0;
	g_AudioParam->SpkAgcH = 3.0;
	g_AudioParam->SpkAgcL = 3.0;
	g_AudioParam->AoAgcH = 1.0;
	g_AudioParam->AoAgcL = 0.8;
}

/*************************************************
  Function:    		play_sound_file
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int play_sound_file(char * FileName, int IsRepeat, void * proc)
{
	AUDIOPLAY_PARAM PlayParam;
	memset(&PlayParam, 0, sizeof(AUDIOPLAY_PARAM));

	// 判断文件格式是否支持
	if ((media_stream_FileExtCmp((const uint8*)FileName, ".wav") == 0)
		|| (media_stream_FileExtCmp((const uint8*)FileName, ".WAV")) == 0)
	{
		PlayParam.FileType = FILE_WAVE;
	}
	else							
	{
		return RT_FAILURE;
	}
	
	PlayParam.callback = (AudioPlay_CallBack)proc;
	PlayParam.IsRepeat  = IsRepeat;
	//memcpy(PlayParam.filename, filename, sizeof(PlayParam.filename));
	strcpy(PlayParam.filename, FileName);
	
	int ret = ms_filter_call_method(mMediaStream.FilePlayer, MS_AUDIO_PLAY_PARAM, &PlayParam);
	if (ret == RT_SUCCESS)
	{
		ret = ms_filter_call_method(mMediaStream.FilePlayer, MS_AUDIO_PLAY_OPEN, NULL);

	}
	return ret;

	
}


/*************************************************
  Function:    		start_play_hint_lyly
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 start_play_hint_lyly(uint8 RemoteDeviceType, char * FileName, void * proc)
{
	AUDIO_HIT_PARAM param;
	memset(&param, 0, sizeof(AUDIO_HIT_PARAM));
	if ((media_stream_FileExtCmp((const uint8*)FileName, ".wav") == 0)
		|| (media_stream_FileExtCmp((const uint8*)FileName, ".WAV")) == 0)
	{
		param.FileType = FILE_WAVE;
	}
	else							
	{
		return HI_FAILURE;
	}

	strcpy(param.filename, FileName);
	param.IsPack = TRUE;
	param.PackNum = 6;
	if (DEVICE_TYPE_STAIR == RemoteDeviceType)
	{
		param.PerFrameNum = 1;		// 梯口机每帧80
	}
	else
	{
		param.PerFrameNum = param.PackNum;
	}
		
	int ret = ms_filter_call_method(mMediaStream.LylyFilePlayer, MS_AUDIO_LYLY_PARAM, (void*)&param);
	if (ret != HI_SUCCESS)
	{
		return ret;
	}

	ms_filter_set_notify_callback(mMediaStream.LylyFilePlayer, proc);
	if (ret != HI_SUCCESS)
	{
		return ret;
	}
	
	ret = ms_filter_call_method(mMediaStream.LylyFilePlayer, MS_AUDIO_LYLY_OPEN, NULL);
	if (ret != HI_SUCCESS)
	{
		return ret;
	}

	return ret;
}

/*************************************************
  Function:    		stop_play_hint_lyly
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 stop_play_hint_lyly(void)
{
	int ret = ms_filter_call_method(mMediaStream.LylyFilePlayer, MS_AUDIO_LYLY_CLOSE, NULL);		
	if (ret != HI_SUCCESS)
	{
		return ret;
	}
	ret = HI_SUCCESS;
	
	return ret;
}

/*************************************************
  Function:    		open_audio_rtp_recv
  Description: 		留影留言中开启音频接口
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int32 start_lyly_audio_recv(int32 address)
{
	return open_audio_rtp_recv(address);
}

/*************************************************
  Function:    		stop_lyly_audio_recv
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void stop_lyly_audio_recv(void)
{
	close_audio_rtp_recv();
}

/*************************************************
  Function:    		open_audio_mode
  Description:		开启音频模式
  Input: 			len=0 忽略长度 其他=每包长度单位字节
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_audio_mode(AUDIO_STATE_E mode, int address, int len)
{
	int ret = -1;
	
	log_printf("len==============%d\n", len);
	#if TEST_AUDIO									// 调试用
	#else
	set_audio_codec_param(&AudioParam, len);
	#endif
		
	switch (mode)
	{
		case AS_NETTALK:							// 网络对讲模式
			ret = start_play_net_audio(address);
			break;

		case AS_PLAY:			                    // 播放模式
	        ret = start_play_file();
	        break;
			
		case AM_HINT_LYLY:
			ret = start_hint_lyly_play();           // 播放留言提示音
			break;
			
		default:
			break;			
	}

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
	int ret = -1;

	switch (mode)
	{
		case AS_NETTALK:
			stop_play_net_audio();
			ret = 0;
			break;

		case AS_PLAY:	
			ret = stop_play_file();
			break;

		case AM_HINT_LYLY:
			ret = stop_hint_lyly_play();           // 留言提示音
			break;

		default:
			break;			
	}

	return ret;
}
