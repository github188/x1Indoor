/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_volume.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-12-02
  Description:  
  				音量设置界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

#if (_LCD_DPI_ == _LCD_800480_)
#define TOP_TEXT_H				52	
#define TOP_TEXT_XPOS			40	
#define TOP_TEXT_YPOS			((TOP_TEXT_H-FONT_16)/2)	
#define DIS_BMP					16
#define CENTER_W				210
#define CENTER_H				48
#define TOPBK5_H				84
#define LEFT_BMP_W				30
#define LEFT_BMP_H				30
#define RIGHT_BMP_W				50
#define RIGHT_BMP_H				50
#define BMP_X					150
#define CHOSE_H					32
#define CHOSE_XPOS				40
#define SEL_BMP_H				40
#define SEL_XPOS				480
#define SEL_YPOS				TOP_TEXT_H+(TOPBK5_H-SEL_BMP_H)/2
#define CHAR_LEN				110
#elif (_LCD_DPI_ == _LCD_1024600_)
#define TOP_TEXT_H				65	
#define TOP_TEXT_XPOS			50	
#define TOP_TEXT_YPOS			((TOP_TEXT_H-WIN_COMM_FONT)/2)	
#define DIS_BMP					20
#define CENTER_W				266
#define CENTER_H				58
#define TOPBK5_H				84
#define LEFT_BMP_W				38
#define LEFT_BMP_H				38
#define RIGHT_BMP_W				64
#define RIGHT_BMP_H				64
#define BMP_X					188
#define CHOSE_H					40
#define CHOSE_XPOS				50
#define SEL_BMP_H				50
#define SEL_XPOS				600
#define SEL_YPOS				TOP_TEXT_H+(TOPBK5_H-SEL_BMP_H)/2
#define CHAR_LEN				110
#endif

#if (_UI_STYLE_ == _E81S_UI_STYLE_)
#undef DIS_BMP
#undef CENTER_W
#undef CENTER_H
#undef BMP_X
#undef SEL_XPOS
#define DIS_BMP					6
#define CENTER_W				312
#define CENTER_H				26
#define BMP_X					120
#define SEL_XPOS				580
#endif

CREATE_FORM(FrmSetVolume);

static HWND g_rightwin;
static uint8 g_index[3];
static uint8 g_state = 0;
static uint8 g_itemIndex = 0;
static RIGHTCTRL_INFO g_rightCtrl;

/*************************************************
  Function:		show_win
  Description: 	画桌面
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 show_win(HDC hdc)
{
	uint32 xpos,ypos;
	RECT Rect;

	if (storage_get_language() != ENGLISH)
	{
		SelectFont(hdc, GetFont(WIN_COMM_FONT));
	}
	else
	{
		SelectFont(hdc, GetFont_ABC(WIN_COMM_FONT));
	}
	SetTextColor(hdc, COLOR_BLACK);

	DrawBmpID(hdc, 0, 0, 0, 0,BID_Set_BK);
	Rect.left = TOP_TEXT_XPOS;
	Rect.top = TOP_TEXT_YPOS;
	Rect.right = Rect.left + CHAR_LEN;
	Rect.bottom = Rect.top + WIN_COMM_FONT;
	DrawTextFont(hdc, get_str(SID_Set_User_Volume), -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	// 铃声音量
	DrawBmpID_EX(hdc, 0, TOP_TEXT_H,  LIST_CTRL_W,  TOPBK5_H, BID_ListCtrl_TopBK5_0);
	Rect.left = CHOSE_XPOS;
	Rect.top = TOP_TEXT_H+(TOPBK5_H-WIN_COMM_FONT)/2;
	Rect.right = Rect.left + CHAR_LEN;
	Rect.bottom = Rect.top + WIN_COMM_FONT;
	DrawTextFont(hdc, get_str(SID_Set_volume_ring), -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	xpos = BMP_X;
	ypos = TOP_TEXT_H + (TOPBK5_H - CENTER_H)/2;
	if (1 == g_state)
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - LEFT_BMP_H)/2, LEFT_BMP_W, LEFT_BMP_H, BID_Set_DelVolume_on);
	}
	else
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - LEFT_BMP_H)/2, LEFT_BMP_W, LEFT_BMP_H, BID_Set_DelVolume);
	}

	xpos += DIS_BMP+LEFT_BMP_W;
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Inter_Volume0_1+g_index[0]);
	#else
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Inter_Volume0+g_index[0]);
	#endif

	xpos += DIS_BMP+CENTER_W;	
	if (2 == g_state)
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - RIGHT_BMP_H)/2, RIGHT_BMP_W, RIGHT_BMP_H, BID_Set_AddVolume_on);
	}
	else
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - RIGHT_BMP_H)/2, RIGHT_BMP_W, RIGHT_BMP_H, BID_Set_AddVolume);
	}

	// 通话音量
	DrawBmpID_EX(hdc, 0, TOP_TEXT_H+TOPBK5_H, LIST_CTRL_W, TOPBK5_H, BID_ListCtrl_BK5_0);
	Rect.left = CHOSE_XPOS;
	Rect.top = TOP_TEXT_H+TOPBK5_H+(TOPBK5_H-WIN_COMM_FONT)/2;
	Rect.right = Rect.left + CHAR_LEN;
	Rect.bottom= Rect.top + WIN_COMM_FONT;
	DrawTextFont(hdc, get_str(SID_Set_volume_talk), -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	xpos = BMP_X;
	ypos = TOP_TEXT_H+(TOPBK5_H-CENTER_H)/2 + TOPBK5_H;
	if (3 == g_state)
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - LEFT_BMP_H)/2 + TOPBK5_H, LEFT_BMP_W, LEFT_BMP_H, BID_Set_DelVolume_on);
	}
	else
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - LEFT_BMP_H)/2 + TOPBK5_H, LEFT_BMP_W, LEFT_BMP_H, BID_Set_DelVolume);
	}

	xpos += DIS_BMP+LEFT_BMP_W;

	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Inter_Volume0_1+g_index[1]);
	#else
	DrawBmpID(hdc, xpos, ypos, 0, 0, BID_Inter_Volume0+g_index[1]);
	#endif

	xpos += DIS_BMP+CENTER_W;	
	if (4 == g_state)
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - RIGHT_BMP_H)/2 + TOPBK5_H, RIGHT_BMP_W, RIGHT_BMP_H, BID_Set_AddVolume_on);
	}
	else
	{
		DrawBmpID(hdc, xpos, TOP_TEXT_H + (TOPBK5_H - RIGHT_BMP_H)/2 + TOPBK5_H, RIGHT_BMP_W, RIGHT_BMP_H, BID_Set_AddVolume);
	}

	// 按键音
	DrawBmpID_EX(hdc, 0, TOP_TEXT_H+TOPBK5_H*2, LIST_CTRL_W, TOPBK5_H, BID_ListCtrl_BK5_0);
	if (g_index[2])
	{
		DrawBmpID(hdc, CHOSE_XPOS, TOP_TEXT_H+TOPBK5_H*2+(TOPBK5_H-CHOSE_H)/2, 0, 0, BID_Com_Chose);
	}
	else
	{
		DrawBmpID(hdc, CHOSE_XPOS, TOP_TEXT_H+TOPBK5_H*2+(TOPBK5_H-CHOSE_H)/2, 0, 0, BID_Com_UnChose);
	}

	Rect.left = CHOSE_XPOS+CHOSE_H+10;
	Rect.top = TOP_TEXT_H+TOPBK5_H*2+(TOPBK5_H-WIN_COMM_FONT)/2;
	Rect.right = Rect.left + CHAR_LEN;
	Rect.bottom = Rect.top + WIN_COMM_FONT;
	DrawTextFont(hdc, get_str(SID_Set_volume_key), -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	if (g_itemIndex == 0)
	{
		DrawBmpID(hdc, SEL_XPOS, SEL_YPOS, 0, 0, BID_Set_Sel);
	}
	else if (g_itemIndex == 1)
	{
		DrawBmpID(hdc, SEL_XPOS, TOPBK5_H+SEL_YPOS, 0, 0, BID_Set_Sel);
	}
	else
	{
		DrawBmpID(hdc, SEL_XPOS, 2*TOPBK5_H+SEL_YPOS, 0, 0,BID_Set_Sel);
	}
	
	return TRUE;
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
		case SID_Right_Save:
			storage_set_volume(g_index[0], g_index[1], g_index[2]);
			show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
			break;

		case BID_Right_Table:
			log_printf("RightCtrlCommand index = %d\n",g_itemIndex);
			if (g_itemIndex == 2)
			{
				g_itemIndex = 0;
			}
			else
			{
				g_itemIndex++;
			}
			g_state = 0;
			InvalidateRect(hDlg, NULL, FALSE);
			break;

		case BID_Right_Left:
			if (g_itemIndex == 0)
			{	
				g_state = 1;
				InvalidateRect(hDlg, NULL, FALSE);
				if (g_index[0] > MIN_RING_VOLUME)
				{
					g_index[0]--;
					log_printf("RightCtrlCommand index = %d\n",g_index[0]);
					sys_start_play_audio(SYS_MEDIA_MUSIC, CALL_RING1_PATH, FALSE, g_index[0], NULL, NULL);
				}
			}
			else if (g_itemIndex == 1)
			{
				g_state = 3;
				InvalidateRect(hDlg, NULL, FALSE);
				if (g_index[1] > MIN_TALK_VOLUME)
				{
					g_index[1]--;
					log_printf("RightCtrlCommand index = %d\n",g_index[1]);
				}
			}
			else
			{
				if (g_index[2])
				{
					g_index[2] = 0;
				}
				else
				{
					g_index[2] = 1;
				}	
			}
			InvalidateRect(hDlg, NULL, FALSE);
			break;

		case BID_Right_Right:
			if (g_itemIndex == 0)
			{
				g_state = 2;
				InvalidateRect(hDlg, NULL, FALSE);
				if (g_index[0] < MAX_RING_VOLUME)
				{
					g_index[0]++;
					sys_start_play_audio(SYS_MEDIA_MUSIC, CALL_RING1_PATH, FALSE, g_index[0], NULL, NULL);
				}
			}
			else if (g_itemIndex == 1)
			{
				g_state = 4;
				InvalidateRect(hDlg, NULL, FALSE);
				if (g_index[1] < MAX_TALK_VOLUME)
				{
					g_index[1]++;
				}
			}
			else
			{
				g_state = 5;
				if (g_index[2])
				{
					g_index[2] = 0;;
				}
				else
				{
					g_index[2] = 1;
				}	
			}
			InvalidateRect(hDlg, NULL, FALSE);
			break;
			
		case BID_Right_Exit:
			close_page();
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		VolumeCommand
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
	uint32 ImageID[5] = {BID_Right_Left, BID_Right_Right, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[5] = {0, 0, 0,SID_Right_Save, 0};

	memset(&g_rightCtrl,0,sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = 5;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
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
  Function:		ui_Volume_win
  Description: 	信息信息
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_volume_win(HWND hDlg)
{ 
	g_state = 0;
	g_itemIndex = 0;
    g_index[0] = storage_get_ringvolume();
    g_index[1] = storage_get_talkvolume();	
    g_index[2] = storage_get_keykeep();	
	form_show(&FrmSetVolume);
	
	return TRUE;
}

/*************************************************
  Function:		VolumeDestroyProc
  Description: 	销毁处理函数
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void VolumeDestroyProc( HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	sys_stop_play_audio(SYS_MEDIA_MUSIC);
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
			
		case MSG_TIMER:
			break;	
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return  0;
		
		case MSG_DESTROY:
			VolumeDestroyProc(hDlg , wParam, lParam);
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

