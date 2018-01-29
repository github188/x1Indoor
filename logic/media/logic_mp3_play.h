/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_mp3_play.h
  Author:   	³Â±¾»Ý
  Version:  	1.0
  Date: 		2014-12-11
  Description:  MP3 ÒôÆµ²¥·Å
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _LOGIC_MP3_PLAY_H_
#define _LOGIC_MP3_PLAY_H_

#include "logic_include.h"
#include "logic_media_core.h"

typedef enum
{
	MP3_STATE_STOP,	                           		// Í£Ö¹
	MP3_STATE_PAUSE,	                            // ÔÝÍ£
	MP3_STATE_PLAY,	                            	// ²¥·Å
}MP3_STATE_E;

typedef struct _Mp3PlayParam{
	MP3_STATE_E state;
	uint8 *val;
	MSThread mp3playthread;
	uint32 m_hMediaHandle;
	PMP3_CALLBACK callback;

}Mp3PlayParam;

#define MS_MP3_PLAY_OPEN	MS_MEDIA_METHOD(MS_MP3_PLAY_ID, 0x00, const char)
#define MS_MP3_PLAY_CLOSE	MS_MEDIA_METHOD_NO_ARG(MS_MP3_PLAY_ID, 0x01)
#define MS_MP3_PLAY_STATE   MS_MEDIA_METHOD(MS_MP3_PLAY_ID, 0x02, const char)   

#endif
