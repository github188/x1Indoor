/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_netdoor_sysinfo.c
  Author:   	caogw
  Version:   	1.0
  Date: 		2015-06-25
  Description:  
  				门前机系统信息界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

/************************常量定义************************/
#if (_LCD_DPI_ == _LCD_800480_)
#define TOP_TEXT_H				52	
#define TOP_TEXT_XPOS			40	
#define TOP_TEXT_YPOS			((TOP_TEXT_H - FONT_20)/2)	
#define SYS_TEXT_XPOS			50	
#define SYS_TEXT_YPOS			(TOP_TEXT_H + 20)	
#define TEXT_YPOS_DIS			(10 + FONT_16)
#define TEXT_XPOS_DIS			200
#define CHAR_LEN				220
#define MAX_ITEM				SYS_INFO_MAX			
#elif (_LCD_DPI_ == _LCD_1024600_)
#define TOP_TEXT_H				65	
#define TOP_TEXT_XPOS			50	
#define TOP_TEXT_YPOS			((TOP_TEXT_H-WIN_COMM_FONT)/2)	
#define SYS_TEXT_XPOS			65	
#define SYS_TEXT_YPOS			(TOP_TEXT_H + 25)	
#define TEXT_YPOS_DIS			(13 + WIN_COMM_FONT)
#define TEXT_XPOS_DIS			250
#define CHAR_LEN				275
#define MAX_ITEM				SYS_INFO_MAX	
#endif

#define GUI_TIMER_ID        	10
typedef enum
{
   SYS_INFO_DEV = 0x00,
   SYS_INFO_ROOM, 
   SYS_INFO_HOSTIP,
   SYS_INFO_HostMAC,  
   SYS_INFO_GATEWAY,  
   SYS_INFO_ManageIP1,
   SYS_INFO_MANAGE1,
   SYS_INFO_HW,
   SYS_INFO_SF,
   SYS_INFO_ChipVer,
   SYS_INFO_MspVer,
   SYS_INFO_RootBoxVer,
   SYS_INFO_KernelVer,
   SYS_INFO_ProdVer,
   SYS_INFO_MAX,
}SYS_INFO_TYPE;

/************************变量定义************************/
static HWND g_rightwin;
static RIGHTCTRL_INFO g_rightCtrl;
static UILIST *ListLetf = NULL; 
static UILIST *ListRight = NULL; 
static uint32 g_CurPage = 0; 
static FULL_DEVICE_NO g_devno;
static FWM_VERSION_T  g_kerver;
static NET_PARAM g_param;
static char g_buildtime[100];
static char g_hwver[100];
static char g_prgver[100];						
static uint8 netdoorflag = 0;                       //进入网络门前机界面标志位

/************************函数定义************************/
CREATE_FORM(FrmSetNetdoorSysInfo);

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
        SendMessage(FrmSetNetdoorSysInfo.hWnd, WM_NETDOOR, CMD_GATEWAY_GET_DEV_INFO, 0);
    }
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
  Function:		start_change_rightkey
  Description:  开始改变右边控件的显示内容
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void change_rightkey(HWND hDlg)
{
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, 0, 0};
	#if (_LCD_DPI_ == _LCD_800480_)
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	#elif (_LCD_DPI_ == _LCD_1024600_)
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	#endif
	if (g_CurPage == 1)
	{
		ImageID[RIGHT_NUM_MAX-2] = BID_Right_Up;
	}
	change_rightctrl_allimagetext(g_rightwin, ImageID, TextID);
}

/*************************************************
  Function:		get_version_str
  Description: 	获取版本信息
  Input: 		
	1.			端口信息
  Output:		无
  Return:		端口名称
  Others:
*************************************************/
static void get_version_str(uint32 ver, char *sver)
{
	if (sver == NULL) 
		return;
	sprintf(sver,"V%d.%d.%d.%d",ver>>24, (ver>>16)&0xff,(ver>>8)&0xff,ver&0xff);
}

/*************************************************
  Function:		init_data
  Description: 	初始化数据
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static int32 init_data(HWND hDlg)
{
	uint8 i;
	int nlen = 0;
	uint16 areano;
	char *tmp;
	char tmp1[6];	
	char *temp1;
	int32 DevLen = 0;
	int8 NetDevIndex = -1;
	char devno[30]={0};
	char devstr[30]={0};
	unsigned char sMac[30] = {""}; 
	uint32 TextId[9] = 
	{
		SID_Set_Prj_DevNo,
		SID_Set_Info_Room,
		SID_Set_Prj_Net_HostIP,
		SID_Set_Info_HostMAC,
		SID_Net_Door_GateWay,
		SID_Set_Prj_Net_ManageIP1,
		SID_Set_Prj_Net_CenterSever,
		SID_Set_Info_HW,
		SID_Set_Info_SF,		
	};	

	uint8 TextId1[][14] = 
	{
		"ChipVer ",
		"Msp Ver",
		"RootBoxVer",
		"Kernel Ver",
		"Producter Ver"
	};

	ListClear(ListLetf);
	ListClear(ListRight);
	for (i = 0; i < SYS_INFO_MAX; i++)
	{
		temp1 = malloc(200);
		tmp = malloc(2);
		if (!temp1 || !tmp)
		{
			return FALSE;
		}
		memset(temp1, 0, sizeof(temp1));	
		if(i<=8)
		{					
			strcpy(temp1, get_str(TextId[i]));		
		}
		else if (i>8)
			{
				strcpy(temp1, TextId1[i-9]);
			}
		memset(tmp, 0, sizeof(tmp));	
		sprintf(tmp,"%s",":");
		strcat(temp1, tmp);
		ListAddItem(ListLetf, temp1);
		
		tmp = malloc(100);
		if (!tmp)
		{
			return FALSE;
		}
		memset(tmp,0,sizeof(tmp));
		switch (i)
		{
			case SYS_INFO_DEV:	
				sprintf(tmp,"%s",g_devno.DeviceNoStr);
				break;
		
			case SYS_INFO_ROOM:						
				sprintf(devno,"%s", g_devno.DeviceNoStr);
				DevLen = strlen(devno);
				if (DevLen)
				{
					if (devno[DevLen-1] == '0')
					{
						get_dev_description(DEVICE_TYPE_DOOR_NET, devno, tmp, 30);
					}
					else
					{
						if (devno[DevLen-1] == '8')
						{
							devno[DevLen-1] = '1';
						}
						else if (devno[DevLen-1] == '9')
						{
							devno[DevLen-1] = '2';
						}
						get_dev_description(DEVICE_TYPE_DOOR_NET, devno, tmp, 30);
					}
				}
				break;
			
	   		case SYS_INFO_HOSTIP:
				sprintf(tmp, "%s", UlongtoIP(g_param.IP));
				break;
				
	   		case SYS_INFO_HostMAC:
				{
					char devno[30]={0};
					int32 DevLen = 0;
					sprintf(devno,"%s", g_devno.DeviceNoStr);
					DevLen = strlen(devno);
						
					if (DevLen == 0)
					{
						NetDevIndex = -1;
					}
					else
					{
						if (devno[DevLen-1] == '0')
						{
							NetDevIndex = 0;
						}
						else if(devno[DevLen-1] == '8')
						{
							NetDevIndex = 1;
						}
						else if(devno[DevLen-1] == '9')
						{
							NetDevIndex = 2;
						}
						else
						{
							NetDevIndex = -1;
						}
					}
						 
					if (NetDevIndex != -1)
					{
						memcpy(sMac, storage_get_mac(NetDevIndex), 6);
					}	
					sprintf(tmp, "%02X-%02X-%02X-%02X-%02X-%02X", sMac[0], sMac[1], sMac[2], sMac[3], sMac[4], sMac[5]);
					break;
				}
				
	   		case SYS_INFO_GATEWAY:
				sprintf(tmp, "%s", UlongtoIP(g_param.DefaultGateway));
				break;
				
	   		case SYS_INFO_ManageIP1:
				sprintf(tmp, "%s", UlongtoIP(g_param.ManagerIP));
				break;

			case SYS_INFO_MANAGE1:
				sprintf(tmp, "%s", UlongtoIP(g_param.CenterIP));
				break;
				
	   		case SYS_INFO_HW:
				sprintf(tmp, "%s", g_hwver);
				break;
				
	   		case SYS_INFO_SF:
				sprintf(tmp, "%s-%s", g_prgver, g_buildtime);
				break;
				
	   		case SYS_INFO_ChipVer:
				get_version_str(g_kerver.u32ChipVer,tmp);
				break;
				
   			case SYS_INFO_MspVer:
				get_version_str(g_kerver.u32MspVer,tmp);
				break;
				
			case SYS_INFO_RootBoxVer:
				get_version_str(g_kerver.u32RootBoxVer,tmp);
				break;

			case SYS_INFO_KernelVer:
				get_version_str(g_kerver.u32LinuxVer,tmp);
				break;

			case SYS_INFO_ProdVer:
				get_version_str(g_kerver.u32ProductVer, tmp);
				sprintf(tmp, "%s  %s ", tmp, g_buildtime);
				break;
				
		}
		ListAddItem(ListRight, tmp);
	}

	return TRUE;
}

/*************************************************
  Function:		sysinfoPaint
  Description: 	画桌面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void show_win(HDC hdc)
{
	uint8 i,index;
	uint32 xpos,ypos;
	char * tmp;
	char * temp1;
	RECT TextRc;
	
	if (storage_get_language() != ENGLISH)
	{
		SelectFont(hdc, GetFont(WIN_COMM_FONT));
	}
	else
	{
		SelectFont(hdc, GetFont_ABC(WIN_COMM_FONT));
	}
	
	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
    //DrawBmpID(hdc, 0, 0, 0, 0, BID_ListCtrl_Title);

    //标题信息
    SetTextColor(hdc, COLOR_BLACK);
	TextRc.left = TOP_TEXT_XPOS;
	TextRc.top = TOP_TEXT_YPOS;
	TextRc.right = TextRc.left + CHAR_LEN;
	TextRc.bottom = TextRc.top + FONT_24;
	DrawTextFont(hdc, get_str(SID_Set_System_Info), -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    //系统信息
    if (storage_get_language() != ENGLISH)
    {
        SelectFont(hdc, GetFont(WIN_COMM_FONT));
    }
    else
    {
        SelectFont(hdc, GetFont_ABC(WIN_COMM_FONT));
    }
    SetTextColor(hdc, COLOR_BLACK);
	init_data(hdc);
	xpos = SYS_TEXT_XPOS;	
	ypos = SYS_TEXT_YPOS;	
	for (i = 0; i < MAX_ITEM; i++)
	{
		index = g_CurPage*MAX_ITEM + i;
		if (index >= SYS_INFO_MAX)
		{
			continue;
		}
		
		temp1 = (char *)ListGetItem(ListLetf, index);
		if (temp1)
		{
			TextRc.left = xpos;
			TextRc.top = ypos;
			TextRc.right = TextRc.left + CHAR_LEN;
			TextRc.bottom = TextRc.top + WIN_COMM_FONT;
			DrawTextFont(hdc, temp1, -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}	
		
		xpos += TEXT_XPOS_DIS;
		tmp = (char *)ListGetItem(ListRight, index);
		if (tmp)
		{
			TextRc.left = xpos;
			TextRc.top = ypos;
			TextRc.right = TextRc.left + CHAR_LEN;
			TextRc.bottom = TextRc.top + WIN_COMM_FONT;
			DrawTextFont(hdc, tmp, -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		xpos = SYS_TEXT_XPOS;
		ypos += TEXT_YPOS_DIS;	
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
	uint32 Image = SendMessage(g_rightwin, WM_Right_Get_Image, lParam, 0);
	
	switch (Image)
	{
		case BID_Right_Down:
			g_CurPage = 1;
			change_rightkey(hDlg);
			ui_reflash(hDlg);
			break;
			
		case BID_Right_Up:
			g_CurPage = 0;
			change_rightkey(hDlg);
			ui_reflash(hDlg);
			break;
		
		case BID_Right_Exit:
			close_page();
			break;
	}
}

/*************************************************
  Function:		sysinfoCommand
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
	uint8 i;
	#if (_LCD_DPI_ == _LCD_800480_)
	uint16 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	#elif (_LCD_DPI_ == _LCD_1024600_)
	uint16 ImageID[RIGHT_NUM_MAX] = {0, 0, 0, 0, BID_Right_Exit};
	#endif
	uint16 TextID[RIGHT_NUM_MAX] = {0, 0, 0, 0, 0};
	
	memset(&g_rightCtrl,0,sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
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
}

/*************************************************
  Function:		ui_netdoorsysinfo_win
  Description: 	信息信息
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_netdoorsysinfo_win(HWND hDlg)
{ 
	form_show(&FrmSetNetdoorSysInfo);
	memset(&g_param, 0, sizeof(NET_PARAM));
	memset(&g_devno, 0, sizeof(FULL_DEVICE_NO));
	memset(&g_kerver, 0, sizeof(FWM_VERSION_T));
	memset(g_buildtime, 0, sizeof(g_buildtime));
	memset(g_prgver, 0, sizeof(g_prgver));
    netdoorflag = 1;
	
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
	ListLetf = ListCreate(LS_NONE, NULL);
	ListRight = ListCreate(LS_NONE, NULL);
	
	CreateRightCtrl(hDlg);
}

/*************************************************
  Function:		ui_sysinfo_destroy
  Description: 	销毁
  Input:		
  	1.hDlg		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_sysinfo_destroy(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	ListDestroy(ListLetf);
	ListDestroy(ListRight);
	
    return TRUE;
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
	char buildtime[100];
	
	switch (wParam)
	{
		case CMD_GATEWAY_GET_DEV_INFO:
			show_msg(hDlg, IDC_SEARCH_LIST, MSG_HIT, SID_Inter_Connecting);
			ret = logic_get_netdevice_netparam(&g_param);
			ret |= logic_get_netdevice_devnoinfo(&g_devno, NULL);
			ret = logic_get_netdoor_version(&g_kerver, buildtime, g_prgver);
			if (NETDOOR_ECHO_SUCESS == ret)
			{
				memset(g_buildtime, 0, sizeof(g_buildtime));
				memset(g_hwver, 0, sizeof(g_hwver));	
				memcpy(g_buildtime, buildtime, 40);		
				memcpy(g_hwver,buildtime+40, 40);
				ui_reflash(hDlg);
			}
			else
			{
				ui_reflash(hDlg);
				show_msg(FrmSetNetdoorSysInfo.hWnd, IDC_MSG_CTRL, MSG_WARNING, SID_Msg_Connect_Outtime);
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

		case MSG_SHOWWINDOW:
			g_CurPage = 0;
			//change_rightkey(hDlg);
			//ui_reflash(hDlg);
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
		
		case MSG_DESTROY:
			break;

		case WM_RELOAD:
			OnPaint(hDlg);
			break;

		case WM_NETDOOR:
			OnNetDoorDeal(hDlg, wParam, lParam);
			break;		
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

