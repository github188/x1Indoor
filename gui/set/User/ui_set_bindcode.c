/*************************************************
  Copyright (C), 2006-2016
  File name:  	ui_set_bindcode.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2015-07-15
  Description:  
				密码界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "gui_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define TOP_TEXT_H				52	
#define TOP_TEXT_XPOS			40	
#define INPUT_ICON_W			102
#define INPUT_ICON_H			42
#elif (_LCD_DPI_ == _LCD_1024600_)
#define TOP_TEXT_H				52	
#define TOP_TEXT_XPOS			40	
#define INPUT_ICON_W			102
#define INPUT_ICON_H			42
#endif
#define TOP_TEXT_YPOS			((TOP_TEXT_H-Font16X16)/2)	
#define CHAR_LEN				200
#define INPUT_ICON_X			(FORM_W - RIGHT_CTRL_W - INPUT_ICON_W)/2
#define INPUT_ICON_Y			(TOP_TEXT_H + 30)

/************************变量定义************************/
static INPUT_INFO g_input;
static KEYBOARD_INFO g_keyboard;
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_RightWin = 0;
static HWND g_InputWin;
static HWND g_KeybordWin = 0;
static char g_bindcode[6];
uint32 * g_code = NULL;

/************************函数定义************************/
CREATE_FORM(FrmBindCode);

/*************************************************
  Function:		draw_bk
  Description: 	画背景
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_bk(HDC hdc)
{
	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
}

/*************************************************
  Function:		draw_string
  Description: 	画背景
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_string(HDC hdc)
{
	RECT TextRc;
	
	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(FONT_16));
	TextRc.left = TOP_TEXT_XPOS;
	TextRc.top = TOP_TEXT_YPOS;
	TextRc.right = TextRc.left + CHAR_LEN;
	TextRc.bottom = TextRc.top + Font16X16;
	DrawText(hdc, get_str(SID_Set_binding_code1), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
}

/*************************************************
  Function:		OnPaint
  Description: 	画控件
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnPaint(HWND hDlg)
{
	HDC hdc;
	RECT rc;
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);
	SetBkMode(hdc, BM_TRANSPARENT);
	draw_bk(hdc);
	draw_string(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		save_param
  Description: 	画桌面
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void save_param(HWND hDlg)
{
	memset(g_bindcode, 0, sizeof(g_bindcode));
	SendMessage(g_InputWin, WM_Input_Get_Text, 0, (LPARAM)g_bindcode);
	if(strlen(g_bindcode) != 6)
	{
		SendMessage(g_InputWin, WM_Input_Clear, 0, 0);
	}
	else
	{
		*g_code = atoi(g_bindcode);
		close_page();
	}
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 image = SendMessage(g_RightWin, WM_Right_Get_ImageOrTextID, lParam, 0);
	switch(image)
	{
	    case BID_Right_Exit:
			close_page();
			break;
			
	    case SID_Right_Sure:
			save_param(hDlg);
			break;
			
		default:
			break;
    }
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
	1.id		返回的控件ID
	2.hWnd		控件句柄
	3.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand(HWND hDlg, WPARAM id, LPARAM lParam)
{
	switch(id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
            break;
			
		default:
			break;
	}
	return;
}

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		get_key_down
  Description: 	按键按下
  Input: 		
	1.hDlg		句柄
	2.x			X坐标
	3.y			Y坐标
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_key_down(HWND hDlg, int x, int y)
{
 
}

/*************************************************
  Function:		get_key_up
  Description: 	按键抬起
  Input: 		
	1.hDlg		句柄
	2.x			X坐标
	3.y			Y坐标
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_key_up(void)
{ 
}
#endif

/*************************************************
  Function:		Init_Key
  Description: 	
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void Init_Key(void)
{
	add_rightctrl_win(g_RightWin, RIGHT_KEY0, g_KeybordWin, WM_Key_Left, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY1, g_KeybordWin, WM_Key_Right, 0, 0);
	add_rightctrl_win(g_RightWin, RIGHT_KEY2, g_KeybordWin, WM_Key_Ok, 0, 0);
}

/*************************************************
  Function:		CreateRightCtrl
  Description: 	初始化右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;
	uint32 ImageID[RIGHT_NUM_MAX] = {BID_Right_Left, BID_Right_Right, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, SID_Right_Input, SID_Right_Sure, 0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
	
	for(i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
	}
	
	g_RightWin = CreateWindow(AU_RIGHT_BUTTON, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y,
				RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);	
	Init_Key();
}

/*************************************************
  Function:		CreateInputCtrl
  Description: 	初始化输入框控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateInputCtrl(HWND hDlg)
{
    memset(&g_input, 0, sizeof(INPUT_INFO));
	g_input.parent = hDlg;
	g_input.mode = INPUT_MODE_COMMON;
	g_input.maxlen = 6;
	g_input.BkID = BID_InputButton_pass3;
	g_input.BkOnID = BID_InputButton_pass2;
	g_input.showtype = 1;
	g_InputWin = CreateWindow(AU_INPUT_CTRL, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_INPUT_CTRL, 
				INPUT_ICON_X, INPUT_ICON_Y, INPUT_ICON_W, INPUT_ICON_H, 
				hDlg, (DWORD)&g_input);	
}

/*************************************************
  Function:		CreateKeyBord
  Description: 	初始化右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateKeyBordCtrl(HWND hDlg)
{
	uint8 i;
	
   	memset(&g_keyboard, 0, sizeof(KEYBOARD_INFO));
    g_keyboard.InputhDlg[0] = g_InputWin;
	g_keyboard.InputNum = 1;
	g_keyboard.Max = KEYBORD_NUM_MAX;
	g_keyboard.Mode = KEYBOARD_MODE0;
	g_KeybordWin = CreateWindow(NUM_KEYBORD, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_KEYBOARD_CTRL, 
				KEYBOARD_CTRL_X, KEYBOARD_CTRL_Y,
				KEYBOARD_CTRL_W, KEYBOARD_CTRL_H, 
				hDlg, (DWORD)&g_keyboard);	
}

/*************************************************
  Function:		create_ctrls
  Description: 	创建控件
  Input: 		
	1.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void create_ctrls(HWND hDlg)	
{	   	
	CreateInputCtrl(hDlg);	
	CreateKeyBordCtrl(hDlg);   
	CreateRightCtrl(hDlg);
}

/*************************************************
  Function:		ui_passin_win
  Description: 		
  Input:			
  	1.win		窗体
  	2.filePath  文件名
  	3.param     
  Output:		无
  Return:			
  Others:		
*************************************************/
uint32 ui_binding_code_win(HWND hDlg, uint32 * code)
{
	g_code = code;
	form_show(&FrmBindCode);
	
	return TRUE;
}
        
/*************************************************
  Function:		WindowProc
  Description: 	消息处理函数
  Input: 		
  	1.hDlg		句柄
  	2.message	消息
  	3.wParam	参数1
  	4.lParam	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static int WindowProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case MSG_CREATE:
			create_ctrls(hDlg);	
			break;
		
		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
			{
				memset(g_bindcode, 0, sizeof(g_bindcode));
				sprintf(g_bindcode, "%d", *g_code);
				SendMessage(g_InputWin, WM_Input_Set_Text, 0, (LPARAM)g_bindcode);
			}
			else if (wParam == SW_HIDE)
			{
				close_page();
			}
			break;
			
		case MSG_PAINT:
			OnPaint(hDlg);
			return 0;
			
       	#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
    		get_key_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	        get_key_up(hDlg, LOWORD(lParam), HIWORD(lParam));
	        break;
        #endif
		
	    case MSG_KEYDOWN:
			SendMessage(g_RightWin, MSG_KEYDOWN, wParam, lParam);
        	break;

		case MSG_KEYUP:
			SendMessage(g_RightWin, MSG_KEYUP, wParam, lParam);
			break;
			
		case MSG_COMMAND:
			OnCommand(hDlg, wParam, lParam);
			break;
			
		case MSG_TIMER:	
			break;
		
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return  0;
		
		case MSG_DESTROY:
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

