/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_audio_ai.h
  Author:		¬ﬁ∑¢¬ª
  Version:		V2.0
  Date: 		2014-04-01
  Description:	“Ù∆µ ‰»Î≥Ã–Ú
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#ifndef _LOGIC_AUDIO_AI_H_
#define _LOGIC_AUDIO_AI_H_

#include "../logic_include.h"
#include "logic_audio.h"

typedef struct AudioAIState
{
	PAYLOAD_TYPE_E enPayloadType;
	int Samplerate;
	int Bitwidth;
	int channelnum;
	MSThread aithread;
	AUDIO_PARAM EncParam;
	uint8 AIIndex;
	uint8 AIPcm[SPEEX_AEC_NUM*2*SAMPLE_AUDIO_PTNUMPERFRM]; 
	uint8 AECSendPcm[SPEEX_AEC_NUM*2*SAMPLE_AUDIO_PTNUMPERFRM]; 
	uint8 AecInit;
	uint8 AecEnable;
	int AecIndex;
	int aidev;
	int dwRecMuteCnt;
	uint8 * AECPcm;
	int started;				//by zxf
	struct timeval tv;
}AudioAIState;

#define MS_AUDIO_AI_OPEN			MS_MEDIA_METHOD(MS_AUDIO_AI_ID, 0x00, const char)
#define MS_AUDIO_AI_CLOSE			MS_MEDIA_METHOD_NO_ARG(MS_AUDIO_AI_ID, 0x01)
#define MS_AUDIO_AI_PARAM			MS_MEDIA_METHOD(MS_AUDIO_AI_ID,0x02, PAUDIO_PARAM)
#define MS_AUDIO_AI_AEC_ENABLE		MS_MEDIA_METHOD(MS_AUDIO_AI_ID, 0x03, const char)
#endif

