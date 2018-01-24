/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_set_netparam_dialog.c
  Author:     	caogw
  Version:    	2.0
  Date: 		2014-11-27
  Description:  
				网络参数界面设置
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "gui_include.h"

#if (_LCD_DPI_ == _LCD_800480_)
#define LIST_ITEM_DISX			40
#define LIST_TOP_H				52
#define SPIN_CTRL_W				320
#define SPIN_CTRL_H				40
#define SPIN_CTRL_X				(FORM_W - RIGHT_CTRL_W - SPIN_CTRL_W)/2
#define SPIN_CTRL_Y				(LIST_TOP_H + 30)
#elif (_LCD_DPI_ == _LCD_1024600_)
#define LIST_ITEM_DISX			40
#define LIST_TOP_H				52
#define SPIN_CTRL_W				320
#define SPIN_CTRL_H				40
#define SPIN_CTRL_X				(FORM_W - RIGHT_CTRL_W - SPIN_CTRL_W)/2
#define SPIN_CTRL_Y				(LIST_TOP_H + 30)
#endif

static SPIN_INFO g_spin;
static KEYBOARD_INFO g_keyboard;
static RIGHTCTRL_INFO g_rightCtrl;
static HWND g_NetWin = 0;
static HWND g_RightWin = 0;
static HWND g_SpinWin = 0;
static HWND g_KeybordWin = 0;
static HWND g_ParenthDlg = 0;

static IP_TYPE g_iptype = HOST_IPADDR;
static uint32 * g_ip;
static uint8 g_dev_type = 0;			// 0:本机 1:网络门前机
//static uint8 g_is_touch = 0;

CREATE_FORM(FrmNetParamDialog);

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
	RECT TextRc = {0, 0, 0, 0};
	
	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(WIN_COMM_FONT));
	TextRc.left = LIST_ITEM_DISX;
	TextRc.top = (LIST_TOP_H - WIN_COMM_FONT)/2;
	TextRc.right = TextRc.left + 63;
	TextRc.bottom = TextRc.top + WIN_COMM_FONT;
	
	if (0 == g_dev_type)
	{
		#if 0
		#ifndef _AURINE_REG_
			#ifdef _USE_ELEVATOR_
			if (g_iptype == DIANTI_IPADDR)
			{
				g_iptype = g_iptype+3;
			}
			#endif
		#endif
		#endif
		DrawText(hdc, get_str(SID_Set_Prj_Net_HostIP + g_iptype), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		DrawText(hdc, get_str(SID_Set_Prj_Net_HostIP + g_iptype), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);	
	}
	else
	{
		DrawText(hdc, get_str(SID_Net_Door_IP + g_iptype), -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		DrawText(hdc, get_str(SID_Net_Door_IP + g_iptype), -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);	
	}

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
	uint8 ip[4];
	uint32 image = SendMessage(g_RightWin, WM_Right_Get_ImageOrTextID, lParam, 0);
	switch(image)
	{
	    case BID_Right_Exit:
			close_page();
			break;
			
	    case SID_Right_Sure:
			ip[0] = SendMessage(g_SpinWin, WM_Spin_Get_IPValue, 0, 0);	
			ip[1] = SendMessage(g_SpinWin, WM_Spin_Get_IPValue, 0, 1);
			ip[2] = SendMessage(g_SpinWin, WM_Spin_Get_IPValue, 0, 2);
			ip[3] = SendMessage(g_SpinWin, WM_Spin_Get_IPValue, 0, 3);
			*g_ip = change_char_to_ip(ip);
			SendMessage(g_ParenthDlg, WM_RELOAD, 0, 0);
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
  Function:		CreateSpinCtrl
  Description: 	初始化右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateSpinCtrl(HWND hDlg)
{
	
    memset(&g_spin, 0, sizeof(SPIN_INFO));
	g_spin.Mode = SPIN_MODE_IP;
	g_spin.ImageBK = BID_Spin_BK;
	g_spin.ImageIpSel = BID_Spin_Sel;
	g_spin.MinNum = 0;
	g_spin.MaxNum = 3;	
	g_spin.selall = 1;	
	change_ip_to_char(*g_ip, g_spin.IP);

	g_SpinWin = CreateWindow(AU_SPIN_CTRL, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_SPIN_CTRL, 
				SPIN_CTRL_X, SPIN_CTRL_Y, SPIN_CTRL_W, SPIN_CTRL_H, 
				hDlg, (DWORD)&g_spin);	
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
   	memset(&g_keyboard, 0, sizeof(KEYBOARD_INFO));
    g_keyboard.InputhDlg[0] = g_SpinWin;
	g_keyboard.InputNum = 1;
	g_keyboard.Max = KEYBORD_NUM_MAX;
	g_keyboard.Mode = KEYBOARD_MODE0;
	g_keyboard.IPmode = 1;
	
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
    g_NetWin = hDlg;
   	CreateSpinCtrl(hDlg);
	CreateKeyBordCtrl(hDlg);
    CreateRightCtrl(hDlg);
}

/*************************************************
  Function:		ui_netparam_dialog_win
  Description: 	设置网络主界面
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_netparam_dialog_win(HWND hDlg, IP_TYPE IPtype, uint32 *IP, uint8 dev_type)
{
	g_iptype = IPtype;
	g_ip = IP;
	g_dev_type = dev_type;
	//g_is_touch = 0;
	g_ParenthDlg = hDlg;
	log_printf("g_iptype = %d \n", g_iptype);
	if (NULL == g_ip)
	{
		return FALSE;
	}
	
	form_show(&FrmNetParamDialog);
	return TRUE;
}

/*************************************************
  Function:		NetParamDialogDestroyProc
  Description: 	销毁处理函数
  Input:		
  	1.win
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 NetParamDialogDestroyProc( HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	g_dev_type = 0;
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
			if (wParam == SW_HIDE)
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
			NetParamDialogDestroyProc(hDlg, wParam, lParam);
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

