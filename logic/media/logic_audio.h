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




// 采样率
typedef enum auAIO_SAMPLE_RATE_E
{
    AIO_SAMPLE_RATE_8       = 8000,     /* 8 k Hz sampling rate.     */
    AIO_SAMPLE_RATE_11025   = 11025,    /* 11.025 k Hz sampling rate.*/
    AIO_SAMPLE_RATE_16      = 16000,    /* 16 k Hz sampling rate.    */
    AIO_SAMPLE_RATE_22050   = 22050,    /* 22.050 k Hz sampling rate.*/
    AIO_SAMPLE_RATE_24      = 24000,    /* 24 k Hz sampling rate.    */
    AIO_SAMPLE_RATE_32      = 32000,    /* 32 k Hz sampling rate.    */
    AIO_SAMPLE_RATE_441     = 44100,    /* 44.1 k Hz sampling rate.  */
    AIO_SAMPLE_RATE_48      = 48000,    /* 48 k Hz sampling rate.    */
    AIO_SAMPLE_RATE_64      = 64000,    /* 64 k Hz sampling rate.    */
    AIO_SAMPLE_RATE_882     = 88000,    /* 88.2 k Hz sampling rate.  */
    AIO_SAMPLE_RATE_96      = 96000,    /* 96 k Hz sampling rate.    */
    AIO_SAMPLE_RATE_1764    = 176400,   /* 176.4 k Hz sampling rate. */
    AIO_SAMPLE_RATE_192     = 192000,   /* 192 k Hz sampling rate.   */
    AIO_SAMPLE_RATE_BUTT
} AIO_SAMPLE_RATE_E;

// 采样位宽
typedef enum auAIO_BIT_WIDTH_E
{
    AIO_BIT_WIDTH_8   = 8,   			/* 8-bit sampling bitwidth.  */
    AIO_BIT_WIDTH_16  = 16,  			/* 16-bit sampling bitwidth. */
    AIO_BIT_WIDTH_20  = 20,   			/* 20-bit sampling bitwidth. */
    AIO_BIT_WIDTH_24  = 24,   			/* 24-bit sampling bitwidth. */
    AIO_BIT_WIDTH_BUTT
}AIO_BIT_WIDTH_E;


// 音频状态模式
typedef enum
{
	AS_NONE			= 0x00,							// 关闭状态
	AS_NETTALK		= 0x01,							// 网络通话状态
	AS_PLAY			= 0x02,							// 本机播放模式
	AS_HINT_LYLY 	= 0x04,							// 播放留言提示音模式
	AS_JRLY_RECORD  = 0x08,							// 本地录制模式
	AS_CLOUD_TALK	= 0x10,							// 云端对讲
}AUDIO_STATE_E;			

// 音频参数
typedef struct
{	
	bool isPack;								
	float AiAgc;
	float AoAgc;
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
  Function:    	audio_dec_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int audio_dec_enable(void);

/*************************************************
  Function:    		audio_dec_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int audio_dec_disable(void);

/*************************************************
  Function:    	audio_ai_enable
  Description: 		
  Input:		无
  Output:		无
  Return:		无		
  Others:
*************************************************/
int audio_ai_enable(void);

/*************************************************
  Function:    		audio_ai_disable
  Description: 		
  Input:		无
  Output:		无
  Return:		无	
  Others:
*************************************************/
int audio_ai_disable(void);

/*************************************************
  Function:    		audio_aec_enable
  Description: 		
  Input:		
  Output:			无
  Return:			无		
  Others:
*************************************************/
int audio_aec_enable(void);

/*************************************************
  Function:    		audio_aec_disable
  Description: 		
  Input:		
  Output:			无
  Return:			无		
  Others:
*************************************************/
int audio_aec_disable(void);

/*************************************************
  Function:    		open_audio_mode
  Description:		开启音频模式
  Input: 			len=0 忽略长度 其他=每包长度单位字节
  Output:			无
  Return:			无
  Others:
*************************************************/
int open_audio_mode(AUDIO_STATE_E mode, void* arg1, void *arg2, void *arg3);

/*************************************************
  Function:    	close_audio_mode
  Description:		
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int close_audio_mode(AUDIO_STATE_E mode);
#endif

