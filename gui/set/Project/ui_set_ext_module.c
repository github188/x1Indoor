/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_ext_module.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		14-11-14
  Description:  
  				外部模块界面设置
  History:                 
    1. Date:	2011-06-11
       Author:	luofl
       Modification:
       			增加家电厂商设置,目前支持X10,PLC-Bus两种
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

static HDLG g_rightwin = 0;
static HDLG g_listwin = 0;
//static HDLG g_upperwin = 0;							// 上一级窗口

static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static LISTCTRL_INFO  g_listctrl;	

#ifdef _JD_MODE_
	#ifdef _DOOR_PHONE_
		#define MAX_EXT_MODE			EXT_MODE_MAX
	#else
		#define MAX_EXT_MODE			(EXT_MODE_MAX-2)
	#endif
#else
	#ifdef _DOOR_PHONE_
		#define MAX_EXT_MODE			(EXT_MODE_MAX - 2)
	#else
		#define MAX_EXT_MODE			(EXT_MODE_MAX - 4)
	#endif
#endif

static uint32 g_ext[EXT_MODE_MAX];
static uint8 CurPage = 0;
//static uint8 JdFactory;
static char JdFactoryStr[JD_FACTORY_MAX+1][40] = {{0}};

CREATE_FORM(FrmExtModeParam);

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
	uint32 i, max = MAX_EXT_MODE;
	PLISTITEM_INFO g_info = NULL;
	uint16 space[3] = {0x0020, 0};
	uint16 maohao[2] = {0xff1a, 0};

	#ifdef _JD_MODE_
		#ifdef _DOOR_PHONE_
			uint32 TextID[MAX_EXT_MODE] = {SID_Set_Ext_Alarm,SID_Net_Door_First,SID_Net_Door_Second, 
			SID_Net_Door_First_A,SID_Net_Door_Second_A,SID_Set_Ext_JiaDian, SID_Jd_Factory_Set,SID_Set_Ext_Stair};
		#else
			uint32 TextID[MAX_EXT_MODE] = {SID_Set_Ext_Alarm,SID_Net_Door_First,SID_Net_Door_Second, 
			SID_Set_Ext_JiaDian, SID_Jd_Factory_Set,SID_Set_Ext_Stair};

		#endif
	#else
		#ifdef	_DOOR_PHONE_
			uint32 TextID[MAX_EXT_MODE] = {SID_Set_Ext_Alarm,SID_Net_Door_First,SID_Net_Door_Second, 
			SID_Net_Door_First_A,SID_Net_Door_Second_A,SID_Set_Ext_Stair};
		#else
			uint32 TextID[MAX_EXT_MODE] = {SID_Set_Ext_Alarm,SID_Net_Door_First,SID_Net_Door_Second,SID_Set_Ext_Stair};
		#endif
	#endif
	
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);

		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			g_info[i].Icon = 0;
			g_info[i].Item[0].Change = FALSE;

			#ifdef _JD_MODE_
				#ifdef _DOOR_PHONE_
				if (i != EXT_MODE_JD_FACTORY)
				{
					if (g_ext[i])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					g_info[i].Item[1].TextID = TextID[i];			
					g_info[i].Item[1].ShowType = STYLE_LEFT;
				}
				// 家电厂商不显示选项框
				else
				{
					g_info[i].Item[0].Image = 0;		
					g_info[i].Item[1].ShowType = STYLE_LEFT;
					memset(g_info[i].Item[1].WText, 0, sizeof(g_info[i].Item[1].WText));
					memcpy(g_info[i].Item[1].WText, JdFactoryStr[JD_FACTORY_MAX], sizeof(JdFactoryStr[0]));
					strcat(g_info[i].Item[1].WText, maohao);
					strcat(g_info[i].Item[1].WText, space);
					strcat(g_info[i].Item[1].WText, JdFactoryStr[g_ext[EXT_MODE_JD_FACTORY]]);
				}
				#else
				if (i < 3)
				{
					if (g_ext[i])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
					g_info[i].Item[0].ShowType = STYLE_LEFT;
					g_info[i].Item[1].TextID = TextID[i];			
					g_info[i].Item[1].ShowType = STYLE_LEFT;
				}
				else
				{
					if (i != EXT_MODE_JD_FACTORY)
					{
						if (g_ext[i+2])
						{
							g_info[i].Item[0].Image = BID_Com_Chose;
						}
						else
						{
							g_info[i].Item[0].Image = BID_Com_UnChose;
						}
						g_info[i].Item[0].ShowType = STYLE_LEFT;
						g_info[i].Item[1].TextID = TextID[i];			
						g_info[i].Item[1].ShowType = STYLE_LEFT;
					}
					// 家电厂商不显示选项框
					else
					{
						g_info[i].Item[0].Image = 0;		
						g_info[i].Item[1].ShowType = STYLE_LEFT;
						memset(g_info[i].Item[1].WText, 0, sizeof(g_info[i].Item[1].WText));
						memcpy(g_info[i].Item[1].WText, JdFactoryStr[JD_FACTORY_MAX], sizeof(JdFactoryStr[0]));
						strcat(g_info[i].Item[1].WText, maohao);
						strcat(g_info[i].Item[1].WText, space);
						strcat(g_info[i].Item[1].WText, JdFactoryStr[g_ext[EXT_MODE_JD_FACTORY]]);
					}
				}
				
				#endif
				
			#else
				#ifdef _DOOR_PHONE_
				if (i > 4)
				{
					if (g_ext[i+2])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
				}
				else
				{
					if (g_ext[i])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
				}
				#else
				if (i > 2)
				{
					if (g_ext[i+4])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
				}
				else
				{
					if (g_ext[i])
					{
						g_info[i].Item[0].Image = BID_Com_Chose;
					}
					else
					{
						g_info[i].Item[0].Image = BID_Com_UnChose;
					}
				}
				#endif
				g_info[i].Item[0].ShowType = STYLE_LEFT;
				g_info[i].Item[1].TextID = TextID[i];			
				g_info[i].Item[1].ShowType = STYLE_LEFT;
			#endif
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

#if 0
/*************************************************
  Function:		show_jd_factory
  Description: 	显示家电厂商信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_jd_factory(HWND hDlg)
{
	uint8 i, max = JD_FACTORY_MAX;
	//uint32 TextID;
	PLISTITEM_INFO listItem = NULL;

	uint32 typeId[JD_FACTORY_MAX] = {JD_FACTORY_X10, JD_FACTORY_ACBUS};
	uint32 typeStrId[JD_FACTORY_MAX] = {SID_Jd_Factory_X10, SID_Jd_Factory_ACBUS};
	
	// 将保存按钮改为确认按钮
	//TextID = SID_Right_Sure;
	//SendMessage(g_rightwin, WM_Right_Set_TextID, 0, (LPARAM)&TextID);

	// 清列表框内容
	SendMessage(g_listwin, WM_List_Set_HeadText, SID_Jd_Factory_Set, 0);
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	listItem = malloc((sizeof(LISTITEM_INFO))*max);
	if (listItem)
	{
		memset(listItem, 0, (sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			listItem[i].Num = 2;
			listItem[i].Item[0].TextID = 0;
			listItem[i].Item[0].ShowType = STYLE_LEFT;
			listItem[i].Item[0].Image = 0;
			if (g_ext[EXT_MODE_JD_FACTORY] == typeId[i])
			{
				listItem[i].Item[0].Image = BID_Set_Sel;
			}
			listItem[i].Icon = 0;
			listItem[i].Item[1].Change = FALSE;
			listItem[i].Item[1].Image = 0;
			listItem[i].Item[1].ShowType = STYLE_LEFT;
			listItem[i].Item[1].TextID = typeStrId[i];
		}
		
		SendMessage(g_listwin, WM_List_Add_Item, max, (LPARAM)listItem);
		free(listItem);
		listItem = NULL;
	}
} 
#endif

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
	uint32 Image;
	uint32 id;
	
	#ifdef _JD_MODE
		#ifdef _DOOR_PHONE_
			id = lParam;
		#else
			if (lParam > 2)
			{
				id = lParam + 2;
			}
			else
			{
				id = lParam;
			}
		#endif
	#else
		#ifdef _DOOR_PHONE_
			if (lParam > 4)
			{
				id = lParam + 2;
			}
			else
			{
				id = lParam;
			}
		#else
			if (lParam > 2)
			{
				id = lParam+4;
			}
			else
			{
				id = lParam;
			}
		#endif
	#endif
	
	if (0 == CurPage)
	{
		if (id < EXT_MODE_MAX)
		{
			if (TRUE == g_ext[id])
			{
				g_ext[id] = FALSE;
				Image = BID_Com_UnChose;
			}
			else
			{
				g_ext[id] = TRUE;
				Image = BID_Com_Chose;
			}
			set_list_Image(g_listwin, lParam, 0, Image, CHECK_IMAGE);
		}
	}
}

/*************************************************
  Function:		save_param
  Description: 	保存
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void save_param(HWND hDlg)
{
	uint8 i;
	JD_FACTORY_TYPE factory = storage_get_extmode(EXT_MODE_JD_FACTORY);
	uint8 reset = 0;

	for (i = 0; i < EXT_MODE_MAX; i++)
	{	
		if (i == (EXT_MODE_MAX-1))
		{	
			storage_set_extmode(1, EXT_MODE_ALARM+i, g_ext[i]);
		}
		else
		{
			storage_set_extmode(0, EXT_MODE_ALARM+i, g_ext[i]);	
		}
	}

	#ifdef _JD_MODE_
	if (storage_get_extmode(EXT_MODE_JD))
	{
		jd_logic_init();
		#ifdef _AURINE_ELEC_NEW_
	    jd_aurine_init();
	    #endif

		#ifdef _IP_MODULE_JD_
		if (get_ipmodule())
		{
			if (factory != g_ext[EXT_MODE_JD_FACTORY])
			{
				reset = 1;
			}
		}
		#endif
	}
	#endif

	if (reset)
	{
		show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Msg_Reset);	
		ui_msg_reboot();

	}
	else
	{
		show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
	}
}

#if 0
/*************************************************
  Function:		change_key_function
  Description: 	将确认按钮改为保存按钮
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void change_key_function(HWND win)
{
	//uint32 Image1, TextID1;
	
	// 将确认按钮改为保存
	//TextID1 = SID_Right_Save;
	//SendMessage(g_rightwin, WM_Right_Set_TextID, 0, &TextID1);
	SendMessage(g_listwin, WM_List_Set_HeadText, SID_Set_Prj_ExtMode, 0);
	//CurPage = 0;
	//show_win(win);
}
#endif

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
	uint32 id = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	
	switch(id)
	{
		case SID_Right_Save:
			save_param(hDlg);
			break;
			
		case SID_Right_Sure:						// 确认
			//change_key_function(hDlg);
			break;
		
		case BID_Right_Exit:						// 退出
			if (CurPage != 0)
			{
				CurPage = 0;
				show_win(hDlg);
			}
			else
			{
				close_page();
			}
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

		#if 0
		case WID_MsgNotice:
			FS_fdelete(JD_DIR_PATH);
			sys_close_lcd();
			OSTimeDly(2);
			SystemReboot();			
			break;
		#endif

		default:
			break;
	}
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
	g_listctrl.Parent = hDlg;
	g_listctrl.Mode = LIST_MODE_COMMON;				// 抬起释放，没有高亮显示
	g_listctrl.Caption = 1;							// 是否有标题栏
	g_listctrl.SideBarMode = 1;
	g_listctrl.KeyCallBack = NULL;
	g_listctrl.Drawcallback = (SidebarCallBack)draw_Sidebar;// 侧边栏
	g_listctrl.fonttype = Font16X16;				// 文字显示大小
	g_listctrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listctrl.HeadInfo.TextID = SID_Set_Prj_ExtMode;
	g_listctrl.Width[0] = 70;
	g_listctrl.Width[1] = 250;

	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listctrl.PageCount = 8;						// 显示个数
	g_listctrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listctrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listctrl.PageCount = 5;						// 显示个数
	g_listctrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listctrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif

	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
				 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listctrl);	
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
	uint32 TextID[RIGHT_NUM_MAX] = {0, SID_Right_Save, 0, SID_Right_Sure, 0};

	memset(&g_rightCtrl, 0, sizeof(RIGHTCTRL_INFO));
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

	g_rightwin = CreateWindow(AU_RIGHT_BUTTON, "", 
				 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_RIGHT_BUTTON, 
				 RIGHT_CTRL_X, RIGHT_CTRL_Y, RIGHT_CTRL_W, RIGHT_CTRL_H, 
				 hDlg, (DWORD)&g_rightCtrl);

	Init_Key(hDlg);
}

/*************************************************
  Function:		ui_extparam_win
  Description: 	设置外部模块主界面
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_extparam_win(HWND hDlg)
{ 
	uint8 i;
    uint16 text_id[JD_FACTORY_MAX] = {SID_Jd_Factory_X10, SID_Jd_Factory_ACBUS};
	
	// 根据ID获取字符串
	for (i = 0; i < JD_FACTORY_MAX; i++)
	{
		strcpy(JdFactoryStr[i], get_str(text_id[i]));
	}
	strcpy(JdFactoryStr[JD_FACTORY_MAX], get_str(SID_Jd_Factory));

	// 取外部模块启用状态
	for (i = 0; i < EXT_MODE_MAX-1; i++)
	{
		g_ext[i] = storage_get_extmode(EXT_MODE_ALARM+i);
	}
	g_ext[EXT_MODE_MAX-1] = storage_get_extmode(EXT_MODE_GENERAL_STAIR);

	#if 0
	for (i = 0; i < EXT_MODE_MAX; i++)
	{
		log_printf("g_ext[%d] = %d i =%d\n",i, g_ext[i]);
	}
	#endif
	
	form_show(&FrmExtModeParam);

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
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

