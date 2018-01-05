/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_netparam.c
  Author:   	yanjl
  Version:   	2.0
  Date: 		14-11-27
  Description:  
  				网络参数界面设置
  History:                 
*********************************************************/

#include "gui_include.h"
#include "storage_include.h"

CREATE_FORM(FrmNetParam);
static HWND g_rightwin;
static HWND g_listwin;
static LISTCTRL_INFO g_listCtrl;
static RIGHTCTRL_INFO g_rightCtrl;
static int16 g_index = -1;
static uint32 g_ip[IP_MAX];
static uint32 g_ipType[] =
{
	HOST_IPADDR,		
	HOST_NETMASK,		
	HOST_GATEWAY,		
	CENTER_IPADDR,		
	MANAGER1_IPADDR,	
	#ifdef _MULTI_CENTER_
	MANAGER2_IPADDR,		
	MANAGER3_IPADDR,	
	#endif
	#ifdef _AURINE_REG_
	AURINE_IPADDR,		
	#endif
	#ifdef _USE_ELEVATOR_
	DIANTI_IPADDR,	
	#endif
	#ifdef _LIVEMEDIA_
	RTSP_IPADDR		
	#endif		
};

/*************************************************
  Function:		show_win
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win(HDC hdc)
{	
	uint32 i, max = 0;
	PLISTITEM_INFO g_info = NULL;
	char tmp[40];
	g_index = -1;
	max = (sizeof(g_ipType))/sizeof(uint32);
	
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			g_info[i].Icon = BID_ListIcon;
			g_info[i].Item[0].Change = FALSE;
			g_info[i].Item[0].Image = 0;
			g_info[i].Item[0].ShowType = STYLE_LEFT;

			memset(g_info[i].Item[0].Text,0,sizeof(g_info[i].Item[0].Text));
			strcpy(g_info[i].Item[0].Text, get_str(SID_Set_Prj_Net_HostIP+g_ipType[i]));

			#if 0
			// 当不启用冠林平台时,将原来显示冠林服务器替换显示成冠林电梯控制器
			#ifndef _AURINE_REG_
			if (i == max-1)
			{
				strcpy(g_info[i].Item[0].Text, get_str(SID_Set_Prj_Net_AurineElevator));
			}
			#endif
			#endif
			
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp,"%s",":");
			strcat(g_info[i].Item[0].Text, tmp);
			g_info[i].Item[1].TextID = 0;			
			g_info[i].Item[1].ShowType = STYLE_LEFT;
			change_ip_to_str(g_ip[HOST_IPADDR+g_ipType[i]], g_info[i].Item[1].Text);
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
	SetBkMode(hdc, BM_TRANSPARENT);
	show_win(hdc);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		ListCtrlCommand
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
	ui_netparam_dialog_win(hDlg, (g_ipType[lParam]+HOST_IPADDR), &g_ip[HOST_IPADDR+g_ipType[lParam]], 0);
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
	uint8 i;
	
	for (i = 0; i < IP_MAX; i++)
	{
		storage_set_netparam(0, HOST_IPADDR+i, g_ip[i]);	
	}
	
	// 存入注册表
	storage_set_netparam(1, HOST_IPADDR, g_ip[0]);	
	net_set_local_param(storage_get_netparam());
	show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
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
			save_param(hDlg);
			break;
			
		case BID_Right_Exit:
			close_page();
			break;
	}
}

/*************************************************
  Function:		NetParamCommand
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

		case IDC_LIST_CTRL:
			ListCtrlCommand(hDlg, lParam);
			break;
	}
	
    return TRUE;
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
	#ifdef _USE_ELEVATOR_
	g_listCtrl.Sidebar = 1;							// 侧边栏
	#else
	g_listCtrl.Sidebar = 0;							// 侧边栏
	#endif
	g_listCtrl.SideBarMode = 1;
	g_listCtrl.KeyCallBack = NULL;
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;// 侧边栏
	g_listCtrl.fonttype = Font16X16;				// 文字显示大小
	#ifdef _TFT_7_
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_Prj_Net;
	g_listCtrl.Width[0] = 150;
	g_listCtrl.Width[1] = 200;
	
	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X, LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
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
	for (i = 0; i < g_rightCtrl.MaxNum; i++)
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
  Function:		ui_netparam_win
  Description: 	设置网络参数主界面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_netparam_win(HWND hDlg)
{ 
	uint8 i;
	
	for (i = 0; i < IP_MAX; i++)
	{
		g_ip[i] = storage_get_netparam_bytype(HOST_IPADDR+i);
	}

	form_show(&FrmNetParam);
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
			
		case WM_RELOAD:
			OnPaint(hDlg);
			break;

		case MSG_DESTROY:
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

