/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_spin_ctrl.h
  Author:   	caogw	
  Version:   	2.0
  Date: 		2014-11-27
  Description:  
                IP控件头文件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UI_SPIN_CTRL_H__
#define __UI_SPIN_CTRL_H__

/********************************************************/
/*						控件结构						*/
/********************************************************/


#define BIDON_NUM							1		
#define BIDUNENABLE_NUM						2

typedef enum
{
   SPIN_MODE_NUM 		= 0x00,						// 数字
   SPIN_MODE_TEXT,									// 文字列表
   SPIN_MODE_IP										// IP特殊模式
}SPIN_MODE_E;

typedef struct
{
	uint32  TextID;
	char	Text[30];
}SPIN_ITEM,*PSPIN_ITEM;

typedef struct 
{
	SPIN_MODE_E	Mode;								// 控件模式
	uint8 	Enable;									// 按键使能
	uint8 	state;									// 当前状态
	uint8	loop;									// 是否循环
	int8    selall;									// 全选标志
	int8	index;									// 0 左边 1 右边
	int32	Value;									// 当前的值
	uint16	MinNum;									// 最小的值
	uint16 	MaxNum;									// 最大的值
	uint32  ImageId[2];								// 图片ID
	uint32  ImageBK;								// BK图片ID
	uint32  ImageIpSel;								// IP专用选择图片
	uint8	IP[4];									// IP地址
	uint8 	active;									// 激活
	UILIST 	*ListList;   							// 菜单列表
}SPIN_INFO,*PSPIN_INFO;

#endif

