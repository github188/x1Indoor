/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	storage_syssave.c
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-10-02
  Description:  系统配置存储(包含系统参数和安防参数)

  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "storage_include.h"

#define SYSCONFIG_SIZE			(sizeof(SYSCONFIG))

#ifdef _ADD_BLUETOOTH_DOOR_
#if SCREEN_CAPTURE
#define DIR_PATH_NUM			15
#else
#define DIR_PATH_NUM			14
#endif
#else
#if SCREEN_CAPTURE
#define DIR_PATH_NUM			14
#else
#define DIR_PATH_NUM			13
#endif
#endif

// 以下参数为恢复出厂设置后使用
SYSCONFIG gRecoSysConfig =
{
    sizeof(SYSCONFIG),
	SaveMagicNumber1,			// 特殊字符，用来查看结构体数据是否正确
	
    /**************************安防参数设置**************************/
    {1,							// b_write 判断安防参数是否初始化过
    DIS_DEFEND,					// defend_state 撤防状态
	0x00,						// enable 防区启用与关闭,默认全部防区不启用
	0x07,						// is_24_hour 一、二、三防区为默认为24小时防区,其中一防区固定为24小时防区,不可更改
	0xFF,						// finder_state 探头状态
	0xFE,						// can_hear 可听(播放声音)
	0xFE,						// can_see 可见(弹出报警界面)
    {0, 1, 2, 3, 4, 5, 6, 7},	// area_type 防区对应的图标类型
    {0, 0, 0, 1, 1, 0, 0, 0},	// delay_time 延时报警时间
    1,							// alarm_time 报警时间
    0,							// 短信验证码	
    0,							// exit_alarm_time 退出预警时间
    0,							// isolation 暂时隔离
    0xEF,						// part_valid 局防有效
    0x00,						// remote_set 远程设置
    0,							// link_param 联动撤防设置
    {3, 4},
    1,							// 是否启用预警声音
   	{{""}, {""}},				// 短信号码
   	{{""}, {""}},				// 报警号码
	// 单个防区状态
    {WORK_SHOW_STATE, WORK_SHOW_STATE, WORK_SHOW_STATE, NORM_SHOW_STATE,
     NORM_SHOW_STATE, NORM_SHOW_STATE, NORM_SHOW_STATE, NORM_SHOW_STATE}},

	/**************************系统参数设置**************************/
	// 网络参数设置
	{{DEFAULT_DEVICEIP,			// IP 设备IP
	DEFAULT_SUBNET,				// SubNet 子网掩码
	DEFAULT_GATEWAY, 			// DefaultGateway 默认网关
	DEFAULT_DNS1,				// DNS1 DNS1服务器IP  DHCP与DNS1同IP
	DEFAULT_DNS2, 				// DNS2 DNS2服务器IP  预留上网用
	DEFAULT_CENTERIP,			// CenterIP 中心服务器IP
	DEFAULT_MANAGERIP, 			// ManagerIP 管理员机IP
	DEFAULT_SIPPROXYSERVER,		// SipProxyServer SIP代理服务器, DNS1负责解析域名, 下同
	DEFAULT_AURINESERVER, 		// AurineServer 冠林服务器
	DEFAULT_STUNSERVER,			// StunServer Stun服务器
	DEFAULT_DEVICEIP1,			// IP1 设备IP1    
	DEFAULT_SUBNET1,			// SubNet1 子网掩码1  
	DEFAULT_MANAGERIP1,			// ManagerIP1 管理员机2IP 
	DEFAULT_MANAGERIP2,			// ManagerIP2 管理员机3IP
	#ifdef _ETHERNET_DHCP_
	DEFAULT_DIANTISERVER,		// DiantiServer 冠林电梯控制器
	1},							// 不启用静态IP	
	#else
	DEFAULT_DIANTISERVER},		// DiantiServer 冠林电梯控制器
	#endif

	// 设备参数
	{DEFAULT_AREANO,			// AreaNo 小区编号
	#ifdef _AU_PROTOCOL_
	{
	#else
	{DEFAULT_DEV_TYPE,			// DeviceType 设备类型
	0,							// 预留位
	DEFAULT_AREANO,				// 小区编号
	#endif
	DEFAULT_DEVICENO1,			// DeviceNo1 设备号高位
	DEFAULT_DEVICENO2},			// DeviceNo2 设备号低位
	{DEFAULT_RULE_STAIRNOLEN,	// StairNoLen 梯号长度，默认4
	DEFAULT_RULE_ROOMNOLEN,		// RoomNoLen 房号长度，默认4
	DEFAULT_RULE_CELLNOLEN,		// CellNoLen 单元号长度，默认2
	DEFAULT_RULE_USECELLNO,		// UseCellNo 启动单元标志，0－false 1－true， 默认1
	DEFAULT_RULE_SUBSECTION},	// Subsection 分段参数，0为不分段，默认224
	DEFAULT_DEV_TYPE,			// DeviceType 设备类型
	DEFAULT_DEVNO_LEN,			// DevNoLen 设备号长度
	DEFAULT_ISRIGHT,			// IsRight 房号是否设置正确
	DEFAULT_DEVNO_STR,			// DeviceNoStr 设备编号字符串
	0},							// 保留位
	DEFAULT_DEV_DESC,			// DevDesc 设备描述符

	// 密码参数
	{0,							// doorserverEnable 家政开门密码使能
	"123456",						// admin 管理员密码
	"12345",						// user 用户密码, edit by xiewr 20101214, 改为"12345"
	"",							// door_user 用户开门密码
	"",							// door_server 家政开门密码	
	0,							// userserverenb 家政密码使能
	""},							// user_server家政密码	
	// 留影留言参数
	{LYLY_TYPE_VIDEO, 			// Mode 留言模式
	1,							// Enable 留言使能
	{0, 0, 0},					// Link留言的联动
	0},							// default_tip留言的默认提示音 0 默认 1 自动录制
	// 免打扰
	{0,							// noface 免打扰使能
	NOFACE_TIME_30},			// time免打扰时长
	// 电子相册参数
	{1,							// used 电子相册使能
	EPHOTO_TIME_1,				// intime 电子相册进入时长
	EPHOTO_TIME_5},				// holdtime 电子相册保持时长
	// MAC码地址
	{{00, 00, 00, 00, 00, 01},	// HouseMac 室内机MAC地址
    {00, 00, 00, 00, 00, 02},	// Door1Mac 门前机1MAC地址
    {00, 00, 00, 00, 00, 03}},	// Door2Mac 门前机2MAC地址
	// 基它参数
	#ifdef _ADD_BLUETOOTH_DOOR_
	{1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
	#else
	{1, 1, 0, 0, 0, 1, 1, 0},	// Extparam 外部模块
	#endif
    0,               			// Bright 亮度调节
	0,               			// DeskId 0-无桌面
    {0, 0, 0, 0, 0, 0, 0, 0},	// RingID 声音ID(梯口 区口 门口 中心 住户 分机 信息 预警)
	4,
	4,							// TalkVolume 通话音量大小
	4,							// MICVolume 咪头输入增益大小
	1,							// KeyBeep 是否启用按键音
	0,							// 平台注册
	#ifdef _LIVEMEDIA_
	0,							// 流媒体服务器
	#endif
	#ifdef _IP_MODULE_
	{0, 0, 0},
	#endif
	{0, 0},						// 网络门前机IP
	0,							// 系统语言
	0},							// 默认开屏

	/**************************视频SDP描述设置**************************/
	{VIDEO_ENCMODE_H264,		// 视频编码格式 0为不受限制
	20,							// 视频帧率     0为不受限制
	VIDEO_SIZE_VGA,				// 视频分辨率   0为不受限制 		
	0,							// 保留位								
	2048000,					// 视频码率 单位(kbps) 0不受限制
	0,							// 保留位
	0},							// 保留位

	/**************************音频SDP描述设置**************************/
	{AUDIO_ENCMODE_PCMA,		// 音频编码格式
	0,							// 保留位
	0,							// 保留位				
	0},							// 保留位
	
	SaveMagicNumber2,			// 特殊字符，用来查看结构体数据是否正确
};

// 以下参数为第一次烧写时用
const SYSCONFIG  gInitSysConfig =
{
    sizeof(SYSCONFIG),
	SaveMagicNumber1,			// 特殊字符，用来查看结构体数据是否正确
	
    /**************************安防参数设置**************************/
    {1,							// b_write 判断安防参数是否初始化过
    SET_DEFEND,					// defend_state 撤防状态
	0xFF,						// enable 防区启用与关闭,默认全部防区不启用
	0x07,						// is_24_hour 一、二、三防区为默认为24小时防区,其中一防区固定为24小时防区,不可更改
	0xFF,						// finder_state 探头状态
	0xFF,						// can_hear 可听(播放声音)
	0xFF,						// can_see 可见(弹出报警界面)
    {0, 1, 2, 3, 4, 5, 6, 7},	// area_type 防区对应的图标类型
    {0, 0, 0, 0, 0, 0, 0, 0},	// delay_time 延时报警时间
    1,							// alarm_time 报警时间
    0,							// 短信验证码	
    0,							// exit_alarm_time 退出预警时间
    0,							// isolation 暂时隔离
    0xEF,						// part_valid 局防有效
    0x00,						// remote_set 远程设置
    0,							// link_param 联动撤防设置
    {3, 4},
    1,							// 是否启用预警声音
   	{{""}, {""}},				// 短信号码
   	{{""}, {""}},				// 报警号码
	// 单个防区状态
    {WORK_SHOW_STATE, WORK_SHOW_STATE, NORM_SHOW_STATE, NORM_SHOW_STATE,
     NORM_SHOW_STATE, NORM_SHOW_STATE, NORM_SHOW_STATE, NORM_SHOW_STATE}},

	/**************************系统参数设置**************************/
	// 网络参数设置
	{{DEFAULT_DEVICEIP,			// IP 设备IP
	DEFAULT_SUBNET,				// SubNet 子网掩码
	DEFAULT_GATEWAY, 			// DefaultGateway 默认网关
	DEFAULT_DNS1,				// DNS1 DNS1服务器IP  DHCP与DNS1同IP
	DEFAULT_DNS2, 				// DNS2 DNS2服务器IP  预留上网用
	DEFAULT_CENTERIP,			// CenterIP 中心服务器IP
	DEFAULT_MANAGERIP, 			// ManagerIP 管理员机IP
	DEFAULT_SIPPROXYSERVER,		// SipProxyServer SIP代理服务器, DNS1负责解析域名, 下同
	DEFAULT_AURINESERVER, 		// AurineServer 冠林服务器
	DEFAULT_STUNSERVER,			// StunServer Stun服务器
	DEFAULT_DEVICEIP1,			// IP1 设备IP1    
	DEFAULT_SUBNET1,			// SubNet1 子网掩码1  
	DEFAULT_MANAGERIP1,			// ManagerIP1 管理员机2IP 
	DEFAULT_MANAGERIP2,			// ManagerIP2 管理员机3IP
	#ifdef _ETHERNET_DHCP_
	DEFAULT_DIANTISERVER,		// DiantiServer 冠林电梯控制器
	1},							// 不启用静态IP	
	#else
	DEFAULT_DIANTISERVER},		// DiantiServer 冠林电梯控制器
	#endif

	// 设备参数
	{DEFAULT_AREANO,			// AreaNo 小区编号
	#ifdef _AU_PROTOCOL_
	{
	#else
	{DEFAULT_DEV_TYPE,			// DeviceType 设备类型
	0,							// 预留位
	DEFAULT_AREANO,				// 小区编号
	#endif
	DEFAULT_DEVICENO1,			// DeviceNo1 设备号高位
	DEFAULT_DEVICENO2},			// DeviceNo2 设备号低位
	{DEFAULT_RULE_STAIRNOLEN,	// StairNoLen 梯号长度，默认4
	DEFAULT_RULE_ROOMNOLEN,		// RoomNoLen 房号长度，默认4
	DEFAULT_RULE_CELLNOLEN,		// CellNoLen 单元号长度，默认2
	DEFAULT_RULE_USECELLNO,		// UseCellNo 启动单元标志，0－false 1－true， 默认1
	DEFAULT_RULE_SUBSECTION},	// Subsection 分段参数，0为不分段，默认224
	DEFAULT_DEV_TYPE,			// DeviceType 设备类型
	DEFAULT_DEVNO_LEN,			// DevNoLen 设备号长度
	DEFAULT_ISRIGHT,			// IsRight 房号是否设置正确
	DEFAULT_DEVNO_STR,			// DeviceNoStr 设备编号字符串
	0},							// 保留位
	DEFAULT_DEV_DESC,			// DevDesc 设备描述符

	// 密码参数
	{0,							// doorserverEnable 家政开门密码使能
	"123456",						// admin 管理员密码
	"12345",						// user 用户密码, edit by xiewr 20101214, 改为"12345"
	"",							// door_user 用户开门密码
	"",							// door_server 家政开门密码	
	0,							// userserverenb 家政密码使能
	""},							// user_server家政密码	
	// 留影留言参数
	{LYLY_TYPE_VIDEO, 			// Mode 留言模式
	1,							// Enable 留言使能
	{0, 0, 0},					// Link留言的联动
	0},							// default_tip留言的默认提示音 0 默认 1 自动录制
	// 免打扰
	{0,							// noface 免打扰使能
	NOFACE_TIME_30},			// time免打扰时长
	// 电子相册参数
	{1,							// used 电子相册使能
	EPHOTO_TIME_1,				// intime 电子相册进入时长
	EPHOTO_TIME_5},				// holdtime 电子相册保持时长
	// MAC码地址
	{{00, 00, 00, 00, 00, 0x6F},// HouseMac 室内机MAC地址
    {00, 00, 00, 00, 00, 02},	// Door1Mac 门前机1MAC地址
    {00, 00, 00, 00, 00, 03}},	// Door2Mac 门前机2MAC地址
	// 基它参数
	#ifdef _ADD_BLUETOOTH_DOOR_
	{1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
	#else
	{1, 1, 0, 0, 0, 1, 1, 0},	// Extparam 外部模块
	#endif
    0,               			// Bright 亮度调节
	0,               			// DeskId 0-无桌面
    {0, 0, 0, 0, 0, 0, 0, 0},	// RingID 声音ID(梯口 区口 门口 中心 住户 分机 信息 预警)
	4,
	4,							// TalkVolume 通话音量大小
	4,							// MICVolume 咪头输入增益大小
	1,							// KeyBeep 是否启用按键音
	0,							// 平台注册
	#ifdef _LIVEMEDIA_
	0,							// 流媒体服务器
	#endif
	#ifdef _IP_MODULE_
	{0, 0, 0},
	#endif
	{0, 0},						// 网络门前机IP
	0,							// 系统语言
	0},							// 默认开屏
	
	/**************************视频SDP描述设置**************************/
	{VIDEO_ENCMODE_H264,		// 视频编码格式 0为不受限制
	20,							// 视频帧率     0为不受限制
	VIDEO_SIZE_VGA,				// 视频分辨率   0为不受限制 		
	0,							// 保留位								
	2048000,					// 视频码率 单位(kbps) 0不受限制
	0,							// 保留位
	0},							// 保留位

	/**************************音频SDP描述设置**************************/
	{AUDIO_ENCMODE_PCMA,		// 音频编码格式
	0,							// 保留位
	0,							// 保留位				
	0},							// 保留位
	
	SaveMagicNumber2,			// 特殊字符，用来查看结构体数据是否正确
};

/*************************************************
  Function:		check_sysconfig_ifcorrect
  Description: 	校验数据是否正确
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void check_sysconfig_ifcorrect(void)
{

	if (gpSysConfig->MagicNumber1 == SaveMagicNumber1 
		&& gpSysConfig->MagicNumber2 == SaveMagicNumber2)
	{
		log_printf("the data of sysconfig is correct!!!!!!!!!!!!!!!!! \n");
	}
	log_printf("gpSysConfig->MagicNumber1 : %x\n", gpSysConfig->MagicNumber1);	
	log_printf("gpSysConfig->MagicNumber2 : %x\n", gpSysConfig->MagicNumber2);
}

/*************************************************
  Function:		get_sysconfig_from_storage
  Description: 	从FLASH中获取系统配置参数
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_sysconfig_from_storage(PSYSCONFIG list)
{
	if (list)
	{
		Fread_common(SYSCONFIG_PARAM_PATH, list, SYSCONFIG_SIZE, 1);
	}
}

/*************************************************
  Function:		save_sysconfig_to_storage
  Description: 	系统配置参数保存到FLASH中
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static ECHO_STORAGE save_sysconfig_to_storage(PSYSCONFIG list)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	
	if (list)
	{
		ret =  Fwrite_common(SYSCONFIG_PARAM_PATH, list, SYSCONFIG_SIZE, 1);
	}
	
	return ret;
}

#if 0
/*************************************************
  Function:		storage_free_sysconfig_memory
  Description: 	释放系统配置参数所占用的内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void storage_free_sysconfig_memory(PSYSCONFIG list)
{
	if (list)
	{
		free(list);	
		list = NULL;
	}
}
#endif

/*************************************************
  Function:		storage_malloc_sysconfig_memory
  Description: 	申请系统配置参数的内存
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void storage_malloc_sysconfig_memory(PSYSCONFIG * list)
{
	* list = (PSYSCONFIG)malloc(SYSCONFIG_SIZE);
}		

/*************************************************
  Function:		create_sys_dir
  Description:	创建系统所需的目录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void create_sys_dir(void)
{	
	int32 i;
	char dir_path[DIR_PATH_NUM][50] = {
		{CFG_PUBLIC_DRIVE},
		{CFG_PRIVATE_DRIVE},
		{SYSCONFIG_DIR_PATH}, 
		{CALL_DIR_PATH}, 
		{AF_DIR_PATH},
		{MSG_DIR_PATH},
		{MSG_VOICE_PATH},
		{SNAP_DIR_PATH},
		{LYLY_DIR_PATH},
		{JRLY_DIR_PATH},
		{JD_DIR_PATH},
		{YUYUE_DIR_PATH},
		#if SCREEN_CAPTURE
		{SCREEN_CAP_DIR_PATH},
		#endif
		#ifdef _ADD_BLUETOOTH_DOOR_
		{DOOR_PIC_DIR_PATH},
		#endif
		{MONITOR_DIR_PATH}};

	// 目录不存在则创建
	for (i = 0; i < DIR_PATH_NUM; i++)
	{
		if ((is_fileexist(dir_path[i])) == FALSE)	
		{
			mkdir(dir_path[i], 0x777);
			log_printf("mkdir %s\n", dir_path[i]);
		}
	}
}

/*************************************************
  Function: 	SaveRegInfo
  Description:	保存注册表-系统配置参数
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void SaveRegInfo(void)
{
	save_sysconfig_to_storage(gpSysConfig);
}

/*************************************************
  Function:    	storage_recover_factory
  Description: 	恢复出厂设置
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_recover_factory(void)
{
	// 删除NAND1-2上的目录和文件
	system("rm -rvf /mnt/nand1-2/*");
	system("sync");

	// MAC码不恢复
	memcpy(gRecoSysConfig.SysFlashData.MacAddr.HouseMac, gpSysParam->MacAddr.HouseMac, sizeof(gpSysParam->MacAddr.HouseMac));
	memcpy(gRecoSysConfig.SysFlashData.MacAddr.Door1Mac, gpSysParam->MacAddr.Door1Mac, sizeof(gpSysParam->MacAddr.Door1Mac));
	memcpy(gRecoSysConfig.SysFlashData.MacAddr.Door2Mac, gpSysParam->MacAddr.Door2Mac, sizeof(gpSysParam->MacAddr.Door2Mac));

	// 语言不恢复
	gRecoSysConfig.SysFlashData.Language = gpSysParam->Language;
	storage_init_get_dev_desc(gRecoSysConfig.SysFlashData.DevDesc, gpSysParam->Language);
	memcpy(gpSysConfig, &gRecoSysConfig, sizeof(SYSCONFIG));
	SaveRegInfo();
}

/*************************************************
  Function:    	storage_format_system
  Description: 	格式化NAND1-2盘
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_format_system(void)
{
	// 删除NAND1-2上的目录和文件
	system("rm -rvf /mnt/nand1-2/INFO/");
	system("rm -rvf /mnt/nand1-2/SNAP/");
	system("rm -rvf /mnt/nand1-2/LYLY/");
	#ifndef _AU_PROTOCOL_
    system("rm -rvf /mnt/nand1-2/JRLY/");
    system("rm -rvf /mnt/nand1-2/JD/");
    system("rm -rvf /mnt/nand1-2/RECORD/");
	system("rm -rvf /mnt/nand1-2/ALARM/");
    system("rm -rvf /mnt/nand1-2/MONITOR/");
    system("rm -rvf /mnt/nand1-2/YUYUE/");
	#endif
	#ifdef _ADD_BLUETOOTH_DOOR_
	system("rm -rvf /mnt/nand1-2/BLEDOORPIC/");
	#endif
	create_sys_dir();
	system("sync");
}

/*************************************************
  Function:		storage_init_sysconfig
  Description: 	初始化系统配置
  Input:		无
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_init_sysconfig(void)
{
	create_sys_dir();
	storage_malloc_sysconfig_memory(&gpSysConfig);
	if (is_fileexist(SYSCONFIG_PARAM_PATH) == FALSE)
	{
		if (gpSysConfig)
		{
			memcpy(gpSysConfig, &gInitSysConfig, sizeof(SYSCONFIG));
			SaveRegInfo();
		}
	}
	else
	{
		get_sysconfig_from_storage(gpSysConfig);
	}
	gpAfParam = &gpSysConfig->AfFlashData;
	gpSysParam = &gpSysConfig->SysFlashData;
	
	memset(&g_venc_parm, 0, sizeof(VIDEO_SDP_PARAM));
	memcpy(&g_venc_parm, &gpSysConfig->VideoSdpData, sizeof(VIDEO_SDP_PARAM));
	
	memset(&g_audio_parm, 0, sizeof(AUDIO_SDP_PARAM));
	memcpy(&g_audio_parm, &gpSysConfig->AudioSdpData, sizeof(AUDIO_SDP_PARAM));
}

