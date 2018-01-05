/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_avi_play.c
  Author:		陈本惠
  Version:		V1.0
  Date:			2014-12-11
  Description:	4.3寸室内机留影留言AVI 文件播放
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef _LOGIC_AVI_PLAY_H_
#define _LOGIC_AVI_PLAY_H_

#include "logic_include.h"
#include "logic_media_core.h"

typedef enum
{
	AVI_STATE_STOP,	                           		// 停止
	AVI_STATE_PAUSE,	                            // 暂停
	AVI_STATE_PLAY,	                            	// 播放
}AVI_STATE_E;

typedef struct _AviPlayParam{
	AVI_STATE_E state;
	uint8 *val;
	MSThread aviplaythread;
	uint32 m_hMediaHandle;
	PAVI_CALLBACK callback;

}AviPlayParam;

#define MS_AVI_PLAY_OPEN	MS_MEDIA_METHOD(MS_AVI_PLAY_ID, 0x00, const char)
#define MS_AVI_PLAY_CLOSE	MS_MEDIA_METHOD_NO_ARG(MS_AVI_PLAY_ID, 0x01)
#define MS_AVI_PLAY_STATE   MS_MEDIA_METHOD(MS_AVI_PLAY_ID, 0x02, AviPlayState)   

#endif
