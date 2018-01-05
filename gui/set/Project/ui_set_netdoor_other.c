/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_netdoor_other.c
  Author:   	caogw
  Version:   	2.0
  Date: 		15-06-24
  Description:  
  				其它设置界面
  History:                 
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

/************************常量定义************************/
#define GUI_TIMER_ID        10

/************************变量定义************************/
static HWND g_rightwin;
static HWND g_listwin;
static RIGHTCTRL_INFO  g_rightCtrl;
static LISTCTRL_INFO	g_listCtrl;
static uint8 g_value[10];
static uint8 netdoorflag = 0;                       // 进入网络门前机界面标志位

/************************函数定义************************/
CREATE_FORM(Frmnetdoorother);

/*************************************************
  Function:     on_timer
  Description:  定时器执行函数
   Input:       
    1.hDlg      句柄
  Output:       无
  Return:       无
  Others:
*************************************************/
static void on_timer(uint32 wParam)
{
    if (netdoorflag)
    {
        netdoorflag = 0;
        KillTimer(self->hWnd, GUI_TIMER_ID);
        SendMessage(Frmnetdoorother.hWnd, WM_NETDOOR, CMD_GATEWAY_GET_OTHERS, 0);
    }
}

/*********************************************************
  Function:			show_win
  Description:	显示具体信息
  Input: 		无
  Output:        无
  Return: 		ture 成功false 失败
  Others:                 
*********************************************************/
static void show_win(HWND hDlg)
{
	uint32 i, item = 0;
	PLISTITEM_INFO g_info = NULL;
	#ifdef _UPLOAD_PHOTO_
	uint8 max = 3;
	uint16  text_id[3] = {SID_Net_Door_Pic_Up, SID_Net_Door_Remote_Moniter, SID_Net_Door_Video_Fmt};
	#else
	uint8 max = 1;
	uint16  text_id[1] = {SID_Net_Door_Remote_Moniter};
	#endif
	uint16  image_id[2] = {BID_Com_UnChose, BID_Com_Chose};

	SendMessage(g_listwin, WM_List_Clear_Item, 0, NULL);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info, 0, (sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			#ifdef _UPLOAD_PHOTO_
			item = i;
			#else
			item = i+1;
			#endif
			if (g_value[item] > 1)
			{
				g_value[item] = 0;
			}
			g_info[i].Item[0].Image = image_id[g_value[item]];
			g_info[i].Item[1].TextID = text_id[i];

			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[1].ShowType = STYLE_CENTER;
		}
		SendMessage(g_listwin, WM_List_Add_Item, max, g_info);
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
	switch (id)
	{
		case SID_Right_Save:
			SendMessage(Frmnetdoorother.hWnd, WM_NETDOOR, CMD_GATEWAY_SET_OTHERS, 0);
			break;
			
		case BID_Right_Exit:
			close_page();
			break;
	}
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
	uint8 item = 0;
	
	#ifdef _UPLOAD_PHOTO_
	item = lParam;
	#else
	item = lParam+1;
	#endif
	g_value[item] = (g_value[item]+1)%2;

	if (0 == g_value[item])
	{
		set_list_Image(g_listwin, (lParam), 0, BID_Com_UnChose, CHECK_IMAGE);
	}
	else
	{
		set_list_Image(g_listwin, (lParam), 0,BID_Com_Chose, CHECK_IMAGE);
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
		
		default:
			break;
	}
	
	return TRUE;
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
	add_rightctrl_win(g_rightwin, RIGHT_KEY3, g_listwin, WM_List_OkKey, 0, 0);
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
	uint32 ImageID[5] = {0, 0, BID_Right_Table,0, BID_Right_Exit};
	uint32 TextID[5] = {0, SID_Right_Save, 0, SID_Right_Sure, 0};
	
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = 5;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
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
	g_listCtrl.fonttype = Font16X16;				// 文字显示大小
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Net_Door_Other_Set;
	g_listCtrl.Width[0] = 100;
	g_listCtrl.Width[1] = 200;	

	#ifdef _E81S_UI_STYLE_
	g_listCtrl.Sidebar = 0;							// 侧边栏
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.Sidebar = 1;							// 侧边栏
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;	// 侧边栏
	g_listCtrl.PageCount = 6;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	
	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
}

/*************************************************
  Function:		ui_set_netdoor_other_win
  Description: 	其它设置主界面
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_set_netdoor_other_win(HWND hDlg)
{ 
	netdoorflag = 1;
	memset(g_value, 0, sizeof(g_value));
	form_show(&Frmnetdoorother);

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
  Function:			OnNetDoorDeal
  Description: 		处理网络门前机
  Input: 		
	1.win:   		当前处理的窗口
	2.wParam:   
  Output:		
  Return:		
  Others:
*************************************************/
static uint32 OnNetDoorDeal(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint32 ret = 0;
	
	switch (wParam)
	{
		case CMD_GATEWAY_GET_OTHERS:
			show_msg(hDlg, IDC_SEARCH_LIST, MSG_HIT, SID_Inter_Connecting);
			ret = logic_get_netdoor_other_settings(g_value);
			if (NETDOOR_ECHO_SUCESS == ret)
			{
				show_win(hDlg);
			}
			else
			{			
				show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Msg_Connect_Outtime);
			}
			break;
			
		case CMD_GATEWAY_SET_OTHERS:
			#if 0
			// add by luofl 2012-03-17 第三字节为语言,第四字节为视频格式
			if (ENGLISH == storage_get_language())
			{
				g_value[2] = 0xAA;							// 语言
			}
			g_value[3] = 0xAA;								// 视频格式
			#endif
			ret = logic_set_netdoor_other_settings(g_value);
			if (NETDOOR_ECHO_SUCESS == ret)
			{
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
			}
			else
			{
				show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Save_ERR);
			}
			break;
	}
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
			if (wParam == SW_SHOWNORMAL)
            {
            	if (netdoorflag)
            	{
	                // 创建一个间隔为 NETDOOR_TIME*10 MS的定时器
	                SetTimer(hDlg, GUI_TIMER_ID, NETDOOR_TIME);
            	}
            }
			else if (wParam == SW_HIDE)
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
            on_timer(wParam);
			break;	
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return  0;
		
		case MSG_DESTROY:
			break;

		case WM_NETDOOR:
			OnNetDoorDeal(hDlg,wParam,lParam);
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

