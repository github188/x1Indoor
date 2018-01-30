/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	AppConfig.h
  Author:     	luofl
  Version:    	2.0
  Date: 
  Description:  N32926方案已实现的功能（通过宏定义来开启相应的功能）
  				#define	_ELEVATOR_MODE_				// 电梯授权功能
  				#define _DOOR_PHONE_				// 模拟门前机
*************************************************/
#ifndef __APPCONFIG_H__
#define __APPCONFIG_H__

#define DEBUG_PRINT				1

//#define AEC_DEBUG									// 消回声保存文件
#define SCREEN_CAPTURE			0					// 屏幕截图,按下开锁键拍一张保存在D盘中。
#define SCREEN_CHECK			0					// 屏幕校正测试
#define _USE_RGB32_				

// ====定义提示音音量========================================
#define MSG_HIT_VOL				4
#define AF_HIT_VOL				4
#define LYLY_VIEW_VOL			8//6
#define MSG_VIEW_VOL			4
#define AVI_PLAY_VOL			60
#define MP3_PLAY_VOL			60
#define JRLY_VOLUME				8//6
#define RING_OUT_VOL			8//6

// ==== 系统方案定义码 =========================================
#define RK2918					11
#define RK3066					12
#define RK2818					13
#define RV1108					14
#define Cx92755					21
#define Hi3515					31
#define N32926					92
#define X1						41

#define SYYCHIPTYPE				X1

// ====定义结构件(不同结构音量等级不同)=====================
#define ML8_V6S					1
#define ML8_V7S					2
#define AH8_E81S				3

// ====版本定义================================================
#define ML8_V6S_JHB_VER			1					// ML8_V6S
#define ML8_V7S_JHB_VER			2					// ML8_V7S
#define AH8_E81S_JHB_VER		3					// AH8_E81S


#define SYS_TYPE				AH8_E81S_JHB_VER

// ====屏幕分辨率================================================
#define _LCD_1024600_			1					//1024*600
#define	 _LCD_800480_			2					//800*480

#define _LCD_DPI_				_LCD_1024600_


// ====UI样式定义================================================
#define _V6_UI_STYLE_			1
#define _E81S_UI_STYLE_			2

#if (SYS_TYPE == AH8_E81S_JHB_VER)
#define _UI_STYLE_				_E81S_UI_STYLE_
#elif (SYS_TYPE == ML8_V7S_JHB_VER)
#define _UI_STYLE_				_V6_UI_STYLE_	
#elif (SYS_TYPE == ML8_V6S_JHB_VER)
#define _UI_STYLE_				_V6_UI_STYLE_	
#endif



// 硬件加密码也就是厂商代码
#define _TYSET_HWENCRYPT_		0XFF5459FF			// 设置项使用的通用厂家代码
#define _ML_HWENCRYPT_			0X00004753			// 米立使用的硬件加密码
#define _TF_HWENCRYPT_			0X54460000			// 天富使用的硬件加密码
#define _ZH_HWENCRYPT_			0X00005A48			// 中海使用的硬件加密码
#define _SD_HWENCRYPT_			0X54465344			// 索迪使用的硬件加密码
#define _HY_HWENCRYPT_			0X54464859			// 天富华鹰的硬件加密码

// 协议版本号
#define _ML_PROTOCOL_VER_		0X0101				// 米立协议版本
#define _TF_PROTOCOL_VER_		0XA0AF				// 天富协议版本
#define _ZH_PROTOCOL_VER_   	0XA101				// 中海协议版本
#define _SD_PROTOCOL_VER_		0XA001				// 索迪协议版本
#define _HY_PROTOCOL_VER_		0XA0A1				// 华鹰协议版本

// 协议加密类型,不同的加密类型有不同的KEY, 0-不加密
#define _NO_PROTOCOL_ENCRYPT_   0x00				// 协议不加密
#define _TF_PROTOCOL_ENCRYPT_ 	0xAF				// 天富协议加密类型
#define _SD_PROTOCOL_ENCRYPT_   0xA0				// 索迪协议加密类型
#define _HY_PROTOCOL_ENCRYPT_   0xA1				// 华鹰协议加密类型


// ====各版本功能定义============================================
//-----------公共功能------------------
//#define _JD_MODE_									// 家电模块
//#define	_YUYUE_MODE_							// 启用预约功能
#define _SHOW_USED_JDDEV_ONLY_						// 家电控制和情景设置界面只显示启用的设备
#define _USE_ELEVATOR_								// 启用电梯控制器
#define _TY_STAIR_									// 支持通用梯口机
#define _AURINE_ELEC_NEW_                           // add by fanfj增加新冠林家电
#define _DIANTI_CALL_								// 主动召梯功能
#define _IP_MODULE_									// IP模块
//#define _LIVEMEDIA_								// RTSP模块
//#define _MULTI_CENTER_							// 多中心
#define _USE_NEW_CENTER_							// 使用新版本中心服务器
#define _SEND_SDP_PARAM_							// 是否发送媒体参数
//#define _RTSP_REG_KEEP_							// RTSP心跳保持(目的获取室内摄像头监视列表)
#define _TIMER_REBOOT_NO_LCD_           			// 定时重启时不开背光
//#define _UPLOAD_PHOTO_							// 网络门前机拍照上传功能
//#define _NEVER_CLOSE_LCD_							// 启用从不关屏
#undef _JD_MODE_
#undef _YUYUE_MODE_
#undef _LIVEMEDIA_
#undef _MULTI_CENTER_
#undef _RTSP_REG_KEEP_

//-----------各版本差异功能-------------
#if (SYS_TYPE == ML8_V6S_JHB_VER)
#define _HW_ENCRYPT_			_ML_HWENCRYPT_			// 该版本使用的硬件加密码
#define _PROTOCOL_VER_			_ML_PROTOCOL_VER_		// 该版本使用的协议版本
#define _SUB_PROTOCOL_ENCRYPT_  _NO_PROTOCOL_ENCRYPT_	// 协议加密类型,米立版本不加密

#define HARD_VER_COMM		    "SNV641-100201"
#define JIEGOU_TYPE				ML8_V6S
#define _ALARM_IO_										// 报警直接用IO口

#elif (SYS_TYPE == ML8_V7S_JHB_VER)
#define _HW_ENCRYPT_			_ML_HWENCRYPT_			// 该版本使用的硬件加密码
#define _PROTOCOL_VER_			_ML_PROTOCOL_VER_		// 该版本使用的协议版本
#define _SUB_PROTOCOL_ENCRYPT_  _NO_PROTOCOL_ENCRYPT_	// 协议加密类型,米立版本不加密

#define HARD_VER_COMM		    "SNV741-100201"
#define JIEGOU_TYPE				ML8_V7S
#define _ALARM_IO_										// 报警直接用IO口
#define _CP2526_TOUCH_KEY_								// 启用感应按键背光灯

#elif (SYS_TYPE == AH8_E81S_JHB_VER)
#define _AU_PROTOCOL_									// 使用冠林协议
#define _UPLOAD_PHOTO_									
#define JIEGOU_TYPE				AH8_E81S
#define _ALARM_IO_										// 报警直接用IO口	
#define _USE_TIME_SET_									// 是否启用时间设置

#define	 SOFTSUBVER				"AH8-E81S"
#define HARDSUBVER				"E81S"

#define _TIMER_REBOOT_NO_LCD_           				// 定时重启时不开背光
#define _CP2526_TOUCH_KEY_								// 启用感应按键背光灯
#define _USE_AURINE_SET_								// 是否启用冠林协议设置工具
//#define _TEST_REBOOT_									// 测试一直重启
#undef _USE_NEW_CENTER_	
#undef _IP_MODULE_	

#endif

#ifdef _IP_MODULE_
#define _IP_MODULE_DJ_									// IP模块对讲
#define _IP_MODULE_ALARM								// 报警功能
#ifdef _JD_MODE_
#define _IP_MODULE_JD_									// IP模块家电
#endif
//#define _IP_MODULE_RTSP_								// RTSP注册
#endif



//-----------硬件版本------------------
#ifndef HARD_VER_COMM
#define HARD_VER_COMM		    "X1-"
#endif

//----------软件版本---------------------
#ifndef SOFT_VER_COMM
#define SOFT_VER_COMM		    "V2.03.0001_"
#endif

#define BUILD_TIME				__TIME__ "_" __DATE__

#ifndef HARDSUBVER
#define HARDSUBVER				"-REL"
#endif

#ifndef SOFTSUBVER
#define SOFTSUBVER				BUILD_TIME
#endif

#define HARD_VER				(HARD_VER_COMM HARDSUBVER)
#define SOFT_VER				(SOFT_VER_COMM SOFTSUBVER)
#define SDK_VER					BUILD_TIME

#endif

