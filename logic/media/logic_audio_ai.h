/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_audio_ai.h
  Author:		罗发禄
  Version:		V2.0
  Date: 		2014-04-01
  Description:	音频输入程序
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#ifndef _LOGIC_AUDIO_AI_H_
#define _LOGIC_AUDIO_AI_H_

#include "logic_audio.h"

typedef struct _AudioAIParam
{	
	bool Enable;					// 接口使能
	bool AecEnable;					// 消回音使能
	float AiAgc;					// 增益值
	MSThread aithread;				
}AudioAIParam;

#define MS_AUDIO_AI_OPEN			MS_MEDIA_METHOD(MS_AUDIO_AI_ID, 0x00, const char)
#define MS_AUDIO_AI_CLOSE			MS_MEDIA_METHOD_NO_ARG(MS_AUDIO_AI_ID, 0x01)
#define MS_AUDIO_AI_PARAM			MS_MEDIA_METHOD(MS_AUDIO_AI_ID,0x02, PAUDIO_PARAM)
#define MS_AUDIO_AI_ENABLE			MS_MEDIA_METHOD(MS_AUDIO_AI_ID, 0x03, const char)
#define MS_AUDIO_AI_AEC_ENABLE		MS_MEDIA_METHOD(MS_AUDIO_AI_ID, 0x04, const char)
#endif

