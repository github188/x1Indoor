/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_eightarea_ctrl.h
  Author:   	yanjl
  Version:   	2.0
  Date: 		14-10-29
  Description: 
  				八防区控件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UIEIGHTAREACTRL_H__
#define __UIEIGHTAREACTRL_H__
	
/************************常量定义************************/
#define AREA_NUM_MAX			8	
#define AREA_KEYDOWN			1
#define AREA_KEYUP				2
#define EADLG_TXTLEN			40
/************************常量定义************************/

/**********************结构体定义************************/
typedef struct  
{
	uint8  Enabled;								// 使能
	uint8  Flash_state;							// 闪烁状态
	uint16 TextID;								// 文字ID
	uint16 ImageID;								// 图片ID
	uint16 Flash_imageID;						// 闪烁ID
	uint16 MidBgImage;							// 中间层背景
	uint32 ImageBK;								// 背景图片
	uint16 Text[EADLG_TXTLEN];					// 文字字符串
}AREA_INFO,*PAREA_INFO;

typedef struct 
{
	int8 Index;								// 当前选中的区域
	uint8 MaxNum;								// 最大区域数
	uint8 State;								// 按下状态
	uint8 Alarming;								// 有报警
	int TimerID;    							// 定时器ID
	AREA_INFO Areas[AREA_NUM_MAX];			// 每个区域信息
}EIGHTAREACTRL_INFO,*PEIGHTAREACTRL_INFO;
/**********************结构体定义************************/
#endif

