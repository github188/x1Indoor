/*********************************************************
  Copyright (C), 2006-2016
  File name:	ui_set_netdoor.c
  Author:   	caogw
  Version:   	1.0
  Date: 		15-06-23
  Description:  
  				设置网络门前机
  History:            
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/************************常量定义************************/
#define MAX_NETDOOR_PAGE_NUM		5		// 网络门前机界面
#define GUI_TIMER_ID            	10

static uint32 g_page1[MAX_NETDOOR_PAGE_NUM][2] = 
{
	{BID_NetDoor_Netparam,			SID_Net_Door_NetParam},       
	{BID_NetDoor_LockType,  		SID_Net_Door_LockType},      
	{BID_NetDoor_Card,				SID_Net_Door_CardMan},     
	{BID_NetDoor_SysInfo,			SID_Net_Door_SysInfo}, 
	{BID_NetDoor_Other,				SID_Net_Door_Other_Set}
};

/************************变量定义************************/
static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static LISTCTRL_INFO g_listCtrl;					// 列表控件信息
static HWND  g_rightwin;
static HWND  g_listwin;
static uint8 g_devIndex = 1;
static uint8 g_netdoorflag = 0;                     //进入网络门前机界面标志位

/************************函数定义************************/
CREATE_FORM(Frmsetnetdoor);

/*************************************************
  Function:		get_page_max
  Description: 	获得当前页的最大数
  Input:		
  	1.page		页面
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint8 get_page_max(uint8 page)
{
	switch (page)
	{
		case 0:
			return MAX_NETDOOR_PAGE_NUM;
			
		default:
			return 0;
	}
}

/*************************************************
  Function:		get_page_iconortext
  Description: 	获得当前页图片文字
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 get_page_iconortext(uint8 page, uint8 index, uint8 mode)
{
	switch (page)
	{
		case 0:
			if (index < MAX_NETDOOR_PAGE_NUM)
			{
				return g_page1[index][mode];
			}
			break;			

		default:
			return 0;
	}
	return 0;
}

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
    if(g_netdoorflag)
    {
        g_netdoorflag = 0;
        KillTimer(self->hWnd, GUI_TIMER_ID);
        SendMessage(Frmsetnetdoor.hWnd, WM_NETDOOR, CMD_NETDOOR_GET_LOCK, 0);
    }
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
	uint32 i, max;
	PLISTITEM_INFO g_info = NULL;
	uint32 TextID[2] = {SID_Net_Door_First, SID_Net_Door_Second};
	
	max = get_page_max(0);
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	if (g_devIndex == 1)
	{
		set_list_text(g_listwin, 0, 0, TextID[0], NULL, NULL, CHECK_HEAD_TEXTID);
	}
	else
	{
		set_list_text(g_listwin, 0, 0, TextID[1], NULL, NULL, CHECK_HEAD_TEXTID);
	}
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Icon = BID_ListIcon;
			g_info[i].Num = 2;
			g_info[i].Item[0].Change = FALSE;
			g_info[i].Item[0].Image = get_page_iconortext(0, i, 0);
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[1].TextID = get_page_iconortext(0, i, 1);			
			g_info[i].Item[1].ShowType = STYLE_LEFT;
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
	show_win(hDlg);
	EndPaint(hDlg, hdc);
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
	uint32 Image = get_list_Image(g_listwin, lParam, 0, CHECK_IMAGE);
	switch (Image)
	{
		case BID_NetDoor_Netparam:
			if (1 == g_devIndex)
			{
				ui_netdoor_netparam_win(hDlg, 1);
			}
			else
			{
				ui_netdoor_netparam_win(hDlg, 2);
			}
			break;

		case BID_NetDoor_LockType:
			ui_netdoor_lock_win(hDlg);
			break;

		case BID_NetDoor_Card:
			ui_netdoor_card_win(hDlg);
			break;

		case BID_NetDoor_SysInfo:
			ui_netdoorsysinfo_win(hDlg);
			break;

		case BID_NetDoor_Other:
			ui_set_netdoor_other_win(hDlg);
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.hDlg		窗口句柄
	2.lParam	列表元素索引
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 Image = 0;
	Image = SendMessage(g_rightwin, WM_Right_Get_Image, lParam, 0);
	
	switch (Image)
	{
		case BID_Right_Exit:
			close_page();
			break;
			
		default:
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

		default:
			break;
	}
}

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		get_key_down
  Description: 	按键按下
  Input: 		
	1.hDlg		句柄
	2.x			X坐标
	3.y			Y坐标
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_key_down(HWND hDlg, int x, int y)
{
}

/*************************************************
  Function:		get_key_up
  Description: 	按键抬起
  Input: 		
	1.hDlg		句柄
	2.x			X坐标
	3.y			Y坐标
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_key_up(HWND hDlg, int x, int y)
{
}
#endif

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
	g_listCtrl.Parent = hDlg;
	g_listCtrl.Mode = LIST_MODE_COMMON;				// 抬起释放，没有高亮显示
	g_listCtrl.Caption = 1;							// 是否有标题栏
	g_listCtrl.SideBarMode = 1;
	g_listCtrl.KeyCallBack = (SidebarKeyCallBack)get_Sidebar_key;
	g_listCtrl.fonttype = Font16X16;				// 文字显示大小
	g_listCtrl.PageCount = 6;						// 显示个数
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.Width[0] = 70;
	g_listCtrl.Width[1] = 200;

	if (1 == g_devIndex)
	{
		g_listCtrl.HeadInfo.TextID = SID_Net_Door_First;
	}
	else
	{
		g_listCtrl.HeadInfo.TextID = SID_Net_Door_Second;
	}
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listCtrl.Sidebar = 0;							// 侧边栏
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.Sidebar = 1;							// 侧边栏
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;	// 侧边栏
	g_listCtrl.PageCount = 5;						// 显示个数
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Sure, 0};

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
  Function:			OnNetDoorDeal
  Description: 		处理网络门前机
  Input: 		
	1.win:   		当前处理的窗口
	2.wParam:   
  Output:		
  Return:		
  Others:
*************************************************/
static uint32 OnNetDoorDeal(HWND hDlg )
{
	//判断网络门前机是否连接
	 show_msg(hDlg, IDC_SEARCH_LIST, MSG_HIT, SID_Inter_Connecting);
	if (NETDOOR_ECHO_SUCESS != logic_netdoor_pub_ini(g_devIndex))
	{
        show_msg(hDlg, IDC_MSG_CTRL, MSG_HIT, SID_Msg_Connect_Outtime);
	}
}

/*************************************************
  Function:		ui_set_netdoor_win
  Description: 	网络门前机界面显示
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_set_netdoor_win(HWND hDlg, HWND doormun)
{ 
    g_netdoorflag = 1;
	g_devIndex = doormun;
	form_show(&Frmsetnetdoor);
	set_netdoorparam_Page(g_devIndex);
	
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

		case MSG_SHOWWINDOW:
			if (wParam == SW_SHOWNORMAL)
            {
            	if (g_netdoorflag)
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
			
		case MSG_ACTIVE:
			break;

		#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
    		get_key_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	        get_key_up(hDlg, LOWORD(lParam), HIWORD(lParam));
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
            on_timer(wParam);
			break;
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return  0;

		case WM_NETDOOR:
			OnNetDoorDeal(hDlg);
			break;

		case MSG_DESTROY:
			if (hDlg)
			{
				KillTimer(hDlg, GUI_TIMER_ID);
			}
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

