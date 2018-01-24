/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_set_devno_dialog.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2014-12-01
  Description:  
				设备编号输入界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "gui_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define	ITEM_LEN				50
#define LIST_ITEM_DISX			40
#define LIST_TOP_H				52
#define DEV_ICON_W				218
#define DEV_ICON_H				42
#define INPUT_CTRL_XPOS			(FORM_W - RIGHT_CTRL_W - DEV_ICON_W)/2
#define INPUT_CTRL_YPOS			(LIST_TOP_H + 30)
#define INPUT_CTRL_WPOS			DEV_ICON_W
#define INPUT_CTRL_HPOS			DEV_ICON_H
#elif (_LCD_DPI_ == _LCD_1024600_)
#define	ITEM_LEN				50
#define LIST_ITEM_DISX			40
#define LIST_TOP_H				52
#define DEV_ICON_W				218
#define DEV_ICON_H				42
#define INPUT_CTRL_XPOS			(FORM_W - RIGHT_CTRL_W - DEV_ICON_W)/2
#define INPUT_CTRL_YPOS			(LIST_TOP_H + 30)
#define INPUT_CTRL_WPOS			DEV_ICON_W
#define INPUT_CTRL_HPOS			DEV_ICON_H
#endif

/************************变量定义************************/
static INPUT_INFO g_input;
static KEYBOARD_INFO g_keyboard;
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_RightWin = 0;
static HWND g_InputWin = 0;
static HWND g_KeybordWin = 0;
static HWND g_ParenthDlg = 0;
static uint32 g_PageTextID = 0;
static uint8 g_selall = 0;
static PFULL_DEVICE_NO g_devparam = NULL;

/************************函数定义************************/
CREATE_FORM(FrmDevNoDialog);

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
	RECT TextRc = {0,0,0,0};

	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
	
	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(WIN_COMM_FONT));
	TextRc.left = LIST_ITEM_DISX;
	TextRc.top = (LIST_TOP_H - WIN_COMM_FONT)/2;
	TextRc.right =TextRc.left + ITEM_LEN;
	TextRc.bottom = TextRc.top + WIN_COMM_FONT;
	DrawText(hdc, get_str(g_PageTextID), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawText(hdc, get_str(g_PageTextID), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
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
	EndPaint(hDlg, hdc);
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
	char tmp[50];
	uint8 nlen = 0;
	uint32 text = 0;
	uint32 image = SendMessage(g_RightWin, WM_Right_Get_ImageOrTextID, lParam, 0);
	switch(image)
	{
	    case SID_Right_Sure:
			memset(tmp,0,sizeof(tmp));
			SendMessage(g_InputWin, WM_Input_Get_Text, 0, (LPARAM)tmp);
			nlen = strlen(tmp);
			text = atoi(tmp);
			switch (g_PageTextID)
			{
				case SID_Set_Dev_Stair_len:
					if (nlen > 0)
					{
						g_devparam->Rule.StairNoLen = text;
					}
					else
					{
						g_devparam->Rule.StairNoLen = 0;
					}	
					break;

				case SID_Set_Dev_Room_len:	
					if (nlen > 0)
					{
						g_devparam->Rule.RoomNoLen = text;
					}
					else
					{
						g_devparam->Rule.RoomNoLen = 0;
					}
					break;
					
				case SID_Set_Dev_Cell_len:	
					if (nlen > 0)
					{
						g_devparam->Rule.CellNoLen = text;
					}
					else
					{
						g_devparam->Rule.CellNoLen = 0;
					}
					break;

				case SID_Set_Prj_DevNo:
					memset(g_devparam->DeviceNoStr,0,sizeof(g_devparam->DeviceNoStr));
					if (nlen > 0)
					{
						memcpy(g_devparam->DeviceNoStr,tmp,nlen);
					}
					break;
			}
			SendMessage(g_ParenthDlg, WM_RELOAD, 0, 0);
			close_page();
			break;
			
	    case BID_Right_Exit:
			close_page();
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
  Description: 	关联按键
  Input:		无
  Output:		无
  Return:		无
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
  Description: 	创建右边控件
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
  Description: 	创建输入框控件
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
	g_input.BkID = BID_InputButton_num;
	g_input.BkOnID = BID_InputButton_num;
	g_input.showtype = 1;
	#if 0
	switch (g_PageTextID)
	{
		case SID_Set_Dev_Stair_len:
			g_input.maxlen = 1;
			g_input.showlen = 1;	
			sprintf(g_input.text,"%d",g_devparam->Rule.StairNoLen);
			break;
			
		case SID_Set_Dev_Room_len:	
			g_input.maxlen = 1;
			g_input.showlen = 1;	
			sprintf(g_input.text,"%d",g_devparam->Rule.RoomNoLen);
			break;
			
		case SID_Set_Dev_Cell_len:	
			g_input.maxlen = 1;
			g_input.showlen = 1;	
			sprintf(g_input.text,"%d",g_devparam->Rule.CellNoLen);
			break;

		case SID_Set_Prj_DevNo:
			g_input.maxlen = g_devparam->DevNoLen;
			g_input.showlen = g_devparam->DevNoLen;	
			sprintf(g_input.text,"%s",g_devparam->DeviceNoStr);
			break;
	}
	#endif
	g_InputWin = CreateWindow(AU_INPUT_CTRL, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_INPUT_CTRL, 
				INPUT_CTRL_XPOS, INPUT_CTRL_YPOS,
				INPUT_CTRL_WPOS, INPUT_CTRL_HPOS, 
				hDlg, (DWORD)&g_input);	
}

/*************************************************
  Function:		CreateKeyBord
  Description: 	创建键盘控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateKeyBordCtrl(HWND hDlg)
{
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
  Function:		ui_DevNoDialog_win
  Description: 	设置网络主界面
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_DevNoDialog_win(HWND hDlg, uint32 textid, PFULL_DEVICE_NO devtype)
{
	g_PageTextID = textid;
	g_devparam = devtype;
	g_ParenthDlg = hDlg;
	if (NULL == g_devparam)
	{
		return FALSE;
	}
	g_selall = 0;
	if (SID_Set_Prj_DevNo == g_PageTextID)
	{
		g_selall = 1;
	}
	form_show(&FrmDevNoDialog);
	
	return TRUE;
}

/*************************************************
  Function:		DevNoDialogDestroyProc
  Description: 	销毁处理函数
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void DevNoDialogDestroyProc(void)
{
	g_PageTextID = 0;
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
				char text[30];
				uint8 maxlen = 0;
				switch (g_PageTextID)
				{
					case SID_Set_Dev_Stair_len:
						maxlen = 1;
						sprintf(text,"%d",g_devparam->Rule.StairNoLen);
						SendMessage(g_InputWin, WM_Input_Set_Textlen, 0, maxlen);
						SendMessage(g_InputWin, WM_Input_Set_Text, 0, (LPARAM)text);
						break;
						
					case SID_Set_Dev_Room_len:	
						maxlen = 1;	
						sprintf(text,"%d",g_devparam->Rule.RoomNoLen);
						SendMessage(g_InputWin, WM_Input_Set_Textlen, 0, maxlen);
						SendMessage(g_InputWin, WM_Input_Set_Text, 0, (LPARAM)text);
						break;
						
					case SID_Set_Dev_Cell_len:	
						maxlen = 1;	
						sprintf(text,"%d",g_devparam->Rule.CellNoLen);
						SendMessage(g_InputWin, WM_Input_Set_Textlen, 0, maxlen);
						SendMessage(g_InputWin, WM_Input_Set_Text, 0, (LPARAM)text);
						break;

					case SID_Set_Prj_DevNo:
						maxlen = g_devparam->DevNoLen;
						sprintf(text,"%s",g_devparam->DeviceNoStr);
						SendMessage(g_InputWin, WM_Input_Set_Textlen, 0, maxlen);
						SendMessage(g_InputWin, WM_Input_Set_Text, 0, (LPARAM)text);
						break;
				}
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
			break;
		
		case MSG_DESTROY:
			DevNoDialogDestroyProc();
			break;

		default:
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

