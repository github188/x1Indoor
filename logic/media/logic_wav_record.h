/********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	logic_wav_record.c
  Author:		³Â±¾»Ý
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

#define MS_WAV_RECORD_OPEN		MS_MEDIA_METHOD(MS_WAV_RECORD_ID, 0x00, const char)
#define MS_WAV_RECORD_CLOSE		MS_MEDIA_METHOD_NO_ARG(MS_WAV_RECORD_ID, 0x01)
#define MS_WAV_RECORD_PARAM   	MS_MEDIA_METHOD(MS_WAV_RECORD_ID, 0x02, const char)   

#endif

