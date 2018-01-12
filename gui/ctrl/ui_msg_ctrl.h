/*********************************************************
  Copyright (C), 2006-2016
  File name:	ui_msg_ctrl.h
  Author:   	caogw
  Version:   	2.0
  Date: 		2014-10-22
  Description:  消息框
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UI_MSG_CTRL_H__
#define __UI_MSG_CTRL_H__

#if (_UI_STYLE_ == _V6_UI_STYLE_)
#define FORM_MSG_W				360					// 窗体大小
#define FORM_MSG_H				200
#define FORM_MSG_X				(SCREEN_WIDTH-FORM_MSG_W-RIGHT_CTRL_W)/2
#define FORM_MSG_Y				(SCREEN_HEIGHT-FORM_MSG_H)/2
#elif (_UI_STYLE_ == _E81S_UI_STYLE_)
#define FORM_MSG_W				360
#define FORM_MSG_H				136
#define FORM_MSG_X				(SCREEN_WIDTH - FORM_MSG_W)/2
#define FORM_MSG_Y				(SCREEN_HEIGHT - FORM_MSG_H)/2

#define FORM_QUERY_W			360
#define FORM_QUERY_H			200
#define FORM_QUERY_X			(SCREEN_WIDTH - FORM_QUERY_W)/2
#define FORM_QUERY_Y			(SCREEN_HEIGHT - FORM_QUERY_H)/2

#define FORM_HIT_W				300
#define FORM_HIT_H				100
#define FORM_HIT_X				(SCREEN_WIDTH - FORM_HIT_W)/2
#define FORM_HIT_Y				(SCREEN_HEIGHT - FORM_HIT_H)/2
#endif

#define MAX_PASSWORD_LEN 6

typedef enum
{
	MSG_EVENT_YES = 0x01,							// 是
	MSG_EVENT_NO,									// 否
	MSG_EVENT_XIECHI								// 挟持密码
} MSG_EVENT;	


typedef struct 
{
	HWND 		parent;                          	// 父窗体
	uint32		ID;
	AU_MSG_TYPE	Mode;								// 消息类型
	uint32		TextID;								// 内容文字ID
	uint32 		MsgBk;								// 消息框背景图
	uint32		MsgIcon;							// 消息图标
	uint32  	TimeId;								// 定时器ID
	uint8 		VideoMode;							// 是否是视频模式
	uint8		State;								// 按键状态
	int8 		Select;								// 按到第几个按键
	uint8 		echo;								// 是否需要通知父窗体
	uint8		xiechi;								// 是否检测挟持密码
	char		InputPass[MAX_PASSWORD_LEN+1];		// 密码
	char 		adminPass[MAX_PASSWORD_LEN+1];		// 工程密码
	char 		Pass[MAX_PASSWORD_LEN+1];			// 比较密码
	uint8 		addrsel;							// 全选
	uint8 		addrpos;							// 地址位置
	uint16 		AddrStart;							// 起始地址
	uint16 		AddrEnd;							// 结束地址
	uint16 		Addr[2];							// 地址
	char		DevName[MSG_CHAR];					// 家电设备名
	uint8		devtype;							// 家电设备
	uint8 		devstate;							// 设备状态
	uint32 		devMaxValue;						// 设备最大值
	uint32 		devMinValue;						// 设备最小值
	uint32 		devNowValue;						// 设备当前值
	uint32 		Hit;								// 提示错误
	uint8       factory;                            // 家电厂商
	uint8       flag;
	uint8       style;                              // 对于空调来说，显示模式还是温度
	uint8       kt_mode;                            // 空调的模式
	uint32		Code;								// IP Code
	RIGHTCTRL_INFO  button;
}MSG_INFO,*PMSG_INFO;		// 消息 警告 错误 结构

#endif /* __UI_MSG_CTRL_H__ */

