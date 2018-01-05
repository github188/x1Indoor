/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_play_lyly_hit.c
  Author:		luofl
  Version:		V2.0
  Date:			2015-03-17
  Description:	播放留影留言提示音
  
History:				 
	1.Date: 
	Author: 
	Modification: 
*********************************************************/

#ifndef _LOGIC_AUDIO_LYLY_H_
#define _LOGIC_AUDIO_LYLY_H_

#include "logic_media_core.h"



// 音频文件播放参数
typedef struct
{
	uint8 IsPack;						// 是否组包
	uint8 PackNum;						// 组包个数
	uint8 PerFrameNum;					// 每帧数据长度 PerFrameNum*80
	char filename[50];					// 文件名称(加上路径)
	FILE_TYPE_E FileType;				// 文件类型
}AUDIO_HIT_PARAM, *PAUDIO_HIT_PARAM;

#define MS_AUDIO_LYLY_OPEN 		MS_MEDIA_METHOD_NO_ARG(MS_FILE_LYLY_ID, 0x00)
#define MS_AUDIO_LYLY_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_FILE_LYLY_ID, 0x01)
#define MS_AUDIO_LYLY_PARAM		MS_MEDIA_METHOD(MS_FILE_LYLY_ID, 0x02, const char)

#endif

