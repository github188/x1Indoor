/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	logic_include.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-04
  Description:  
				逻辑头文件集合,包含logic各模块的头文件,
				外部要使用logic的接口只要包含该文件就可以
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __LOGIC_INCLUDE_H__
#define __LOGIC_INCLUDE_H__

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
//#include "SysGlobal.h"

#include "timer/logic_timer.h"
#include "timer/logic_time.h"

#include "netcom/net_command.h"
#include "netcom/net_comm_types.h"
#include "netcom/logic_udp_comm.h"
#include "netcom/logic_send_event.h"
#include "netcom/logic_packet_list.h"
#include "netcom/logic_net_param.h"
#include "netcom/logic_net_comm.h"
#include "netcom/logic_getip.h"
#include "netcom/logic_distribute.h"

#include "media/logic_media.h"

#include "call/logic_call.h"
#include "call/logic_interphone.h"
#include "call/logic_monitor.h"
#include "call/loigc_rtsp_monitor.h"

#include "alarm/logic_alarm.h"
#include "alarm/logic_alarm_net.h"

#include "message/logic_message.h"
#include "set/logic_wlmqj_set.h"

#include "linkage/logic_linkage.h"
#include "linkage/logic_sys_arbitration.h"

#include "jiadian/logic_jiadian.h"

#include "yuyue/logic_yuyue.h"

#include "dianti/logic_dianti.h"

#include "ipmodule/logic_ipmodule.h"

#include "comm/logic_appinit.h"
#include "comm/logic_pubcmd.h"

#include "../driver/hw_ioctrl_api.h"
#include "../driver/hw_com_api.h"

#include "SysGlobal.h"
#endif

