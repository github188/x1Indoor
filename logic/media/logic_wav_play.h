
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


