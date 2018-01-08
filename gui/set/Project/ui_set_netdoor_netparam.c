/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_netdoor_netparam.c
  Author:   	caogw
  Version:   	2.0
  Date: 		15-06-25
  Description:  
  				网络门前参数界面设置
  History:                 
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

/************************常量定义************************/
#define NETDOOR_LOCAL_IP		0x0A640101 			// 10.100.1.1
#define NETDOOR_LOCAL_GATEIP	0x0A6401FE 			// 10.100.1.254
#define GUI_TIMER_ID        	10

/************************变量定义************************/
static HWND g_rightwin;
static HWND g_listwin;
static LISTCTRL_INFO g_listCtrl;
static RIGHTCTRL_INFO g_rightCtrl;
static uint32 nd_ip[3];	
static uint8 HWaddr[20] = {0};
static NET_PARAM g_ndparam;
static FULL_DEVICE_NO g_netdoorno;
static uint8 g_SelectPage;							// 1-网络门前机1, 2-网络门前机2
static uint8 readmacflag = FALSE;					//设置读MAC时退出该界面时恢复原来的IP值标志位						
static uint8 netdoorflag = 0;                      	//进入网络门前机界面标志位
uint32 *nd_ipp[3] = {&g_ndparam.IP, &g_ndparam.SubNet, &g_ndparam.DefaultGateway};	

/************************函数定义************************/
CREATE_FORM(FrmNetdoorNetParam);

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
    if(netdoorflag)
    {
        netdoorflag = 0;
        KillTimer(self->hWnd, GUI_TIMER_ID);
        SendMessage(FrmNetdoorNetParam.hWnd, WM_NETDOOR, CMD_GATEWAY_GET_NET_PARAM, 0);
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
static void show_win(HDC hdc)
{	
	uint32 i,max = 4;
	PLISTITEM_INFO g_info = NULL;
	unsigned short temp[80];
	char tmp[40];
	uint8 enable = 0;
	uint32 nd_ip[3];			
	nd_ip[0] = g_ndparam.IP;
	nd_ip[1] = g_ndparam.SubNet;
	nd_ip[2] = g_ndparam.DefaultGateway;
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
			strcpy(g_info[i].Item[0].Text, get_str(SID_Net_Door_IP+i));
			memset(tmp,0,sizeof(tmp));
			memset(temp,0,sizeof(temp));	
			sprintf(tmp,"%s",":");
			strcat(g_info[i].Item[0].WText, temp);
			g_info[i].Item[1].TextID = 0;			
			g_info[i].Item[1].ShowType = STYLE_LEFT;
			if (i < max-1)
			{
				change_ip_to_str(nd_ip[i], g_info[i].Item[1].Text);
			}
			else if (i == (max -1))
			{
				g_info[i].Icon = 0;
				sprintf(g_info[i].Item[1].Text, "%02X-%02X-%02X-%02X-%02X-%02X",
					HWaddr[0], HWaddr[1], HWaddr[2], HWaddr[3], HWaddr[4], HWaddr[5]);
			}
		}
		SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)g_info);
	
		free(g_info);
		g_info = NULL;
	}
	SendMessage(g_listwin, WM_List_Set_Enable, 3, &enable);
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
	switch(lParam)
	{
		case 0:
		case 1:
		case 2:
		ui_netparam_dialog_win(hDlg, (lParam), nd_ipp[lParam], 1);
		break;

		case 3:
			break;
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
	uint32 Image = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	switch(Image)
	{
		case SID_Right_Save:
			SendMessage(hDlg, WM_NETDOOR, CMD_GATEWAY_SET_NET_PARAM, 0);
			break;
			
		case SID_Net_Door_GetMAC:
			SendMessage(hDlg, WM_NETDOOR, CMD_GATEWAY_GET_MAC, 0);
			break;
			
		case BID_Right_Exit:
			if (readmacflag)
			{
				net_set_local_param(storage_get_netparam());
			}
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
	g_listCtrl.fonttype = Font16X16;				// 文字显示大小
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_Prj_Net;
	g_listCtrl.Width[0] = 150;
	g_listCtrl.Width[1] = 200;
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
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
	uint32 TextID[5] = {SID_Net_Door_GetMAC, SID_Right_Save, 0, SID_Right_Sure, 0};

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
  Function:		ui_netdooorparam_win
  Description: 	设置门前网络参数主界面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_netdoor_netparam_win(HWND hDlg,HWND doornum)
{ 
	g_SelectPage = doornum;
	memset(HWaddr, 0, sizeof(HWaddr));
	memset(&g_ndparam, 0, sizeof(NET_PARAM));
	memcpy(HWaddr, storage_get_mac(DOOR1_MAC+g_SelectPage-1), 6);
	form_show(&FrmNetdoorNetParam);
    netdoorflag = 1;
	readmacflag = FALSE;

	return TRUE;
}

/*************************************************
  Function:		set_default_ip
  Description: 	设置默认IP与网络门前机同一网段
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void set_default_ip(void)
{
	storage_netdoor_set_ipaddr(NETDOOR_LOCAL_IP, DEFAULT_SUBNET, NETDOOR_LOCAL_GATEIP);
	readmacflag = TRUE;
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
static uint32 OnNetDoorDeal(HWND hDlg , WPARAM wParam, LPARAM lParam)
{
	uint8 i;
	uint32 ret = 0;
	PNET_PARAM pnet_param = storage_get_netparam();
	switch (wParam)
	{
		case CMD_GATEWAY_GET_NET_PARAM:
			show_msg(hDlg, IDC_SEARCH_LIST, MSG_HIT, SID_Inter_Connecting);
			memset(&g_ndparam, 0, sizeof(g_ndparam));
			ret = logic_get_netdevice_netparam(&g_ndparam);
			if (NETDOOR_ECHO_SUCESS == ret)
			{
				show_win(hDlg);
			}
			else
			{
				show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Msg_Connect_Outtime);
			}
			break;
			
		case CMD_GATEWAY_SET_NET_PARAM:
		    //设置网络门前机设备编号以及规则
			ret = logic_set_netdoor_devnoinfo(&g_netdoorno, NULL);
            log_printf("logic_set_netdoor_devnoinfo---ret............. %x\n",ret);
			// 将中心机等参数从本机中取重新设置门前机
			memcpy((char*)&g_ndparam+12, (char*)pnet_param+12, sizeof(NET_PARAM)-12);
			if (ENGLISH == storage_get_language())
			{
				g_ndparam.DNS1 = 0xEE;				// 语言,英语
			}
			g_ndparam.DNS2 = 0xCF;					// 视频格式,CIF格式
			
			//设置网络门前机网络参数
			ret |= logic_set_netdoor_netparam(&g_ndparam);
			if (NETDOOR_ECHO_SUCESS == ret)
			{
				net_set_local_param(storage_get_netparam());	
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
			}
			else
			{
				show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Save_ERR);
			}
			storage_set_netdoor_ip(1, (g_SelectPage-1), g_ndparam.IP);
			break;
			
		case CMD_GATEWAY_GET_MAC:
			set_default_ip();
			DelayMs_nops(10);
			ret = logic_get_netdoor_mac(g_SelectPage, HWaddr);
            log_printf("ret: %d, MAC-HWaddr: [%x-%x-%x-%x-%x-%x]  \n", ret, 
                HWaddr[0], HWaddr[1], HWaddr[2], HWaddr[3], HWaddr[4], HWaddr[5]);
			if (NETDOOR_ECHO_SUCESS == ret)
			{				
				memcpy(&g_ndparam, storage_get_netparam(), sizeof(NET_PARAM));		
				show_win(hDlg);
			}
			else
			{	
				show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Msg_Connect_Outtime);
				if (readmacflag)
				{
					net_set_local_param(storage_get_netparam());
					readmacflag = FALSE; 
				}
			}
			break;
	}
}

/*************************************************
  Function:		NdParamReload
  Description: 	重画页面
  Input:		
  	1.win
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 NdParamReload(HWND hDlg)
{
	show_win(hDlg);
	return TRUE;
}

/*************************************************
  Function:		set_netdoorparamfrm_Page
  Description: 	设置显示页面
  Input: 		
  	1.pageIndex	索引页1-网络门前机1, 2-网络门前机2
  Output:		无
  Return:		无
  Others:
*************************************************/
void set_netdoorparam_Page(uint8 pageIndex )
{
	char DevText[20];
	PFULL_DEVICE_NO pdoorno = storage_get_devparam();

	g_SelectPage = pageIndex;
 	g_netdoorno = *pdoorno;
	strncpy(DevText, g_netdoorno.DeviceNoStr, g_netdoorno.DevNoLen - 1);
	switch(g_SelectPage)
	{	
		case 0:
			DevText[g_netdoorno.DevNoLen-1] = '0';
			break;

		case 1:
			DevText[g_netdoorno.DevNoLen-1] = '8';
			break;

		case 2:
			DevText[g_netdoorno.DevNoLen-1] = '9';
			break;
	}
	DevText[g_netdoorno.DevNoLen] = 0;
	strcpy(g_netdoorno.DeviceNoStr, DevText);
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
			if(wParam == SW_SHOWNORMAL)
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
					
		case MSG_ACTIVE:
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
			
		case WM_RELOAD:
			NdParamReload(hDlg);
			break;

		case WM_NETDOOR:
			OnNetDoorDeal(hDlg,wParam,lParam);
			break;

		case MSG_DESTROY:
			break;
			
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

