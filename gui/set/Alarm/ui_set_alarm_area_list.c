/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_alarm_set_area_list.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		14-10-09
  Description:  
  				防区隔离局防有效及防区属性设置界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

static HWND g_rightwin = 0;
static HWND g_listwin = 0;
static HWND g_upperwin = 0;							// 上一级窗口

static RIGHTCTRL_INFO	g_rightCtrl;
static LISTCTRL_INFO	g_listCtrl;

static uint8  g_isolate = 0;
static uint8  g_part_valid = 0;
static uint8  g_page_type = 0;						// 1:隔离 2:局防有效 3:防区属性 4:防区类型
static uint8  g_select = 0;			      
 
CREATE_FORM(FrmAlarmAreaList);

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
	uint32 i, max = 8;
	uint16 icon_id = 0;
	uint8 tmp_val = 0;
	uint32 len;
	uint8 enable = 1;
	PLISTITEM_INFO g_info = NULL;
	uint16 text[50];
	char text_tmp[60];
	char tmp[50];
 	uint16 image_id[8] = {BID_BJ_Urgency2, BID_BJ_Fire2, BID_BJ_Gas2, BID_BJ_Door2, BID_BJ_Room2, BID_BJ_Window2, BID_BJ_Window2,  BID_BJ_Balcony2};
	AF_FLASH_DATA	alarm_data	 = {0};

	logic_get_alarm_param((uint8*)&alarm_data);
	memset(text, 0, sizeof(text));
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	switch (g_page_type)
	{
		case 1:
			tmp_val = g_isolate;
			icon_id = BID_BJ_Set_ISO_TM;
			break;
			
		case 2:
			tmp_val = g_part_valid;
			icon_id = BID_BJ_Set_Part_Vald;
			break;
			
		case 3:
			tmp_val = 0xFF;
			icon_id = BID_ListIcon;
			break;
			
		case 4:
			icon_id = 0;
			break;
			
		default:
			return;
	}
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 3;
			if (0x01 == (0x01&(tmp_val>>i)))
			{
				g_info[i].Icon = icon_id;
			}
			else
			{
				g_info[i].Icon = 0;
			}
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[0].Change = FALSE;
			len = CHAR_SIZE;
			if (4 != g_page_type)
			{
				g_info[i].Item[0].Image = image_id[alarm_data.area_type[i]];
				g_info[i].Item[1].TextID = 0;
				strcpy(g_info[i].Item[1].Text,get_str(SID_Bj_Fangqu1+i));
				
				memset(tmp,0,sizeof(tmp));
				sprintf(tmp,"%s","-------");
				strcat(g_info[i].Item[1].Text, tmp);
					
				memset((char *)text_tmp, 0, sizeof(text_tmp));
				strcpy(text_tmp,get_str(SID_Bj_SOS+alarm_data.area_type[i]));
			}
			else
			{
				g_info[i].Item[0].Image = image_id[i];
				g_info[i].Item[1].TextID = 0;
				len = 30;
				memset((char *)text_tmp, 0, sizeof(text_tmp));
				strcpy(text_tmp,get_str(SID_Bj_SOS+i));
			}
			strcat(g_info[i].Item[1].Text, text_tmp);

			g_info[i].Item[1].ShowType = STYLE_LEFT;

			g_info[i].Item[2].ShowType = STYLE_LEFT;

		}
		SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)g_info);
		if (1== g_page_type)
		{
			SendMessage(g_listwin, WM_List_Set_Sel, g_select, (LPARAM)g_info);
		}
		free(g_info);
		g_info = NULL;
		if (2 == g_page_type)
		{
			for (i = 0; i < max; i++)
			{
				if (1 == ((alarm_data.is_24_hour>>i)&0x01))
				{
					enable = 0;
					SendMessage(g_listwin, WM_List_Set_Enable, i, (LPARAM)&enable);
				}
			}
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
	SetBrushColor(hdc, PAGE_BK_COLOR);
	FillBox(hdc, rc.left, rc.top, rc.right, rc.bottom);
	show_win(hdc);
	EndPaint(hDlg, hdc);
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
	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listCtrl.Sidebar = 0;										// 侧边栏
	#else
	g_listCtrl.Sidebar = 1;										// 侧边栏
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;	// 侧边栏
	#endif
	
	#if (_LCD_DPI_ == _LCD_800480_)
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#elif (_LCD_DPI_ == _LCD_1024600_)
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#endif
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
 	g_listCtrl.Width[0] = 60;
	g_listCtrl.Width[1] = 250;

	switch (g_page_type)
	{
		case 1:
			g_listCtrl.HeadInfo.TextID = SID_Set_Alarm_AfGeLi;
			break;
			
		case 2:
			g_listCtrl.HeadInfo.TextID = SID_Set_Alarm_AfJuFang;
			break;
			
		case 3:
			g_listCtrl.HeadInfo.TextID = SID_Bj_Fangqu_Settings;
			break;
			
		case 4:
			g_listCtrl.HeadInfo.TextID = SID_Bj_Tantou_Type;
			break;
			
		default:
			return;
	}

	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
}

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
  Function:		CreateRightCtrl
  Description: 	初始化右边控件
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void CreateRightCtrl(HWND hDlg)
{
	uint32 i;
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, SID_Right_Save, 0, SID_Right_Sure,  0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = RIGHT_NUM_MAX;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
	g_rightCtrl.parent = hDlg;

	// 后两种无保存按钮
	if  (g_page_type > 2)
	{
		ImageID[1] = 0;
		TextID[1] = 0;
	}

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
  Function:		RightCtrlCommand
  Description: 	右边控件按下的处理函数
  Input:		
  	1.hDlg		窗口句柄
  	2.lParam	控件索引
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void RightCtrlCommand(HWND hDlg, LPARAM lParam)
{
	uint32 id;
	id = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	
	switch (id)
	{
		case SID_Right_Save:
			if (1 == g_page_type )
			{
				logic_set_area_isolation(g_isolate);
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);

			}
			else if (2 == g_page_type)
			{
				logic_set_part_valid(g_part_valid);
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
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
	switch (g_page_type)
	{
		case 1:						// 隔离
			if (1 == (0x01&(g_isolate>>(lParam))))
			{
				SET_ONE_VALUE(g_isolate, 0, (lParam));
			}
			else
			{
				SET_ONE_VALUE(g_isolate, 1, (lParam));
			}
			g_select = lParam;
			show_win(hDlg);
			break;
			
		case 2:						// 局防有效
			if (1 == (0x01&(g_part_valid>>(lParam))))
			{
				SET_ONE_VALUE(g_part_valid, 0, (lParam));
			}
			else
			{
				SET_ONE_VALUE(g_part_valid, 1, (lParam));
			}
			show_win(hDlg);
			break;
			
		case 3:
			close_page();
			SetAlarmSingleAttr(FrmAlarmWholeAttr.hWnd, lParam);
			break;
			
		case 4:
			SendMessage(g_upperwin, MSG_COMMAND, FrmAlarmAreaList.hWnd, (LPARAM)&lParam);
			close_page();
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
static void OnCommand(HWND hDlg, WPARAM wParam, WPARAM lParam)
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
  Function:			SetAlarmAreaList
  Description: 		安防防区列表窗口
  Input: 		
	1.hDlg   		当前处理的窗口
	2.type			1:隔离 2:局防有效 3:防区属性 4:探头类型
  Output:		
  Return:		
  Others:
*************************************************/
uint32 SetAlarmAreaList(HWND hDlg, uint8 type)
{
	if (FALSE == storage_get_extmode(EXT_MODE_ALARM))
	{
		show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Af_Mode_Unused);
		return FALSE;
	}
	if (DIS_DEFEND != storage_get_defend_state())
	{
		show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Bj_Set_Err);
		return FALSE;
	}

	g_upperwin = hDlg;

	g_select = 0;
	g_page_type = type;
	g_isolate = logic_get_area_isolation();
	g_part_valid  = logic_get_part_valid();

	form_show(&FrmAlarmAreaList);
	FrmAlarmAreaList.Parent->hWnd = g_upperwin;
	
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
	        switch ((int)wParam)
	        {
	        	case RIGHT_SCANCODE_F2:
					if(g_page_type < 2)
					{
						SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYDOWN, wParam, lParam);
					}
					break;
					
		        case RIGHT_SCANCODE_F3:
		        case RIGHT_SCANCODE_F4:
				case RIGHT_SCANCODE_F5:
					SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYDOWN, wParam, lParam);
					break;
					
				default:
					break;		
	        }
 			break;
			
		case MSG_KEYUP:
	        switch ((int)wParam)
	        {
	        	case RIGHT_SCANCODE_F2:
					if (g_page_type < 2)
					{
						SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
					}
					break;
					
				case RIGHT_SCANCODE_F3:
		        case RIGHT_SCANCODE_F4:
				case RIGHT_SCANCODE_F5:
					SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), MSG_KEYUP, wParam, lParam);
		            break;
					
				default:
					break;
	        }
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
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

