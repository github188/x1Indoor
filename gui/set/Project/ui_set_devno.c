/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_devno.c
  Author:   	yanjl
  Version:   	2.0
  Date: 		14-11-27
  Description:  
  				设备编号设置页面
  History:                 
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

CREATE_FORM(FrmDevNoParam);
static HWND g_rightwin;
static HWND g_listwin;
static PFULL_DEVICE_NO g_devparam = NULL;
static RIGHTCTRL_INFO g_rightCtrl;
static LISTCTRL_INFO g_listCtrl;

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
	uint32 i,max = 6;
	uint32 TextID[6] = {0,SID_Set_Dev_Stair_len,SID_Set_Dev_Room_len,0,SID_Set_Dev_Cell_len,SID_Set_Prj_DevNo};
	uint8 devrule[6] ;
	PLISTITEM_INFO g_info = NULL;
	char tmp[40];
	int nlen = 0;
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);

	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info && g_devparam)
	{
		devrule[1] = g_devparam->Rule.StairNoLen;
		devrule[2] = g_devparam->Rule.RoomNoLen;
		devrule[3] = g_devparam->Rule.UseCellNo;
		devrule[4] = g_devparam->Rule.CellNoLen;
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			switch (i)
			{
				case 0:
					g_info[i].Num = 1;
					g_info[i].Icon = 0;
					g_info[i].Item[0].Change = FALSE;
					g_info[i].Item[0].Image = 0;
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					memset(g_info[i].Item[0].Text,0,sizeof(g_info[i].Item[0].Text));
					nlen = CHAR_SIZE;
					strcpy(g_info[i].Item[0].Text, get_str(SID_Set_Dev_Rule+i));
					memset(tmp,0,sizeof(tmp));
					sprintf(tmp,"%s",":");
					strcat(g_info[i].Item[0].Text, tmp);
					break;

				case 1:
				case 2:
				case 4:
					g_info[i].Num = 2;
					g_info[i].Icon = BID_ListIcon;
					g_info[i].Item[0].Change = FALSE;
					g_info[i].Item[0].Image = 0;
					g_info[i].Item[0].ShowType = STYLE_CENTER;
					memset(g_info[i].Item[0].Text,0,sizeof(g_info[i].Item[1].Text));
					nlen = CHAR_SIZE;
					strcpy(g_info[i].Item[0].Text, get_str(TextID[i]));
					memset(tmp,0,sizeof(tmp));
					sprintf(tmp,"%s  %d",":",devrule[i]);
					strcat(g_info[i].Item[0].Text, tmp);
					memset(tmp,0,sizeof(tmp));	
					strcpy(tmp, get_str(SID_Set_Dev_BIT));
					strcat(g_info[i].Item[0].Text, tmp);
					break;

				case 3:
					g_info[i].Num = 2;
					g_info[i].Icon = 0;
					g_info[i].Item[0].Change = FALSE;
					if (devrule[i])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					g_info[i].Item[0].Width = 100;
					g_info[i].Item[1].TextID = SID_Set_Dev_UseCell;
					g_info[i].Item[1].ShowType = STYLE_LEFT;
					break;

				case 5:
					g_info[i].Num = 1;
					g_info[i].Icon = BID_ListIcon;
					g_info[i].Item[0].Change = FALSE;
					g_info[i].Item[0].Image = 0;
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					memset(g_info[i].Item[0].Text,0,sizeof(g_info[i].Item[0].Text));
					nlen = CHAR_SIZE;
					strcpy(g_info[i].Item[0].Text, get_str(TextID[i]));
					memset(tmp,0,sizeof(tmp));
					sprintf(tmp,"%s %s",":",g_devparam->DeviceNoStr);
					strcat(g_info[i].Item[0].Text, tmp);
					break;	
			}
		}
		SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)g_info);
	
		free(g_info);
		g_info = NULL;
	}
	max = 0;
	SendMessage(g_listwin, WM_List_Set_Enable, 0, (LPARAM)&max);
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
  Function:    		comepare_devno_rule
  Description:		比较设备编号规则
  Input: 
	1.rule			设备编号规则
  Output:			无
  Return:			0-成功 1-单元长度非法 2-梯号长度非法 3-房号长度非法
  					4-总长度不能大于18
  Others:
*************************************************/
static uint32 comepare_devno_rule(DEVICENO_RULE Rule)
{
	//合法判断
	if (Rule.CellNoLen > 2)
	{
		return 1;
	}
	if (Rule.StairNoLen < Rule.CellNoLen || Rule.StairNoLen > 9)
	{
		return 2;
	}
	if (Rule.RoomNoLen < 3 || Rule.RoomNoLen > 9)
	{
		return 3;
	}
	if ((Rule.StairNoLen+Rule.RoomNoLen) > 17) 
	{
		return 4;
	}
	if (Rule.CellNoLen == 0)
	{
		Rule.UseCellNo = FALSE;
	}
	
	return 0;
}

/*************************************************
  Function:    		comepare_devno_len
  Description:		判断设备编号长度是否正确
  Input: 
	1.devno			设备号字符串
  Output:			无
  Return:			
  	0-成功 
  	1-长度非法 
  	2-房号段全为0   
  	3-区口机编号必须在1-40之间 
  Others:
*************************************************/
static uint32 comepare_devno_len(char* DevNo)
{
	char temp[20];
	int nlen = 0;

	if (NULL == DevNo)
	{
		return 1;
	}
	
	nlen = strlen(DevNo);
	if (nlen != g_devparam->Rule.StairNoLen + g_devparam->Rule.RoomNoLen + 1)
	{
		return 1;
	}

	strncpy(temp, DevNo+g_devparam->Rule.StairNoLen, g_devparam->Rule.RoomNoLen);
	temp[g_devparam->Rule.RoomNoLen] = 0;
	if (0 == atoi(temp))
	{
		return 2;
	}

	return 0;
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
	uint8 ret1, ret = TRUE;
	ret1 = FALSE;	
	
	//ret = storage_set_devno_rule(TRUE, g_devparam->Rule);
	ret = comepare_devno_rule(g_devparam->Rule);
	switch (ret)
	{
		case 0:										// 成功
			//memcpy(&g_devparam->Rule,storage_get_devrule(),sizeof(DEVICENO_RULE));
			break;

		case 1:										// 单元长度非法
			show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Msg_Cell_Err);
			break;

		case 2:										// 梯号长度非法
			show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Msg_Stair_Err);
			break;

		case 3:										// 房号长度非法
			show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Msg_Room_Err);	
			break;

		case 4:										// 总长度不能大于18
			show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Msg_RuleAll_Err);	
			break;
	}

	if (0 == ret)
	{
		//ret1 = storage_set_devno(TRUE, DEVICE_TYPE_ROOM, g_devparam->DeviceNoStr);
		ret1 = comepare_devno_len(g_devparam->DeviceNoStr);
		switch(ret1)
		{
			case 0:
				g_devparam->Rule.Subsection = (g_devparam->Rule.StairNoLen - g_devparam->Rule.CellNoLen)*100+g_devparam->Rule.CellNoLen*10+g_devparam->Rule.RoomNoLen;
				storage_save_devno(TRUE, g_devparam->Rule, g_devparam->DeviceNoStr);
				memcpy(g_devparam,storage_get_devparam(),sizeof(FULL_DEVICE_NO));
				net_change_comm_deviceno();
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
				break;

			case 1:
				show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Msg_Len_Err);
				break;

			case 2:
				show_msg(hDlg, IDC_MSG_CTRL, MSG_INFORMATION, SID_Msg_RoomNo_Err);
				break;	
		}
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
	switch(id)
	{
		case SID_Right_Save:
			save_param(hDlg);
			break;
			
		case BID_Right_Exit:
			close_page();
			break;

		default:
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
	uint32 TextID[6] = {0, SID_Set_Dev_Stair_len, SID_Set_Dev_Room_len, 0, SID_Set_Dev_Cell_len, SID_Set_Prj_DevNo};

	switch (lParam)
	{
		case 1:
		case 2:
		case 5:	
			g_devparam->DevNoLen = g_devparam->Rule.StairNoLen + g_devparam->Rule.RoomNoLen + 1;
			ui_DevNoDialog_win(hDlg, TextID[lParam], g_devparam);
			break;

		case 4:
			if (g_devparam->Rule.UseCellNo)
			{
				ui_DevNoDialog_win(hDlg, TextID[lParam], g_devparam);
			}
			break;


		case 3:
			if (g_devparam->Rule.UseCellNo)
			{
				g_devparam->Rule.UseCellNo = 0;
				set_list_Image(g_listwin, lParam, 0, BID_Com_UnChose, CHECK_IMAGE);
			}
			else
			{
				g_devparam->Rule.UseCellNo = 1;
				set_list_Image(g_listwin, lParam, 0, BID_Com_Chose, CHECK_IMAGE);
			}
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
	#ifdef _TFT_7_
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listCtrl.Sidebar = 0;								
	#else
	g_listCtrl.Sidebar = 1;							
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;					// 侧边栏
	#endif
	
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_Prj_DevNo;
	g_listCtrl.Width[0] = 200;
	g_listCtrl.Width[1] = 120;	
	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
}

/*************************************************
  Function:		ui_devno_win
  Description: 	设置设备编号界面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_devno_win(HWND hDlg)
{ 
	if (g_devparam) 
	{
		FreeClass(g_devparam);
		g_devparam = NULL;
	}
	g_devparam = (PFULL_DEVICE_NO)MallocClass(sizeof(FULL_DEVICE_NO));
	if (NULL == g_devparam)
	{
		return FALSE;
	}
	memcpy(g_devparam, storage_get_devparam(), sizeof(FULL_DEVICE_NO));
	form_show(&FrmDevNoParam);

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
		
		case MSG_DESTROY:
			break;

		default:
			break;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}


