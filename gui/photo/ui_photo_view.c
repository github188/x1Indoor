/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_photo_view.c
  Author:   	caogw
  Version:   	2.0
  Date: 		14-12-10
  Description:  
  				照片查看界面
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
#define TEXT_LEFT1			280				
#define TEXT_TOP1			400
#define TEXT_LEFT2			242				
#define TEXT_TOP2			420
#define TEXT_LEN			150
#define PHOTO_PIC_LEFT		0
#define PHOTO_PIC_TOP		0
#define PHOTO_PIC_RIGHT		640
#define PHOTO_PIC_BOTTOM	480
#elif (_LCD_DPI_ == _LCD_1024600_)
#define TEXT_LEFT1			280				
#define TEXT_TOP1			400
#define TEXT_LEFT2			242				
#define TEXT_TOP2			420
#define TEXT_LEN			150
#define PHOTO_PIC_LEFT		0
#define PHOTO_PIC_TOP		0
#define PHOTO_PIC_RIGHT		640
#define PHOTO_PIC_BOTTOM	480
#endif

/************************变量定义************************/
static PPHOTOLIST_INFO g_photolist = NULL;					// 信息列表
static int16 g_index = -1;
static HWND g_RighthDlg = -1;
static HWND g_ParenthDlg;
static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static uint32 g_RightIndex = 0;
static uint8 g_LoadFlag = FALSE;					// 第一次进入页面加载时的Flag

/************************函数定义************************/
CREATE_FORM(FrmPhotoView);

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
	if (g_photolist->Count)
	{	
		DrawBmpID(hdc, 0, 0, 0, 0, BID_Inter_VideoWinBK);
	}
	else
	{
		DrawBmpID(hdc, 0, 0, 0, 0, BID_Snap_Bk);
	}
}

/*************************************************
  Function:		draw_dev_hint
  Description: 	画设备号提示
  Input:
   1.hdc		句柄
  Output:		无
  Return:			
  Others:		
*************************************************/
static void draw_dev_hint(HDC hdc)
{
	if (!g_photolist->Count)
	{
		return;
	}
	
	RECT rc;
	char devtext[50];
	char time[50];
	
	memset(devtext, 0, sizeof(devtext));
	memset(time, 0, sizeof(time));
	
	SetTextColor(hdc, COLOR_GREEN);
	SelectFont(hdc, GetFont(FONT_16));
	get_dev_description(g_photolist->PhotoInfo[g_index].Type, g_photolist->PhotoInfo[g_index].DevNo, devtext, 50);	
	rc.left = TEXT_LEFT1;
	rc.top = TEXT_TOP1;
	rc.right = rc.left + TEXT_LEN;
	rc.bottom = rc.top + Font16X16;
	DrawText(hdc, devtext, -1, &rc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawText(hdc, devtext, -1, &rc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	sprintf(time,"%04d-%02d-%02d %02d:%02d:%02d",g_photolist->PhotoInfo[g_index].Time.year,g_photolist->PhotoInfo[g_index].Time.month,g_photolist->PhotoInfo[g_index].Time.day
		,g_photolist->PhotoInfo[g_index].Time.hour,g_photolist->PhotoInfo[g_index].Time.min,g_photolist->PhotoInfo[g_index].Time.sec);
	rc.left = TEXT_LEFT2;
	rc.top = TEXT_TOP2;
	rc.right = rc.left + TEXT_LEN;
	rc.bottom = rc.top + Font16X16;
	DrawText(hdc, time, -1, &rc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	DrawText(hdc, time, -1, &rc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
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
	SetBrushColor(hdc, PAGE_BK_COLOR);
	draw_bk(hdc);
	draw_dev_hint(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		start_change_rightkey
  Description:  开始改变右边控件的显示内容
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void start_change_rightkey(HWND hDlg)
{
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Sure, 0};
	change_rightctrl_allimagetext(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), ImageID, TextID);
}

/*************************************************
  Function:		finish_change_rightkey
  Description:  结束时改变右边控件的显示内容
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void finish_change_rightkey(HWND hDlg)
{
	uint32 ImageID[RIGHT_NUM_MAX] = { 0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Del, 0};
	change_rightctrl_allimagetext(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), ImageID, TextID);
}

/*************************************************
  Function:		Loadphoto
  Description:  开始加载信息内容
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void Loadphoto(void)
{
	char * data;
	char filename[100];
	if (NULL != g_photolist)
	{
		// 解图片
		memset(filename, 0, sizeof(filename));
		if (g_photolist->Count)
		{
			data = get_photo_path(filename, &g_photolist->PhotoInfo[g_index].Time);
			if (NULL != data)
			{
				media_start_show_pict(data, PHOTO_PIC_LEFT, PHOTO_PIC_TOP, PHOTO_PIC_RIGHT, PHOTO_PIC_BOTTOM);
				data = NULL;              
			}
		}
	}	
	reflash_page();	
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
static void RightCtrlCommand(HWND hDlg, LPARAM index)
{
	uint32 Image;
	
	g_RightIndex = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, index, 0);
	Image = g_RightIndex;
	switch (Image)
	{
		case BID_Right_Up:
			g_index = (g_index+g_photolist->Count-1) % (g_photolist->Count);
			break;

		case BID_Right_Table:	
		case BID_Right_Down:
			if (-1 != g_index && g_index < g_photolist->Count && g_photolist->Count)
			{
				g_index = (g_index+1 )% g_photolist->Count;
				Loadphoto();
			}
			break;
			
		case SID_Right_Del:
			if (-1 != g_index && g_index < g_photolist->Count && g_photolist->Count)
			{
				start_change_rightkey(hDlg);
				show_msg(hDlg, IDC_MSG_CTRL, MSG_QUERY, SID_Bj_Query_Del_Rec_One);
			}
			break;
			
		case BID_Right_Exit:
			SendMessage(g_ParenthDlg, MSG_COMMAND, IDC_FORM, 0);
			close_page();
			break;
	}
}

/*************************************************
  Function:		MsgCtrlCommand
  Description: 	消息框控件的处理函数
  Input:		
  	1.hDlg		窗口句柄
  	2.event		事件消息
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void MsgCtrlCommand(HWND hDlg, LPARAM event)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	if (MSG_EVENT_YES == event)
	{
		switch(g_RightIndex)
		{
			// 删除单条照片
			case SID_Right_Del:
				if (-1 != g_index && g_photolist && g_index < g_photolist->Count)
				{
					ret = storage_del_photo(g_index);
					if (ECHO_STORAGE_OK == ret)
					{
						storage_free_photo_memory(&g_photolist);
						storage_get_photo(&g_photolist);
						if (g_photolist && g_index > (g_photolist->Count-1))
						{
							g_index = g_photolist->Count-1;
						}
						show_msg(hDlg, IDC_Msg_Notice, MSG_RIGHT, SID_Oper_OK);
						if (g_photolist->Count)
						{
							Loadphoto();
						}
						else
						{
							media_stop_show_pict();
							reflash_page();
						}
					}
					else
					{
						show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Oper_ERR);
					}	
				}
				break;
		}
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

		
		case IDC_MSG_CTRL:
			MsgCtrlCommand(hDlg, index);
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
uint32 ui_photo_view(HWND hDlg, uint8 index, PPHOTOLIST_INFO photolist)
{
	g_ParenthDlg = hDlg;		// 保存父窗体句柄
	g_index = index;
	g_photolist = photolist;
	g_LoadFlag = TRUE;
	form_show(&FrmPhotoView);
	
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
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Del, 0};
	
	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
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
			if (g_LoadFlag)
			{
				Loadphoto();
				g_LoadFlag = FALSE;	
			}
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
			switch((int)wParam)
			{
				case TK_CENTER:						
				case TK_MONITOR:					
					media_stop_show_pict();	
					break;
			}
			SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
            break;

		case MSG_COMMAND:
			OnCommand(hDlg, wParam, lParam);
			break;
			
		case WM_Change_RightKey:
			finish_change_rightkey(hDlg);
			break;
			
		case MSG_TIMER:
			break;	

		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			OnDestroy(hDlg);
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

