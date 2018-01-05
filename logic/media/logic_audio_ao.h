/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_audio_ao.h
  Author:		ÂÞ·¢Â»
  Version:		V2.0
  Date: 		2015-03-14
  Description:	ÒôÆµAO²¥·Å³ÌÐò
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#ifndef _LOGIC_AUDIO_AO_H_
#define _LOGIC_AUDIO_AO_H_
#include "../logic_include.h"
#include "logic_audio.h"

typedef struct AudioAoState
{
	PAYLOAD_TYPE_E enPayloadType;
	int Samplerate;
	int Bitwidth;
	int channelnum;
	AUDIO_PARAM	AParam;
	NT_BOOL AoEnable;
	int aoDev;
}AudioAoState; 

#define MS_AUDIO_AO_OPEN		MS_MEDIA_METHOD(MS_ALAW_AO_ID,0x00,const char)
#define MS_AUDIO_AO_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_ALAW_AO_ID,0x01)
#define MS_AUDIO_AO_PARAM		MS_MEDIA_METHOD(MS_ALAW_AO_ID,0x02, PAUDIO_PARAM)
#define MS_AUDIO_AO_ENABLE		MS_MEDIA_METHOD(MS_ALAW_AO_ID,0x03, int)

#endif

