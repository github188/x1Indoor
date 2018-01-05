/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:  logic_net_comm.h
  Author:     xiewr
  Version:    1.0
  Date: 
  Description:  网络通讯模块头文件
  
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#ifndef _NET_COMM_H_
#define _NET_COMM_H_

#if 0
#include "include.h"
#include "lwip_opt.h"
#include "sockets.h"
#include "net_comm_types.h"
#include "net_command.h"
#include "storage_types.h"
#include "logic_udp_comm.h"
#include "logic_getip.h"
#include "logic_net_param.h"
#include "logic_packet_list.h"
#include "logic_distribute.h"
#include "logic_send_event.h"
#include "logic_lwip_init.h"
#endif
#include "logic_include.h"

#define NET_COMM_DEBUG		0
#if     NET_COMM_DEBUG
#define	NETDEBUG(...)				printf("%s[%d] : ", __FUNCTION__, __LINE__); \
									printf(__VA_ARGS__) 
#else
		#define	NETDEBUG(...)   
#endif
#endif

