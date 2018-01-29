/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_input_ctrl.h
  Author:   	caogw
  Version:   	2.0
  Date: 		2014-10-22
  Description:  
   				输入框控件头文件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UI_INPUT_CTRL_H__
#define __UI_INPUT_CTRL_H__

typedef enum
{
	INPUT_MODE_COMMON	       = 0x00,				// 普通显示 
	INPUT_MODE_PASSWORD,							// 密码显示
	INPUT_MODE_CALL,								// 呼叫模式
	INPUT_MODE_NEW,
	INPUT_MODE_TIME,								// 呼叫模式	
}INPUT_MODE;				

typedef struct 
{
    HWND parent;                                    // 父窗体
	INPUT_MODE mode;								// 模式 
	uint8 enable;									// 使能
	uint8 active;									// 激活状态
	uint8 videomode;								// 视频模式
	uint8 maxlen;									// 最大长度
	uint8 showlen;									// 显示长度
	uint8 len;										// 当前长度
	uint16 maxdata;									// 最大值
	char text[50];						 		   //输入的数字值
	//WCHAR textw[MAX_CHAR_SHOW];
	uint32 BkID;									// 背景图片
	uint32 BkOnID;									// 背景ON图片	
	uint8  Font;									// 字体
	//TxtStyle txtstyle;							// 字体显示标准
	uint32 hit;										// 提示
	uint32 textid;                                  // 提示字的id
	uint8 showtype;									// 显示类型 0-* 1-明文
}INPUT_INFO,*PINPUT_INFO;	
#endif

