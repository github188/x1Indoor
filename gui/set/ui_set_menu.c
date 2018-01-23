/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_menu.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		14-10-09
  Description:  
  				设置主界面
  History:            
*********************************************************/

#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/************************常量定义************************/
#define MAX_PAGE1_NUM		4						// 设置界面
#if (_UI_STYLE_ == _E81S_UI_STYLE_)
#define MAX_PAGE2_NUM		5						// 系统设置
#else
#define MAX_PAGE2_NUM		4						// 系统设置
#endif
#define MAX_PAGE3_NUM		8						// 工程设置
#define MAX_PAGE4_NUM		4						// 安防设置
#ifdef _IP_MODULE_
#define MAX_PAGE5_NUM		4						// 用户设置
#else
#define MAX_PAGE5_NUM		3						// 用户设置
#endif

static uint32 g_page1[MAX_PAGE1_NUM][2] = 
{
	{BID_Set_SYS,				SID_Set_Menu_System},       
	{BID_Set_PRJ,  				SID_Set_Menu_Project},      
	{BID_Set_AF,				SID_Set_Menu_Alarm},     
	{BID_Set_USER,				SID_Set_Menu_User}    
};

static uint32 g_page2[MAX_PAGE2_NUM][2] = 
{
	{BID_Set_Language,			SID_Set_System_language},  	
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	{BID_Set_Date,				SID_Set_System_Time},	
	#endif
	{BID_Set_Screen_Light,		SID_Set_Screen_Lcd},
	{BID_Set_SDcard,			SID_Set_System_Storage}, 
	{BID_Set_SYSInfo,			SID_Set_System_Info}
};

static uint32 g_page3[MAX_PAGE3_NUM][2] = 
{
	{BID_Set_PrjPass,			SID_Set_Prj_Pass},       
	{BID_Set_AFproperty, 		SID_Set_Prj_Alarm},  
	{BID_Set_Netparam,		  	SID_Set_Prj_Net},   
	{BID_Set_Ext, 				SID_Set_Prj_ExtMode}, 
	{BID_Set_Room,				SID_Set_Prj_DevNo}, 
	{BID_Set_Netdoor,			SID_Net_Door_First}, 
	{BID_Set_Netdoor_Second,	SID_Net_Door_Second}, 
	{BID_Set_Back,				SID_Set_Prj_Default}
};

static uint32 g_page4[MAX_PAGE4_NUM][2] = 
{
	{BID_BJ_Set_Pass1,			SID_Set_Alarm_UserPwd},             
	{BID_BJ_Set_Isolate1, 		SID_Set_Alarm_AfGeLi}, 
	{BID_BJ_Set_Joint1,			SID_Set_Alarm_AfLink},   
	{BID_BJ_Set_Lock_Pass1,  	SID_Set_Alarm_DoorPwd}
};

static uint32 g_page5[MAX_PAGE5_NUM][2] = 
{
	#ifdef _IP_MODULE_
	{BID_Set_IP_protocol,		SID_Set_Prj_Set_IPModule},
	#endif
	{BID_Set_Melody,			SID_Set_User_Sound},       
	{BID_Set_Volume,  			SID_Set_User_Volume},  
	{BID_Set_LYLY,  			SID_Set_User_LyLy}
};

/************************变量定义************************/
static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static LISTCTRL_INFO g_listCtrl;					// 列表控件信息
static uint8 g_pageindex = 0;
static HWND  g_rightwin;
static HWND  g_listwin;

/************************函数定义************************/
CREATE_FORM(FrmSet);

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
			return MAX_PAGE1_NUM;

		case 1:
			return MAX_PAGE2_NUM;

		case 2:
			return MAX_PAGE3_NUM;	

		case 3:
			return MAX_PAGE4_NUM;	

		case 4:
			if (is_main_DeviceNo())					// 分机不显示留影留言设置
			{
				return MAX_PAGE5_NUM;
			}
			else
			{
				return MAX_PAGE5_NUM-1;
			}

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
			if (index < MAX_PAGE1_NUM)
			{
				return g_page1[index][mode];
			}
			break;	

		case 1:
			if (index < MAX_PAGE2_NUM)
			{
				return g_page2[index][mode];
			}
			break;	

		case 2:
			if (index < MAX_PAGE3_NUM)
			{
				return g_page3[index][mode];
			}
			break;		

		case 3:
			if (index < MAX_PAGE4_NUM)
			{
				return g_page4[index][mode];
			}
			break;		

		case 4:	
			if (index < MAX_PAGE5_NUM)
			{
				return g_page5[index][mode];
			}
			break;		

		default:
			return 0;
	}

	return 0;
}

/*************************************************
  Function:		show_win
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win(HWND hDlg, uint8 page)
{
	uint32 i,max;
	PLISTITEM_INFO g_info = NULL;
	uint32 TextID[MAX_PAGE1_NUM+1] = {SID_MainSet,SID_Set_Menu_System,SID_Set_Menu_Project,SID_Set_Menu_Alarm,SID_Set_Menu_User};
	uint8 Icon[MAX_PAGE1_NUM+1] = {MAX_PAGE1_NUM,MAX_PAGE2_NUM,MAX_PAGE3_NUM,MAX_PAGE4_NUM,MAX_PAGE5_NUM};

	max = get_page_max(page);
	g_pageindex = page;
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	set_list_text(g_listwin, 0, 0, TextID[page], NULL, NULL, CHECK_HEAD_TEXTID);
	g_info = malloc((sizeof(LISTITEM_INFO))*max);
	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*max);
		for (i = 0; i < max; i++)
		{
			if (i < Icon[page])
			{
				g_info[i].Icon = BID_ListIcon;
			}
			g_info[i].Num = 2;
			g_info[i].Item[0].Change = FALSE;
			g_info[i].Item[0].Image = get_page_iconortext(page, i, 0);
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[1].TextID = get_page_iconortext(page, i, 1);			
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
	SetBrushColor(hdc, PAGE_BK_COLOR);
	FillBox(hdc, rc.left, rc.top, rc.right, rc.bottom);
	show_win(hDlg, g_pageindex);
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
	uint8 devIndex = 1;
	uint32 Image = get_list_Image(g_listwin, lParam, 0, CHECK_IMAGE);
	
	switch (Image)
	{
		case BID_Set_SYS:
			show_win(hDlg, 1);
			break;

		case BID_Set_PRJ:
			show_msg_pass(hDlg, IDC_Msg_Admin, PASS_TYPE_ADMIN, 1);
			break;

		case BID_Set_AF:
			show_msg_pass(hDlg, IDC_Msg_User, PASS_TYPE_USER, 1);
			break;

		case BID_Set_USER:
			show_win(hDlg, 4);
			break;

		// 系统设置
		case BID_Set_Language:
			ui_language_win(hDlg);
			break;
			
		case BID_Set_Screen_Light:
			ui_screensave_win(hDlg);
			break;
			
		case BID_Set_SDcard:
			ui_storage_win(hDlg);
			break;

		case BID_Set_SYSInfo:
			ui_sysinfo_win(hDlg);
			break; 	

		case BID_Set_Date:
			#ifdef _USE_TIME_SET_
			ui_datetime_win(hDlg);
			#endif
			break; 	

		// 工程设置
		case BID_Set_PrjPass:
			ui_password_win(PASS_TYPE_ADMIN);
			break;

		case BID_Set_AFproperty:
			SetAlarmWholeAttr(hDlg);
			break;	
		
		case BID_Set_Netparam:
			ui_netparam_win(hDlg);
			break;

		case BID_Set_Ext:
			ui_extparam_win(hDlg);
			break;
		
		case BID_Set_Room:
			ui_devno_win(hDlg);
			break;

		case BID_Set_Netdoor:
			devIndex = 1;
			if (FALSE == storage_get_extmode(EXT_MODE_ALARM+devIndex))
			{
                show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Net_Door_Unused_First);
			}
			else
			{
				ui_set_netdoor_win(hDlg, devIndex);
			}
			break;	

		// 网络门前机2
		case BID_Set_Netdoor_Second:
			devIndex = 2;
			if (FALSE == storage_get_extmode(EXT_MODE_ALARM+devIndex))
			{
                show_msg(hDlg, IDC_MSG_CTRL, MSG_WARNING, SID_Net_Door_Unused_Second);
			}
			else
			{
                ui_set_netdoor_win(hDlg, devIndex);
			}
			break;	
		
		case BID_Set_Back:
			show_msg(hDlg, IDC_Set_Back, MSG_QUERY, SID_Msg_FactoryMode);
			break;	
			
		// 安防设置
		case BID_BJ_Set_Pass1:
			SetLockPwd(hDlg, 0);
			break;
			
     	case BID_BJ_Set_Isolate1:
			SetAlarmAreaList(hDlg, 1);
			break;
			
     	case BID_BJ_Set_Joint1:
			SetAlarmLink(hDlg);
			break;
			
   	 	case BID_BJ_Set_Lock_Pass1:
			SetLockPwd(hDlg, 1);
			break;

		// 用户设置	
		case BID_Set_Melody:
			ui_ring_win(hDlg);
			break;

		case BID_Set_Volume:
			ui_volume_win(hDlg);
			break;	
			
		case BID_Set_LYLY:
			ui_lyly_win(hDlg);
			break;

		#ifdef _IP_MODULE_
		case BID_Set_IP_protocol:
			ui_Set_IP_protocol_win(hDlg);
			break;
		#endif	
			
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
			if (0 == g_pageindex)
			{	
				close_page();
			}
			else
			{
				show_win(hDlg, 0);
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

		case IDC_Msg_Admin:
			//log_printf("WID_Msg_Admin------ %d\n",lParam);
			if (lParam == MSG_EVENT_YES || lParam == MSG_EVENT_XIECHI)
			{
				show_win(hDlg, 2);
				if (lParam == MSG_EVENT_XIECHI)
				{
					log_printf("xiechi----------------\n");
					force_alarm_report();
				}
			}
			break;

		case IDC_Msg_User:
			if (lParam == MSG_EVENT_YES || lParam == MSG_EVENT_XIECHI)
			{
				show_win(hDlg, 3);
				if (lParam == MSG_EVENT_XIECHI)
				{
					log_printf("xiechi----------------\n");
					force_alarm_report();
				}
			}
			break;

		case IDC_Set_Back:
			if (lParam == MSG_EVENT_YES)
			{
				storage_recover_factory();
				//sys_sync_hint_state();
				show_msg(hDlg, IDC_Msg_Notice, MSG_INFORMATION, SID_Msg_Recover_OK);
				ui_msg_reboot();
			}
			break;

		case IDC_Msg_Notice:
			//GuiDspDeInitialize();
			//GuiDspInitialize();

		default:
			break;
	}
}

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
	add_rightctrl_win(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), RIGHT_KEY2, GetDlgItem(hDlg, IDC_LIST_CTRL), WM_List_Go_NextSel, 0, 0);
	add_rightctrl_win(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), RIGHT_KEY3, GetDlgItem(hDlg, IDC_LIST_CTRL), WM_List_OkKey, 0, 0);
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
	g_listCtrl.Sidebar = 1;							// 侧边栏
	g_listCtrl.SideBarMode = 1;
	g_listCtrl.KeyCallBack = (SidebarKeyCallBack)get_Sidebar_key;
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar;// 侧边栏
	g_listCtrl.fonttype = LIST_CTRL_FONT;				// 文字显示大小
	g_listCtrl.Width[0] = 88;
	g_listCtrl.Width[1] = 250;
	#if (_UI_STYLE_ == _E81S_UI_STYLE_)
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_MainSet;

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
			if (wParam != SW_HIDE)
			{
				g_pageindex = 0;
			}
			else
			{
				close_page();
			}
			break;
			
		case MSG_PAINT:
			OnPaint(hDlg);
			break;

		case MSG_LBUTTONDOWN:
    		get_key_down(hDlg, LOWORD(lParam), HIWORD(lParam));
   			break;
   			
	    case MSG_LBUTTONUP:
	        get_key_up(hDlg, LOWORD(lParam), HIWORD(lParam));
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
			//DestroyMainWindow(hDlg);
			//PostQuitMessage(hDlg);
			return 0;
		
		case MSG_DESTROY:
			return 0;
	}
	
	return DefaultMsgProc(self, hDlg, message, wParam, lParam);
}

