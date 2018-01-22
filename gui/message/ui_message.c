/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_message.c
  Author:   	luofl
  Version:   	2.0
  Date: 		14-11-08
  Description:  
  				信息界面
  History:     
  	1. Date:	
		   Author:	
		   Modification:
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

static HWND g_RighthDlg = -1;
static HWND g_ListhDlg = -1;
static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static LISTCTRL_INFO g_ListCtrl;					// 列表控件信息
static PMSGLIST gp_MsgList = NULL;					// 信息列表
static int32 g_ListIndex = -1;
static uint32 g_RightIndex = 0;

CREATE_FORM(FrmMsg);

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
	char text[50];
	uint32 i, max;
	PLISTITEM_INFO g_info = NULL;
	
	g_RightIndex = -1;
	SendMessage(g_ListhDlg, WM_List_Clear_Item, 0, 0);
	if (NULL != gp_MsgList)
	{
		free(gp_MsgList);
		gp_MsgList = NULL;
	}
	
	gp_MsgList = storage_read_msg_list();
	if (gp_MsgList && gp_MsgList->ncount > 0)
	{
		log_printf("gp_MsgList->ncount: %d \n", gp_MsgList->ncount);
		max = gp_MsgList->ncount;
		g_info = malloc((sizeof(LISTITEM_INFO))*max);
		if (g_info)
		{
			memset(g_info, 0, (sizeof(LISTITEM_INFO))*max);
			for (i = 0; i < max; i++)
			{
				memset((char*)text, 0, sizeof(text));
				g_info[i].Num = 4;
				g_info[i].Item[0].Change = FALSE;
				if (1 == gp_MsgList->pinfo_data[i].is_unread)
				{
					g_info[i].Item[0].Image = BID_Message_UnRead;
				}
				else
				{
					g_info[i].Item[0].Image = BID_Message_Read;
				}
				g_info[i].Item[0].ShowType = STYLE_LEFT;
				
				// 发件人
				g_info[i].Item[1].ShowType = STYLE_CENTER;
				//log_printf("gp_MsgList->pinfo_data[i].des: %s \n", gp_MsgList->pinfo_data[i].des);
				if (0xff == gp_MsgList->pinfo_data[i].des[0])
				{
					g_info[i].Item[1].TextID = SID_Info_Centre_Des;
				}
				else
				{
					sprintf(g_info[i].Item[1].Text, "%s", gp_MsgList->pinfo_data[i].des);
				}
				
				// 主题
				memset((char*)text, 0, sizeof(text));
				g_info[i].Item[2].ShowType = STYLE_LEFT;
				if (0xff == gp_MsgList->pinfo_data[i].Head.title[0])
				{
					g_info[i].Item[2].TextID = SID_Info_Sms;
				}
				else
				{
					sprintf(g_info[i].Item[2].Text, "%s", gp_MsgList->pinfo_data[i].Head.title);
				}
				
				// 日期
				g_info[i].Item[3].ShowType = STYLE_RIGHT;
				sprintf(g_info[i].Item[3].Text, "%04d-%02d-%02d %02d:%02d:%02d", gp_MsgList->pinfo_data[i].time.year, 
					gp_MsgList->pinfo_data[i].time.month, gp_MsgList->pinfo_data[i].time.day,
					gp_MsgList->pinfo_data[i].time.hour, gp_MsgList->pinfo_data[i].time.min, 
					gp_MsgList->pinfo_data[i].time.sec);	
			}
			
			SendMessage(g_ListhDlg, WM_List_Add_Item, max, (LPARAM)g_info);
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
	#if 0
	HDC hdc;
	RECT rc;
	
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	SetBrushColor(hdc, PAGE_BK_COLOR);
	FillBox(hdc, rc.left, rc.top, rc.right, rc.bottom);
	show_win(hDlg);
	EndPaint(hDlg, hdc);
	#endif
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
	uint32 ImageID[RIGHT_NUM_MAX] = { 0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {SID_Right_Clear, SID_Right_Del, 0, SID_Right_View, 0};
	change_rightctrl_allimagetext(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), ImageID, TextID);
}

/*************************************************
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.hDlg		窗口句柄
	2.index		列表元素索引
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM index)
{
	uint32 Image = 0;

	g_ListIndex = SendMessage(GetDlgItem(hDlg, IDC_LIST_CTRL), WM_List_Get_Sel, index, 0);
	g_RightIndex = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, index, 0);
	Image = g_RightIndex;
	switch (Image)
	{
		case SID_Right_View:
			if (g_ListIndex > -1)
			{
				ui_msg_view(hDlg, g_ListIndex, gp_MsgList);
			}
			break;
			
		case SID_Right_Del:
			if (-1 != g_ListIndex && gp_MsgList && g_ListIndex < gp_MsgList->ncount)
			{
				start_change_rightkey(hDlg);
				show_msg(hDlg, IDC_MSG_CTRL, MSG_QUERY, SID_Bj_Query_Del_Rec_One);
			}
			break;
			
		case SID_Right_Clear:
			if (gp_MsgList && 0 != gp_MsgList->ncount)
			{
				start_change_rightkey(hDlg);
				show_msg(hDlg, IDC_MSG_CTRL, MSG_QUERY, SID_Bj_Query_Del_Rec_All);
			}
			break;
			
		case BID_Right_Exit:
			close_page();
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		MsgCtrlCommand
  Description: 	消息框控件的处理函数
  Input:		
  	1.hDlg		窗口句柄
  	2.event		事件消息
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void MsgCtrlCommand(HWND hDlg, WPARAM event)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;

	if (MSG_EVENT_YES == event)
	{
		switch(g_RightIndex)
		{
			// 删除单条信息
			case SID_Right_Del:
				g_ListIndex = SendMessage(g_ListhDlg, WM_List_Get_Sel, 0, 0);
				if (-1 != g_ListIndex && gp_MsgList &&g_ListIndex < gp_MsgList->ncount)
				{
					ret = storage_delete_msg(g_ListIndex);
					if (ECHO_STORAGE_OK == ret)
					{
						SendMessage(g_ListhDlg, WM_List_Del_Item, g_ListIndex, 0);
						if (0 != (gp_MsgList->ncount-g_ListIndex-1))
						{
							memcpy((char*)&gp_MsgList->pinfo_data[g_ListIndex], (char*)&gp_MsgList->pinfo_data[g_ListIndex+1], sizeof(MSGDATA)*(gp_MsgList->ncount-g_ListIndex-1));
						}
						else
						{
							memset((char*)&gp_MsgList->pinfo_data[g_ListIndex], 0, sizeof(MSGDATA));
						}
						gp_MsgList->ncount--;
						show_msg(hDlg, IDC_Msg_Notice, MSG_RIGHT, SID_Oper_OK);
					}
					else
					{
						show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Oper_ERR);
					}
				}
				break;

			// 清空信息
			case SID_Right_Clear:
				if (gp_MsgList && gp_MsgList->ncount)
				{
					ret = storage_clear_msg();
					if (ECHO_STORAGE_OK == ret)
					{
						g_ListIndex = -1;
						if (NULL != gp_MsgList)
						{
							free(gp_MsgList);
							gp_MsgList = NULL;
						}
						show_msg(hDlg, IDC_Msg_Notice, MSG_RIGHT, SID_Oper_OK);
						show_win(hDlg);
					}
					else
					{
						show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Oper_ERR);
					}
				}
				break;
		}
		sys_sync_hint_state();
	}
}

/*************************************************
  Function:		OnCommand
  Description: 	控件事件处理函数
  Input: 		
	1.id		返回的控件ID
	2.index		控件索引
	3.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:
*************************************************/
static void OnCommand(HWND hDlg, WPARAM id, LPARAM index)
{	
	switch (id)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, index);
			break;

		case IDC_LIST_CTRL:
			g_ListIndex = index;
			break;

		case IDC_MSG_CTRL:
			MsgCtrlCommand(hDlg, index);
			break;
			
		case IDC_FORM:
			show_win(hDlg);
			break;

		default:
			break;
	}
}

/*************************************************
  Function:		Init_Key
  Description: 	初始化列表框控件响应右边控件消息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void Init_Key(HWND hDlg)
{
	add_rightctrl_win(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), RIGHT_KEY2, GetDlgItem(hDlg, IDC_LIST_CTRL), WM_List_Go_NextSel, 0, 0);
	//add_rightctrl_win(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), RIGHT_KEY3, GetDlgItem(hDlg, IDC_LIST_CTRL), WM_List_OkKey, 0, 0);
}

/*************************************************
  Function:		CreateListCtrl
  Description: 	创建列表框控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateListCtrl(HWND hDlg)
{
	memset(&g_ListCtrl, 0, sizeof(LISTCTRL_INFO));
	g_ListCtrl.Mode = LIST_MODE_LIGHT;				// 抬起释放，没有高亮显示
	g_ListCtrl.Caption = 1;							// 是否有标题栏
	g_ListCtrl.Sidebar = 1;							// 侧边栏
	g_ListCtrl.SideBarMode = 1;
	g_ListCtrl.KeyCallBack = (SidebarKeyCallBack)get_Sidebar_key;
	g_ListCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;	// 侧边栏
	g_ListCtrl.fonttype = Font16X16;				// 文字显示大小
	g_ListCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_ListCtrl.HeadInfo.TextID = SID_MainInfo;
	#if (_LCD_DPI_ == _LCD_800480_)
	g_ListCtrl.Width[0] = 40;
	g_ListCtrl.Width[1] = 120;
	g_ListCtrl.Width[2] = 120;
	g_ListCtrl.Width[3] = 200;
	#elif (_LCD_DPI_ == _LCD_1024600_)
	g_ListCtrl.Width[0] = 40;
	g_ListCtrl.Width[1] = 120;
	g_ListCtrl.Width[2] = 120;
	g_ListCtrl.Width[3] = 200;
	#endif
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_ListCtrl.PageCount = 8;						// 显示个数
	g_ListCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_ListCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_ListCtrl.PageCount = 5;						// 显示个数
	g_ListCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_ListCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	
	g_ListhDlg = CreateWindow(AU_LIST_CTRL, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_LIST_CTRL, 
				LIST_CTRL_X, LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				hDlg, (DWORD)&g_ListCtrl);
}

/*************************************************
  Function:		CreateRightCtrl
  Description: 	创建右边控件
  Input:		
  	1.hDlg		窗口句柄
  Output:		无
  Return:		无
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;
	uint32 ImageID[RIGHT_NUM_MAX] = { 0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {SID_Right_Clear, SID_Right_Del, 0, SID_Right_View, 0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = RIGHT_CTRL_FONT;
	g_rightCtrl.parent = hDlg;
	for (i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
	}
	g_RighthDlg = CreateWindow(AU_RIGHT_BUTTON, "", 
				WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				IDC_RIGHT_BUTTON, 
				RIGHT_CTRL_X, RIGHT_CTRL_Y, RIGHT_CTRL_W, RIGHT_CTRL_H, 
				hDlg, (DWORD)&g_rightCtrl);
	
	Init_Key(hDlg);
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

#ifdef _ENABLE_TOUCH_
/*************************************************
  Function:		get_touch_down
  Description: 	触摸屏按下
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int get_touch_down(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

/*************************************************
  Function:		get_touch_up
  Description: 	触摸屏抬起
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static int get_touch_up(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
#endif

/*************************************************
  Function:		CallRecordDestroyProc
  Description: 	销毁处理函数
  Input:		
  	1.win
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static uint32 MsgDestroyProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	g_ListIndex = -1;
	if (NULL != gp_MsgList)
	{
		free(gp_MsgList);
		gp_MsgList = NULL;
	}

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
			if (wParam == SW_SHOWNORMAL)
			{
				show_win(hDlg);
			}
			else if (wParam == SW_HIDE)
			{
				close_page();
			}
			break;
			
		case MSG_PAINT:
			OnPaint(hDlg);
			break;

		#ifdef _ENABLE_TOUCH_
		case MSG_LBUTTONDOWN:
			if (GetCapture() == hDlg)
			{
				break;
			}
			SetCapture(hDlg);
    		get_touch_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	    	if (GetCapture() != hDlg)
			{
				break;
			}
			ReleaseCapture();
	        get_touch_up(hDlg, LOWORD(lParam), HIWORD(lParam));
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
			break;	

		case WM_Change_RightKey:
			finish_change_rightkey(hDlg);
			break;
			
		case MSG_CLOSE:
			DestroyMainWindow(hDlg);
			PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			MsgDestroyProc(hDlg, wParam, lParam);
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

