/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_language.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-11-28
  Description:  
  				语言界面设置
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

CREATE_FORM(FrmSetLanguage);
static HWND g_rightwin;
static HWND g_listwin;
static LISTCTRL_INFO g_listCtrl;
static RIGHTCTRL_INFO g_rightCtrl;
static LANGUAGE_E g_language = CHINESE;

/*************************************************
  Function:		show_win
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win(HDC hDlg)
{
	uint32 i,max = MAX_LANGUAGE;
	PLISTITEM_INFO g_info = NULL;
	uint32 ImageID[3]={BID_Set_CHINESE,BID_Set_CHBIG,BID_Set_ENGLISH};

	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			g_info[i].Icon = 0;
			g_info[i].Item[0].Change = FALSE;
			if (g_language == CHINESE+i)
			{
				g_info[i].Item[0].Image = BID_Set_Sel;
			}
			else
			{
				g_info[i].Item[0].Image = 0;
			}
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[1].Image = ImageID[i];			
			g_info[i].Item[1].ShowType = STYLE_LEFT;
		}
		SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)g_info);
		SendMessage(g_listwin, WM_List_Set_Sel, g_language, (LPARAM)g_info);
	
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
	SetBkMode(hdc, BM_TRANSPARENT);
	show_win(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		save_param
  Description: 	保存
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void save_param(HWND hDlg)
{
	if (g_language != storage_get_language())
	{
		show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Msg_Recover_OK);
		storage_set_language(g_language);
		ui_msg_reboot();
	}
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Save, 0};
	change_rightctrl_allimagetext(g_rightwin, ImageID, TextID);
}

/*************************************************
  Function:		ListViewCommand
  Description: 	列表控件按下的处理函数
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void ListCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 Image;
	uint32 oldsel;
	
	if (g_language != (CHINESE+lParam) && lParam < MAX_LANGUAGE)
	{
		oldsel = g_language-CHINESE;
		g_language = (CHINESE+lParam);
		Image = BID_Set_Sel;
		set_list_Image(g_listwin, lParam, 0, Image, CHECK_IMAGE);
		Image = 0;
		set_list_Image(g_listwin, oldsel, 0, Image, CHECK_IMAGE);
	}
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
			//save_param(hDlg);
			start_change_rightkey(hDlg);
			show_msg(hDlg, IDC_Set_Back, MSG_QUERY, SID_Set_Language);
			break;
			
		case BID_Right_Exit:
			close_page();
			break;
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
			RightCtrlCommand(hDlg,lParam);
			break;

		case IDC_LIST_CTRL:
			ListCtrlCommand(hDlg,lParam);
			break;
			
		case IDC_Set_Back:
			if (lParam == MSG_EVENT_YES)
			{
				save_param(hDlg);
			}
			break;

		default:
			break;
	}
}

/*************************************************
  Function:		CreateListCtrl
  Description: 	初始化列表控件
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void CreateListCtrl(HWND hDlg)
{
	g_listCtrl.Parent = hDlg;
	g_listCtrl.Mode = LIST_MODE_COMMON;				// 抬起释放，没有高亮显示
	g_listCtrl.Caption = 1;							// 是否有标题栏
	g_listCtrl.SideBarMode = 1;
	g_listCtrl.KeyCallBack = (SidebarKeyCallBack)get_Sidebar_key;
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;	// 侧边栏
	g_listCtrl.fonttype = Font16X16;				// 文字显示大小
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_System_language;
	g_listCtrl.Width[0] = 100;
	g_listCtrl.Width[1] = 200;
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	
 	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
	
	SendMessage(g_listwin, WM_List_SetKeyMode, 1, 0);
}

/*************************************************
  Function:		Init_Key
  Description: 	
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void Init_Key(HWND hDlg)
{
	add_rightctrl_win(g_rightwin, RIGHT_KEY2, g_listwin, WM_List_Go_NextSel, 0, 0);
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
	uint16 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint16 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Save, 0};

	memset(&g_rightCtrl,0,sizeof(RIGHTCTRL_INFO));
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
  Function:		ui_language_win
  Description: 	设置语言界面
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_language_win(HWND hDlg)
{ 
	g_language = storage_get_language();
	log_printf("g_language: %d\n", g_language);
	form_show(&FrmSetLanguage);
	
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

		case WM_Change_RightKey:
			finish_change_rightkey(hDlg);
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

