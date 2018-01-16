/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_h264_enc.h
  Author:		chenbh
  Version:		V2.0
  Date: 		2015-06-06
  Description:	 ”∆µ±‡¬Î≥Ã–Ú
  
	History:				 
		1.Date: 
		Author: 
		Modification: 
*********************************************************/
#ifndef _LOGIC_H264_ENC_H_
#define _LOGIC_H264_ENC_H_

#include "logic_include.h"
#include "logic_media_core.h"


typedef struct _H264EncState
{
	int favc_enc_fd;
	int frameCount;
	int frameCntMax;
	int force_Iframe;
	video_params enc_param;
	MSThread thread;
}H264EncState;

#define MS_H264_ENC_OPEN		MS_MEDIA_METHOD(MS_H264_ENC_ID,0x01,const char)
#define MS_H264_ENC_PARAM		MS_MEDIA_METHOD(MS_H264_ENC_ID,0x02,const char)
#define MS_H264_ENC_IDR			MS_MEDIA_METHOD_NO_ARG(MS_H264_ENC_ID,0x04)
#define MS_H264_ENC_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_H264_ENC_ID,0x05)

#define MS_CLOUD_H264_ENC_OPEN			MS_MEDIA_METHOD(MS_CLOUD_H264_ENC_ID,0x01,const char)
#define MS_CLOUD_H264_ENC_PARAM			MS_MEDIA_METHOD(MS_CLOUD_H264_ENC_ID,0x02,const char)
#define MS_CLOUD_H264_ENC_IDR			MS_MEDIA_METHOD_NO_ARG(MS_CLOUD_H264_ENC_ID,0x04)
#define MS_CLOUD_H264_ENC_CLOSE			MS_MEDIA_METHOD_NO_ARG(MS_CLOUD_H264_ENC_ID,0x05)
#endif

