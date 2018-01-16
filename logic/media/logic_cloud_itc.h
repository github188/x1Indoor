/*********************************************************
  Copyright (C), 2013-2015
  File name:	logic_cloud_itc.h
  Author:   	
  Version:   	
  Date: 		
  Description:  
  History:            
*********************************************************/
#ifndef _LOGIC_CLOUD_ITC_H_
#define _LOGIC_CLOUD_ITC_H_

#include "logic_include.h"
#include "logic_audio.h"

#ifdef _ENABLE_CLOUD_

typedef struct _CloudItcState
{
	uint8 PayloadType;
	uint8 Enable;
	unsigned int Sequence;
	MsCloudDataFunc func;
}CloudItcState;

#define MS_CLOUD_SEND_AUDIO_OPEN		MS_MEDIA_METHOD(MS_CLOUD_SEND_AUDIO_ID, 0x00, const char)
#define MS_CLOUD_SEND_AUDIO_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_CLOUD_SEND_AUDIO_ID, 0x01)
#define MS_CLOUD_SEND_AUDIO_PAYLOAD		MS_MEDIA_METHOD(MS_CLOUD_SEND_AUDIO_ID, 0x12, const char)
#define MS_CLOUD_SEND_AUDIO_FUNC		MS_MEDIA_METHOD(MS_CLOUD_SEND_AUDIO_ID, 0x13, const char)
#define MS_CLOUD_SEND_AUDIO_ENABLE		MS_MEDIA_METHOD(MS_CLOUD_SEND_AUDIO_ID, 0x14, int)

#define MS_CLOUD_SEND_VIDEO_OPEN		MS_MEDIA_METHOD(MS_CLOUD_SEND_VIDEO_ID, 0x00, const char)
#define MS_CLOUD_SEND_VIDEO_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_CLOUD_SEND_VIDEO_ID, 0x01)
#define MS_CLOUD_SEND_VIDEO_PAYLOAD		MS_MEDIA_METHOD(MS_CLOUD_SEND_VIDEO_ID, 0x12, const char)
#define MS_CLOUD_SEND_VIDEO_FUNC		MS_MEDIA_METHOD(MS_CLOUD_SEND_VIDEO_ID, 0x13, const char)
#define MS_CLOUD_SEND_VIDEO_ENABLE		MS_MEDIA_METHOD(MS_CLOUD_SEND_VIDEO_ID, 0x14, int)

#define MS_CLOUD_RECV_AUDIO_OPEN		MS_MEDIA_METHOD(MS_CLOUD_RECV_AUDIO_ID, 0x00, const char)
#define MS_CLOUD_RECV_AUDIO_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_CLOUD_RECV_AUDIO_ID, 0x01)
#define MS_CLOUD_RECV_AUDIO_ENABLE		MS_MEDIA_METHOD(MS_CLOUD_RECV_AUDIO_ID, 0x13, int)
#define MS_CLOUD_RECV_AUDIO_DATA		MS_MEDIA_METHOD(MS_CLOUD_RECV_AUDIO_ID, 0x14, int)
#endif
#endif

