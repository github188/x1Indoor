/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	storage_path.h
  Author:		luofl
  Version:		2.0
  Date: 		2014-10-02
  Description:	存储路径

  History:		  
	1. Date:
	   Author:
	   Modification:
	2. ...
*************************************************/
#ifndef __STORAGE_PATH_H__
#define __STORAGE_PATH_H__


/**************************************************************************/
/*				公用路径后续路径作为本路径的子目录 						  */	
/**************************************************************************/
#define SOFTWARE_VER_PATH		"/etc/firmVer"									// 软件版本路径
#define HARDWARE_VER_PATH		"/etc/chipVer"									// 固件版本路径
#if 0
#define CFG_PUBLIC_DRIVE		"/sdcard/user/nand-2"						// 存放记录 临时文件
#define CFG_PRIVATE_DRIVE		"/sdcard/user/nand-1"						// 系统配置 重要文件
#define CFG_RES_DRIVE			"/sdcard/user/res/"							// 资源文件 必须原本就存在的路径
#else
#define CFG_PUBLIC_DRIVE		"/nand/user/nand-2"								// 存放记录 临时文件
#define CFG_PRIVATE_DRIVE		"/nand/user/nand-1"								// 系统配置 重要文件
#define CFG_RES_DRIVE			"/nand/user/res/"								// 资源文件 必须原本就存在的路径
#endif



/**************************************************************************/
/*							资源文件路径								  */	
/**************************************************************************/
#define UI_PIC_DIR_PATH			CFG_RES_DRIVE"/app/"							// ui图片资源路径
#define AUDIO_RES_DIR_PATH		CFG_RES_DRIVE"/ring/"							// 铃声路径



// 来电铃声
#define CALL_RING1_PATH			AUDIO_RES_DIR_PATH"/Ring/RING1.WAV"			// 铃声1
#define CALL_RING2_PATH			AUDIO_RES_DIR_PATH"/Ring/RING2.WAV"			// 铃声2
#define CALL_RING3_PATH			AUDIO_RES_DIR_PATH"Ring/RING3.WAV"			// 铃声3
#define CALL_RING4_PATH			AUDIO_RES_DIR_PATH"Ring/RING4.WAV"			// 铃声4
#define CALL_RING5_PATH			AUDIO_RES_DIR_PATH"Ring/RING5.WAV"			// 铃声5
#define CALL_RING6_PATH			AUDIO_RES_DIR_PATH"Ring/RING6.WAV"			// 铃声6
#define CALL_RING7_PATH			AUDIO_RES_DIR_PATH"Ring/RING7.WAV"			// 铃声7
#define CALL_RING8_PATH			AUDIO_RES_DIR_PATH"Ring/RING8.WAV"			// 铃声8
#define CALL_RINGOUT_PATH		AUDIO_RES_DIR_PATH"Ring/RINGOUT.WAV"		// 回铃声


// 中文提示音路径
#define AF_YJ_PATH				AUDIO_RES_DIR_PATH"Alarm/YUJING.WAV"		// 预警声音路径
#define AF_BJ_PATH				AUDIO_RES_DIR_PATH"Alarm/BAOJING.WAV"		// 报警声音路径
#define AF_YJ_WC_PATH			AUDIO_RES_DIR_PATH"Alarm/YJ_WC.WAV"			// 预警的“外出请关好门”的声音
#define AF_YJQ_PATH				AUDIO_RES_DIR_PATH"Alarm/YJQ.WAV"			// 撤防时有警情语音提示“请注意有警情”
#define AF_FQCF_PATH			AUDIO_RES_DIR_PATH"Alarm/FQCF.WAV"			// "防区触发不能布防"的语音提示
#define AF_JJYJC_PATH			AUDIO_RES_DIR_PATH"Alarm/JJYJC.WAV"			// 进行撤防的语音提示：你好！警戒已解除
#define AF_WANAN_PATH			AUDIO_RES_DIR_PATH"Alarm/WANAN.WAV"			// 进行局防时语音提示：晚安
#define AF_ZONE1_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE1.WAV"			// 请注意有警情, 1防区触发
#define AF_ZONE2_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE2.WAV"	
#define AF_ZONE3_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE3.WAV"	
#define AF_ZONE4_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE4.WAV"	
#define AF_ZONE5_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE5.WAV"	
#define AF_ZONE6_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE6.WAV"	
#define AF_ZONE7_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE7.WAV"	
#define AF_ZONE8_PATH			AUDIO_RES_DIR_PATH"Alarm/ZONE8.WAV"	
// 英文提示音路径
#define AF_YJ_WC_PATH_E			AUDIO_RES_DIR_PATH"Alarm/YJ_WC_E.WAV"		// 预警的“外出请关好门”的声音
#define AF_YJQ_PATH_E			AUDIO_RES_DIR_PATH"Alarm/YJQ_E.WAV"		// 撤防时有警情语音提示“请注意有警情”
#define AF_FQCF_PATH_E			AUDIO_RES_DIR_PATH"Alarm/FQCF_E.WAV"		// "防区触发不能布防"的语音提示
#define AF_JJYJC_PATH_E			AUDIO_RES_DIR_PATH"Alarm/JJYJC_E.WAV"		// 进行撤防的语音提示：你好！警戒已解除
#define AF_WANAN_PATH_E			AUDIO_RES_DIR_PATH"Alarm/WANAN_E.WAV"		// 进行局防时语音提示：晚安
#define AF_ZONE1_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE1_E.WAV"		// 请注意有警情, 1防区触发
#define AF_ZONE2_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE2_E.WAV"	
#define AF_ZONE3_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE3_E.WAV"	
#define AF_ZONE4_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE4_E.WAV"	
#define AF_ZONE5_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE5_E.WAV"	
#define AF_ZONE6_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE6_E.WAV"	
#define AF_ZONE7_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE7_E.WAV"	
#define AF_ZONE8_PATH_E			AUDIO_RES_DIR_PATH"Alarm/ZONE8_E.WAV"	

// 预警提示音
#define AF_YJ1_PATH				AUDIO_RES_DIR_PATH"Alarm/YUJING1.WAV"		// 预警提示音1
#define AF_YJ2_PATH				AUDIO_RES_DIR_PATH"Alarm/YUJING2.WAV"		// 预警提示音2
#define AF_YJ3_PATH				AUDIO_RES_DIR_PATH"Alarm/YUJING3.WAV"		// 预警提示音3
#define AF_YJ4_PATH				AUDIO_RES_DIR_PATH"Alarm/YUJING4.WAV"		// 预警提示音4

// 信息提示音
#define MSG_HIT1_PATH			AUDIO_RES_DIR_PATH"Hit/MSG1.WAV"			// 信息提示音1
#define MSG_HIT2_PATH			AUDIO_RES_DIR_PATH"Hit/MSG2.WAV"			// 信息提示音2
#define MSG_HIT3_PATH			AUDIO_RES_DIR_PATH"Hit/MSG3.WAV"			// 信息提示音3
#define MSG_HIT4_PATH			AUDIO_RES_DIR_PATH"Hit/MSG4.WAV"			// 信息提示音4

// 留言提示音
#define LYLY_HIT_CN_PATH		AUDIO_RES_DIR_PATH"Hit/LYLYCN.WAV"			// 默认中文留言提示音
#define LYLY_HIT_EN_PATH		AUDIO_RES_DIR_PATH"Hit/LYLYEN.WAV"			// 默认英文留言提示音



/**************************************************************************/
/*							系统存储文件路径							  */	
/**************************************************************************/
#define SYSCONFIG_DIR_PATH		CFG_PRIVATE_DRIVE"/SYSCONFIG/"   				// 系统配置路径
#define CALL_DIR_PATH			CFG_PUBLIC_DRIVE"/RECORD/"   					// 对讲的路径
#define AF_DIR_PATH				CFG_PUBLIC_DRIVE"/ALARM/"						// 安防路径
#define MSG_DIR_PATH			CFG_PUBLIC_DRIVE"/INFO/"						// 信息文件夹
#define MSG_VOICE_PATH			CFG_PUBLIC_DRIVE"/INFO/VOICE/"					// 信息声音文件
#define SNAP_DIR_PATH			CFG_PUBLIC_DRIVE"/SNAP/"						// 抓拍文件夹		
#define LYLY_DIR_PATH			CFG_PUBLIC_DRIVE"/LYLY/"						// 留影留言文件夹		
#define JRLY_DIR_PATH			CFG_PUBLIC_DRIVE"/JRLY/"						// 家人留言文件夹
#define JD_DIR_PATH				CFG_PUBLIC_DRIVE"/JD/"						// 家电文件夹
#define YUYUE_DIR_PATH			CFG_PUBLIC_DRIVE"/YUYUE/"						// 预约文件
#define MONITOR_DIR_PATH		CFG_PUBLIC_DRIVE"/MONITOR/"				
#define SCREEN_CAP_DIR_PATH		CFG_PUBLIC_DRIVE"/SCREEN_CAP/"				
#define DOOR_PIC_DIR_PATH		CFG_PUBLIC_DRIVE"/BLEDOORPIC/"					// 蓝牙门前拍照上传图片文件夹		


// 系统配置存储(包含系统参数和安防参数)
#define SYSCONFIG_PARAM_PATH	SYSCONFIG_DIR_PATH"/PARAM.BIN"   				// 系统配置参数


// 对讲
#define CALL_IN_PATH			CALL_DIR_PATH"/IN.BIN"   						// 对讲呼入记录
#define CALL_OUT_PATH			CALL_DIR_PATH"/OUT.BIN"  						// 对讲呼出记录
#define CALL_MISS_PATH			CALL_DIR_PATH"/MISS.BIN" 						// 对讲未接记录


// 报警
#define AF_CZ_REC_PATH			AF_DIR_PATH"/CZREC.BIN"						// 安防操作记录
#define AF_BJ_REC_PATH			AF_DIR_PATH"/BJBRE.BIN"						// 安防报警记录


// 信息
#define MSG_MANAGE_PATH			MSG_DIR_PATH"/MANAGE.BIN"						// 信息管理文件


// 抓拍
#define SNAP_MANAGE_PATH		SNAP_DIR_PATH"/MANAGE.BIN"						// 抓拍管理文件


// 留影留言
#define LYLY_MANAGE_PATH		LYLY_DIR_PATH"/MANAGE.BIN"						// 留影留言管理文件
#define LYLY_HIT_RE_PATH		LYLY_DIR_PATH"/LYLY.WAV"						// 录制留言提示音


// 家人留言
#define JRLY_MANAGE_PATH		JRLY_DIR_PATH"/MANAGE.BIN"						// 家人留言管理文件


// 家电
#define JD_LIGHT_VALUE_PATH     JD_DIR_PATH"/ALIGHT.BIN"
#define JD_KONGTIAO_VALUE_PATH  JD_DIR_PATH"/AKONGTIAO.BIN"
#define JD_DEVICE_LIGHT_PATH	JD_DIR_PATH"/LIGHT.BIN"						// 家电灯光管理文件
#define JD_DEVICE_WINDOW_PATH	JD_DIR_PATH"/WINDOW.BIN"						// 家电窗帘管理文件
#define JD_DEVICE_KONGTIAO_PATH	JD_DIR_PATH"/KONGTIAO.BIN"						// 家电空调管理文件
#define JD_DEVICE_POWER_PATH	JD_DIR_PATH"/POWER.BIN"						// 家电电源管理文件
#define JD_DEVICE_GAS_PATH		JD_DIR_PATH"/GAS.BIN"							// 家电煤气管理文件
#define JD_SCENE_PATH			JD_DIR_PATH"/SCENE"							// 家电情景管理文件
#define JD_AURINE_SCENE_PATH    JD_DIR_PATH"/ASCENE.BIN"
#ifdef _AIR_VOLUME_MODE_
#define JD_KONGTIAO_VOLUME_PATH  JD_DIR_PATH"/VOLUMEKONGTIAO.BIN"				// 风量
#endif


// 预约
#define YUYUE_MANAGE_PATH		YUYUE_DIR_PATH"/YUYUE.BIN"						// 预约列表


// 视频监视
#define HOME_CAMERA_PATH		MONITOR_DIR_PATH"/HOME_CAMERA.BIN"	
#define COMMUNITY_CAMERA_PATH	MONITOR_DIR_PATH"/COMMUNITY_CAMERA.BIN"

#define MONITOR_DOOR_PATH		MONITOR_DIR_PATH"/MONITOR_DOOR.BIN"	
#define MONITOR_STAIR_PATH		MONITOR_DIR_PATH"/MONITOR_STAIR.BIN"	
#define MONITOR_AREA_PATH		MONITOR_DIR_PATH"/MONITOR_AREA.BIN"	

// 保存注册成功设备编号
#ifdef _USE_NEW_CENTER_
#define PRE_DEVNO_FILENAME		CFG_PUBLIC_DRIVE"/pre_devno.dat"				
#endif

// 蓝牙门前机拍照上传图片管理
#ifdef _ADD_BLUETOOTH_DOOR_
#define DOOR_PIC_1_PATH			DOOR_PIC_DIR_PATH"/BLEDOORPIC1.BIN"				// 蓝牙门前拍照上传图片管理文件
#define DOOR_PIC_2_PATH			DOOR_PIC_DIR_PATH"/BLEDOORPIC2.BIN"				// 蓝牙门前拍照上传图片管理文件
#endif

// 视频参数管理
#define VIDEO_PARAM_PATH		CFG_PRIVATE_DRIVE"/VIDEO_PARAM.INI"				
#endif 

