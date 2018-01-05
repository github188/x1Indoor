/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	storage_include.h.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-04
  Description:  
				存储头文件集合,包含storage各模块的头文件,
				外部要使用storage的接口只要包含该文件就可以
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/vfs.h>

#include "au_types.h"
#include "AppConfig.h"
#include "au_debug.h"

#include "comm/au_language.h"
#include "comm/storage_types.h"
#include "comm/storage_path.h"
#include "comm/storage_comm.h"
#include "comm/storage_syssave.h"
#include "jiadian/storage_jd.h"
#include "call/storage_call.h"
#include "alarm/storage_alarm.h"
#include "message/storage_message.h"
#include "jrly/storage_jrly.h"
#include "photo/storage_photo.h"
#include "lyly/storage_lyly.h"
#include "yuyue/storage_yuyue.h"
#include "rtsp/storage_rtsp.h"
#include "ipmodule/storage_ipmodule.h"
#include "timer/storage_timer.h"

#ifdef _ADD_BLUETOOTH_DOOR_
#include "netdoor/storage_netdoor.h"
#endif

#include "../driver/hw_ioctrl_api.h"
#include "../driver/hw_com_api.h"

#include "SysGlobal.h"
#endif

