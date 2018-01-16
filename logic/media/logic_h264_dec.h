/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_h264_dec.h
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
#ifndef _AU_H264_DEC_H_
#define _AU_H264_DEC_H_

#include "logic_include.h"
#include "logic_media_core.h"

#define MS_H264_DEC_OPEN	MS_MEDIA_METHOD(MS_H264_DEC_ID, 0x00, const char)
#define MS_H264_DEC_CLOSE	MS_MEDIA_METHOD_NO_ARG(MS_H264_DEC_ID, 0x01)
#define MS_H264_DEC_PARAM   MS_MEDIA_METHOD(MS_H264_DEC_ID, 0x02, V_RECT_S)
#define MS_H264_DEC_LEVEL   MS_MEDIA_METHOD(MS_H264_DEC_ID, 0x03, V_RECT_S)
#define MS_H264_DEC_SNAP	MS_MEDIA_METHOD(MS_H264_DEC_ID, 0x04, const char)

#endif

