/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_media_record.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机留影留言AVI 文件录制
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef _LOGIC_AVI_RECORD_H_
#define _LOGIC_AVI_RECORD_H_

#include "logic_include.h"
#include "logic_media_core.h"

typedef enum{
	eVIN_DEV_UVC_RAW,		//UVC YUV422 format
	eVIN_DEV_UVC_JPEG,		//UVC JPEG format
	eVIN_DEV_UVC_H264,		//UVC H264 format
}E_VIN_DEV;


#define MS_AVI_RECORD_OPEN	MS_MEDIA_METHOD(MS_AVI_RECORD_ID, 0x00, const char)
#define MS_AVI_RECORD_CLOSE	MS_MEDIA_METHOD_NO_ARG(MS_AVI_RECORD_ID, 0x01)
#define MS_AVI_RECORD_PARAM   MS_MEDIA_METHOD(MS_AVI_RECORD_ID, 0x02, RecordParam)   

#endif

