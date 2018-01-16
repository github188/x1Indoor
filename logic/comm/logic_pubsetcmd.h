/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	logic_pubsetcmd.h
  Author:   	wufn
  Version:   	
  Date: 		
  Description:  公共命令
  
  History:                   
    1. Date:			 
       Author:				
       Modification:	
    2. ...
*********************************************************/
#ifndef __LOGIC_PUBSETCMD_H__
#define __LOGIC_PUBSETCMD_H__

//#include "logic_net_comm.h"
#include "logic_include.h"
#include "storage_include.h"

//功能类型代码
typedef enum
{
	DATA_ID_SYSPARAM	= 1,		// 系统参数
	DATA_ID_SECUPARAM	= 2,		// 安防参数
	DATA_ID_ELECPARAM	= 3,		// 家电参数
	DATA_ID_ELEVATORLIST = 4,		// 电梯列表
	DATA_ID_CAMERALIST	= 5,		// 摄像头列表
	DATA_ID_STAIRPARAM	= 6,		// 梯口设置参数
}FUNCTION_CODE_E;

typedef enum
{
	LAN_BIG5	= 1028,				// 繁体
	LAN_ENGLISH = 1033,				// 英文
	LAN_CHINESE = 2052,				// 简体
}WORLD_LANGUAGE_E;					// 世界语言标示

typedef enum
{
	OPER_RECOVER = 0X00,			// 恢复出厂
	OPER_REBOOT  = 0X01,			// 重启
}TERMINAL_OPER_E;					// 操作类型

#pragma pack(push,1) 

//网络参数
typedef struct
{
	uint32	IP;						// 设备IP，默认10.100.1.4
	uint32	SubNet;					// 子网掩码，默认255.255.255.0
	uint32	DefaultGateway;			// 默认网关，默认10.100.1.254
	uint32	DNS1;					// DNS1服务器，默认0.0.0.0
	uint32	DNS2;					// DNS2服务器，默认0.0.0.0
	uint32	CenterIP;				// 中心服务器IP，默认0.0.0.0
	uint32	RtspServerIP;			// RTSP服务器，默认0.0.0.0
	uint32	ManagerIP[8];			// 管理员机IP，最多8台，默认0.0.0.0
	uint32  NetdoorIP[2];        	// 网络门前机，默认0.0.0.0
}NET_SETPARAM,*PNET_SETPARAM;	

//系统参数信息
typedef struct
{
	NET_SETPARAM NetParam;			// 网络参数
	#ifdef	_USE_AURINE_SET_
	DEVICE_SET_NO DeviceNo;
	#else
	DEVICE_NO DeviceNo;				// 设备编号
	#endif
	DEVICENO_RULE DeviceRule;		// 编号规则

	char ProjectPwd[8];				// 工程密码，默认13572468
	uint8 ExtModule[8];				// 外部模块：家电控制器、门前机1、门前机2
									// 门前机3、门前机4、支持通用梯口机
									// 启用接收警情功能、启用接收SOS警情	
	uint16 language;				// 语言
	uint8 LockType;					// 锁类型: 0 常开 1 常闭 , 默认常开
	uint8 LockTime;					// 锁参数

	uint8 NetComm;					// 是否启用网络通信
	uint8 IsCallConvert;			// 是否呼叫转移	
	char ConvertNumber[20];			// 转移设备号
	uint16 Reserver2;				// 预留

	char HardwareVer[64];			// 硬件版本
	char SoftwareVer[64];			// 软件版本
	char Mac[6];					// mac码
}SYSTEM_SETINFO;					// 用于应答

//防区属性
typedef struct
{
	uint8 Enable;				// 是否启用
	uint8 ProbeType;			// 探头类型，0-7		
	uint8 DelayTime;			// 延迟时间 			
	uint8 IsPartValid;			// 是否局防有效
	uint8 Is24Hour;				// 是否24小时防区
	uint8 IsSee;				// 是否可见
	uint8 IsSound;				// 是否可听
	uint8 ProbeState;			// 探头状态：0 常开 1 常闭
}SECU_ZONEINFO;

//家电设备信息
typedef struct
{
	uint8	JdType;				// 设备类型，见EHOME_TYPE_E定义
	uint8	Index;				// 设备索引号，从0开始
	uint16	Address;			// 设备地址(bit7-15:高;bit0-7:低)，默认0
	char	Name[30];			// 名称，默认为空
	uint8	IsUsed;				// 是否启用(1:已用，0:未用)，默认不启用
	uint8	IsTune;				// 是否可调节(灯光：调光，空调：调温)，默认不可调
}ELEC_DEVINFO;

//室内摄像机结构
typedef struct 
{
    uint8 	Enable;        		//是否启用，默认不启用
	char 	DeviceName[32];		//设备名称，默认为空
	uint32 	DeviceIP;			//设备IP，默认为0.0.0.0
	uint16	DevPort	;			//设备端口，默认为80
	uint16	Channel;			//通道号码，默认0
	char 	FactoryName[12];    //设备厂家名称，默认为空
	char 	UserName[16];		//设备登陆用户名，默认为空
	char 	Password[16];		//设备登陆密码，默认为空
	uint32	CanControlPTZ; 		//云台可控标志(0: 不可控制；1:可控制)，默认不可控
	int      IsOnline;			//是否在线 （无法判断在线的默认固定为在线）
								//<0: 无法确定在线否；0：离线；1：在线
}NEWHOMECAMERA;

#pragma pack(pop)  									// 结束定义数据包, 恢复原对齐方式

#endif

