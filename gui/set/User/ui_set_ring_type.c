/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_ring.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-12-02
  Description:  
  				铃声类型界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

CREATE_FORM(FrmringType);

static HWND g_rightwin;
static HWND g_listwin;
static uint8 g_page = 0;
static uint8 g_index[MAX_RING_ID];					// 声音ID(梯口 区口 门口 中心 住户 分机 信息 预警)
static RIGHTCTRL_INFO g_rightCtrl;
static LISTCTRL_INFO g_listCtrl;

/*************************************************
  Function:		show_win_2
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void  show_win(HDC hdc)
{
	uint32 i,max = 0;
	PLISTITEM_INFO g_info = NULL;
	char tmp[40];
	uint32 TextID;

	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	SendMessage(g_listwin, WM_List_Set_HeadText, SID_Set_ring_Stair+g_page-1, 0);
		
	switch(g_page)
	{
		case 7:
			max = MAX_MSG_ID;
			TextID = SID_Set_hit;
			break;

		case 8:
			max = MAX_YJ_ID;
			TextID = SID_Set_hit;
			break;

		default:
			max = MAX_RING_ID;
			TextID = SID_Set_ring;
			break;
	}

	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			g_info[i].Item[0].TextID = 0;
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[0].Image = 0;
			if (i == (g_index[g_page-1]))
			{
				g_info[i].Item[0].Image = BID_Set_VolumeIcon;
			}
			g_info[i].Icon = 0;
			g_info[i].Item[1].Change = FALSE;
			g_info[i].Item[1].Image = 0;
			g_info[i].Item[1].ShowType = STYLE_LEFT;
			memset(g_info[i].Item[1].Text,0,sizeof(g_info[i].Item[1].Text));
			strcpy(g_info[i].Item[1].Text, get_str(TextID));

			memset(tmp,0,sizeof(tmp));	
			sprintf(tmp,"%d",i+1);
			strcat(g_info[i].Item[1].Text, tmp);
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
	uint8 ret = TRUE;
	
	sys_stop_play_audio(SYS_MEDIA_MUSIC);
	ret = storage_set_ring_id((g_page-1),g_index[g_page-1]);
	if (ret)
	{
		show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
	}
	else
	{
		show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Save_ERR);
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
	
	switch (id)
	{
		case SID_Right_Save:
			save_param(hDlg);
			break;
			
		case BID_Right_Exit:
			sys_stop_play_audio(SYS_MEDIA_MUSIC);
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
	uint32 oldsel;
	
	oldsel = g_index[g_page-1];
	g_index[g_page-1] = lParam;

	set_list_Image(g_listwin, oldsel, 0, 0, CHECK_IMAGE);
	set_list_Image(g_listwin, lParam, 0, BID_Set_VolumeIcon, CHECK_IMAGE);

	switch (g_page-1)
	{
		case 6:
			sys_start_play_audio(SYS_MEDIA_MUSIC, (char *)storage_get_msg_hit_byID(lParam), FALSE, storage_get_ringvolume(), NULL, NULL);
			break;

		case 7:
			sys_start_play_audio(SYS_MEDIA_MUSIC, (char *)storage_get_yj_path_byID(lParam), FALSE, storage_get_ringvolume(), NULL, NULL);
			break;
	
		default:
			sys_start_play_audio(SYS_MEDIA_MUSIC, (char *)storage_get_ring_by_id(lParam), FALSE, storage_get_ringvolume(), NULL, NULL);
			break;
	}
}

/*************************************************
  Function:		ringCommand
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, 0, 0, SID_Right_Save, 0};
	
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
	memset(&g_listCtrl, 0, sizeof(LISTCTRL_INFO));
	g_listCtrl.Parent = hDlg;
	g_listCtrl.Mode = LIST_MODE_COMMON;				// 抬起释放，没有高亮显示
	g_listCtrl.Caption = 1;							// 是否有标题栏
	g_listCtrl.SideBarMode = 1;
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;// 侧边栏
	g_listCtrl.fonttype = LIST_CTRL_FONT;				// 文字显示大小
	#if (_LCD_DPI_ == _LCD_800480_)
	g_listCtrl.Sidebar = 0;							// 不显示侧边栏
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	g_listCtrl.Width[0] = 100;
	g_listCtrl.Width[1] = 200;
	#elif (_LCD_DPI_ == _LCD_1024600_)
	g_listCtrl.Sidebar = 0;							// 不显示侧边栏
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	g_listCtrl.Width[0] = 125;
	g_listCtrl.Width[1] = 250;
	#endif
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_ring_Stair+g_page-1;
	
	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
	
	SendMessage(g_listwin, WM_List_SetKeyMode, 1, 0);
}

/*************************************************
  Function:		ui_ring_win
  Description: 	铃声类型界面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_ringtype_win(HWND hDlg, uint8 page)
{ 
	uint8 i;
	
	g_page = page;
	log_printf("g_page = %d \n",g_page);
	for (i = 0; i < MAX_RING_TYPE; i++)
    {
    	g_index[i] = storage_get_ring_id(i);
	}
	form_show(&FrmringType);

	return TRUE;
}

/*************************************************
  Function:		ringDestroyProc
  Description: 	销毁处理函数
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void ringDestroy( HWND hDlg , WPARAM wParam, LPARAM lParam)
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
			ringDestroy(hDlg , wParam, lParam);
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

