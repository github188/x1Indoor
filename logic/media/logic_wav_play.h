
/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_audio_play.c
  Author:   	陈本惠
  Version:  	1.0
  Date: 		2015-04-29
  Description:  WAV音频播放
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#ifndef _LOGIC_AUDIO_PLAY_H_
#define _LOGIC_AUDIO_PLAY_H_

#include "logic_media_core.h"
#include "logic_audio.h"

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


typedef enum
{
	AUDIO_STATE_STOP,	                 // 停止
	AUDIO_STATE_PAUSE,	                 // 暂停
	AUDIO_STATE_START,	                 // 播放
}AUDIO_PLAY_STATE_E;

// 音频参数
typedef struct
{
	uint8 IsPack;						// 是否组包
	uint8 PackNum;						// 组包个数
	uint8 channels;						// 通道数	
	AIO_SAMPLE_RATE_E rate;				// 采样率
	AIO_BIT_WIDTH_E bit_width;			// 采样位宽	
}WAV_AUDIO_PARAM, *PWAV_AUDIO_PARAM;


typedef struct _AudioPlayState
{
	int StreamID;
	WAV_AUDIO_PARAM audio_param;
	AUDIO_PLAY_STATE_E Play_State;
	AUDIOPLAY_PARAM AudioPlay_Param;
	MSThread msplaythread;

}AudioPlayState;



#define MS_AUDIO_PLAY_OPEN		MS_MEDIA_METHOD(MS_FILE_PLAYER_ID, 0x01, const char)
#define MS_AUDIO_PLAY_PARAM		MS_MEDIA_METHOD(MS_FILE_PLAYER_ID, 0x02, const char)
#define MS_AUDIO_PLAY_CONTROL   MS_MEDIA_METHOD(MS_FILE_PLAYER_ID, 0x03, const char)   
#define MS_AUDIO_PLAY_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_FILE_PLAYER_ID, 0x01)

#endif


