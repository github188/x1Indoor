/*********************************************************
  Copyright (C), 2013-2015
  File name:	logic_audio_enc.h
  Author:   	
  Version:   	
  Date: 		
  Description:  
  History:            
*********************************************************/
#ifndef _LOGIC_AUDIO_SFENC_H_
#define _LOGIC_AUDIO_SFENC_H_

#include "logic_include.h"
#include "logic_audio.h"

typedef struct AudioSfEncState
{
	PAYLOAD_TYPE_E PayloadType;
	int Samplerate;
	int Bitwidth;
	int channelnum;
	AUDIO_PARAM EncParam;
	uint8 AlawIndex;
	MSThread aencthread;
	uint8 Alaw[4 + AUDIO_NUM*2*SAMPLE_AUDIO_PTNUMPERFRM]; 
}AudioSfEncState;

#define MS_AUDIO_SF_ENC_OPEN	MS_MEDIA_METHOD(MS_ALAW_SF_ENC_ID, 0x00, const char)
#define MS_AUDIO_SF_ENC_CLOSE	MS_MEDIA_METHOD_NO_ARG(MS_ALAW_SF_ENC_ID, 0x01)
#define MS_AUDIO_SF_ENC_PARAM	MS_MEDIA_METHOD(MS_ALAW_SF_ENC_ID, 0x012, PAUDIO_PARAM)
#define MS_AUDIO_SF_ENC_LOCAL	MS_MEDIA_METHOD(MS_ALAW_SF_ENC_ID, 0x013, const char)

#endif

