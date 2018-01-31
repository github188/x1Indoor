/*********************************************************
  Copyright (C), 2009-2012
  File name:	ui_msg_ctrl.c
  Author:   	caogw
  Version:   	2.0
  Date: 		2014-11-18
  Description:  消息控件
  History:            
*********************************************************/
#include "gui_include.h"

/************************常量定义************************/
#define GUI_TIMER_ID			1					// 定时器ID
#define TIMEOUT					2

#if (_LCD_DPI_ == _LCD_800480_)
#define MSG_ICON_W				40
#define MSG_ICON_H				40
#define MSG_ICON_XPOS			(FORM_MSG_W-MSG_ICON_W)/2
#define MSG_ICON_YPOS			(68+(132-MSG_ICON_H)/2)
#define MSG_TEXT_LEFT			0
#define MSG_TEXT_TOP1			9
#define MSG_TEXT_TOP2			26
#define MSG_TEXT_RIGHT			FORM_MSG_W
#define MSG_TEXT_BOTTOM			68

#define QUERY_ICON_XPOS			15
#define QUERY_ICON_YPOS			(68-MSG_ICON_H)/2
#define QUERY_TEXT_LEFT			70
#define QUERY_TEXT_TOP			(68-16)/2
#define QUERY_TEXT_DISX			(FORM_MSG_W-QUERY_TEXT_LEFT)

#define QUERY_BUTTON_W			100
#define QUERY_BUTTON_H			52
#define QUERY_BUTTON_XPOS		((FORM_MSG_W-2*QUERY_BUTTON_W)/3)
#define QUERY_BUTTONON_XPOS		2*QUERY_BUTTON_XPOS + QUERY_BUTTON_W
#define QUERY_BUTTON_YPOS		(68+(132-QUERY_BUTTON_H)/2)

#define QUERY_YES_DISX			QUERY_BUTTON_W
#define QUERY_YES_LEFT1			QUERY_BUTTON_XPOS+42	// 显示中文时的坐标
#define QUERY_YES_TOP1			(68+(132-16)/2)
#define QUERY_NO_LEFT1			QUERY_BUTTONON_XPOS+42
#define QUERY_YES_LEFT2			QUERY_BUTTON_XPOS+42	// 显示英文时的坐标
#define QUERY_YES_TOP2			(68+(132-16)/2)
#define QUERY_NO_LEFT2			QUERY_BUTTONON_XPOS+42

#define HIT_ICON_W				72
#define HIT_ICON_H				72
#define HIT_ICON_XPOS			(FORM_MSG_W-HIT_ICON_W)/2
#define HIT_ICON_YPOS			(FORM_MSG_H-68-HIT_ICON_H)/2
#define HIT_TEXT_LEFT			0
#define HIT_TEXT_TOP			132+(68-16)/2
#define HIT_TEXT_RIGHT			FORM_MSG_W
#define HIT_TEXT_BOTTOM			FORM_MSG_H
#elif (_LCD_DPI_ == _LCD_1024600_)
#define MSG_ICON_W				50
#define MSG_ICON_H				50
#define MSG_ICON_XPOS			(FORM_MSG_W - MSG_ICON_W) / 2
#define MSG_ICON_YPOS			(86 + (167 - MSG_ICON_H) / 2)
#define MSG_TEXT_LEFT			0
#define MSG_TEXT_TOP1			12
#define MSG_TEXT_TOP2			33
#define MSG_TEXT_RIGHT			FORM_MSG_W
#define MSG_TEXT_BOTTOM			85

#define QUERY_ICON_XPOS			19
#define QUERY_ICON_YPOS			(86 - MSG_ICON_H) / 2
#define QUERY_TEXT_LEFT			88
#define QUERY_TEXT_TOP			(88 - 20)/2
#define QUERY_TEXT_DISX			(FORM_MSG_W-QUERY_TEXT_LEFT)

#define QUERY_BUTTON_W			128
#define QUERY_BUTTON_H			66
#define QUERY_BUTTON_XPOS		((FORM_MSG_W - 2 * QUERY_BUTTON_W) / 3)
#define QUERY_BUTTONON_XPOS		2 * QUERY_BUTTON_XPOS + QUERY_BUTTON_W
#define QUERY_BUTTON_YPOS		(86 + (167 - QUERY_BUTTON_H) / 2)

#define QUERY_YES_DISX			QUERY_BUTTON_W
#define QUERY_YES_LEFT1			QUERY_BUTTON_XPOS + 53	// 显示中文时的坐标
#define QUERY_YES_TOP1			(86 + (167 - 20) / 2)
#define QUERY_NO_LEFT1			QUERY_BUTTONON_XPOS + 53
#define QUERY_YES_LEFT2			QUERY_BUTTON_XPOS + 53	// 显示英文时的坐标
#define QUERY_YES_TOP2			(86 + (167 - 20) / 2)
#define QUERY_NO_LEFT2			QUERY_BUTTONON_XPOS + 53

#define HIT_ICON_W				92
#define HIT_ICON_H				92
#define HIT_ICON_XPOS			(FORM_MSG_W - HIT_ICON_W) / 2
#define HIT_ICON_YPOS			(FORM_MSG_H - 86 - HIT_ICON_H) / 2
#define HIT_TEXT_LEFT			0
#define HIT_TEXT_TOP			167 + (86 - 20) / 2
#define HIT_TEXT_RIGHT			FORM_MSG_W
#define HIT_TEXT_BOTTOM			FORM_MSG_H
#endif

#if (_UI_STYLE_ == _E81S_UI_STYLE_)
#if (_LCD_DPI_ == _LCD_800480_)
#define MSG_ICON_Y				44					// 图标Y位置
#define MSG_ICON_X				44					// 图标X位置
#define MSG_TEXT_DIS			30

#define ICON_W					48
#define ICON_H					48
#define HIT_TEXT_XPOS			116
#define FORM_HIT_H				100
#elif (_LCD_DPI_ == _LCD_1024600_)
#define MSG_ICON_Y				55					// 图标Y位置
#define MSG_ICON_X				55					// 图标X位置
#define MSG_TEXT_DIS			38

#define ICON_W					60
#define ICON_H					60
#define HIT_TEXT_XPOS			145
#define FORM_HIT_H				125
#endif
#endif

/************************变量定义************************/
static HWND g_MsgWin = 0;
static MSG_INFO g_pData;
static uint8 g_RebootFlag = 0;

/************************函数定义************************/
CREATE_FORM(FrmMsgCtrl);

/*************************************************
  Function:		ui_reflash
  Description: 	刷新屏幕
  Input:	
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash(HWND hDlg)
{
	InvalidateRect(hDlg, NULL, FALSE);
}

#if (_UI_STYLE_ == _E81S_UI_STYLE_)
/*************************************************
  Function:		draw_aurine_hit
  Description: 	提示报警、新来电、新信息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_aurine_hit(HDC hdc)
{
	RECT Textrc;
	DrawBmpID(hdc, 0, 0, 0, 0, g_pData.MsgBk);

	SetTextColor(hdc, COLOR_WHITE);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
	if (g_pData.TextID > 0)
	{
		if (ENGLISH == storage_get_language())
		{
			Textrc.left = HIT_TEXT_XPOS - 25;
		}
		else
		{
			Textrc.left = HIT_TEXT_XPOS;
		}
		Textrc.top = (FORM_HIT_H - WIN_COMM_FONT)/2;
		Textrc.right = Textrc.left + 313;
		Textrc.bottom = Textrc.top + WIN_COMM_FONT;
		DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_LEFT);
	}
}

/*************************************************
  Function:		draw_aurine_msg
  Description: 	提示消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_aurine_msg(HDC hdc)
{
	RECT Textrc;
	
	DrawBmpID(hdc, 0, 0, 0, 0, g_pData.MsgBk);
	DrawBmpID(hdc, MSG_ICON_X, MSG_ICON_Y, 0, 0, g_pData.MsgIcon);
	
	SetTextColor(hdc, COLOR_WHITE);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
	if (g_pData.TextID > 0)
	{	
		// 单行可容纳25个字符
		if (ENGLISH != storage_get_language())
		{
			if (strlen(get_str(g_pData.TextID)) > 25)
			{
				Textrc.left = MSG_ICON_X + ICON_W + MSG_TEXT_DIS;
                Textrc.top = MSG_ICON_Y + (ICON_H - WIN_COMM_FONT)/2 - 17;
                Textrc.right = (FORM_MSG_W - (MSG_ICON_X + MSG_TEXT_DIS));
                Textrc.bottom = Textrc.top + WIN_COMM_FONT;
                DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_LEFT|DT_WORDBREAK );
			}
			else
			{
				Textrc.left = MSG_ICON_X + ICON_W + MSG_TEXT_DIS;
                Textrc.top = MSG_ICON_Y + (ICON_H - WIN_COMM_FONT)/2;
                Textrc.right = (FORM_MSG_W - (MSG_ICON_X + MSG_TEXT_DIS));
                Textrc.bottom = Textrc.top + WIN_COMM_FONT;
                DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CENTER);            
			}
		}
		else
		{
			Textrc.left = MSG_ICON_X + ICON_W + MSG_TEXT_DIS;
			Textrc.top = MSG_ICON_Y + (ICON_H - WIN_COMM_FONT)/2;
			Textrc.right = MSG_TEXT_RIGHT;
			Textrc.bottom = Textrc.top + WIN_COMM_FONT;
			DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CENTER);
		}
	}
}

/*************************************************
  Function:		draw_aurine_query
  Description: 	提示消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:		e81s确认消息框
*************************************************/
static void draw_aurine_query(HDC hdc)
{	
	RECT Textrc, Textrc1;
	uint32 Image1, Image2;
	
	DrawBmpID(hdc, 0, 0, 0, 0, g_pData.MsgBk);
	DrawBmpID(hdc, MSG_ICON_X, MSG_ICON_Y, 0, 0, g_pData.MsgIcon);
	
	SetTextColor(hdc, COLOR_WHITE);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
	if (g_pData.TextID > 0)
	{
			Textrc.left = QUERY_TEXT_LEFT + 50;
			Textrc.top = QUERY_TEXT_TOP + 33;
			Textrc.right = Textrc.left + QUERY_TEXT_DISX;
			Textrc.bottom = Textrc.top + WIN_COMM_FONT;
			DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CALCRECT);
			DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_LEFT);
	}
	
	Image1 = BID_QueryButton;
	Image2 = BID_QueryButton;
	switch (g_pData.Select)
	{
		case 0:
			Image1 = BID_QueryButtonON;
			break;

		case 1:
			Image2 = BID_QueryButtonON;
			break;

		default:
			break;
	}
	DrawBmpID(hdc, QUERY_BUTTON_XPOS, QUERY_BUTTON_YPOS + 25, 0, 0, Image1);	
    DrawBmpID(hdc, QUERY_BUTTONON_XPOS, QUERY_BUTTON_YPOS + 25, 0, 0, Image2);

	Textrc.left = QUERY_YES_LEFT1;
	Textrc.top = QUERY_YES_TOP1 + 15;
	Textrc.right = Textrc.left + QUERY_YES_DISX;
	Textrc.bottom = Textrc.top + WIN_COMM_FONT;

	Textrc1.left = QUERY_NO_LEFT1;
	Textrc1.top = QUERY_YES_TOP1 + 15;
	Textrc1.right = Textrc1.left + QUERY_YES_DISX;
	Textrc1.bottom = Textrc1.top + WIN_COMM_FONT;

    if(0 == g_pData.Select)
    {
        SetTextColor(hdc, COLOR_BLACK);
    }
    else
    {
        SetTextColor(hdc, COLOR_WHITE);
    }
	DrawTextFont(hdc, get_str(SID_Msg_Yes), -1, &Textrc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawTextFont(hdc, get_str(SID_Msg_Yes), -1, &Textrc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
    if(1 == g_pData.Select)
    {
        SetTextColor(hdc, COLOR_BLACK);
    }
    else
    {
        SetTextColor(hdc, COLOR_WHITE);
    }
	DrawTextFont(hdc, get_str(SID_Msg_No), -1, &Textrc1, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawTextFont(hdc, get_str(SID_Msg_No), -1, &Textrc1, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
#endif

/*************************************************
  Function:		draw_hit
  Description: 	提示报警、新来电、新信息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_hit(HDC hdc)
{
	RECT Textrc;
	
	DrawBmpID(hdc, 0, 0, 0, 0, g_pData.MsgBk);
	DrawBmpID(hdc, HIT_ICON_XPOS, HIT_ICON_YPOS, 0, 0, g_pData.MsgIcon);
	SetTextColor(hdc, COLOR_WHITE);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));

	if (g_pData.TextID > 0)
	{
		Textrc.left = HIT_TEXT_LEFT;
		Textrc.top = HIT_TEXT_TOP;
		Textrc.right = HIT_TEXT_RIGHT;
		Textrc.bottom = HIT_TEXT_BOTTOM;
		DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CENTER);
	}
}

/*************************************************
  Function:		draw_msg
  Description: 	提示消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_msg(HDC hdc)
{
	RECT Textrc;
	
	DrawBmpID(hdc, 0, 0, 0, 0, g_pData.MsgBk);
	DrawBmpID(hdc, MSG_ICON_XPOS, MSG_ICON_YPOS, 0, 0, g_pData.MsgIcon);
	
	SetTextColor(hdc, COLOR_WHITE);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
	if (g_pData.TextID > 0)
	{	
		// 单行可容纳25个字符
		if (ENGLISH != storage_get_language())
		{
			if (strlen(get_str(g_pData.TextID)) > 25)
			{
				Textrc.left = MSG_TEXT_LEFT;
				Textrc.top = MSG_TEXT_TOP1;
				Textrc.right = MSG_TEXT_RIGHT;
				Textrc.bottom = MSG_TEXT_BOTTOM;
				DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_LEFT|DT_WORDBREAK );
			}
			else
			{
				Textrc.left = MSG_TEXT_LEFT;
				Textrc.top = MSG_TEXT_TOP2;
				Textrc.right = MSG_TEXT_RIGHT;
				Textrc.bottom = MSG_TEXT_BOTTOM;
				DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CENTER);
			}
		}
		else
		{
			#if (_LCD_DPI_ == _LCD_800480_)
			if (strlen(get_str(g_pData.TextID)) > 28)
			{
				Textrc.left = MSG_TEXT_LEFT;
				Textrc.top = MSG_TEXT_TOP1;
				Textrc.right = MSG_TEXT_RIGHT;
				Textrc.bottom = MSG_TEXT_BOTTOM;
				DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_LEFT|DT_WORDBREAK );
			}
			else
			#endif
			{
				Textrc.left = MSG_TEXT_LEFT;
				Textrc.top = MSG_TEXT_TOP2;
				Textrc.right = MSG_TEXT_RIGHT;
				Textrc.bottom = MSG_TEXT_BOTTOM;
				DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CENTER);
			}
		}
	}
}

/*************************************************
  Function:		draw_query
  Description: 	提示消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_query(HDC hdc)
{	
	RECT Textrc, Textrc1;
	uint32 Image1, Image2;
	
	DrawBmpID(hdc, 0, 0, 0, 0, g_pData.MsgBk);
	DrawBmpID(hdc, QUERY_ICON_XPOS, QUERY_ICON_YPOS, 0, 0, g_pData.MsgIcon);
	
	SetTextColor(hdc, COLOR_WHITE);
	SelectFont(hdc, GetBoldFont(WIN_COMM_FONT));
	if (g_pData.TextID > 0)
	{
		Textrc.left = QUERY_TEXT_LEFT;
		Textrc.top = QUERY_TEXT_TOP;
		Textrc.right = Textrc.left + QUERY_TEXT_DISX;
		Textrc.bottom = Textrc.top + WIN_COMM_FONT;
		DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_CALCRECT);
		DrawTextFont(hdc, get_str(g_pData.TextID), -1, &Textrc, DT_LEFT);
	}
	
	Image1 = BID_QueryButton;
	Image2 = BID_QueryButton;
	switch (g_pData.Select)
	{
		case 0:
			Image1 = BID_QueryButtonON;
			break;

		case 1:
			Image2 = BID_QueryButtonON;
			break;

		default:
			break;
	}
	DrawBmpID(hdc, QUERY_BUTTON_XPOS, QUERY_BUTTON_YPOS, 0, 0, Image1);
	DrawBmpID(hdc, QUERY_BUTTONON_XPOS, QUERY_BUTTON_YPOS, 0, 0, Image2);
	
	if (ENGLISH != storage_get_language())
	{
		Textrc.left = QUERY_YES_LEFT1;
		Textrc.top = QUERY_YES_TOP1;
		Textrc.right = Textrc.left + QUERY_YES_DISX;
		Textrc.bottom = Textrc.top + WIN_COMM_FONT;

		Textrc1.left = QUERY_NO_LEFT1;
		Textrc1.top = QUERY_YES_TOP1;
		Textrc1.right = Textrc1.left + QUERY_YES_DISX;
		Textrc1.bottom = Textrc1.top + WIN_COMM_FONT;
	}
	else
	{
		Textrc.left = QUERY_YES_LEFT2;
		Textrc.top = QUERY_YES_TOP2;
		Textrc.right = Textrc.left + QUERY_YES_DISX;
		Textrc.bottom = Textrc.top + WIN_COMM_FONT;

		Textrc1.left = QUERY_NO_LEFT2;
		Textrc1.top = QUERY_YES_TOP2;
		Textrc1.right = Textrc1.left + QUERY_YES_DISX;
		Textrc1.bottom = Textrc1.top + WIN_COMM_FONT;
	}
	
	DrawTextFont(hdc, get_str(SID_Msg_Yes), -1, &Textrc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawTextFont(hdc, get_str(SID_Msg_Yes), -1, &Textrc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	DrawTextFont(hdc, get_str(SID_Msg_No), -1, &Textrc1, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawTextFont(hdc, get_str(SID_Msg_No), -1, &Textrc1, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

#if 0
/*************************************************
  Function:		draw_ts
  Description: 	校准提示消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_ts(HDC hdc)
{	
}
/*************************************************
  Function:		draw_new_pass
  Description: 	密码框
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_new_pass(HDC hdc)
{
}

/*************************************************
  Function:		draw_jd
  Description: 	家电地址框
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_jd(HDC hdc)
{
}

/*************************************************
  Function:		draw_jd_control
  Description: 	家电调解
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_jd_control(HDC hdc)
{
}

/*************************************************
  Function:		draw_jd_control
  Description: 	家电调解
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_aurine_jd_control(HDC hdc)
{

}

/*************************************************
  Function:		draw_jd_control
  Description: 	家电调解
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void draw_aurine_jd_window_control(HDC hdc)
{
}

/*************************************************
  Function:		draw_jd_control
  Description: 	家电调解
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
void draw_aurine_jd_mode_control(HDC hdc)
{

}
#endif

/*************************************************
  Function:		ui_msgctrl_paint
  Description: 	按钮画屏消息
  Input:		无 		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_msgctrl_paint(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
	
   	HDC hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rect);
	SetBkMode(hdc, BM_TRANSPARENT);
	switch (g_pData.Mode)
	{
		case MSG_INFORMATION:						// 消息
		case MSG_WARNING:							// 警告
		case MSG_ERROR:								// 错误
		case MSG_RIGHT:
		case MSG_HIT:
			#if (_UI_STYLE_ == _E81S_UI_STYLE_)
			draw_aurine_msg(hdc);
			#else	
			draw_msg(hdc);
			#endif
			break;

		case MSG_QUERY:								// 确认
			#if (_UI_STYLE_ == _E81S_UI_STYLE_)
			draw_aurine_query(hdc);
			#else	
			draw_query(hdc);
			#endif
			break;

		case MSG_CALL_MESSAGE:						// 新来电、新信息、新警情
		case MSG_MESSAGE:					
		case MSG_ALARM:								
			#if (_UI_STYLE_ == _E81S_UI_STYLE_)
			draw_aurine_hit(hdc);
			#else
			draw_hit(hdc);
			#endif
			break;

		case MSG_IPCODE:	
		case MSG_PASS:								// 密码框
			break;

		case MSG_NEW_PASS:							// 密码框
			break;
			
		case MSG_TS:								// 校准提示
			break;

		case MSG_JDADDR:
			break;

		case MSG_JD_TJ:								// 家电调节
			break;

	    case MSG_AURINE_JD_TJ:
	        break;

	    case MSG_AURINE_JD_MODE:
	        break;

		default:
			break;
	}
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		msg_tick_proc
  Description: 	定时
  Input:		
  	1.hDlg      窗口句柄
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void msg_tick_proc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	//KillTimer(hDlg, GUI_TIMER_ID);
	form_close(&FrmMsgCtrl);
	if (g_RebootFlag)
	{
		g_RebootFlag = 0;
		hw_stop_feet_dog();
	}
}

/*************************************************
  Function:		KeyCommand
  Description: 	控件事件处理函数
  Input: 		
	1.id		返回的控件ID
	2.hWnd		控件句柄
	3.hDlg		句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void KeyCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch ((uint32)wParam)
	{
		case RIGHT_SCANCODE_F1:
			break;

		case RIGHT_SCANCODE_F2:
			break;
			
		case RIGHT_SCANCODE_F3:
			sys_key_beep();
			if (g_pData.Mode == MSG_QUERY)
			{
				if (g_pData.Select)
				{
					g_pData.Select = 0;
				}
				else
				{
					g_pData.Select = 1;
				}
				ui_reflash(hDlg);
			}
			SetScreenTimer();
			break;
			
		case RIGHT_SCANCODE_F4:
			sys_key_beep();
			if (g_pData.Mode == MSG_QUERY)
			{
				form_close(&FrmMsgCtrl);
				if (g_pData.Select)
				{
					SendMessage(g_pData.parent, MSG_COMMAND, g_pData.ID, MSG_EVENT_NO);
				}
				else
				{
					SendMessage(g_pData.parent, MSG_COMMAND, g_pData.ID, MSG_EVENT_YES);
				}
				SendMessage(g_pData.parent,	WM_Change_RightKey, g_pData.ID, 0);
			}
			SetScreenTimer();
			break;
		
		case RIGHT_SCANCODE_F5:
			sys_key_beep();
			if (g_pData.Mode == MSG_QUERY)
			{
				SendMessage(g_pData.parent,	WM_Change_RightKey, g_pData.ID, 0);
				form_close(&FrmMsgCtrl);
			}
			SetScreenTimer();
			break;
				
		default:
			break;
	}
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
			g_MsgWin = hDlg;
			break;
		
		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;
			
		case MSG_ACTIVE:
			break;

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
			{
				if (g_pData.Mode != MSG_QUERY)
				{
					if (g_pData.ID != IDC_SEARCH_LIST)
					{
						SetTimer(hDlg, GUI_TIMER_ID, TIMEOUT*50);
					}
				}
			}
			else if (wParam == SW_HIDE)
			{
				if (g_pData.Mode == MSG_QUERY)
				{
					SendMessage(g_pData.parent,	WM_Change_RightKey, g_pData.ID, 0);
				}
				else
				{
					KillTimer(hDlg, GUI_TIMER_ID);
				}
				form_close(&FrmMsgCtrl);
			}
			break;
			
		case MSG_PAINT:
			ui_msgctrl_paint(hDlg, wParam, lParam);
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
        	break;

		case MSG_KEYUP:
			KeyCommand(hDlg, wParam, lParam);
			break;
			
		case MSG_COMMAND:
			break;
			
		case MSG_TIMER:	
			msg_tick_proc(hDlg, wParam, lParam);
			break;

		case WM_MSG_Close:
			//KillTimer(hDlg, GUI_TIMER_ID);
			if (g_pData.Mode == MSG_QUERY)
			{
				SendMessage(g_pData.parent,	WM_Change_RightKey, g_pData.ID, 0);
			}
			form_close(&FrmMsgCtrl);
			break;
		
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			if (hDlg)
			{
				KillTimer(hDlg, GUI_TIMER_ID);
			}
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

/*************************************************
  Function:		ui_msg_win
  Description: 	消息框界面
  Input:			
  	1.parten	父窗口
  	2.style		消息类别
  Output:		无
  Return:		无	
  Others:		
*************************************************/
void ui_msg_win(PMSG_INFO info)
{
	memset(&g_pData, 0, sizeof(MSG_INFO));
	memcpy(&g_pData, info, sizeof(MSG_INFO));
	//form_show(&FrmMsgCtrl);
	SendMessage(FrmMain.hWnd, WM_FORM_SHOW, 0, (PGUI_FORM)&FrmMsgCtrl);

	if (g_pData.ID == IDC_SEARCH_LIST)
	{
		ui_msgctrl_paint(g_MsgWin, 0, 0);
		usleep(500*1000);
		form_close(&FrmMsgCtrl);
	}
}

/*************************************************
  Function:		ui_msg_reboot
  Description: 	消息消失后重启
  Input:		无
  Output:		无
  Return:		无	
  Others:		
*************************************************/
void ui_msg_reboot(void)
{
	g_RebootFlag = 1;
}

