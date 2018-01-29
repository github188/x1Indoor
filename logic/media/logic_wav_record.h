/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_wav_record.c
  Author:		陈本惠
  Version:		V1.0
  Date:			
  Description:	
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef _LOGIC_WAV_RECORD_H_
#define _LOGIC_WAV_RECORD_H_

#include "logic_include.h"
#include "logic_media_core.h"
#include "wav_record.h"

// 音频文件播放参数
typedef struct
{
	char 	filename[50];					// 文件名称(加上路径)
	E_WavFormatTag format;					// 数据源格式
	unsigned int	u32SamplingRate;		// 采样率
	unsigned short	u16Channels;			// 通道数
	unsigned short	u16BitsPerSample;		// 采样位宽
}AUDIORECORD_PARAM, *PAUDIORECORD_PARAM;

#define MS_WAV_RECORD_OPEN		MS_MEDIA_METHOD(MS_WAV_RECORD_ID, 0x00, const char)
#define MS_WAV_RECORD_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_WAV_RECORD_ID, 0x01)
#define MS_WAV_RECORD_PARAM   	MS_MEDIA_METHOD(MS_WAV_RECORD_ID, 0x02, const char)   

#endif

