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
#ifndef _AU_AUDIO_H_
#define _AU_AUDIO_H_
#include "logic_include.h"

#define _SW_AEC_				0			// 自己公司的消回音
#define _HW_AEC_				1			// 硬件消回音或者其他方案的消回音
#define _AEC_TYPE_				_HW_AEC_

#define AUDIO_NUM				6
//#define AUDIO_AEC_NUM			6


#if (_AEC_TYPE_ == _HW_AEC_)
#define SPEEX_AEC_NUM   		2 
#else
#define SPEEX_AEC_NUM   		6
#endif

#define SAMPLE_AUDIO_PTNUMPERFRM	80

#define _AEC_BUF_SIZE_			(6000)
#define _AEC_FRAME_SIZE			(2*SAMPLE_AUDIO_PTNUMPERFRM)
#if 1
#define AACRECORDER_BIT_RATE	64000				// 64K bps
#define AACRECORDER_CHANNEL_NUM	1		   			// Mono
#define AACRECORDER_QUALITY		90		   			// 1 ~ 999
#define AUDIO_SAMPLE_RATE_8000 	8000      			// 采样率
#define AUDIO_BIT_WIDTH_16     	16        			// 采样位数
#endif
			
// 音频状态模式
typedef enum
{
	AS_CLOSE,										// 关闭状态
	AS_NETTALK,										// 网络通话状态
	AS_PLAY,										// 本机播放模式
	AM_HINT_LYLY,									// 播放留言提示音模式
}AUDIO_STATE_E;		

// 音频参数
typedef struct
{
	uint8 MicValue;									// mic大小
	uint8 SpkValue;									// spk大小
	uint8 IsAec;									// 是否回声抵消
	uint8 IsPack;									// 是否组包
	uint8 PackNum;									// 组包个数
	uint8 AgcType;
	uint8 VideoMode;
	uint8 recsrc;
	int byte_per_packet;							// 每包有多少字节数
	float MultAgcH;
	float MultAgcL;
	float AIAgcH;
	float AIAgcL;
	float SpkAgcH;
	float SpkAgcL;
	float AoAgcH;
	float AoAgcL;	
}AUDIO_PARAM, * PAUDIO_PARAM;

typedef void (*PMEDIA_AU_CALLBACK)(uint32 cmd, uint32 time, uint32 percent);

// 文件格式
typedef enum
{
	FILE_NONE				= 0x00,
	FILE_TXT				= 0x01,
	FILE_BMP				= 0x02,
	FILE_JPG				= 0x03,
	FILE_GIF				= 0x04,
	FILE_PNG				= 0x05,
	FILE_FLASH				= 0x06,
	FILE_MP3				= 0x20,
	FILE_WAVE				= 0x21,	
	FILE_AAC				= 0x22,
	FILE_MP4				= 0x23,
	FILE_ASF				= 0x24,
	FILE_BUTT				= 0xFF
}FILE_TYPE_E;

// 播放顺序及次数
typedef enum
{
    AR_ONCE,										// 单次播放
    AR_REPEAT,										// 单曲循环
    AR_ALLONCE,										// 顺序播放
    AR_ALLREPEAT,									// 循环播放
    AR_TRY											// 测试
}AUDIO_REPEAT_E;

// 音频模式
typedef enum
{
	AM_CLOSE,										// 关闭模式
	AM_NET_TALK,									// 网络通话模式
	AM_NET_REC,										// 网络录制留影留言模式
	AM_NET_PLAY,									// 播放留言提示音模式
	AM_LOCAL_REC,									// 家人留言模式
	AM_LOCAL_PLAY,									// 本机播放模式
	AM_LYLY_PLAY									// 留影留言播放
}AUDIO_MODE_E;	

//音频控制
typedef enum
{
	AS_LPLAY,										// 播放
    AS_FFD, 										// 快进
    AS_FFW, 										// 快退
    AS_PAUSE,										// 暂停
    AS_STOP,  										// 停止
    AS_ERROR,										// 错误
    AS_REC,
    AS_TALK,
    AS_SAVE											// 保存,用在录音或留影留言时
}AUDIO_CONTROL_STATE_E;	

typedef struct _MEDIA_AU_CTRL
{
	FILE * filehald;								// 文件指针
	char filename[50];								// 文件名
	uint32 FileSize;								// 文件大小
	uint8 * data;									// 文件内容
	
	PMEDIA_AU_CALLBACK	callback;					// 回调
	AUDIO_MODE_E	mode;							// 工件模式
	AUDIO_STATE_E 	state;							// 工作状态
	unsigned long 	totaltime;						// 文件总时间
	unsigned long 	remaintime;						// 剩余时间
	unsigned long 	currenttime;					// 当前播放时间
	uint8			volume;							// 播放音量
	AUDIO_REPEAT_E	repeat;							// 播放模式
	unsigned int	codectype;						// 音频编码类型

	unsigned long	bitrate;						// 每秒采样的位数
	unsigned long	samplerate;						// 采样率,每秒采样的次数
	uint8			channels;						// 通道数
}MEDIA_AU_CTRL, * PMEDIA_AU_CTRL;

extern AUDIO_PARAM AudioParam;

typedef void (*AudioPlay_CallBack)(int cmd, int time, int percent);

// 音频文件播放参数
typedef struct
{
	char filename[50];					// 文件名称(加上路径)
	uint8 IsRepeat;						// 是否循环
	FILE_TYPE_E FileType;				// 文件类型
	AudioPlay_CallBack callback;		// 文件播放回调函数
}AUDIOPLAY_PARAM, *PAUDIOPLAY_PARAM;

/*************************************************
  Function:    		set_audio_pack_mode
  Description: 		设置音频是否组包
  Input:			无
  Output:			无
  Return:			无		
  Others:
*************************************************/
void set_audio_pack_mode(uint8 IsPack);

/*************************************************
  Function:    	close_audio_local_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_local_enc(void);

/*************************************************
  Function:    	open_audio_local_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_local_enc(void);

/*************************************************
  Function:    		set_audio_codec_param
  Description:		设置音频参数
  Input: 		
  Output:			AudioParam 音频参数变量
  Return:			无
  Others:
*************************************************/
void set_audio_codec_param(AUDIO_PARAM *g_AudioParam, int len);

/*************************************************
  Function:    	open_audio_ai
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_ai(void);

/*************************************************
  Function:    	close_audio_ai
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_ai(void);

/*************************************************
  Function:    	open_audio_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_enc(void);

/*************************************************
  Function:    	close_audio_enc
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_enc(void);

/*************************************************
  Function:    		set_audio_ai_enable
  Description: 		设置是否开启消回声
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int set_audio_aec_enable(unsigned char enable);

/*************************************************
  Function:    		open_audio_mode
  Description:		开启音频模式
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_audio_mode(AUDIO_STATE_E mode, int address, int len);

/*************************************************
  Function:    		close_audio_mode
  Description:		
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
int close_audio_mode(AUDIO_STATE_E mode);

/*************************************************
  Function:    	open_dec_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_dec_enable(void);

/*************************************************
  Function:    		open_dec_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int open_dec_disable(void);

/*************************************************
  Function:    		get_audio_addr_count
  Description: 		获取RTP音频连接数
  Input:			无
  Output:			无
  Return:			已连接的数量		
  Others:
*************************************************/
int get_audio_addr_count(void);

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
void del_audio_sendaddr(uint32 IP, uint16 AudioPort);

/*************************************************
  Function:			play_sound_file
  Description: 		开始播放声音文件和家人留言
  Input: 			
  	1.FileName		文件名称
  	2.proc			播放进度回调函数,可以为空
  Output:			播放ID, 0为失败, 结束时用此ID
  Return:			无
*************************************************/
int play_sound_file(char * FileName, int IsRepeat, void * proc);

/*************************************************
  Function:			stop_play_file
  Description: 		停止播放声音
  Input: 			
  	1.FileName		
  	2.proc			
  Output:			
  Return:			
*************************************************/
 int stop_play_file(void);

/*************************************************
  Function:    	start_play_hint_lyly
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 start_play_hint_lyly(uint8 RemoteDeviceType, char * FileName, void * proc);

/*************************************************
  Function:    	stop_play_hint_lyly
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 stop_play_hint_lyly(void);

/*************************************************
  Function:    	close_audio_rtp_send
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int close_audio_rtp_send(void);

/*************************************************
  Function:    	open_audio_rtp_send
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int open_audio_rtp_send(void);

/*************************************************
  Function:    		open_audio_rtp_recv
  Description: 		留影留言中开启音频接口
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int32 start_lyly_audio_recv(int32 address);

/*************************************************
  Function:    		stop_lyly_audio_recv
  Description: 		
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void stop_lyly_audio_recv(void);

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
int32 add_audio_sendaddr(uint32 IP, uint16 AudioPort);
#endif

