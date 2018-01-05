/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	SysGlobal.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-06
  Description:  
				全局变量定义头文件
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __SYSGLOBAL_H__
#define __SYSGLOBAL_H__

#ifdef   SYS_GLOBALS
#define  SYS_EXT
#else
#define  SYS_EXT  extern
#endif
//SYS_EXT uint16 g_StrID[20];
//SYS_EXT void * g_StrID[20];
SYS_EXT uint8 g_China;
SYS_EXT uint8 g_Big5;
SYS_EXT uint8 g_LylyNum;
SYS_EXT uint8 g_MsgNum;

SYS_EXT VIDEO_SDP_PARAM g_venc_parm;
SYS_EXT AUDIO_SDP_PARAM g_audio_parm;

SYS_EXT PSYSCONFIG gpSysConfig;
SYS_EXT PAF_FLASH_DATA	gpAfParam;
SYS_EXT PSYS_FLASH_DATA gpSysParam;
#endif

