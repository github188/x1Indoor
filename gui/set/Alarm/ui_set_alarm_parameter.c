/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_alarm_parameter.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		14-10-10
  Description:  
  				防区属性设置界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

static HDLG g_rightwin = 0;
static HDLG g_listwin = 0;
static HWND g_upperwin = 0;

static uint8 g_time_param[3] = {0};
static uint32 g_list_order = 0;

static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static LISTCTRL_INFO  g_listctrl;					// 列表控件信息

CREATE_FORM(FrmAlarmPara);

/*************************************************
  Function:		show_win
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win(HWND hDlg)
{
	uint32 i,max = 3;
	PLISTITEM_INFO g_info = NULL;
	uint16 text_id[2] = {SID_Bj_Alarm_Time, SID_Bj_Early_Warning_Time};
	uint16 alarm_id[5] = { SID_Bj_1Minute, SID_Bj_3Minutes, SID_Bj_5Minutes, SID_Bj_10Minutes, SID_Bj_15Minutes};
	uint16 exit_time_id[6] = {SID_Bj_30Seconds, SID_Bj_1Minute, SID_Bj_2Minutes, SID_Bj_3Minutes,SID_Bj_5Minutes};
	uint8 alarm_time = g_time_param[0];
	uint8 exit_alarm_time = g_time_param[1];
	uint8 used_exit_sound = g_time_param[2];
	uint16  image_id[2] = {BID_Com_UnChose, BID_Com_Chose};

	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			if (0 == i)
			{
				g_info[i].Icon = BID_ListIcon;
				g_info[i].Num = 2;
				g_info[i].Item[0].ShowType = STYLE_LEFT;
				g_info[i].Item[0].TextID = text_id[i];	
				g_info[i].Item[1].ShowType = STYLE_CENTER;
				g_info[i].Item[1].TextID = alarm_id[alarm_time];	
			}
			else if (1 == i)
			{
				g_info[i].Icon = BID_ListIcon;
				g_info[i].Num = 2;
				g_info[i].Item[0].ShowType = STYLE_LEFT;
				g_info[i].Item[0].TextID = text_id[i];	
				g_info[i].Item[1].ShowType = STYLE_CENTER;
				g_info[i].Item[1].TextID = exit_time_id[exit_alarm_time];	
			}
			else
			{
				g_info[i].Num = 2;
				g_info[i].Item[0].Image =image_id[used_exit_sound];
				g_info[i].Item[0].ShowType = STYLE_LEFT;
				g_info[i].Item[0].TextID = text_id[i];	
				g_info[i].Item[1].ShowType = STYLE_CENTER;
				g_info[i].Item[1].TextID = SID_Bj_Early_Warning_Voice;
			}
		}
		SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)g_info);
	
		free(g_info);
		g_info = NULL;
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
	//SetBkMode(hdc, BM_TRANSPARENT);
	//SetBrushColor(hdc, PAGE_BK_COLOR);
	//FillBox(hdc, rc.left, rc.top, rc.right, rc.bottom);
	show_win(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		CreateListCtrl
  Description: 	初始化列表控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateListCtrl(HWND hDlg)
{
	g_listctrl.Parent = hDlg;
	g_listctrl.Mode = LIST_MODE_COMMON;				// 抬起释放，没有高亮显示
	g_listctrl.Caption = 1;							// 是否有标题栏
	g_listctrl.Sidebar = 0;							// 侧边栏
	g_listctrl.SideBarMode = 1;
	g_listctrl.Drawcallback = NULL;// 侧边栏
	g_listctrl.fonttype = LIST_CTRL_FONT;				// 文字显示大小
	g_listctrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listctrl.HeadInfo.TextID = SID_Bj_Security_Parameter;
	#if (_LCD_DPI_ == _LCD_800480_)
	g_listctrl.Width[0] = 150;
	g_listctrl.Width[1] = 150;
	#elif (_LCD_DPI_ == _LCD_1024600_)
	g_listctrl.Width[0] = 188;
	g_listctrl.Width[1] = 188;
	#endif

	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listctrl.PageCount = 8;						// 显示个数
	g_listctrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listctrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listctrl.PageCount = 5;						// 显示个数
	g_listctrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listctrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif

	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
				 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listctrl);	
}

/*************************************************
  Function:		Init_Key
  Description: 	
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void Init_Key(HWND hDlg)
{
	add_rightctrl_win(g_rightwin, RIGHT_KEY2, g_listwin, WM_List_Go_NextSel, 0, 0);
	add_rightctrl_win(g_rightwin, RIGHT_KEY3, g_listwin, WM_List_OkKey, 0, 0);
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0,BID_Right_Table ,0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, SID_Right_Save, 0, SID_Right_Sure, 0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
	g_rightCtrl.parent = hDlg;
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

	Init_Key(hDlg);
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.hDlg		窗口句柄
  	2.lPara		控件索引
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 StrID;
	StrID = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, lParam, 0);
	
	switch (StrID)
	{
		case SID_Right_Save:
			storage_set_time_param(g_time_param);
			show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
			break;
			
		case BID_Right_Exit:	
			close_page();
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		ListViewCommand
  Description: 	列表控件按下的处理函数
  Input:		
  	1.hDlg		窗口句柄
  	2.lParam	列表元素索引	
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void ListCtrlCommand(HWND hDlg, LPARAM lParam)
{
	switch(lParam)
	{
		case 0:						// 报警时间
			g_list_order = 0;
			SetAlarmTime(hDlg, 0, g_time_param[0]);
			break;
		case 1:						// 退出预警时间
			g_list_order = 1;
			SetAlarmTime(hDlg, 1, g_time_param[1]);
			break;
		case 2:
			if(1 == g_time_param[2])
			{
				g_time_param[2] = 0;
				set_list_Image(g_listwin, lParam, 0, BID_Com_UnChose, CHECK_IMAGE);
			}
			else
			{
				g_time_param[2] = 1;		
				set_list_Image(g_listwin, lParam, 0, BID_Com_Chose, CHECK_IMAGE);
			}
			return;
	}
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
  	1.hDlg		窗口句柄
  	2.wParam	控件ID
  	3.lParam	参数
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	switch (wParam)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
			break;

		case IDC_LIST_CTRL:
			ListCtrlCommand(hDlg, lParam);
			break;
			
		default:
			if (wParam== FrmAlarmTime.hWnd)
			{
				g_time_param[g_list_order] = *((uint32*)lParam);
				InvalidateRect(hDlg, NULL, true);
			}
			break;
	}
}

/*************************************************
  Function:		SetAlarmPara
  Description: 	安防设置参数主窗口
  Input: 		无
  Output:		
  Return:		
  Others:
*************************************************/
uint32 SetAlarmPara(HWND hDlg)
{
	storage_get_time_param(g_time_param);
	form_show(&FrmAlarmPara);
	g_upperwin = hDlg;

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
	CreateListCtrl(hDlg);
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
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

