/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_rtsp.h
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __STORAGE_RTSP_H__
#define __STORAGE_RTSP_H__

#include "storage_include.h"
//#include "storage_types.h"

#define MAX_HOME_NUM  			16					

#define HOME_CAMERA				0
#define COMMUNITY_CAMERA		1

typedef enum
{
    RTSP_IP = 0x00,		
	RTSP_PORT,	
	RTSP_CHANNEL,
	RTSP_USER,
	RTSP_PASSWORD,
	
	RTSP_MAX
}RTSP_TYPE;				

typedef struct
{
	uint32 index;									// IPC 编号：0～9
	uint32 address;									// IPC 地址
} SelfIPCamera;										// 室内 IPC 信息结构

typedef struct 
{
   	char DeviceName[32];							// 设备名称	
	uint32 DeviceIP;								// 设备IP		
	uint16 DevPort;									// 设备端口
	uint16 ChannelNumber;							// 通道号码
	char FactoryName[12];               			// 设备厂家名称
	uint32 CanControlPTZ;		                    // 云台是否可用
}COMMUNITYDEVICE,*PCOMMUNITYDEVICE;

//室内摄像机信息结构
typedef struct 
{
   	char DeviceName[32];							// 设备名称	
	uint32 DeviceIP;								// 设备IP		
	uint16 DevPort;									// 设备端口
	uint16 ChannelNumber;							// 通道号码
	char FactoryName[12];               			// 设备厂家名称
	uint32 CanControlPTZ;		                    // 云台是否可用
	int isOnLine;									// 是否在线 (无法判断是否在线的默认为在线) <0 无法确认 0:离线 1:在线
}NEWCOMMUNITYDEVICE,*PNEWCOMMUNITYDEVICE;

//室内摄像机信息结构
typedef struct 
{
    uint32 	EnableOpen;         					// 是否开放浏览
	char 	DeviceName[32];							// 设备名称
	uint32 	DeviceIP;								// 设备IP
	uint16 	DevPort	;								// 设备端口
	uint16 	ChannelNumber;							// 通道号码
	char 	FactoryName[12];    					// 设备厂家名称
	char 	UserName[16];							// 设备登陆用户名
	char 	Password[16];							// 设备登陆密码
	uint32  CanControlPTZ;          				// 云台可控标志,0: 云台不可控制；1:云台可以控制
}HOMEDEVICE, *PHOMEDEVICE;

typedef struct 
{
    uint32 	EnableOpen;         					// 是否开放浏览
	char 	DeviceName[32];							// 设备名称
	uint32 	DeviceIP;								// 设备IP
	uint16 	DevPort	;								// 设备端口
	uint16 	ChannelNumber;							// 通道号码
	char 	FactoryName[12];    					// 设备厂家名称
	char 	UserName[16];							// 设备登陆用户名
	char 	Password[16];							// 设备登陆密码
	uint32  CanControlPTZ;          				// 云台可控标志,0: 云台不可控制；1:云台可以控制
	int isOnLine;									// 是否在线 (无法判断是否在线的默认为在线) <0 无法确认 0:离线 1:在线
}NEWHOMEDEVICE, *PNEWHOMEDEVICE;


typedef struct 
{
	uint32 Comnum;									
	uint32 Homenum;
   	COMMUNITYDEVICE *Comdev;	
   	HOMEDEVICE* Homedev;
}MonitorDeviceList,*PMonitorDeviceList;		

typedef struct 
{
	uint32 DevNum;									
   	HOMEDEVICE* Devinfo;
}RtspDeviceList,*PRtspDeviceList;	

typedef struct 
{
	uint32 Comnum;									
	uint32 Homenum;
   	NEWCOMMUNITYDEVICE *Comdev;	
   	NEWHOMEDEVICE* Homedev;
}NewMonitorDeviceList,*PNewMonitorDeviceList;	


/*************************************************
  Function:		storage_get_monitordev
  Description: 	
  Input:		
  	3.Time		时间
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_get_monitordev(PMonitorDeviceList *Devlist);

/*************************************************
  Function:		storage_get_home_monitordev
  Description: 	
  Input:		
  Output:		无
  Return:		ECHO_STORAGE
  Others:		获取家居监视列表
*************************************************/
void storage_get_home_monitordev(PMonitorDeviceList *Devlist);

/*************************************************
  Function:		storage_get_comm_monitordev
  Description: 	
  Input:		
  Output:		无
  Return:		ECHO_STORAGE
  Others:		获取社区监视列表
*************************************************/
void storage_get_comm_monitordev(PMonitorDeviceList *Devlist);

/*************************************************
  Function:		storage_malloc_new_monitordev_memory
  Description: 	申请存储内存
  Input:
  	1.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_malloc_new_monitordev_memory (uint32 HomeMaxNum, uint32 CommMaxNum, PNewMonitorDeviceList *list);

/*************************************************
  Function:		storage_get_index
  Description: 	获取设备的编号
  Input:		无
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
uint8 storage_get_index(uint8 index);
#endif 

