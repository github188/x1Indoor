/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_message_view.c
  Author:   	caogw
  Version:   	2.0
  Date: 		15-01-05
  Description:  
  				信息查看界面
  History:     
  	1. Date:	
		   Author:	
		   Modification:
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define CPATION_ITEM_H				52
#if 0
#define MESSAGE_PIC_LEFT			104
#define MESSAGE_PIC_TOP				103
#define MESSAGE_PIC_RIGHT			432
#define MESSAGE_PIC_BOTTOM			324
#endif
#elif (_LCD_DPI_ == _LCD_1024600_)
#define CPATION_ITEM_H				45
#define CPATION_ITEM_H				52
#if 0
#define MESSAGE_PIC_LEFT			104
#define MESSAGE_PIC_TOP				103
#define MESSAGE_PIC_RIGHT			432
#define MESSAGE_PIC_BOTTOM			324
#endif
#endif
#define MESSAGE_PIC_LEFT			0
#define MESSAGE_PIC_TOP				CPATION_ITEM_H
#define MESSAGE_PIC_RIGHT			(FORM_W - RIGHT_CTRL_W)
#define MESSAGE_PIC_BOTTOM			(FORM_H - MESSAGE_PIC_TOP)

/************************变量定义************************/
static PMSGLIST g_pMsgList = NULL;					// 信息列表
static int16 g_index = -1;
static uint8 g_PlaySnd = FALSE;
static HWND g_RighthDlg = -1;
static HWND g_ParenthDlg;
static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息

/************************函数定义************************/
CREATE_FORM(FrmMsgView);

/*************************************************
  Function:		reflash_page
  Description: 	刷新界面
  Input: 		无	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void reflash_page(void)
{
	RECT rc;
	
	rc.left = FORM_X;
	rc.top = FORM_Y;
	rc.right = FORM_W;
	rc.bottom = FORM_H;
	InvalidateRect(self->hWnd, &rc, FALSE);
}

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
	if (g_pMsgList->pinfo_data[g_index].Head.ImageLen > 0)
	{	
		DrawBmpID(hdc, 0, 0, 0, 0, BID_Message_ImageBK1);
	}
	else
	{
		DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
	}
}

/*************************************************
  Function:		draw_text
  Description: 	画信息
  Input:
   1.hdc		句柄
  Output:		无
  Return:			
  Others:		
*************************************************/
static void draw_text(HDC hdc)
{
	RECT rc;
	char * content;
	char text[300], text1[6]="      ";
	
	// 标题
	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(FONT_16));
	rc.left = LEFT_ICON_DIS_X;
	rc.top = (CPATION_ITEM_H - Font16X16)/2;
	rc.right = rc.left + TOP_TEXT_SIZE;
	rc.bottom = rc.top + Font16X16;
	if (0xff == g_pMsgList->pinfo_data[g_index].Head.title[0])
	{
		DrawText(hdc, get_str(SID_Info_Sms), -1, &rc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}
	else
	{
		sprintf(text, "%s", g_pMsgList->pinfo_data[g_index].Head.title);
		DrawText(hdc, text, -1, &rc,DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		DrawText(hdc, text, -1, &rc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}

	// 文字
	if (g_pMsgList->pinfo_data[g_index].Head.CharLen > 0)
	{
		content = storage_read_msg_data(&g_pMsgList->pinfo_data[g_index], g_index);
		if (NULL == content)
		{
			log_printf("NULL == content \n");
			return;
		}

		SetTextColor(hdc, COLOR_BLACK);
		SelectFont(hdc, GetFont(FONT_20));
		memset(text, 0, sizeof(text));
		sprintf(text, "%s%s",text1,content);
		rc.left = TEXT_X + 23;
		rc.top = (CPATION_ITEM_H + 20);
		rc.right = FORM_W - RIGHT_CTRL_W - rc.left;
		rc.bottom = rc.top + 9*FONT_20;
		DrawText(hdc, text, -1, &rc, DT_LEFT);
		
		free(content);
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
	
	if (NULL == g_pMsgList || -1 == g_index || g_index > (g_pMsgList->ncount-1))
	{
		return;
	}
	
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetBrushColor(hdc, PAGE_BK_COLOR);
	draw_bk(hdc);
	draw_text(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		msg_sound_callback
  Description:  信息播放声音回调
  Input:		
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void msg_sound_callback(int32 param1,int32 param2,int32 state)
{
	if (FALSE == state)
	{
		g_PlaySnd = FALSE;
		return;
	}
	if (100 == param2)
	{
		g_PlaySnd = FALSE;
	}
}

/*************************************************
  Function:		LoadMsgContene
  Description:  开始加载信息内容
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void LoadMsgContene(void)
{
	char * data;
	char path[50];
	
	if (NULL != g_pMsgList)
	{	
		// 解图片
		if (g_pMsgList->pinfo_data[g_index].Head.ImageLen > 0)
		{
			data = storage_read_msg_data(&g_pMsgList->pinfo_data[g_index], g_index);
			if (NULL != data)
			{
				media_start_show_pict(data, MESSAGE_PIC_LEFT, MESSAGE_PIC_TOP, MESSAGE_PIC_RIGHT, MESSAGE_PIC_BOTTOM);
				free(data);                 
			}
		}
		// 播放声音
		if (g_pMsgList->pinfo_data[g_index].Head.VoiceLen > 0)
		{
			storage_read_msg_voice_path(&g_pMsgList->pinfo_data[g_index], (uint8 *)path);
			sys_start_play_audio(SYS_MEDIA_MUSIC, path, FALSE, MSG_VIEW_VOL,(MediaPlayCallback)msg_sound_callback, NULL);
			// 把纯声音信息未读设置为已读
			if (1 == g_pMsgList->pinfo_data[g_index].is_unread)
			{
				storage_set_msg_readed(g_index);
			}
			g_PlaySnd = TRUE;
		}
		else
		{
			g_PlaySnd = FALSE;
		}	
		sys_sync_hint_state();
	}	
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.win
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM index)
{
	uint32 Image;
	if (NULL == g_pMsgList || 0 == g_pMsgList->ncount)
	{
		return;
	}
	
	if (TRUE == g_PlaySnd)
	{
		g_PlaySnd = FALSE;
		sys_stop_play_audio(SYS_MEDIA_MUSIC);
	}
	
	Image = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_Image, index, 0);
	switch (Image)
	{
		case BID_Right_Table:	
			g_index = (g_index+1 )% g_pMsgList->ncount;
			if (g_pMsgList->pinfo_data[g_index].Head.ImageLen == 0)
			{
				// 关framebuff
				media_stop_show_pict();
			}
			reflash_page();	
			break;
			
		case BID_Right_Exit:
			SendMessage(g_ParenthDlg, MSG_COMMAND, IDC_FORM, 0);
			close_page();
			break;
	}
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
	1.id		返回的控件ID
	2.index		控件索引
	3.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand( HWND hDlg, WPARAM id, LPARAM index)
{	
	switch (id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, index);
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		ui_msg_view
  Description: 	信息查看主界面
  Input:		
  
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_msg_view(HWND hDlg, uint8 index, PMSGLIST pmsg_list)
{
	g_ParenthDlg = hDlg;		// 保存父窗体句柄
	g_index = index;
	g_pMsgList = pmsg_list;
	form_show(&FrmMsgView);
	
	return TRUE;
}

/*************************************************
  Function:		OnDestroy
  Description: 	销毁处理函数
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 OnDestroy(HWND hDlg)
{
	g_index = -1;
	if (TRUE == g_PlaySnd)
	{
		sys_stop_play_audio(SYS_MEDIA_MUSIC);
		g_PlaySnd = FALSE;
	}
	media_stop_show_pict();

	return TRUE;
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, 0, 0};
	
	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
	g_rightCtrl.parent = hDlg;
	for (i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
	}
	g_RighthDlg = CreateWindow(AU_RIGHT_BUTTON, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y, RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);
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

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		get_touch_down
  Description: 	触摸屏按下
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int get_touch_down(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/*************************************************
  Function:		get_touch_up
  Description: 	触摸屏抬起
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int get_touch_up(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
#endif

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
			LoadMsgContene();
			break;

		#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
			if (GetCapture() == hDlg)
			{
				break;
			}
			SetCapture(hDlg);
    		get_touch_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	    	if (GetCapture() != hDlg)
			{
				break;
			}
			ReleaseCapture();
	        get_touch_up(hDlg, LOWORD(lParam), HIWORD(lParam));
	        break;
		#endif

		case MSG_KEYDOWN:
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYDOWN, wParam, lParam);
        	break;

		case MSG_KEYUP:
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
			break;
			
		case MSG_COMMAND:
			OnCommand(hDlg, wParam, lParam);
			break;

		case MSG_TIMER:
			break;
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			OnDestroy(hDlg);
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

