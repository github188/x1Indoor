/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_jpeg_enc.h
  Author:		³Â±¾»Ý
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3´çÊÒÄÚ»újpeg Í¼Æ¬±àÂë
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef _AU_JPEG_ENC_H_
#define _AU_JPEG_ENC_H_

#include "logic_media_core.h"
#include "logic_video.h"

typedef struct _JpegEncParam{
	char mName[100];
	V_RECT_S vorect;
	STREAM_SRC_E src_mode;
	FB_DISPLAY_MODE_E FB_Mode;	

}JpegEncParam;

#define MS_JPEG_ENC_PARAM		MS_MEDIA_METHOD(MS_JPEG_ENC_ID,0x00, JpegEncParam)

#define MS_JPEG_ENC_OPEN		MS_MEDIA_METHOD_NO_ARG(MS_JPEG_ENC_ID,0x02)
#define MS_JPEG_ENC_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_JPEG_ENC_ID,0x03)

#define MS_JPEG_ENC_SUCCESS		MS_MEDIA_METHOD_NO_ARG(MS_JPEG_ENC_ID,0x10)

#endif
	
