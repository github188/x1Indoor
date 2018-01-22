/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_sysinfo.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-12-01
  Description:  
  				系统信息界面
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
#define TOP_TEXT_YPOS			((TOP_TEXT_H-Font16X16)/2)	
#define SYS_TEXT_XPOS			50	
#define SYS_TEXT_YPOS			(TOP_TEXT_H+20)	
#define TEXT_YPOS_DIS			(10+Font16X16)
#define TEXT_XPOS_DIS			200
#define CHAR_LEN				200
#define MAX_ITEM				SYS_INFO_MAX			
#elif (_LCD_DPI_ == _LCD_1024600_)
#define TOP_TEXT_H				52	
#define TOP_TEXT_XPOS			40	
#define TOP_TEXT_YPOS			((TOP_TEXT_H-Font16X16)/2)	
#define SYS_TEXT_XPOS			50	
#define SYS_TEXT_YPOS			(TOP_TEXT_H+20)	
#define TEXT_YPOS_DIS			(10+Font16X16)
#define TEXT_XPOS_DIS			200
#define CHAR_LEN				200
#define MAX_ITEM				SYS_INFO_MAX	
#ifdef _USE_NEW_CENTER_
#define TEXT_XPOS_DIS			145
#else
#define TEXT_XPOS_DIS			190
#endif
#define CHAR_LEN				190
#define MAX_ITEM				8
#endif

typedef enum
{
   SYS_INFO_DEV = 0x00,
   SYS_INFO_ROOM, 
   SYS_INFO_HOSTIP,
   SYS_INFO_MASK,
   SYS_INFO_GATEWAY,
   SYS_INFO_MAC,
   #ifdef _AURINE_REG_
   SYS_INFO_AURINE,
   #endif
   SYS_INFO_CENTERIP,
   SYS_INFO_MANAGE1,
   #ifdef _MULTI_CENTER_
   SYS_INFO_MANAGE2, 
   SYS_INFO_MANAGE3,
   #endif
   #ifdef _USE_ELEVATOR_
   SYS_INFO_DIANTI,
   #endif
   #ifdef _IP_MODULE_
   SYS_INFO_IP_MODILE,
   #endif
   SYS_INFO_HW,
   SYS_INFO_SF,
   #ifndef _USE_NEW_CENTER_  
   SYS_INFO_SDK,		
   #endif
   SYS_INFO_MAX,
}SYS_INFO_TYPE;

CREATE_FORM(FrmSetSysInfo);
static HWND g_rightwin;
static RIGHTCTRL_INFO g_rightCtrl;
static UILIST *ListLetf = NULL; 
static UILIST *ListRight = NULL; 
static uint32 g_CurPage = 0; 

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
	char tmp2[20];
	char *temp1;
	uint32 TextId[SYS_INFO_MAX] = 
	{
		SID_Set_Prj_DevNo,
		SID_Set_Info_Room,
		SID_Set_Prj_Net_HostIP,
		SID_Set_Prj_Net_HostNetMask,
		SID_Set_Prj_Net_HostGateWay,
		SID_Set_Info_HostMAC,
		#ifdef _AURINE_REG_
		SID_Set_Prj_Net_AurineSever,
		#endif
		SID_Set_Prj_Net_CenterSever,
		SID_Set_Prj_Net_ManageIP1,
		#ifdef _MULTI_CENTER_
		SID_Set_Prj_Net_ManageIP2,
		SID_Set_Prj_Net_ManageIP3,
		#endif
		#ifdef _USE_ELEVATOR_
		SID_Set_Prj_Net_AurineElevator,
		#endif
		#ifdef _IP_MODULE_
	    SID_IP_Con_Addr,
	    #endif
		SID_Set_Info_HW,
		SID_Set_Info_SF,
		#ifndef _USE_NEW_CENTER_
		SID_Set_Info_SDK,		
		#endif
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
		strcpy(temp1, get_str(TextId[i]));
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
				areano = storage_get_areano();
				sprintf(tmp,"%d-%s",areano,storage_get_devno_str());
				break;
		
			case SYS_INFO_ROOM:						
				nlen = 100;
				get_dev_description(DEVICE_TYPE_ROOM, storage_get_devno_str(), tmp, nlen);
				break;
				
	   		case SYS_INFO_HOSTIP:
				change_ip_to_str((storage_get_netparam_bytype(HOST_IPADDR)), tmp);
				break;
				
	   		case SYS_INFO_MASK:
				change_ip_to_str((storage_get_netparam_bytype(HOST_NETMASK)), tmp);
				break;
				
	   		case SYS_INFO_GATEWAY:
				change_ip_to_str((storage_get_netparam_bytype(HOST_GATEWAY)), tmp);
				break;
				
	   		case SYS_INFO_MAC:
				memcpy(tmp1, storage_get_mac(HOUSE_MAC), sizeof(tmp1));
				sprintf(tmp,"%02x-%02x-%02x-%02x-%02x-%02x",tmp1[0],tmp1[1],tmp1[2],tmp1[3],tmp1[4],tmp1[5]);
				break;

			#ifdef _AURINE_REG_
			case SYS_INFO_AURINE:
				change_ip_to_str((storage_get_netparam_bytype(AURINE_IPADDR)), tmp);
				break;
			#endif
				
	   		case SYS_INFO_CENTERIP:
				change_ip_to_str((storage_get_netparam_bytype(CENTER_IPADDR)), tmp);
				#ifdef _USE_NEW_CENTER_
				uint32 ret = logic_reg_center_state();
				memset(tmp2, 0, sizeof(tmp2));
				if (ret == 2)
				{
					sprintf(tmp2, "  ( %s )",get_str(SID_Registered));
					strcat(tmp, tmp2);	
				}
				else
				{
					sprintf(tmp2, "  ( %s )",get_str(SID_Unregistered));
					strcat(tmp, tmp2);	
				}
				#endif
				break;
				
	   		case SYS_INFO_MANAGE1:
				change_ip_to_str((storage_get_netparam_bytype(MANAGER1_IPADDR)), tmp);
				break;

			#ifdef _MULTI_CENTER_
			case SYS_INFO_MANAGE2:
				change_ip_to_str((storage_get_netparam_bytype(MANAGER2_IPADDR)), tmp);
				break;

			case SYS_INFO_MANAGE3:
				change_ip_to_str((storage_get_netparam_bytype(MANAGER3_IPADDR)), tmp);
				break;
			#endif

			#ifdef _USE_ELEVATOR_
			case SYS_INFO_DIANTI:
				change_ip_to_str((storage_get_netparam_bytype(DIANTI_IPADDR)), tmp);
				break;
			#endif
				
	   		case SYS_INFO_HW:
				sprintf(tmp, "%s", HARD_VER);
				break;
				
	   		case SYS_INFO_SF:				
				sprintf(tmp, "%s", SOFT_VER);
				break;
				
			#ifndef _USE_NEW_CENTER_	// modi by chenbh 2015-03-25
	   		case SYS_INFO_SDK:
				sprintf(tmp,"%s",SDK_VER);
				break;
			#endif

			#ifdef _IP_MODULE_
   			case SYS_INFO_IP_MODILE:
				change_ip_to_str((get_ipmodule_addr()), tmp);
				break;
			#endif
				
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
		SelectFont(hdc, GetFont(FONT_16));
	}
	else
	{
		SelectFont(hdc, GetFont_ABC(FONT_16));
	}
	SetTextColor(hdc, COLOR_BLACK);
	
	DrawBmpID(hdc, 0, 0, 0, 0, BID_Set_BK);
	TextRc.left = TOP_TEXT_XPOS;
	TextRc.top = TOP_TEXT_YPOS;
	TextRc.right = TextRc.left + CHAR_LEN;
	TextRc.bottom = TextRc.top + Font16X16;
	DrawText(hdc, get_str(SID_Set_System_Info), -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

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
			TextRc.bottom = TextRc.top + Font16X16;
			DrawText(hdc, temp1, -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}	
		
		xpos += TEXT_XPOS_DIS;
		tmp = (char *)ListGetItem(ListRight, index);
		if (tmp)
		{
			TextRc.left = xpos;
			TextRc.top = ypos;
			TextRc.right = TextRc.left + CHAR_LEN;
			TextRc.bottom = TextRc.top + Font16X16;
			DrawText(hdc, tmp, -1, &TextRc, DT_CALCRECT | DT_LEFT| DT_VCENTER | DT_SINGLELINE);
			DrawText(hdc, tmp, -1, &TextRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
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
  Function:		ui_sysinfo_win
  Description: 	信息信息
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_sysinfo_win(HWND hDlg)
{ 
	g_CurPage = 0;
	form_show(&FrmSetSysInfo);
	
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
			ui_sysinfo_destroy(hDlg, wParam, lParam);
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

