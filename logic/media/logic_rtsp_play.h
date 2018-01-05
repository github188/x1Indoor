/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_rtsp_play.h
  Author:		chenbh
  Version:		V1.0
  Date:			2015-09-28
  Description:	
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef _LOGIC_RTSP_PLAY_H_
#define _LOGIC_RTSP_PLAY_H_

#include "logic_include.h"
#include "logic_media_core.h"


typedef struct _RtspPlayParam{
	uint8 *val;
	V_RECT_S rect;
	MSThread rtspplaythread;
	//HMEDIA m_hMediaHandle;
	PRTSP_CALLBACK callback;

}RtspPlayParam;

#define MS_RTSP_PLAY_OPEN		MS_MEDIA_METHOD(MS_RTSP_PLAY_ID, 0x00, const char)
#define MS_RTSP_PLAY_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_RTSP_PLAY_ID, 0x01)
#define MS_RTSP_PLAY_PARAM   	MS_MEDIA_METHOD(MS_RTSP_PLAY_ID, 0x02, const char) 
#define MS_RTSP_PLAY_LEVEL		MS_MEDIA_METHOD(MS_RTSP_PLAY_ID, 0x03, const char) 
#endif






