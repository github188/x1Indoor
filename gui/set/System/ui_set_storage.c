/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_storage.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-11-28
  Description:  
  				存储界面设置
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

#ifdef _TFT_7_
#define TOP_TEXT_XPOS			40	
#define TEXT_BK_H				52
#define CHAR_LEN 				100
#define TEXT1_XPOS_DIS			520
#define TEXT_BK_YPOS			32
#else
#define TOP_TEXT_XPOS			10	
#define TEXT_BK_H				45
#define CHAR_LEN 				100
#define TEXT1_XPOS_DIS			260
#define TEXT_BK_YPOS			0
#endif

#if (_UI_STYLE_ == _E81S_UI_STYLE_)
#undef TOP_TEXT_XPOS
#undef TEXT_BK_YPOS
#define TOP_TEXT_XPOS			10	
#define TEXT_BK_YPOS			0
#endif

CREATE_FORM(FrmSetstorage);

static HWND g_rightwin;
static RIGHTCTRL_INFO g_rightCtrl;

/*************************************************
  Function:		change_rightkey
  Description:  改变右边控件的显示内容
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void change_rightkey(HWND hDlg)
{
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Sure, 0};
	
	change_rightctrl_allimagetext(g_rightwin, ImageID, TextID);
}

/*************************************************
  Function:		recover_change_rightkey
  Description:  恢复右边控件的显示内容
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void recover_change_rightkey(HWND hDlg)
{
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Storage_Format, 0};
	#else
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Clear, 0};
	#endif
	
	change_rightctrl_allimagetext(g_rightwin, ImageID, TextID);
}

/*************************************************
  Function:		show_win
  Description: 	画桌面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void show_win(HWND hdc)
{
	uint8 i;
	uint32 xpos, ypos;
	RECT TextRc;
	char tmp[30];
	uint32 TextID[3] = {SID_MainInfo, SID_Storage_Pic, SID_MainLyLy};
	uint32 storage[3] = {0, 0, 0};

	if (storage_get_language() != ENGLISH)
	{
		SelectFont(hdc, GetFont(FONT_16));
	}
	else
	{
		SelectFont(hdc, GetFont_ABC(FONT_16));
	}
	SetTextColor(hdc, COLOR_BLACK);

	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
	TextRc.left = TOP_TEXT_XPOS ;
	TextRc.top =  (TEXT_BK_H - Font16X16)/2 ;
	TextRc.right = TextRc.left + CHAR_LEN+100;
	TextRc.bottom = TextRc.top + Font16X16;
	DrawText(hdc, get_str(SID_Set_System_Storage), -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	storage[0] = (storage_get_msg_allsize()/1024);
	storage[1] = (storage_set_photo_size()/1024);
	storage[2] = (storage_set_lylyrecord_size()/1024);
	
	xpos = 0;	
	ypos = TEXT_BK_H;	
	for (i = 0; i < 3; i++)
	{
		if (0 == i)
		{
			#if (_UI_STYLE_ == _E81S_UI_STYLE_)
			DrawBmpID(hdc, xpos, ypos, LIST_CTRL_W, 0, BID_ListCtrl_TopBK8_0);	
			#else
			DrawBmpID(hdc, xpos, ypos, LIST_CTRL_W, 0, BID_ListCtrl_TopBK5_0);	
			#endif
		}
		else
		{
			#if (_UI_STYLE_ == _E81S_UI_STYLE_)
			DrawBmpID(hdc, xpos, ypos, LIST_CTRL_W, 0, BID_ListCtrl_BK8_0);	
			#else
			DrawBmpID(hdc, xpos, ypos, LIST_CTRL_W, 0, BID_ListCtrl_BK5_0);	
			#endif
		}

		TextRc.left = xpos+TOP_TEXT_XPOS+10;
		TextRc.top = ypos + (TEXT_BK_H+TEXT_BK_YPOS - Font16X16)/2;
		TextRc.right =  TextRc.left + CHAR_LEN;
		TextRc.bottom = TextRc.top + Font16X16;
		DrawText(hdc, get_str(TextID[i]), -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		TextRc.left = TEXT1_XPOS_DIS;
		TextRc.top = ypos + (TEXT_BK_H+TEXT_BK_YPOS - Font16X16)/2;
		TextRc.right =  TextRc.left + CHAR_LEN;
		TextRc.bottom = TextRc.top + Font16X16;
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%d KB", storage[i]);
		DrawText(hdc, tmp, -1, &TextRc, DT_RIGHT| DT_VCENTER | DT_SINGLELINE);
		ypos += TEXT_BK_H + TEXT_BK_YPOS;
	}
}

/*************************************************
  Function:		OnPaint
  Description: 	显示窗口
  Input: 		
	1.hDlg		窗口句柄
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
	show_win(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 id = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	
	switch(id)
	{
		#if (_UI_STYLE_ == _E81S_UI_STYLE_)
		case SID_Storage_Format:
			change_rightkey(hDlg);
			show_msg(hDlg, IDC_Msg_Notice, MSG_QUERY, SID_Msg_Format);
			break;
		#else
		case SID_Right_Clear:
			change_rightkey(hDlg);
			show_msg(hDlg, IDC_Msg_Notice, MSG_QUERY, SID_Set_Query_Del_All);
			break;
		#endif
	
		case BID_Right_Exit:
			close_page();
			break;
	}
}

/*************************************************
  Function:		storageCommand
  Description: 	主菜单消息命令消息处理
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 OnCommand(HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
			break;

		case IDC_Msg_Notice:
			if (MSG_EVENT_YES == lParam)
			{
				storage_format_system();
				storage_set_lyly_tip(FALSE);
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Oper_OK);
				sys_sync_hint_state();
				InvalidateRect(hDlg, NULL, FALSE);
			}
			break;
	}
	
    return TRUE;
}

/*************************************************
  Function:		CreateRightCtrl
  Description: 	初始化右边控件
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Storage_Format, 0};
	#else
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Clear, 0};
	#endif

	memset(&g_rightCtrl,0,sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
	for(i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
	}

	g_rightwin = CreateWindow(AU_RIGHT_BUTTON, "", 
			 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
			 IDC_RIGHT_BUTTON, 
			 RIGHT_CTRL_X, RIGHT_CTRL_Y, RIGHT_CTRL_W, RIGHT_CTRL_H, 
			 hDlg, (DWORD)&g_rightCtrl);

}

/*************************************************
  Function:		ui_storage_win
  Description: 	信息信息
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_storage_win(HWND hDlg)
{ 
	form_show(&FrmSetstorage);

	return TRUE;
}

/*************************************************
  Function:		create_ctrls
  Description: 	创建控件
  Input: 		
	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void create_ctrls(HWND hDlg)	
{
	CreateRightCtrl(hDlg);
}

/*************************************************
  Function:		WindowProc
  Description: 	消息处理函数
  Input: 		
  	1.hDlg		窗口句柄
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
			break;

		case MSG_LBUTTONDOWN:
   			break;
   			
	    case MSG_LBUTTONUP:
	        break;

		case MSG_KEYDOWN:
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYDOWN, wParam, lParam);
			break;

		case MSG_KEYUP:
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
            break;

		case MSG_COMMAND:
			OnCommand(hDlg, wParam, lParam);
			break;

		case WM_Change_RightKey:
			recover_change_rightkey(hDlg);
			break;
			
		case MSG_TIMER:
			break;	
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

