/*********************************************************************
  COPYRIGHT (C), 2006-2010, AURINE
  FILE NAME: 		logic_wlmqj_set.h
  AUTHOR:   		xiewr
  VERSION:   		1.0 
  DATE:     		2010-06-04
  DESCRIPTION:		网络门前机设置

  History:        
    1. Date:
       Author:
       Modification:
    2. ...     
*********************************************************************/
#ifndef _LOGIC_WLMQJ_SET_H_
#define _LOGIC_WLMQJ_SET_H_

#include "netcom/logic_net_comm.h"

#define NETDOOR_FAULT_IP		0x0A640104 						// 10.100.1.4

#define NETDOOR_ECHO_SUCESS		0								//操作成功

#define NETDOOR_ECHO_COMM		0x030000

#define NETDOOR_ERR_SEND		(NETDOOR_ECHO_COMM|0x00)		//网络数据发送错误
#define NETDOOR_ERR_ECHO		(NETDOOR_ECHO_COMM|0x01)		//应答一般错误
#define NETDOOR_ERR_MAC			(NETDOOR_ECHO_COMM|0x09)		//MAC吗错误

typedef struct hiVERSION_INFO_S
{
    uint32 u32ProductVer;       /* The product version  */
    uint32 u32RootBoxVer;       /* RootBox version      */
    uint32 u32LinuxVer;         /* Linux kernel version */
    uint32 u32MspVer;           /* The MSP version      */
    uint32 u32AlgVer;           /* The codec alg version*/
    uint32 u32ChipVer;          /* The chip version     */
} VERSION_INFO_S, FWM_VERSION_T;

#ifdef _ADD_BLUETOOTH_DOOR_
//蓝牙门前记录类型
typedef enum
{
	BLE_RECORD_TYPE_OPEN		= 0x0,  			//开锁记录
	BLE_RECORD_TYPE_STATUS		= 0x01, 			//锁状态
	BLE_RECORD_TYPE_OPER		= 0x02, 			//操作记录
}BLE_RECORD_TYPE_E;

//开锁设备类型定义
typedef enum
{
	BLE_OPENDEV_SELF		= 0x00, 			// 本机开锁
	BLE_OPENDEV_NETDOOR		= 0x10, 			// 门前机开锁
	BLE_OPENDEV_PHONE		= 0x20, 			// 手机开锁
}BLE_OPENLOCK_DEV_E;

//开锁类型定义
typedef enum
{
	BLE_OPENTYPE_KEY			= 0x00, 			//机械开锁
	BLE_OPENTYPE_PWD			= 0x01, 			//密码开锁
	BLE_OPENTYPE_CARD			= 0x02, 			//刷卡开锁
	BLE_OPENTYPE_FINGER			= 0x03, 			//指纹开锁
	BLE_OPENTYPE_FACE			= 0x04, 			//人脸识别开锁
	BLE_OPENTYPE_VISTOR			= 0x06, 			//访客开锁
	BLE_OPENTYPE_MONITOR		= 0x07, 			//监视开锁
	BLE_OPENTYPE_BLEPHONE		= 0x08, 			//手机开锁
	BLE_OPENTYPE_TEMP_AUTHOR	= 0x09, 			//临时授权开锁
}BLE_OPENLOCK_TYPE_E;

// 操作记录类型
typedef enum
{
	BLE_OPER_DEL_USER			= 0X00,				//删除用户
	BLE_OPER_SET_CHANGKAI		= 0X01,				//设置门常开
	BLE_OPER_CANCEL_CHANGKAI	= 0X02,				//取消门常开
	BLE_OPER_RING				= 0X03,				//振铃
	BLE_OPER_DEFAULT			= 0X0F,				
}BLE_OPER_TYPES_E;

// 记录结构体
typedef struct
{
	uint32 UserId;							// 用户ID
	char UserName[16];						// 用户姓名
	uint32 RecordType;						// 记录类型
	char RecordTime[24];						// 记录时间
}NET_RECORD_INFO, *PNET_RECORD_INFO;

typedef struct
{
	uint32 counts;
	NET_RECORD_INFO *info;	
}NET_RECORD_LIST, *PNET_RECORD_LIST;

// 室内绑定列表结构体
typedef struct
{
	DEVICE_NO DestNo;						// 设备编号
	uint32 DestNoIP;						// IP
}NET_BING_ROOM, *PNET_BING_ROOM;

typedef struct
{
	uint32 counts;
	NET_BING_ROOM *info;	
}NET_BING_ROOM_LIST, *PNET_BING_ROOM_LIST;
#endif

/*************************************************
  Function:    		send_netdoor_cmd_noecho
  Description:		往网络门前机发送设置命令
  					此接口主要用于无应答数据的发送
  					带MAC,MAC在数据前面
  Input: 
  	1.cmd:			发送的命令
  	2.data: 		数据内容
  	3.datelen:		数据的长度
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
uint32 send_netdoor_cmd_noecho(NET_COMMAND cmd ,unsigned char * SendData, uint32 len);

/*************************************************
  Function:			logic_set_netdoor_netparam
  Description:		设置网络门前机网络参数
  Input: 				
    1.netparam		网络参数
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发生错误:0x030000
  Others:
*************************************************/
int32 logic_set_netdoor_netparam(PNET_PARAM netparam);

/*************************************************
  Function:			logic_get_netdevice_netparam
  Description:		获得网络设备网络参数
  Input: 				
  Output:			
  	1.netparam		网络参数
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:			
*************************************************/
int32 logic_get_netdevice_netparam(PNET_PARAM netparam);

/*************************************************
  Function:			logic_set_netdoor_devnoinfo
  Description:		设置网络门前机设备编号以及规则
  Input: 				
    1.devno			设备编号
    2.rule 			编号规则
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_set_netdoor_devnoinfo(PFULL_DEVICE_NO devno, DEVICENO_RULE * rule);

/*************************************************
  Function:			logic_get_netdevice_devnoinfo
  Description:		获取网络门前机设备编号以及规则
  Input: 			
  Output:			
    1.devno			设备编号
    2.rule 			编号规则
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_netdevice_devnoinfo(PFULL_DEVICE_NO devno, DEVICENO_RULE * rule);

/*************************************************
  Function:			logic_get_netdoor_version
  Description:		获得网络门前机内核和程序版本号
  Input: 			无
  Output:			
    1.KerVer		内核版本号
  	2.BuildTime		编译时间
  	3.ver			程序版本号
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_netdoor_version(FWM_VERSION_T * KerVer, char * BuildTime, char * ver);

/*************************************************
  Function:			logic_mng_netdoor_card
  Description:		管理卡信息
  Input: 	
  	1.OperType		操作类型
    2.CardNo		卡号指针
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001/196609
  					ECHO_MAC:0x030009/196617
  					NETDOOR_ERR_SEND:0x030000/196608		
  Others:
*************************************************/
int32 logic_mng_netdoor_card(uint8 OperType, char * CardNo);

/*************************************************
  Function:			logic_get_netdevice_lockinfo
  Description:		获得锁信息
  Input: 			无
  Output:			
  	1.LockType		锁类型
  	2.LockTime		锁时长
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001/196609
  					ECHO_MAC:0x030009/196617
  					NETDOOR_ERR_SEND:0x030000/196608		
  Others:
*************************************************/
int32 logic_get_netdevice_lockinfo(uint8 * LockType, int32 * LockTime);

/*************************************************
  Function:			logic_set_netdevice_lockinfo
  Description:		设置锁信息
  Input: 	
  	1.LockType		锁类型
    2.LockTime		锁时长
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001/196609
  					ECHO_MAC:0x030009/196617
  					NETDOOR_ERR_SEND:0x030000/196608		
  Others:
*************************************************/
int32 logic_set_netdevice_lockinfo(uint8 LockType, int32 LockTime);

/*************************************************
  Function:    		get_netdoor_mac
  Description:		获得网络门前机的MAC地址
  Input: 			
    1.index			网络门前机的ID
  Output:			
	1.HWaddr		获得的硬件地址
  Return:			成功与否, TRUE/FALSE
  Others:			在进入门前机设置时首先调用此函数
*************************************************/
uint32 logic_get_netdoor_mac(uint8 index, unsigned char * HWaddr);

/*************************************************
  Function:			logic_set_netdoor_other_settings
  Description:		网络设备其他设置
  Input: 				
    1.OtherSettings	设置值 0:不启用,1:启用
  Output:			无
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发生错误:0x030000
  Others:			数组大小为10,数组第一个元素为拍照设置的值,
  					第二个元素为监视设置的值,第三个元素为语言
  					第四个元素为视频格式
*************************************************/
uint32 logic_set_netdoor_other_settings(unsigned char * OtherSettings);

/*************************************************
  Function:			logic_get_netdoor_other_settings
  Description:		获得网络设备其他设置
  Input: 				
  Output:			
  	1.OtherSettings	其他设置值
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					数据发生错误:0x030000
  					其他:0x0300**
  Others:			
*************************************************/
uint32 logic_get_netdoor_other_settings(unsigned char * OtherSettings);

/*************************************************
  Function:    		logic_netdoor_pub_ini
  Description:		初始化:获得网络门前机地址和MAC地址
  Input: 			无
  Output:			
  Return:			成功与否, TRUE/FALSE
  Others:			无线终端与网关交互操作之前要调用此函数
*************************************************/
uint32 logic_netdoor_pub_ini(uint8 index);

#ifdef _ADD_BLUETOOTH_DOOR_
/*************************************************
  Function:    		logic_netdoor_pub_ini
  Description:		初始化:获得网络门前机地址和MAC地址
  Input: 			无
  Output:			
  Return:			成功与否, TRUE/FALSE
  Others:			无线终端与网关交互操作之前要调用此函数
*************************************************/
uint32 logic_bluetoothdoor_pub_ini(uint8 index, uint32 NetdoorIp);

/*************************************************
  Function:			logic_get_bluetooth_door_sysinfo
  Description:		获取蓝牙门前系统信息
  Input: 	
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_bluetooth_door_sysinfo(char * data);

/*************************************************
  Function:			logic_get_bluetooth_door_sysinfo
  Description:		获取蓝牙门前绑定室内列表
  Input: 	
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_bluetooth_door_room_list(PNET_BING_ROOM_LIST *prec_data);

/*************************************************
  Function:			logic_get_bluetooth_door_sysinfo
  Description:		设置蓝牙门前时间
  Input: 	
  Output:			
  	reccontent		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_set_bluetooth_door_time();

/*************************************************
  Function:			logic_get_bluetooth_door_record
  Description:		查看门前最近24小时记录和历史记录
  Input: 	
    1.cmd			命令
    2.index			获取第几台蓝牙门前记录 1:蓝牙门前1;   2:蓝牙门前2
    3.type 			记录类型
    4.start			开始记录标志
    5.num			获取记录条数
  Output:			
  	1.recnum		门前返回记录条数
  	2.content		门前返回内容
  Return:			ECHO_OK: 0
  					ECHO_ERROR:0x030001
  					ECHO_MAC:0x030009
  					其他:0x0300**
  Others:
*************************************************/
int32 logic_get_bluetooth_door_record(NET_COMMAND cmd, uint8 index, uint8 type, uint32 start, uint32 num, PNET_RECORD_LIST *prec_data);

/*************************************************
  Function:			recv_bluetoothdoor_pic_start
  Description:		接收蓝牙门前抓拍图片开始(采用多包接收)
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			无
  Others:
*************************************************/
void recv_bluetoothdoor_pic_start(const PRECIVE_PACKET recPacket);
#endif

#endif
