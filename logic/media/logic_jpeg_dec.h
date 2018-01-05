/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_jpeg_dec.h
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机视频解码
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef _AU_JPEG_DEC_H_
#define _AU_JPEG_DEC_H_

#include "logic_media_core.h"
#include "logic_video.h"

typedef struct _JpegDecParam{
	char mName[100];
	V_RECT_S vorect;	

}JpegDecParam;

#define MS_JPEG_DEC_PARAM		MS_MEDIA_METHOD(MS_JPEG_DEC_ID,0x00, JpegDecParam)

#define MS_JPEG_DEC_OPEN		MS_MEDIA_METHOD_NO_ARG(MS_JPEG_DEC_ID,0x02)
#define MS_JPEG_DEC_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_JPEG_DEC_ID,0x03)

#define MS_JPEG_DEC_SUCCESS		MS_MEDIA_METHOD_NO_ARG(MS_JPEG_DEC_ID,0x10)
#endif


