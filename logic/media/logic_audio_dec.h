/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_audio_dec.h
  Author:		ÂÞ·¢Â»
  Version:		V2.0
  Date: 		2014-04-01
  Description:	ÒôÆµ½âÂë³ÌÐò
  
History:				 
	1.Date: 
	Author: 
	Modification: 
*********************************************************/
#ifndef _LOGIC_AUDIO_DEC_H_
#define _LOGIC_AUDIO_DEC_H_

#include "logic_audio.h"

typedef struct _AudioDecParam
{
	bool Enable;	
}AudioDecParam; 

#define MS_AUDIO_DEC_OPEN		MS_MEDIA_METHOD(MS_ALAW_DEC_ID, 0x00, const char)
#define MS_AUDIO_DEC_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_ALAW_DEC_ID, 0x01)
#define MS_AUDIO_DEC_PARAM		MS_MEDIA_METHOD(MS_ALAW_DEC_ID, 0x02, PAUDIO_PARAM)
#define MS_AUDIO_DEC_ENABLE		MS_MEDIA_METHOD(MS_ALAW_DEC_ID, 0x03, int)
#endif

