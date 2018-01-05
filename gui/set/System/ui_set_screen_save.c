/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_screen_save.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-11-28
  Description:  
  				屏保界面设置
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"
typedef struct
{
	uint8 page;
	EPHOTO_TIME time;
	EPHOTO_PARAM param;
}EPHOTO_INFO, * PEPHOTO_INFO;			

CREATE_FORM(FrmScreenSave);

static HWND rightwin;
static HWND listwin;
static PEPHOTO_INFO g_pageinfo = NULL;
static RIGHTCTRL_INFO g_rightCtrl;
static LISTCTRL_INFO g_listCtrl;
static void Change_Key(HWND hDlg);

/*************************************************
  Function:		get_index_str
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void get_index_str(HWND hDlg, uint32 ID, char *str)
{
	char tmp[80];
	int time = 0;
	uint32 TextID;
	
	memset(str,0,sizeof(str));	
	memset(tmp,0,sizeof(tmp));			
	switch (ID)
	{
		case EPHOTO_TIME_HALF:
			time = 30;
			TextID = SID_Time_Sec;
			break;
			
		case EPHOTO_TIME_1:
			time = 1;
			TextID = SID_Time_Min;
			break;
			
		case EPHOTO_TIME_3:
			time = 3;
			TextID = SID_Time_Min;
			break;
			
		case EPHOTO_TIME_5:
			time = 5;
			TextID = SID_Time_Sec;
			break;
			
		case EPHOTO_TIME_15:
			time = 15;
			TextID = SID_Time_Sec;
			break;
			
		case EPHOTO_TIME_30:
			time = 30;
			TextID = SID_Time_Sec;
			break;
			
		case EPHOTO_TIME_60:	
			time = 1;
			TextID = SID_Time_Min;
			break;

		case EPHOTO_TIME_120:	
			time = 2;
			TextID = SID_Time_Min;
			break;
	}

	sprintf(tmp,"%d",time);
	strcat(str, tmp);
	memset(tmp,0,sizeof(tmp));	
	strcpy(tmp,get_str(TextID));
	strcat(str, tmp);	
}

/*************************************************
  Function:		show_win_1
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win_1(HWND hDlg)
{
	uint8 i, max = 1;
	PLISTITEM_INFO g_info = NULL;

	SendMessage(listwin, WM_List_Set_HeadText, SID_Set_Screen_Lcd, 0);
	SendMessage(listwin, WM_List_Clear_Item, 0, 0);

	g_info = malloc((sizeof(LISTITEM_INFO)) * max);
	if (g_info)
	{
		memset(g_info, 0, (sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			switch (i)
			{
				case 0:
					g_info[i].Num = 2;
					g_info[i].Item[0].TextID = SID_Set_Screen_Lcd;
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					g_info[i].Icon = BID_ListIcon;
					g_info[i].Item[1].Change = FALSE;
					g_info[i].Item[1].Image = 0;
					g_info[i].Item[1].ShowType = STYLE_LEFT;
					memset(g_info[i].Item[1].Text, 0, sizeof(g_info[i].Item[1].Text));
					get_index_str(hDlg, g_pageinfo->param.holdtime, g_info[i].Item[1].Text);
					break;

				case 1:
					g_info[i].Num = 2;
					g_info[i].Item[0].TextID = SID_Set_Screen_InTime;
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					g_info[i].Icon = BID_ListIcon;
					g_info[i].Item[1].Change = FALSE;
					g_info[i].Item[1].Image = 0;
					g_info[i].Item[1].ShowType = STYLE_LEFT;
					memset(g_info[i].Item[1].Text, 0, sizeof(g_info[i].Item[1].Text));
					get_index_str(hDlg, g_pageinfo->param.intime, g_info[i].Item[1].Text);
					break;

				case 2:
					g_info[i].Num = 2;
					g_info[i].Item[0].TextID = SID_Set_Screen_Lcd;
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					g_info[i].Icon = BID_ListIcon;
					g_info[i].Item[1].Change = FALSE;
					g_info[i].Item[1].Image = 0;
					g_info[i].Item[1].ShowType = STYLE_LEFT;
					memset(g_info[i].Item[1].Text, 0, sizeof(g_info[i].Item[1].Text));
					get_index_str(hDlg, g_pageinfo->param.holdtime, g_info[i].Item[1].Text);
					break;	
			}
		}
		
		SendMessage(listwin, WM_List_Add_Item, max, (LPARAM)g_info);
		free(g_info);
		g_info = NULL;
	}
} 

/*************************************************
  Function:		show_win_2
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win_2(HWND hDlg)
{
	uint8 i, max;
	uint8 g_page[2] = {5, 5};
	EPHOTO_TIME time;
	PLISTITEM_INFO g_info = NULL;
	uint32 g_headtext[2] ={SID_Set_Screen_InTime, SID_Set_Screen_Lcd};
	uint32 g_time[2][5] = 
	{
		{EPHOTO_TIME_HALF, EPHOTO_TIME_1, EPHOTO_TIME_3, EPHOTO_TIME_5, EPHOTO_TIME_15},
		{EPHOTO_TIME_5, EPHOTO_TIME_15, EPHOTO_TIME_30, EPHOTO_TIME_60, EPHOTO_TIME_120}	
	};
	
	SendMessage(listwin, WM_List_Set_HeadText, g_headtext[g_pageinfo->page-1], 0);
	SendMessage(listwin, WM_List_Clear_Item, 0, 0);
	max = g_page[g_pageinfo->page-1];
	switch(g_pageinfo->page)
	{
		case 0:
			break;

		case 1:
			time = g_pageinfo->param.intime;
			break;

		case 2:
			time = g_pageinfo->param.holdtime;
			break;
	}

	g_pageinfo->time = time;

	g_info = malloc((sizeof(LISTITEM_INFO)) * max);
	if (g_info)
	{
		memset(g_info, 0, (sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			g_info[i].Item[0].TextID = 0;
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[0].Image = 0;
			if (time == g_time[g_pageinfo->page-1][i])
			{
				g_info[i].Item[0].Image = BID_Set_Sel;
			}
			g_info[i].Icon = 0;
			g_info[i].Item[1].Change = FALSE;
			g_info[i].Item[1].Image = 0;
			g_info[i].Item[1].ShowType = STYLE_LEFT;
			memset(g_info[i].Item[1].Text, 0, sizeof(g_info[i].Item[1].Text));
			get_index_str(hDlg, g_time[g_pageinfo->page-1][i], g_info[i].Item[1].Text);
		}
		
		SendMessage(listwin, WM_List_Add_Item, max, (LPARAM)g_info);
		SendMessage(listwin, WM_List_SetKeyMode, 1, 0);
		
		if (g_pageinfo->page == 2)
		{
			switch(g_pageinfo->param.holdtime)
			{
				case EPHOTO_TIME_5:
					time = 0;
					break;
				case EPHOTO_TIME_15:
					time = 1;
					break;
				case EPHOTO_TIME_30:
					time = 2;
					break;
				case EPHOTO_TIME_60:
					time = 3;
					break;
				case EPHOTO_TIME_120:
					time = 4;
					break;
				default:
					time = 0;
					break;
			}
			SendMessage(listwin, WM_List_Set_Sel, time, 0);
		}
	
		free(g_info);
		g_info = NULL;
	}
} 

/*************************************************
  Function:		OnPaint
  Description: 	画桌面
  Input:		
  	1.hDlg
  	2.wParam
  	3.lParam
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void OnPaint(HWND hDlg)
{
	HDC hdc;
	RECT rc;
	
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	show_win_2(hdc);
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
	
	ret = storage_set_screenparam(&g_pageinfo->param);
	InitScreenTimer();
	if (ret)
	{
		//InitScreenTimer();
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
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 TextID;
	uint32 id = SendMessage(rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	
	switch(id)
	{
		case SID_Right_Save:
			g_pageinfo->param.holdtime = g_pageinfo->time; 
			save_param(hDlg);
			break;

		case SID_Right_Sure:
			if (g_pageinfo->page == 2)
			{
				TextID = SID_Right_Save;
				SendMessage(rightwin, WM_Right_Set_TextID, 0, (LPARAM)&TextID);			
				if (1 == g_pageinfo->page)
				{
					g_pageinfo->param.intime = g_pageinfo->time; 
				}
				else
				{
					g_pageinfo->param.holdtime = g_pageinfo->time; 
				}
				g_pageinfo->page = 0;
				Change_Key(hDlg);
				show_win_1(hDlg);
				SendMessage(listwin, WM_List_SetKeyMode, 0, 0);
			}
			#if 0
			Image1 = BID_RightCtrl_Save1;
			TextID1 = SID_Right_Save;
			SendMessage(rightwin, WM_Right_Set_Image, 0, &Image1);
			SendMessage(rightwin, WM_Right_Set_Text, 0, &TextID1);			
			if (1 == g_pageinfo->page)
			{
				g_pageinfo->param.intime = g_pageinfo->time; 
			}
			else
			{
				g_pageinfo->param.holdtime = g_pageinfo->time; 
			}
			g_pageinfo->page = 0;
			Change_Key(hDlg);
			show_win_1(hDlg);
			SendMessage(listwin, WM_List_SetKeyMode, 0, NULL);
			#endif
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
	uint32 TextID;
	uint32 oldsel;
	
	if (0 == g_pageinfo->page)
	{
		switch(lParam)
		{
			case 0:
				g_pageinfo->page = 2;
				Change_Key(hDlg);
				show_win_2(hDlg);
				SendMessage(listwin, WM_List_SetKeyMode, 1, 0);
				break;
				
			case 1:
				if (g_pageinfo->param.used)
				{
					g_pageinfo->page = 1;
					TextID = SID_Right_Sure;
					SendMessage(rightwin, WM_Right_Set_TextID, 0, (LPARAM)&TextID);	
					show_win_2(hDlg);
				}
				break;

			case 2:	
				g_pageinfo->page = 2;
				TextID = SID_Right_Sure;
				SendMessage(rightwin, WM_Right_Set_TextID, 0, (LPARAM)&TextID);	
				show_win_2(hDlg);
				SendMessage(listwin, WM_List_SetKeyMode, 1, 0);
				break;
		}
	}
	else 
	{
		if (1 == g_pageinfo->page)
		{
			oldsel = g_pageinfo->time - EPHOTO_TIME_HALF;
			g_pageinfo->time = lParam + EPHOTO_TIME_HALF;

			set_list_Image(listwin, oldsel, 0, 0, CHECK_IMAGE);
			set_list_Image(listwin, lParam, 0, BID_Set_Sel, CHECK_IMAGE);
		}
		else
		{
			switch (g_pageinfo->time)
			{
				case EPHOTO_TIME_5:
					oldsel = 0;
					break;
					
				case EPHOTO_TIME_15:
					oldsel = 1;
					break;
					
				case EPHOTO_TIME_30:
					oldsel = 2;
					break;
					
				case EPHOTO_TIME_60:
					oldsel = 3;
					break;

				case EPHOTO_TIME_120:
					oldsel = 4;
					break;

				default:
					oldsel = 0;
					break;
			}

			switch (lParam)
			{
				case 0:
					g_pageinfo->time = EPHOTO_TIME_5;
					break;
					
				case 1:
					g_pageinfo->time = EPHOTO_TIME_15;
					break;
					
				case 2:
					g_pageinfo->time = EPHOTO_TIME_30;
					break;
					
				case 3:
					g_pageinfo->time = EPHOTO_TIME_60;
					break;

				case 4:
					g_pageinfo->time = EPHOTO_TIME_120;
					break;
			}
			set_list_Image(listwin, oldsel, 0, 0, CHECK_IMAGE);
			set_list_Image(listwin, lParam, 0, BID_Set_Sel, CHECK_IMAGE);
			#if 0
			Image = BID_RightCtrl_Save1;
			TextID = SID_Right_Save;
			SendMessage(rightwin, WM_Right_Set_Image, 0, &Image);
			SendMessage(rightwin, WM_Right_Set_Text, 0, &TextID);			
			if (1 == g_pageinfo->page)
			{
				g_pageinfo->param.intime = g_pageinfo->time; 
			}
			else
			{
				g_pageinfo->param.holdtime = g_pageinfo->time; 
			}
			g_pageinfo->page = 0;
			Change_Key(hDlg);
			show_win_1(hDlg);
			SendMessage(listwin, WM_List_SetKeyMode, 0, NULL);
			#endif
		}
	}
}

/*************************************************
  Function:		OnCommand
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
	add_rightctrl_win(rightwin, RIGHT_KEY2, listwin, WM_List_Go_NextSel, 0, 0);
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
	uint32 ImageID[5] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[5] = {0, 0, 0, SID_Right_Save,0};

	memset(&g_rightCtrl,0,sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
	g_rightCtrl.parent = hDlg;
	for(i = 0; i < g_rightCtrl.MaxNum; i++)
	{
		g_rightCtrl.buttons[i].Enabled = TRUE;
		g_rightCtrl.buttons[i].ImageID = ImageID[i];
		g_rightCtrl.buttons[i].TextID = TextID[i];
	}
	
	rightwin = CreateWindow(AU_RIGHT_BUTTON, "", 
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
	g_listCtrl.KeyCallBack = (SidebarKeyCallBack)get_Sidebar_key;
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;	// 侧边栏
	g_listCtrl.fonttype = Font16X16;					// 文字显示大小
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_System_Screen;
	g_listCtrl.Width[0] = 100;
	g_listCtrl.Width[1] = 200;
	
	#ifdef _E81S_UI_STYLE_
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	
 	listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 			WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 			IDC_LIST_CTRL, 
				 			LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 			hDlg, (DWORD)&g_listCtrl);
}

/*************************************************
  Function:		Change_Key
  Description: 	
  Input:		
  	1.win
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void Change_Key(HWND hDlg)
{
	uint32 TextID;
	SendMessage(rightwin, WM_Right_Clear_Win, 3, 0);
	if (g_pageinfo->page == 2)
	{
		TextID = 0;
		SendMessage(rightwin, WM_Right_Set_TextID, 0, (LPARAM)&TextID);	

		TextID = 0;
		SendMessage(rightwin, WM_Right_Set_TextID, 1, (LPARAM)&TextID);	

		TextID = SID_Right_Sure;
		SendMessage(rightwin, WM_Right_Set_TextID, 3, (LPARAM)&TextID);	

		TextID = 0;
		SendMessage(rightwin, WM_Right_Set_TextID, 2, (LPARAM)&TextID);

		add_rightctrl_win(rightwin, RIGHT_KEY2, listwin, WM_List_Go_NextSel, 0, 0);
		
	}
	else
	{
		TextID = SID_Right_Save;
		SendMessage(rightwin, WM_Right_Set_TextID, 0, (LPARAM)&TextID);	

		TextID = 0;
		SendMessage(rightwin, WM_Right_Set_TextID, 1, (LPARAM)&TextID);	

		TextID = SID_Right_Sure;;
		SendMessage(rightwin, WM_Right_Set_TextID, 3, (LPARAM)&TextID);	

		TextID = 0;
		SendMessage(rightwin, WM_Right_Set_TextID, 2, (LPARAM)&TextID);

		add_rightctrl_win(rightwin, RIGHT_KEY3, listwin, WM_List_OkKey, 0, 0);
	}
}

/*************************************************
  Function:		ui_screensave_win
  Description: 	屏保主窗口
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_screensave_win(HWND hDlg)
{ 
	if (g_pageinfo)
	{
		free(g_pageinfo);
		g_pageinfo = NULL;
	}
	g_pageinfo = (PEPHOTO_INFO)malloc(sizeof(EPHOTO_INFO));
	if (NULL == g_pageinfo)
	{
		return FALSE;
	}
	g_pageinfo->page = 2;
	memcpy(&g_pageinfo->param, (PEPHOTO_PARAM)storage_get_screenparam(), sizeof(EPHOTO_PARAM));
	
	form_show(&FrmScreenSave);

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
			return 0;
		
		case MSG_DESTROY:
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

