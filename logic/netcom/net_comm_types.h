/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	net_comm_types.h
  Author:		xiewr
  Version:  	1.0
  Date: 
  Description:  冠林网络通讯协议_类型定义
				说明：本系统枚举类型除特殊说明外，
				在网络包中均转换为unsigned char。
  History:        
    1. Date:	
       Author:	
       Modification:
    2. ...
*************************************************/
#ifndef _NET_COMM_TYPES_H_
#define _NET_COMM_TYPES_H_

//#include "typedef.h"
//#include "storage_types.h"
#include "storage_include.h"
//#include "logic_include.h"

#ifdef  __cplusplus
extern "C"
{
#endif
// 方向标志
#define DIR_SEND				0xAA 				// 主动
#define DIR_ACK 				0x00 				// 应答
#define DIR_DONOTHING           0xFF                // 不处理
#define SOCK_ADDR_SIZE			(sizeof(struct sockaddr_in))  			 // 地址长度
#define NET_PACKBUF_SIZE		(1024*4)             					 // UDP包最大值

#ifdef _AU_PROTOCOL_
#define _SUB_PROTOCOL_ENCRYPT_	0x00									// 冠林协议不加密
#define _PROTOCOL_VER_ 			0x0200  								// 协议版本
#define	MAIN_NET_HEAD_SIZE		0										// 主协议网络包头长度
#define	SUB_NET_HEAD_SIZE		(sizeof(NET_HEAD))						// 子协议网络包头长度
#define	NET_HEAD_SIZE			(sizeof(NET_HEAD))	// 网络包头长度
#define MANAGER_NUM				50					// 管理员机起始编号
#define MANAGER_LEN				2					// 管理员机编号长度
#define SCREENSAVE_NUM			12					// 屏保数量

#define NETCMD_UDP_PORT			20625 				// 统一的命令端口,外区为映射后的端口
#define	NETVIDEO_UDP_PORT		24010				// 视频端口
#define	NETAUDIO_UDP_PORT		24020				// 音频端口
#else
#define MAINPROTOCOL_VER        0x0801              // 主协议版本
#define SUBPROTOCALTYPE         0x08 				// 子协议类型
#define SUBPROTOCALPACKTYPE     0x00				// 子协议打包方式
#define SUBPROTOCALECHO         0x00                // 子协议应答码

#if (_SUB_PROTOCOL_ENCRYPT_ == _ML_PROTOCOL_ENCRYPT_)
#define ENCRYPT_KEY		"ML80_20140315@yfzx.ml"
#elif (_SUB_PROTOCOL_ENCRYPT_ == _SD_PROTOCOL_ENCRYPT_)
#define ENCRYPT_KEY		"TFSUODI_20141201@yfzx.cn"
#elif (_SUB_PROTOCOL_ENCRYPT_ == _TF_PROTOCOL_ENCRYPT_)
#define ENCRYPT_KEY		"TF_20150201@tianfu.cn"
#elif (_SUB_PROTOCOL_ENCRYPT_ == _HY_PROTOCOL_ENCRYPT_)
#define ENCRYPT_KEY		"TFHUAYING_20160310@tianfu.cn"
#endif

#if (_SUB_PROTOCOL_ENCRYPT_ == _NO_PROTOCOL_ENCRYPT_)
#define ENCRYPT_KEY_LEN		0
#else
#define ENCRYPT_KEY_LEN		strlen(ENCRYPT_KEY)
#endif

//#define INADDR_BROADCAST 		0xffffffff

#define	MAIN_NET_HEAD_SIZE		(sizeof(MAIN_NET_HEAD))					// 主协议网络包头长度
#define	SUB_NET_HEAD_SIZE		(sizeof(NET_HEAD))						// 子协议网络包头长度
#define	NET_HEAD_SIZE			(sizeof(MAIN_NET_HEAD)+sizeof(NET_HEAD))// 网络包头长度
#define MANAGER_NUM				100					// 管理员机起始编号
#define MANAGER_LEN				3					// 管理员机编号长度
#define SCREENSAVE_NUM			6					// 屏保数量

#define NETCMD_UDP_PORT			14301				// 统一的命令端口,外区为映射后的端口
#define	NETVIDEO_UDP_PORT		31410
#define	NETAUDIO_UDP_PORT		31420
#endif

#define NETCMD_ELEVATOR_PORT	8008				// 电梯授权命令端口
#define NET_DATA_MAX			(NET_PACKBUF_SIZE-NET_HEAD_SIZE) 	// 附加数据最大
/***************************以下为枚举类型定义***********************/

// 子协议加密类型
typedef enum
{
	SUB_ENCRYPT_NONE           = 0x00,				// 不加密
	SUB_ENCRYPT_YH             = 0x01				// 异或方式加密
	
}SUB_PROTOCAL_ENCRYPT;

// 主协议包类型
typedef enum
{
	MAIN_PACKTYPE_NORM			= 0x0000,				// 普通包
	MAIN_PACKTYPE_TC			= 0xFFFF				// 透传包

}MAIN_PROTOCAL_PACKTYPE;

#ifdef _AU_PROTOCOL_
// 子系统代号
typedef enum
{
	SSC_PUBLIC					= 0x00,				// 公共部分
	SSC_INTERPHONE				= 0x10,				// 对讲，含区间通话
	SSC_IPPHONE					= 0x11,				// IP电话
	SSC_PSTNPHONE				= 0x12,				// 普通电话
	SSC_ALARM					= 0x20,				// 报警
	SSC_INFO					= 0x30,				// 信息
	SSC_EHOME					= 0x40,				// 家电
	SSC_MULTIMEDIA				= 0x50,				// 多媒体
	SSC_ELIVE					= 0x60,				// E生活
	SSC_OTHER					= 0x70,				// 便利功能
	SSC_VIDEOMONITORING 		= 0x75,		 	   	// 视频监视
	SSC_IPDOORPHONE				= 0x80,				// 家庭网关 add by luofl 2007-9-29
}SUB_SYS_CODE_E;
#else
//  子系统代号
typedef enum
{
	SSC_PUBLIC					= 0xFF,				// 公共部分
	SSC_INTERPHONE				= 0xF0,				// 对讲，含区间通话
	SSC_ALARM					= 0xF5,				// 报警
	SSC_INFO					= 0xE5,				// 信息
	SSC_EHOME					= 0xE0,				// 家电
	SSC_MULTIMEDIA				= 0xD0,				// 多媒体
	SSC_ELIVE					= 0xD1,				// E生活
	SSC_OTHER					= 0xD2,				// 便利功能
	SSC_VIDEOMONITORING 		= 0xC0,		 	   	// 视频监视
	SSC_IPDOORPHONE				= 0xF1,				// 网络门前机
	SSC_SOFTWARE 				= 0XB0				// 冠林网络间协议
}SUB_SYS_CODE_E;
#endif

// 网络应答码
typedef enum
{
	ECHO_OK						= 0x00,				// 正确应答
	ECHO_ERROR					= 0x01,				// 一般错误
	ECHO_BUSY					= 0x02,				// 忙应答
	ECHO_NO_RECORD				= 0x03,				// 请求的信息不存在
	ECHO_UNALLOWDD				= 0x04,				// 不允许远程控制
	ECHO_NO_APPLY				= 0x05,				// 未申请该服务（或无此服务）
	ECHO_CARD_FULL				= 0x06,				// 卡区已满
	ECHO_HAVED					= 0x07,				// 记录已经存在
	ECHO_OFFLINE				= 0x08,				// 请求方离线
	ECHO_MAC					= 0x09,				// MAC码错误
	ECHO_REPEAT					= 0x0A,				// 设置重复
	ECHO_ALARM_ERROR			= 0x0B,				// 安防控制失败, 主要用于布防时，报警防区处于故障、触发等状态
	ECHO_NOT_SUPPORT			= 0x0C              // 不支持该媒体参数
}ECHO_VALUE_E;

#if 0
// 控制执行者（报警远程控制、家电远程控制）
typedef enum
{
	EXECUTOR_LOCAL_HOST			= 0x01,				// 本机控制
	EXECUTOR_REMOTE_DEVICE		= 0x02,				// 遥控器

	EXECUTOR_LOCAL_FJ			= 0x05,				// 本地网络分机(包含无线终端、家庭PC)
	EXECUTOR_LOCAL_MNFJ			= 0x06,				// 本地模拟分机

	EXECUTOR_MANAGER			= 0x10,				// 管理员机
	EXECUTOR_STAIR				= 0x11,				// 梯口机
	EXECUTOR_AREA				= 0x12, 			// 区口机
	EXECUTOR_IE_USER			= 0x13,				// 远程IE平台
	EXECUTOR_SHORT_MSG			= 0x20,				// 住户短信
	EXECUTOR_PHONE_USER			= 0x21,				// 住户电话
	EXECUTOR_SERVER				= 0x22				// 中心服务器
}EXECUTOR_E;
#endif

// 事件发送方
typedef enum
{
	EST_AURINE					= 0x01,				// 发送到冠林平台
	EST_SERVER					= 0x02,				// 发送到中心服务器
	EST_MANAGER					= 0x03				// 发送到管理员机
}EVENT_SENDTO_E;

// 发送包状态
typedef enum
{
	 SEND_STATE_SEND_READY		= 0,				// 等待发送
	 SEND_STATE_SENDING, 							// 正在发送
	 SEND_STATE_WAIT_ECHO,							// 等待应答
	 SEND_STATE_RECIVED_ECHO,						// 收到应答包
	 SEND_STATE_ECHO_TIMEOUT						// 应答超时	
}SEND_STATE_E;

// 网络包优先级
typedef enum
{
	PRIRY_HIGHEST = 0x00,							//最高优先级，一般用于报警
	PRIRY_REALTIME = 0x01,							//实时命令，如开锁等
	PRIRY_DEFAULT = 0xFF							//默认参数；没有定义优先级的，默认填该参数
}PRIRY_TYPE_E;
/***************************以上为枚举类型定义***********************/

/***************************以下为网络包定义**************************/
// 网络发送包
typedef struct SEND_PACKET_NODE
{
	struct SEND_PACKET_NODE * next;
	struct SEND_PACKET_NODE * prev;
	char SendState;									// 发送状态, 见TSendState
	char SendTimes; 								// 当前发送次数
	char SendTimesMax; 								// 最多发送次数
	int TimeOut;									// 发送超时时间, 秒
	int WaitCount; 									// 等待时间计数
	uint32 address;									// 对方IP地址
	uint16 port;									// 对方端口号
	int size;										// 数据包大小
	char * data;									// 发送数据
}SEND_PACKET, *PSEND_PACKET;

// 网络接收包
typedef struct RECIVE_PACKET_NODE
{
	struct RECIVE_PACKET_NODE * next;
	uint32 address;									// 对方IP地址
	uint16 port;									// 对方端口号
	int size;										// 数据包大小
	char * data;									// 发送数据
}RECIVE_PACKET, *PRECIVE_PACKET;
/***************************以上为网络包定义**************************/

/********************以下为网络命令相关的结构类型定义************************/

// 开始定义数据包, 使用字节对齐方式
#pragma pack(push,1)  

// 网络主包头 ML80
typedef struct
{
	uint8 		subProtocolType;                   	// 子协议类型
	uint8 		DirectFlag;							// 包方向标志
	uint16 		mainProtocolType;					// 主协议版本
	uint8 		hwEncrypt[16];						// 硬件设备加密密码
	uint32 		mainSeq;							// 数据包序列号
	uint8 		sysChipType;						// 系统芯片方案定义码
	uint8 		subProtocolPackType;				// 子协议打包类型
	uint8 		subProtocolEncrypt;					// 子协议加密类型
	uint8 		EchoValue;							// 应答码
	uint16	 	mainPacketType;						// 主协议包类型
	uint16 		subPacketLen;                       // 子协议包长度
} MAIN_NET_HEAD, *PMAIN_NET_HEAD;

#ifdef _AU_PROTOCOL_
// 网络包头
typedef struct
{
	uint16		PackageID;							// 包标识
	uint16		ProtocolVer;						// 协议版本
	uint32		AreaCode;							// 小区编号
	DEVICE_NO	DeviceNo;							// 设备编号
	uint8		DeviceType;							// 设备类型
	uint8		DirectFlag;							// 方向标志（主动：0xAA；应答：0x00）
	uint8		SubSysCode;							// 子系统代号
	uint8		command;							// 命令值
	uint8		EchoValue;							// 应答码
	uint8		Encrypt;							// 加密类型
	uint16		DataLen;							// 数据长度
}NET_HEAD, *PNET_HEAD;
#else
typedef struct
{ 
	uint32		PackageID;                          // 包标识
	uint16 		ProtocolVer;						// 协议版本
	uint16 		Priry;								// 包优先级
	DEVICE_NO 	SrcDeviceNo;						// 源设备编号
	DEVICE_NO 	DestDeviceNo;						// 目标设备编号
	//uint8 		Resver;								// 保留位
	uint8 		DeviceType;
	uint8 		DirectFlag;							// 方向标志
	uint8 		SubSysCode;							// 子系统代号
	uint8		command;							// 命令
	uint8		EchoValue;							// 应答码
	uint8 		Encrypt;							// 加密类型
	uint16 		DataLen;							//数据长度
}NET_HEAD, *PNET_HEAD;
#endif

// 网络时间结构体
typedef struct
{
	uint16 		year;
	uint8  		month;
	uint8  		day;
	uint8  		week;
	uint8  		hour;
	uint8  		min;
	uint8  		sec;
}NET_DATA_TIME, *PNET_DATA_TIME;

// 主发的网络命令类型
typedef enum
{
	NET_COMM_TYPE_NONE         = 0x00,        // 无
	NET_COMM_TYPE_CALL         = 0x01,		  // 对讲命令
	NET_COMM_TYPE_EVENT        = 0x02,		  // 对方编号为中心机
	NET_COMM_TYPE_CONTROL      = 0x03		  // 对方编号为广播编号
}NET_COMM_TYPE_E;
#pragma pack(pop)  									// 结束定义数据包, 恢复原对齐方式
/********************以上为网络命令相关的结构类型定义************************/

#ifdef __cplusplus
}
#endif
#endif 

