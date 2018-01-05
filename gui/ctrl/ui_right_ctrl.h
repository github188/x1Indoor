/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_right_ctrl.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-12
  Description:  
				右边控件程序头文件
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __UI_RIGHT_CTRL_H__
#define __UI_RIGHT_CTRL_H__

/********************************************************/
/*						控件结构						*/
/********************************************************/
#define RIGHT_NUM_MAX						5		// 最大支持按键数
#define KEYDOWN  							1
#define KEYUP  								0
#define DEFAULT_IMAGE_H						100	

#define RIGHT_KEY0							0
#define RIGHT_KEY1							1
#define RIGHT_KEY2							2
#define RIGHT_KEY3							3
#define RIGHT_KEY4							4
#define RIGHT_KEY_MAX						5

#define KEY_WINMAX							5

typedef void(*RightbarCallBack)(void);

typedef struct 
{
	HWND hDlg;
	int iMsg;
	WPARAM wParam;
	LPARAM lParam;
}WIN_MSG, * PWIN_MSG;

typedef struct 
{
	uint8 count;
	WIN_MSG button[KEY_WINMAX];
}WIN_INFO, * PWIN_INFO;

typedef struct 
{
	uint8 Enabled;									// 使能
	uint8 Hold;										// 按键长按
	uint32 TextID;									// 文字ID
	uint32 ImageID;									// 图片ID	
	uint16 TextLen;									// 文字数据长度
	uint32 ImageW;									// 图片高度
	uint32 ImageH;									// 图片宽度
}BUTTON_INFO, * PBUTTON_INFO;

typedef struct 
{
	HWND parent;									// 父窗体
	uint16 BmpBk;									// 背景图片
	uint8 holdmode;									// 按钮保持模式
	int8 VideoMode;									// 视频画法
	int8 Index;										// 当前按下第几个按键
	uint8 state;									// 按下状态
	uint8 MaxNum;									// 最大按键数
	uint8 TextSize;									// 文字大小
	BUTTON_INFO buttons[RIGHT_NUM_MAX+1];			// 每个按键信息
	WIN_INFO wininfo[RIGHT_NUM_MAX+1];				// 控件信息
	RightbarCallBack proc;
}RIGHTCTRL_INFO, * PRIGHTCTRL_INFO;

#if 0
/*************************************************
  Function:		ui_draw_rightctrl_button
  Description: 	画右边按键
  Input:		
  	1.hWnd		窗体
  	2.rect		区域
  	3.WButObj 	控件结构体		
  Output:		无
  Return:		无
  Others:
*************************************************/
void ui_draw_rightctrl_button(HWND hWnd, RECT * rect, PRIGHTCTRL_INFO WButObj);

/*************************************************
  Function:		draw_rightctrl_bk
  Description: 	画右边背景
  Input:	
  	1.hWnd		窗体
  	2.rect		区域
  	3.WButObj 	控件结构体	
  Output:		无
  Return:		无
  Others:
*************************************************/
void ui_draw_rightctrl_bk(HWND hWnd, RECT * rect, PRIGHTCTRL_INFO WButObj);
#endif
/*************************************************
  Function:		change_rightctrl_allimagetext
  Description: 	
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
void change_rightctrl_allimagetext(HWND hDlg , uint32 * BmpID, uint32 * StrID);

/*************************************************
  Function:		add_rightctrl_win
  Description: 	添加窗口
  Input:		
  	1.Parent	窗体
  	2.hDlg		窗体
  	3.wParam	参数1
  	4.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
void add_rightctrl_win(HWND Parent, int Index, HWND hDlg, int msg, WPARAM wParam, LPARAM lParam);
#endif
