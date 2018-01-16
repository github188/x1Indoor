/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_keyboard_ctrl.c
  Author:   	caogw	
  Version:   	2.0
  Date: 		2014-10-22
  Description:  
                按键控件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define KEYBOARD_BK_X			0
#define KEYBOARD_BK_Y      	    0
#define KEYBOARD_BK_W       	640
#define KEYBOARD_BK_H       	90			   		             		
#define KEYBOARD_NUM_DISX       52             		// 选择键跳跃间距
#define KEYBOARD_SELECT_XPOS    1              		
#define KEYBOARD_SELECT_YPOS    8 
#elif (_LCD_DPI_ == _LCD_1024600_)
#define KEYBOARD_BK_X			0
#define KEYBOARD_BK_Y      	    0
#define KEYBOARD_BK_W       	640
#define KEYBOARD_BK_H       	90			   		             		
#define KEYBOARD_NUM_DISX       52             		// 选择键跳跃间距
#define KEYBOARD_SELECT_XPOS    1              		
#define KEYBOARD_SELECT_YPOS    8 
#endif

#define KEYBOARD_MODE0_BK		BID_KeyBoardBK_Pass
#define KEYBOARD_MODE1_BK		BID_KeyBoardBK_Call
#define KEYBOARD_MODE2_BK		BID_KeyBoardBK_Pass_1

/************************变量定义************************/
static uint8 g_FirstFlag = 1; 						// 是否重新创建了控件
/*************************************************
  Function:		ui_reflash_ctrl
  Description: 	刷新控件
  Input:	
  	1.hDlg		窗口句柄
  	2.wParam 	参数1
   	3.lParam    参数2	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash_key(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hDlg, NULL, TRUE);
}

/*************************************************
  Function:		KeyboardCtrlPaint_mode0
  Description: 	无
  Input:		
    1.hdc		句柄
   	2.Keyval    输入键值	
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 KeyboardCtrlPaint_mode0(HDC hdc, int8 Keyval)
{
	uint32 xpos,ypos;
	
	DrawBmpID(hdc, KEYBOARD_BK_X, KEYBOARD_BK_Y, KEYBOARD_BK_W, KEYBOARD_BK_H, KEYBOARD_MODE0_BK);
	xpos = KEYBOARD_SELECT_XPOS + KEYBOARD_NUM_DISX * Keyval;
	ypos = KEYBOARD_SELECT_YPOS;
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Keyboard_Select);
	
	return TRUE;
}

/*************************************************
  Function:		KeyboardCtrlPaint_mode1
  Description: 	无
  Input:		
    1.hdc		句柄
   	2.Keyval    输入键值 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 KeyboardCtrlPaint_mode1(HDC hdc, int8 Keyval)
{
	uint32 xpos,ypos;
	
	DrawBmpID(hdc, KEYBOARD_BK_X, KEYBOARD_BK_Y, KEYBOARD_BK_W, KEYBOARD_BK_H, KEYBOARD_MODE1_BK);
	xpos = KEYBOARD_SELECT_XPOS + KEYBOARD_NUM_DISX * Keyval;
	ypos = KEYBOARD_SELECT_YPOS;
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Keyboard_Select);
	
	return TRUE;
}

/*************************************************
  Function:		KeyboardCtrlPaint_mode2
  Description: 	无
  Input:		
    1.hdc		句柄
   	2.Keyval    输入键值 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 KeyboardCtrlPaint_mode2(HDC hdc, int8 Keyval)
{
	uint32 xpos,ypos;
	
	DrawBmpID(hdc, KEYBOARD_BK_X, KEYBOARD_BK_Y, KEYBOARD_BK_W, KEYBOARD_BK_H, KEYBOARD_MODE2_BK);
	xpos = KEYBOARD_SELECT_XPOS + KEYBOARD_NUM_DISX * Keyval;
	ypos = KEYBOARD_SELECT_YPOS;
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Keyboard_Select);
	
	return TRUE;
}

/*************************************************
  Function:		KeyboardCtrlPaint
  Description: 	无
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_KeyboardCtrl_paint(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
	PKEYBOARD_INFO WButObj = (PKEYBOARD_INFO)GetWindowAdditionalData(hDlg);
	
   	if (NULL == WButObj)
	{
		log_printf("WButObj is NULL!!!");
		return FALSE;
	}
	
	HDC hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rect);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	
	switch(WButObj->Mode)
	{
		case KEYBOARD_MODE0:
			KeyboardCtrlPaint_mode0(hdc, WButObj->KeyValue);
			break;

		case KEYBOARD_MODE1:
			KeyboardCtrlPaint_mode1(hdc, WButObj->KeyValue);
			break;
			
		case KEYBOARD_MODE2:
			KeyboardCtrlPaint_mode2(hdc, WButObj->KeyValue);
			break;
			
		default:
			break;
	}
    EndPaint(hDlg, hdc);
	
	return TRUE;
}

/*************************************************
  Function:		ui_KeyboardCtrl_left
  Description: 	select键左移
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_KeyboardCtrl_left(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PKEYBOARD_INFO WButObj = (PKEYBOARD_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == WButObj)
	{
		log_printf("WButObj is NULL!!!");
		return FALSE;
	}
	
	if (WButObj->KeyValue == 0)
	{
		WButObj->KeyValue = WButObj->Max;
	}
	else
	{
		WButObj->KeyValue--;
	}
	ui_reflash_key(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:		ui_KeyboardCtrl_right
  Description: 	select键右移
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2   		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_KeyboardCtrl_right(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PKEYBOARD_INFO WButObj = (PKEYBOARD_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == WButObj)
	{
		log_printf("WButObj is NULL!!!");
		return FALSE;
	}
	
	if (WButObj->KeyValue == WButObj->Max)
	{
		WButObj->KeyValue = 0;
	}
	else
	{
		WButObj->KeyValue++;
	}
	ui_reflash_key(hDlg, wParam, lParam);
	 
	return TRUE;
}

/*************************************************
  Function:		ui_KeyboardCtrl_Ok
  Description: 	发送数据给输入框控件
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_KeyboardCtrl_Ok(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint8 old;
	
	static uint8 active_win = 0;
	PKEYBOARD_INFO WButObj = (PKEYBOARD_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == WButObj)
	{
		log_printf("WButObj is NULL!!!");
		return FALSE;
	}

	if (WButObj->IPmode)
	{
		SendMessage(WButObj->InputhDlg[0], WM_Spin_Get_Value, IDC_KEYBOARD_CTRL, WButObj->KeyValue);
		return TRUE;
	}
	
	if (g_FirstFlag)
	{
		active_win = 0;
		g_FirstFlag = 0;
	}
	// 呼叫和密码模式 只有一个活动窗口
	if (WButObj->Mode == KEYBOARD_MODE1 || WButObj->Mode == KEYBOARD_MODE2)
	{
		active_win = 0;
	}
	if (WButObj->InputhDlg[active_win])
	{
		if (WButObj->KeyValue == 10)
		{
			if (WButObj->Mode == KEYBOARD_MODE1)
			{
				SendMessage(WButObj->InputhDlg[active_win], WM_Input_Clear, 0, 0);
			}
			else
			{
				// 两个输入框时TAB进行切换
				if (WButObj->InputNum == 2)
				{
					old = active_win;
					active_win ++;
					if (active_win >= 2)
					{
						active_win = 0;
					}
					SendMessage(WButObj->InputhDlg[old], WM_Input_Set_Active, 0, 1);
					SendMessage(WButObj->InputhDlg[active_win], WM_Input_Set_Active, 0, 0);
				}

				// 两个输入框时TAB进行切换
				if (WButObj->InputNum == 5)
				{
					old = active_win;
					active_win ++;
					if (active_win >= 5)
					{
						active_win = 0;
					}
					SendMessage(WButObj->InputhDlg[old], WM_Input_Set_Active, 0, 1);
					SendMessage(WButObj->InputhDlg[active_win], WM_Input_Set_Active, 0, 0);
				}
			}
		}
		else if (WButObj->KeyValue == 11)
		{
			SendMessage(WButObj->InputhDlg[active_win], WM_Input_del_Num, 0, 0);
		}
		else
		{
			SendMessage(WButObj->InputhDlg[active_win],WM_Input_Get_Value,IDC_KEYBOARD_CTRL, WButObj->KeyValue);
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_KeyboardCtrl_exit
  Description: 	退出时select键回到起始位置
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_KeyboardCtrl_exit(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PKEYBOARD_INFO WButObj = (PKEYBOARD_INFO)GetWindowAdditionalData(hDlg); 
	if (NULL == WButObj)
	{
		log_printf("WButObj is NULL!!!");
		return FALSE;
	}
	if (WButObj)
	{
		WButObj->KeyValue = 0;
	}
	ui_reflash_key(hDlg, wParam, lParam);
	 
	return TRUE;
}

/*************************************************
  Function:		ui_KeyboardCtrl_destroy
  Description: 	无
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_KeyboardCtrl_destroy(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PKEYBOARD_INFO WButObj = (PKEYBOARD_INFO)GetWindowAdditionalData(hDlg);
	if (WButObj)
	{
    	free(WButObj);
		WButObj = NULL;
	}
	
	return TRUE;
}

/*************************************************
  Function:			MyControlProc
  Description: 		模板-消息处理函数
  Input: 		
	1.hwnd			句柄
	2.message		消息类型
	3.wParam		附加数据
	4.lParam		附加数据
  Output:			无
  Return:			无
  Others:
*************************************************/
static int MyControlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_CREATE:
			g_FirstFlag = 1;
        	break;
						
		case MSG_PAINT:
			ui_KeyboardCtrl_paint(hWnd, wParam, lParam);
			break;				
			
		 case WM_Key_Left:
		 	ui_KeyboardCtrl_left(hWnd, wParam, lParam);
			break;
			
		 case WM_Key_Right:
		 	ui_KeyboardCtrl_right(hWnd, wParam, lParam);
			break;
			
   		 case WM_Key_Ok:
		 	ui_KeyboardCtrl_Ok(hWnd, wParam, lParam);
			break;
			
   		 case WM_Key_Exit:
		 	ui_KeyboardCtrl_exit(hWnd, wParam, lParam);
			break;

   		 //case MSG_DESTROY:
		 //	ui_KeyboardCtrl_destroy(hWnd, wParam, lParam);
         // break;

		case MSG_CLOSE:
			return 0;

		case MSG_DESTROY:
			return 0;
	}
	
	return DefaultControlProc (hWnd, message, wParam, lParam);
}

/*************************************************
  Function:		register_num_keybord
  Description: 	注册控件
  Input: 		
	1.void		空	
  Output:		
  Return:		
  Others:
*************************************************/
BOOL register_num_keybord(void)
{
	WNDCLASS MyClass;

	MyClass.spClassName = NUM_KEYBORD;
	MyClass.dwStyle     = WS_NONE;
	MyClass.dwExStyle   = WS_EX_NONE;
	MyClass.hCursor     = GetSystemCursor (IDC_ARROW);
	MyClass.iBkColor    = PAGE_BK_COLOR;
	MyClass.WinProc     = MyControlProc;
	
	return RegisterWindowClass (&MyClass);
}

/*************************************************
  Function:		unregister_num_keybord
  Description: 	取消注册
  Input: 		
	1.void		空	
  Output:		
  Return:		
  Others:
*************************************************/
void unregister_num_keybord(void)
{
	UnregisterWindowClass (NUM_KEYBORD);
}

