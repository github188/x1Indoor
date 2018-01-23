/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	ui_monitor_app.c
  Author:     	yanjl
  Version:    	2.0
  Date: 		2014-12-05
  Description:  
				获取监视列表界面
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/************************变量定义************************/
static PMONITORLISTINFO   g_Monitorlist;
static DEVICE_TYPE_E   g_devtype = DEVICE_TYPE_NONE;
static HWND g_rightwin = 0;
static HWND g_listwin;
static int8 g_index = 0;
static uint32 g_rightFlag = FALSE;
static uint32 g_tickFlag = FALSE;
static LISTCTRL_INFO g_listCtrl;
static RIGHTCTRL_INFO g_rightCtrl;

/************************函数定义************************/
CREATE_FORM(FrmMonitorlist);

/*************************************************
  Function:		get_dev_textid
  Description: 	获取设备描述字符
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static uint16 get_dev_textid(void)
{
	uint16 TextId;
	if (DEVICE_TYPE_DOOR_NET == g_devtype)
	{
		TextId = SID_MainMonitor_Door;
	}
	else if (DEVICE_TYPE_STAIR == g_devtype)
	{
		TextId = SID_MainMonitor_Stair;
	}
	else if (DEVICE_TYPE_AREA == g_devtype)
	{
		TextId = SID_MainMonitor_Area;
	}
	return TextId;
}

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
	uint32 i,max = 0;
	PLISTITEM_INFO g_info = NULL;
	char devno[20] = {0};
	int nlen = 0;
	g_index = -1;

	if (g_rightFlag)
	{
		SendMessage(g_rightwin, WM_Right_Set_TextID, 1, 0);
	}
	else
	{
		SendMessage(g_rightwin, WM_Right_Set_TextID, 1, SID_Inter_GetList);
	}
	
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	SendMessage(g_listwin, WM_List_Set_HeadText, get_dev_textid(), 0);

	if (g_Monitorlist)
	{
		if (g_Monitorlist->pMonitorInfo)
		{
			free(g_Monitorlist->pMonitorInfo);
			g_Monitorlist->pMonitorInfo = NULL;
		}
		free(g_Monitorlist);
		g_Monitorlist = NULL;
	}
	
	g_Monitorlist = storage_get_monitorlist(g_devtype);	
	if (NULL == g_Monitorlist)
	{
		return;
	}
	
	max = g_Monitorlist->MonitorCount;
	if (max)
	{
		g_info = malloc((sizeof(LISTITEM_INFO))*max);
		if (g_info)
		{
			memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
			for (i = 0; i < max; i++)
			{
				g_info[i].Num = 2;
				g_info[i].Icon = 0;
				g_info[i].Item[0].Change = FALSE;
				g_info[i].Item[0].Image = BID_Inter_ListCamera;
				g_info[i].Item[0].ShowType = STYLE_LEFT;
				g_info[i].Item[1].ShowType = STYLE_LEFT;
				nlen = CHAR_SIZE;
				memset(g_info[i].Item[1].Text,0,sizeof(g_info[i].Item[1].Text));
				fill_devno_by_index(g_Monitorlist->pMonitorInfo[i].DeviceType,g_Monitorlist->pMonitorInfo[i].index, devno);
				get_dev_description(g_Monitorlist->pMonitorInfo[i].DeviceType, devno, g_info[i].Item[1].Text, nlen);
			}
			SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)g_info);
		
			free(g_info);
			g_info = NULL;
		}
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
  Function:		rtsp_state_callbak
  Description: 		 
  Input:		无	
  Output:		无
  Return:			
  Others:			
*************************************************/
void monitorlist_state_callbak(uint32 param1, uint32 param2)
{
	switch (param1)
	{
		case MONITOR_GETLIST:
			{	
				if (TRUE == param2)
				{
					show_win(FrmMonitorlist.hWnd);
				}
				else if (FALSE == param2)
				{
					show_msg(FrmMonitorlist.hWnd, IDC_SEARCH_LIST, MSG_HIT, SID_Inter_SearchFailed);
				}
			}
			break;

		default:
			break;
				
	}	
}

/*************************************************
  Function:		ListViewCommand
  Description: 	列表控件按下的处理函数
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void ListCtrlCommand(HWND hDlg, LPARAM id)
{
	g_index = id;
	g_devtype = g_Monitorlist->pMonitorInfo[g_index].DeviceType;
	log_printf("g_devtype:%d \n",g_devtype);
	close_page();
	// 点击直接开始监视
	ui_monitor_appstart(g_devtype, g_index);
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
static void RightCtrlCommand(HWND hDlg, LPARAM id)
{
	uint32 Image = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, id, 0);
	
	switch(Image)
	{
		case SID_Inter_GetList:
			{
				storage_clear_monitorlist(g_devtype);
				show_win(FrmMonitorlist.hWnd);
				show_msg(FrmMonitorlist.hWnd, IDC_SEARCH_LIST, MSG_HIT, SID_Inter_Search);
				monitorlist_sync_devlist(monitorlist_state_callbak, g_devtype);
			}
			break;
	
		case BID_Right_Exit:
			close_page();
			break;
	}
}

/*************************************************
  Function:		MonitorListCommand
  Description: 	主菜单消息命令消息处理
  Input:		无
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
	memset(&g_listCtrl,0,sizeof(LISTCTRL_INFO));
	g_listCtrl.Parent = hDlg;
	g_listCtrl.Mode = LIST_MODE_LIGHT;				// 抬起释放，没有高亮显示
	g_listCtrl.Caption = 1;							// 是否有标题栏
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.Sidebar = 1;							// 侧边栏
	g_listCtrl.SideBarMode = 1;
	g_listCtrl.KeyCallBack = (SidebarKeyCallBack)get_Sidebar_key;
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;// 侧边栏
	g_listCtrl.fonttype = LIST_CTRL_FONT;				// 文字显示大小
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = get_dev_textid();
	#if (_LCD_DPI_ == _LCD_800480_)
	g_listCtrl.Width[0] = 80;
	g_listCtrl.Width[1] = 220;
	#elif (_LCD_DPI_ == _LCD_1024600_)
	g_listCtrl.Width[0] = 100;
	g_listCtrl.Width[1] = 275;
	#endif
 	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
	 						  WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
							  IDC_LIST_CTRL, 
				 			  LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
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
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;	
	uint32 ImageID[5] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[5] = {0, SID_Inter_GetList, 0, SID_Right_Sure, 0};

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

	Init_Key(hDlg);
}

/*************************************************
  Function:			ui_monitor_list_appstart
  Description: 		启动监视应用
  Input:			
  	1.DevType		监视设备类型
  Output:			无
  Return:			
  Others:		
*************************************************/
void ui_monitor_list_appstart(DEVICE_TYPE_E DevType)
{
	int32 ret = ui_show_win_arbitration(0, SYS_OPER_MONITOR);

	if (ret == TRUE)
	{	
		
		g_devtype = DevType;
		back_main_page();
		form_show(&FrmMonitorlist);
	}
	else
	{
		log_printf("ui_monitor_appstart : sys is busy now!\n");
	}
}

/*************************************************
  Function:		MonitorListDestroyProc
  Description: 	销毁处理函数
  Input:		
  	1.win
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void MonitorListDestroyProc( HWND win , WPARAM wParam, LPARAM lParam )
{
	free_monitorlist_memory(&g_Monitorlist);
	//g_devtype = DEVICE_TYPE_NONE;
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
			return 0;
		
		case MSG_DESTROY:
			MonitorListDestroyProc(hDlg, wParam, lParam);
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

