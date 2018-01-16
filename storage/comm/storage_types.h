/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	storage_types.h
  Author:		xiewr
  Version:  	1.0
  Date: 
  Description:  数据类型定义

  History:        
    1. Date:	
       Author:	
       Modification:
    2. ...
*************************************************/
#ifndef _STORAGE_TYPES_H_
#define _STORAGE_TYPES_H_

#include "au_types.h"
#include "AppConfig.h"

#ifdef  __cplusplus
extern "C"
{
#endif

 #pragma pack(push,1)  								// 开始定义数据包, 使用字节对齐方式

// 软件版本
typedef struct
{
	uint8	MajorVer;								// 主版本
	uint8	MinorVer;								// 次版本
	uint16	BuildVer;								// build版本
}SOFT_VERSION;

// 设备类型1B＋软件版本2b+软件大小2B+文件名称50B
typedef struct 
{
	uint8 			DevType;
	SOFT_VERSION 	SoftVer;
	uint32 			FileSize;
	char 			FileName[50];
}SOFT_INFO, * PSOFT_INFO;							// 网络门前机上报的信息

// 时间结构
typedef struct
{
	uint16		year;								// 年
	uint8		month;								// 月
	uint8		day;								// 日
	uint8		week;								// 星期
	uint8		hour;								// 小时
	uint8		min;								// 分
	uint8		sec;								// 秒
}DATE_TIME, *PDATE_TIME;

typedef struct
{
	uint32 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;	
	uint8 week;
}ZONE_DATE_TIME, * PZONE_DATE_TIME;					// 时间结构体

// 室内机向服务器注册的信息
typedef struct
{
	uint32 		Address;							// 本机地址
	uint16 		CmdPort;							// 命令端口
	uint16 		SipPort;							// Sip端口
	uint16 		AudioPort;							// 音频端口
	uint16 		VideoPort;							// 视频端口
}REG_DATA, *PREG_DATA;

typedef enum
{ 
	CHINESE = 0,
	CHNBIG5 = 1,
	ENGLISH = 2,
	MAX_LANGUAGE
}LANGUAGE_E;					// 语言类型

/**********************************************************************/
/*								报警参数							  */
/**********************************************************************/
typedef struct
{
	uint8 		b_write;							// 判断安防信息是否初始化过	
	uint8   	defend_state;						// 安防状态,在家，外出，夜间

	// 属性显示页面所需数据，一个字节表示八防区(报警时间,延时报警时间,类型除外)
	uint8 		enable;		   						// 启用 或关闭   
	uint8       is_24_hour;							// 是否24小时防区
	uint8		finder_state;						// 探头类型(1-常开 0-常闭)
	uint8       can_hear;                           // 可听
	uint8       can_see;  							// 可见
	uint8   	area_type[8];						// 防区对应的图标类型
	uint8       delay_time[8];            			// 延时报警时间
	uint8   	alarm_time;							// 报警时间
	uint32      validate_code;						// 短信验证码
	// 用户设置页面数据
	uint8		exit_alarm_time;				    // 退出预警时间
	uint8 		isolation;							// 暂时隔离	1 = 暂时隔离	
	uint8       part_valid;							// 是否是局防有效 1 = 有效，0 = 无效
	uint8 		remote_set;							// bit0:远程安防设置,bit1:远程撤防,bit2:远程布防,bit3:远程局防
	uint8       link_param;							// 安防联动
	uint8		link_scene[2];						// 安防联动情景模式		
	uint8       used_exit_sound;					// 是否启用预警声音
	uint8  		short_msg[2][15];  					// 短信号码
	uint8		alarm_num[2][15];					// 报警号码

	// 报警显示页面，只用以下一个变量表示
	uint8 		show_state[8];						// 单个防区显示状态
}AF_FLASH_DATA, * PAF_FLASH_DATA;

/**********************************************************************/
/*								系统参数							  */
/**********************************************************************/

/***********************以下为网络参数结构定义***********************/
#define DEFAULT_DEVICEIP				0XC0A8003A 	// 本机IP  192.168.0.10
#define DEFAULT_SUBNET					0XFFFFFF00 	// 子网掩码 255.255.255.0
#define DEFAULT_GATEWAY					0XC0A800FE 	// 网关 192.168.1.254

#define DEFAULT_DNS1					0X00 		// 0.0.0.0
#define DEFAULT_DNS2					0X00 		// 0.0.0.0
//#define DEFAULT_CENTERIP				0XC0A80002 	// 中心服务器 192.168.0.2
#define DEFAULT_CENTERIP				0X00	 	// modi by chenbh 2015-03-25
//#define DEFAULT_MANAGERIP				0XC0A80003 	// 管理员机1 192.168.0.3
#define DEFAULT_MANAGERIP				0X00	 	// modi by chenbh 2015-03-25
#define DEFAULT_SIPPROXYSERVER			0X00 		// 0.0.0.0
#define DEFAULT_DIANTISERVER			0x0A6E70EB  // 冠林电梯控制器默认IP 10.110.112.235
#define DEFAULT_AURINESERVER			0xDCFA1E36  // 冠林服务器 220.250.30.54
#define DEFAULT_STUNSERVER				0X00 		// 0.0.0.0
#define DEFAULT_DEVICEIP1       		0X00  	   	// 0.0.0.0 
#define DEFAULT_SUBNET1         		0X00 		// 0.0.0.0   
#define DEFAULT_MANAGERIP1				0X00 		// 管理员机2
#define DEFAULT_MANAGERIP2				0X00 		// 管理员机3

typedef struct
{
	uint32	IP;										// 设备IP
	uint32	SubNet;									// 子网掩码
	uint32	DefaultGateway;							// 默认网关
	uint32	DNS1;									// DNS1服务器IP  DHCP与DNS1同IP
	uint32	DNS2;									// DNS2服务器IP  预留上网用
	uint32	CenterIP;								// 中心服务器IP
	uint32	ManagerIP;								// 管理员机IP
	uint32	SipProxyServer;							// SIP代理服务器, DNS1负责解析域名, 下同
	uint32	AurineServer;							// 冠林服务器
	uint32	StunServer;								// Stun服务器
	uint32  IP1;          							// 设备IP1    
	uint32  SubNet1;        						// 子网掩码1  
	uint32	ManagerIP1;								// 管理员机2IP 
	uint32	ManagerIP2;								// 管理员机3IP
	uint32	DiantiServer;							// 冠林电梯控制器
	#ifdef _ETHERNET_DHCP_
	uint8	Enable;									// 是否启用静态IP 0: 不启用 1:启用
	#endif
}NET_PARAM, * PNET_PARAM;

typedef struct
{
	uint32	IP;										// 设备IP
	uint32	SubNet;									// 子网掩码
	uint32	DefaultGateway;							// 默认网关
	uint32	DNS1;									// DNS1服务器IP  DHCP与DNS1同IP
	uint32	DNS2;									// DNS2服务器IP  预留上网用
	uint32	CenterIP;								// 中心服务器IP
	uint32	ManagerIP;								// 管理员机IP
	char	SipProxyServer[50];						// SIP代理服务器, DNS1负责解析域名, 下同
	char	AurineServer[50];						// 冠林服务器
	char	StunServer[50];							// Stun服务器
	uint32  IP1;          							// 设备IP1      hyy添加，2007.11.23
	uint32  SubNet1;        						// 子网掩码1    hyy添加，2007.11.23
}NET_PARAM_SEND,*PNET_PARAM_SEND;

typedef struct
{
	uint32	DoorIP1;								// 网络门前机IP1
	uint32	DoorIP2;								// 网络门前机IP1
}NET_DOOR_PARAM, * PNET_DOOR_PARAM;

typedef enum
{
    HOST_IPADDR = 0x00,												
	HOST_NETMASK,
	HOST_GATEWAY,	
	CENTER_IPADDR,
	MANAGER1_IPADDR,
	MANAGER2_IPADDR,
	MANAGER3_IPADDR,
	AURINE_IPADDR,
	DIANTI_IPADDR,
	RTSP_IPADDR,
	#ifdef _SEND_EVENT_TO_STAIR_
	STUN_IPADDR,
	#endif
	IP_MAX
}IP_TYPE;											// 有用的 IP 类型

/***********************以上为网络参数结构定义***********************/

/***********************以下为设备编号结构定义***********************/
#define DEFAULT_AREANO					1			// 区号，默认1
#define DEFAULT_DEVICENO1				0			// 设备号1，默认0

// modi by luofl 2012-02-13 前面有0报到中心会显示乱码
#define DEFAULT_DEVICENO2				10101010	// 设备号2，默认010101010

#define DEFAULT_RULE_STAIRNOLEN			4			// 梯号长度，默认4
#define DEFAULT_RULE_ROOMNOLEN			4			// 房号长度，默认4
#define DEFAULT_RULE_CELLNOLEN			2			// 单元号长度，默认2	
#define DEFAULT_RULE_USECELLNO			1			// 启动单元标志，0－false 1－true， 默认1
#define DEFAULT_RULE_SUBSECTION			224			// 分段参数，0为不分段，默认224
#define DEFAULT_DEV_TYPE				0x32		// DEVICE_TYPE_ROOM	默认设备类型
#define DEFAULT_DEVNO_LEN				(DEFAULT_RULE_STAIRNOLEN+DEFAULT_RULE_ROOMNOLEN+1)
#define DEFAULT_ISRIGHT					1
#define DEFAULT_DEVNO_STR				"010101010"	// 默认设备编号



// 默认设备描述符 XX幢XX单元XXXX房
#if 1
#define DEFAULT_DEV_DESC				{0xb6,0xb0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
									0xb5,0xa5,0xd4,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0xb7,\
									0xbf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
#else
#define DEFAULT_DEV_DESC				{0xb4,0xc9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
									0xb3,0xe6,0xa4,0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0xa9,\
									0xd0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}

#endif

#ifdef _AU_PROTOCOL_
typedef struct
{
	uint32		DeviceNo1;							// 设备号高位，不大于999999999
	uint32		DeviceNo2;							// 设备号低位，不大于999999999
}DEVICE_NO, *PDEVICE_NO;
#else
// 设备编号（房号）
typedef struct
{
	uint8 		DeviceType;                         // 设备类型 
	uint8 		Resver;								// 预留位
	uint16 		AreaNo;								// 小区编号
	uint32		DeviceNo1;							// 设备号高位，不大于999999999
	uint32		DeviceNo2;							// 设备号低位，不大于999999999
}DEVICE_NO, *PDEVICE_NO;
#endif

#ifdef	_USE_AURINE_SET_
typedef struct
{
	uint8 		DeviceType;                         // 设备类型 
	uint8 		Resver;								// 预留位
	uint16 		AreaNo;								// 小区编号
	uint32		DeviceNo1;							// 设备号高位，不大于999999999
	uint32		DeviceNo2;							// 设备号低位，不大于999999999
}DEVICE_SET_NO, *PDEVICE_SET_NO;
#endif

// 设备编号规则, 或放在存储头文件中定义
typedef struct
{
	uint8		StairNoLen;							// 梯号长度，默认4
	uint8		RoomNoLen;							// 房号长度，默认4
	uint8		CellNoLen;							// 单元号长度，默认2
	uint8		UseCellNo;							// 启动单元标志，0－false 1－true， 默认1
	uint32		Subsection;							// 分段参数，0为不分段，默认224
}DEVICENO_RULE;

/*
|-----------|----------------|------------------------------------|-------------------------|
|设备		|设备代号字符串	 |说明								  |备注                     |
|-----------|----------------|------------------------------------|-------------------------|
|室内机		|xx-xx-xx-xx	 |栋号1B(1~99)-单元号1B(1~99)-层号	  |1. “x” －表示0～9的数字|
|			|				 |1B(1~99)-房号1B(1~99)				  |						  	|
|-----------|----------------|------------------------------------|2. “-” －为分割符号，实|
|梯口机		|xx-xx-00-0x	 |栋号1B(1~99)-单元号1B(1~99)-0x00-	  |	际无此分割符	      	|
|			|				 |梯口分机号1B(0~9)					  |                         |
|-----------|----------------|------------------------------------|3. DNS请求查询设备IP时主 |
|区口机		|00-00-01-xx	 |00-00-01-编号1B(0～99),最后一个字	  |	  机名称为:设备代号字符	|
|			|				 |节表示区口机编号，00为主区口，其	  |   串＋'.'+区号字符串。  |
|			|				 |它为次区口						  |	  区号由本机获得，如果 	|
|-----------|----------------|------------------------------------|   本机的区号与DNS服务器 |
|管理员机	|00-00-02-xx	 |00-00-02-编号1B(1～99),最后一个字	  |	  的区口不同，拒绝服务 	|
|			|				 |节表示管理员机编号				  |	                      	|
|-----------|----------------|------------------------------------|                         |
|服务器		|00-00-00-00	 |服务器编号唯一					  |					  		|	
|-----------|----------------|------------------------------------|-------------------------|
*/

typedef struct
{
	uint32  AreaNo;	 								// 小区编号
	DEVICE_NO DeviceNo;								// 设备编号
	DEVICENO_RULE Rule;								// 设备编号规则
	uint8 DeviceType;								// 设备类型
	uint8 DevNoLen;									// 设备号长度
	uint8 IsRight;									// 房号是否设置正确
	char DeviceNoStr[20];							// 设备编号字符串
	uint8 Ret;										// 保留位
}FULL_DEVICE_NO,*PFULL_DEVICE_NO;	

// 设备类型
typedef enum
{
	DEVICE_TYPE_NONE					= 0x00,	
	DEVICE_TYPE_AURINE_SERVER_COMM 		= 0x10,		// 冠林服务器
	DEVICE_TYPE_AURINE_SERVER_WEB		= 0x11,		// 冠林WEB服务器
	DEVICE_TYPE_AURINE_SERVER_SERVICE	= 0x12,		// 冠林服务子系统
	DEVICE_TYPE_AURINE_SERVER_STREAMINGSERVER = 0x13,  // 流媒体服务器
	DEVICE_TYPE_AURINE_SERVER_SMSMMSSERVER    = 0x14,  // SMS/MMS 服务器
	DEVICE_TYPE_CENTER					= 0x20,		// 中心服务器
	DEVICE_TYPE_MANAGER,							// 管理员机
	DEVICE_TYPE_UNIT_MANAGER,                       // 楼道管理员机
	DEVICE_TYPE_AREA					= 0x30,		// 区口机
	DEVICE_TYPE_STAIR,								// 梯口机
	DEVICE_TYPE_ROOM,								// 室内机
	DEVICE_TYPE_FENJI_NET,							// 网络分机
	DEVICE_TYPE_AREA_ROXY,							// 小区代理，目前计划DNS Server、DHCP Server运行在同一台设备上
	DEVICE_TYPE_MEDIA_AD,							// 媒体发布，广告播放
	DEVICE_TYPE_PHONE					= 0x40,		// 普通电话
	DEVICE_TYPE_DOOR_PHONE,							// 门前机(电话)
	DEVICE_TYPE_DOOR_NET,							// 门前机(网络)
	DEVICE_TYPE_GATEWAY					= 0x50,		// 家庭网关
	DEVICE_TYPE_IPCAMERA,							// IP Camera
	DEVICE_TYPE_IPC						= 0x70,		// 社区IPC设备
	DEVICE_TYPE_HOME_IPC				= 0x71		// 家居IPC设备
}DEVICE_TYPE_E;
/***********************以上为设备编号结构定义***********************/

/***********************以下为MAC参数结构定义***********************/
typedef enum
{
    HOUSE_MAC = 0x00,							// 管理员密码
	DOOR1_MAC,									// 用户密码
	DOOR2_MAC									// 用户开门密码
}MAC_TYPE;

typedef struct
{
	uint8 HouseMac[6];
	uint8 Door1Mac[6];
	uint8 Door2Mac[6];
}MAC_PARAM;
/***********************以上为MAC参数结构定义***********************/

/***********************以下为密码参数结构定义***********************/
#define MAX_ADMIN_LEN					6			
#define MAX_USER_LEN					5 		
#define MAX_DOOR_USER_LEN				5
#define MAX_DOOR_SERVER_LEN				5

typedef enum
{
    PASS_TYPE_ADMIN = 0x00,							// 管理员密码
	PASS_TYPE_USER,									// 用户密码
	PASS_TYPE_DOOR_USER,							// 用户开门密码
	PASS_TYPE_DOOR_SERVER,							// 家政开门密码	
	PASS_TYPE_SERVER,								// 家政密码
	PASS_TYPE_MAX
}PASS_TYPE;

typedef struct
{
	uint8 doorserverEnable;							// 家政开门密码使能
	uint8 admin[MAX_ADMIN_LEN+1];					// 管理员密码
	uint8 user[MAX_ADMIN_LEN+1];					// 用户密码
	uint8 door_user[MAX_ADMIN_LEN+1];				// 用户开门密码
	uint8 door_server[MAX_ADMIN_LEN+1];				// 家政开门密码	
	uint8 userserverenb; 							// 家政密码使能
	uint8 user_server[MAX_ADMIN_LEN+1];				// 家政密码	
}PASS_PARAM, *PPASS_PARAM;

/***********************以上为密码参数结构定义***********************/

/***********************以下为留言参数结构定义***********************/
typedef enum
{
    LYLY_LINK_ALARMOUT = 0x00,						// 留言外出联动
	LYLY_LINK_ALARMNIGHT,							// 留言夜间联动
	LYLY_LINK_NOFACE,								// 留言免打扰联动
	LYLY_LINK_MAX	
}LYLY_LINK_TYPE;

typedef enum
{
	LYLY_TYPE_AUDIO = 0x00,							// 声音模式
	LYLY_TYPE_PIC_AUDIO,							// 影音模式
	LYLY_TYPE_VIDEO,								// 视频模式
    LYLY_TYPE_MAX		
}LYLY_TYPE;

typedef struct
{
	LYLY_TYPE Mode;									// 留言模式
	uint8 Enable;									// 留言使能
	uint8 Link[LYLY_LINK_MAX];						// 留言的联动 1 启用 0不启用
	uint8 default_tip;								// 留言的默认提示音 0 默认 1 自动录制
}LYLY_PARAM, *PLYLY_PARAM;

/***********************以上为留言参数结构定义***********************/

/***********************以下为免打扰参数结构定义**********************/
typedef enum
{
	NOFACE_TIME_30 = 0x00,
	NOFACE_TIME_60,
	NOFACE_TIME_120,
	NOFACE_TIME_180,
	NOFACE_TIME_300,
	NOFACE_TIME_480,
	NOFACE_TIME_MAX
}NOFACE_TIME;										// 免打扰时间

typedef struct
{
	uint8 noface;									// 免打扰使能
	NOFACE_TIME time;								// 免打扰时长
}NOFACE_PARAM, *PNOFACE_PARAM;						// 免打扰参数
/***********************以上为免打扰参数结构定义**********************/

/**********************以下为电子相册参数结构定义*********************/
typedef enum
{
	EPHOTO_TIME_HALF = 0x00,
	EPHOTO_TIME_1,
	EPHOTO_TIME_3,
	EPHOTO_TIME_5,
	EPHOTO_TIME_10,
	EPHOTO_TIME_15,
	EPHOTO_TIME_30,
	EPHOTO_TIME_60,
	EPHOTO_TIME_120,
	EPHOTO_TIME_NEVER,
}EPHOTO_TIME;										// 电子相册时间

typedef struct
{
	uint8 used;										// 电子相册使能
	EPHOTO_TIME intime;								// 电子相册进入时长
	EPHOTO_TIME holdtime;							// 电子相册保持时长
}EPHOTO_PARAM, *PEPHOTO_PARAM;						
/**********************以下为电子相册参数结构定义*********************/

typedef enum
{
    EXT_MODE_ALARM = 0x00,							// 报警模块
	EXT_MODE_NETDOOR1,								// 网络门前机1模块
	EXT_MODE_NETDOOR2,								// 网络门前机2模块
	EXT_MODE_PHONEDOOR1,							// 模拟门前机 1
	EXT_MODE_PHONEDOOR2,							// 模拟门前机 2
	EXT_MODE_JD,									// 家电模块
	EXT_MODE_JD_FACTORY,							// 家电厂商
	EXT_MODE_GENERAL_STAIR,							// 通用梯口机
	#ifdef _ADD_BLUETOOTH_DOOR_
	EXT_MODE_BLUETOOTHDOOR1,						// 蓝牙门前机1模块
	EXT_MODE_BLUETOOTHDOOR2,						// 蓝牙门前机2模块
	#endif
	EXT_MODE_MAX
}EXT_MODE_TYPE;

#define MAX_RING_VOLUME			8
#define MIN_RING_VOLUME			1
#define MAX_TALK_VOLUME			8
#define MIN_TALK_VOLUME			1
#define MIN_MIC_VOLUME			1
#define MAX_MIC_VOLUME			8

#define MAX_RING_TYPE			8					// 铃声种类
#define MAX_RING_ID				8					// 最大铃声ID
#define MAX_MSG_ID				4					// 最大信息ID
#define MAX_YJ_ID				4					// 最大预警ID

/**********************以下为IP模块参数结构定义*********************/
typedef struct
{
	uint32 	State;									// 绑定状态: 0-未绑定 1-绑定
	uint32	BindCode;								// 捆绑验证码
	uint32	IpAddr;									// IP地址
}IPMODULE_INFO,*PIPMODULE_INFO;						// IP模块信息
/**********************以上为IP模块参数结构定义*********************/

typedef struct
{
	NET_PARAM Netparam;								// 网络参数	
	FULL_DEVICE_NO 	Devparam;						// 设备编号
	char DevDesc[70];								// 设备描述符
	PASS_PARAM Passparam;							// 密码参数
	LYLY_PARAM Lylyparam;							// 留言参数
	NOFACE_PARAM Nofaceparam;						// 免打扰参数
	EPHOTO_PARAM Ephotoparam;						// 电子相册参数
	MAC_PARAM MacAddr;								// MAC码地址
	uint8 Extparam[EXT_MODE_MAX];					// 外部模块
	uint8 Bright;             						// 背光亮度
	uint8 DeskId;									// 桌面索引
	uint8 RingID[MAX_RING_TYPE];					// 声音ID(梯口 区口 门口 中心 住户 分机 信息 预警)
	uint8 RingVolume;								// 铃声音量大小
	uint8 TalkVolume;								// 通话音量大小
	uint8 MicVolume;								// MIC量大小
	uint8 KeyBeep;									// 是否启用按键音 1 启用  0 不启用
	uint32 RegCode;									// 平台注册号码
	#ifdef _LIVEMEDIA_
	uint32 RtspServer;								// 流媒体服务器
	#endif
	#ifdef _IP_MODULE_
	IPMODULE_INFO IpModule;							// IP模块
	#endif
	NET_DOOR_PARAM NetDoor;							// 网络门前机IP
	LANGUAGE_E Language;							// 系统语言
	uint8 OpenScreen;								// 是否开屏 0:开屏 1:不开屏	
}SYS_FLASH_DATA, * PSYS_FLASH_DATA;					// 系统参数

/********************* 以下为视频SDP参数 ******************/
typedef struct
{
	uint8   encMode;				// 视频编码格式 0为不受限制
	uint8   frameRate;				// 视频帧率     0为不受限制
	uint8   imgSize;				// 视频分辨率   0为不受限制
	uint8   resver0;				// 保留位
	uint32  bitRate;				// 视频码率 单位(kbps) 0不受限制
	uint32  resver1;				// 保留位
	uint32  resver2;				// 保留位
} VIDEO_SDP_PARAM, * PVIDEO_SDP_PARAM;

// 视频编码格式
typedef enum	
{
	VIDEO_ENCMODE_NONE 	= 0,
	VIDEO_ENCMODE_H264 	= 1
}VIDEO_ENCMODE_E;

// 视频分辨率
typedef enum
{
	VIDEO_SIZE_NONE		= 0,
	VIDEO_SIZE_QQVGA	= 1,
	VIDEO_SIZE_QCIF		= 2,
	VIDEO_SIZE_QVGA		= 3,
	VIDEO_SIZE_CIF		= 4,
	VIDEO_SIZE_VGA		= 5,
	VIDEO_SIZE_4CIF		= 6, //704*576
	VIDEO_SIZE_D1		= 7,
	VIDEO_SIZE_SVGA		= 8, //800*600
	VIDEO_SIZE_720P		= 9
}VIDEO_SIZE_E;

/********************* 以上为视频SDP参数 ******************/


/**************** 以下为音频SDP参数 ******************/
typedef struct
{
	uint8 encMode;			// 音频格式 0为不受限制
	uint8 resver0;
	uint16 resver1;
	uint32 resver2;
}AUDIO_SDP_PARAM, * PAUDIO_SDP_PARAM;

// 音频格式
typedef enum
{
	AUDIO_ENCMODE_NONE		= 0,
	AUDIO_ENCMODE_PCMA		= 1,
	AUDIO_ENCMODE_PCMU		= 2
}AUDIO_ENCMODE_E;

typedef struct __AU_VIDEO_CONFIG_S
{
	uint8 brightness;       // 亮度 0-255
	uint8 contrast;			// 对比度 0-255
	uint8 hue;				// 色度 0-255
	uint8 saturation;		// 饱和度 0-255
}AU_VIDEO_CONFIG_S;

/**************** 以上为音频SDP参数 ******************/

extern VIDEO_SDP_PARAM g_venc_parm;
extern AUDIO_SDP_PARAM g_audio_parm;
 
/*********************** 以下为软件版本升级回馈状态值 **********************/
typedef enum
{
	ECHO_VER_UPG_OK				  = 0x00,           // 可以正常升级
	ECHO_VER_UPG_NOT_SUPPORT      = 0x01,		    // 不支持该升级模式
	ECHO_VER_UPG_HW_ERR           = 0x02,			// 硬件版本错误
	ECHO_VER_UPG_SF_ERR			  = 0x03,			// 软件版本较低或移植
	ECHO_VER_UPG_FLIE_ERR		  = 0x04			// 升级文件太大

}ECHO_VER_STATE_E;
/*********************** 以上为软件版本升级回馈状态值 **********************/

typedef struct
{
	DEVICE_NO 	deviceno;		// rtsp 服务器设备编号
	uint32	  	ip;				// rtsp 服务器IP
	uint32 	  	comm_port;		// 其他通讯端口	
	uint32 		rtsp_port;		// rtsp 端口
}RTSP_SERVER_INFO;

typedef struct 
{
	uint32 ip;					// 上报地址
	uint32 port;				// 上报端口
}RP_EVENT_SERVER_INFO;

extern RTSP_SERVER_INFO	 g_Rtsp_ServerInfo;
extern RP_EVENT_SERVER_INFO g_Event_Server;

#ifdef _APP_CALL_ELEVATOR_
typedef struct _stNetSubCtrlData
{
		uint32 systype;			// 发送方系统类型
		uint32 cmdtype;			// 命令类型（透传子命令值）
		uint32 dire;			// 方向
		uint32 seq;				// 序列号,自动递增
		uint32 dataSize;		// 数据长度（透传子命令）
		char * dataparam;		// 具体的数据参数（透传子命令）
}TunnelSendData;

typedef struct _stNetSubReplyData
{
		uint32 systype;			//应答方系统类型
		uint32 cmdtype;			// 命令类型，透传子命令
		uint32 dire;			// 方向
		uint32 seq;				// 序列号,和发送包相同
		uint32 dataSize;		// 数据长度
		char * dataparam;		// 具体的数据参数
}TunnelReplayData;

typedef enum
{
	CMDTYPE_MEDIA			= 0xC1,
	CMDTYPE_ELEVATOR		= 0xC2,
}CMDTYPE;
#endif

#pragma pack(pop)  									// 结束定义数据包, 恢复原对齐方式

#ifdef  __cplusplus
}
#endif

#endif

