/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_set_alarm_link.c
  Author:   	yanjl
  Version:   	1.0
  Date: 		2014-11-14
  Description:  
  				安防联动界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

static HDLG g_rightwin = 0;
static HDLG g_listwin = 0;

static RIGHTCTRL_INFO g_rightCtrl;					// 右边控件信息
static LISTCTRL_INFO  g_listctrl;	

static uint8 g_link;
static uint8 CurPage = 0;
static uint8 ScenLink[2];

CREATE_FORM(FrmAlarmLink);

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
	uint8 item = 0;
	uint32 i, max;
	uint8 BF_Mode, CF_Mode;
	
	PLISTITEM_INFO g_info = NULL;
	uint16  text_id[8] = {SID_AfJoint_Tk_Chefang, SID_AfJoint_Mk_Chefang, SID_AfJoint_DoorRing, 
		SID_AfJoint_ScenBf, 0, SID_AfJoint_ScenCf, 0, SID_Auto_Capture};
	uint16  image_id[2] = {BID_Com_UnChose, BID_Com_Chose};
	uint32 typeStrId[JD_SCENE_MAX] = {SID_Jd_Mode_Huike, SID_Jd_Mode_Jiucan,
		SID_Jd_Mode_Yejian, SID_Jd_Mode_Jiedian, SID_Jd_Mode_Putong};

	#ifdef _JD_MODE_
	#ifdef _AUTO_CAPTURE_
	max = 8;
	#else
	max = 7;
	#endif
	#else
	#ifdef _AUTO_CAPTURE_
	max = 4;
	text_id[3] = SID_Auto_Capture; 
	#else
	max = 3;
	#endif
	#endif
	
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_info = malloc((sizeof(LISTITEM_INFO)) * max);
	if (g_info)
	{
		memset(g_info, 0, (sizeof(LISTITEM_INFO)) * max);
		for (i = 0; i < max; i++)
		{
			g_info[i].Num = 2;
			#ifdef _JD_MODE_
			#ifdef _AUTO_CAPTURE_
			BF_Mode = max - 4;
			CF_Mode = max - 2;
			#else
			CF_Mode = max - 1;
			BF_Mode = max - 3;
			#endif
			if (i == BF_Mode)
			{
				g_info[i].Item[0].Image = 0;
				g_info[i].Item[1].TextID = typeStrId[ScenLink[0]];
			}
			else if (i == CF_Mode)
			{
				g_info[i].Item[0].Image = 0;
				g_info[i].Item[1].TextID = typeStrId[ScenLink[1]];
			}
			else
			{
				if (i > BF_Mode)
				{
					#ifdef _AUTO_CAPTURE_
					if (i > CF_Mode)
					{
						item = i - 2;
					}
					else
					#endif
					{
						item = i - 1;
					}
				}
				else
				{
					item = i;
				}
				g_info[i].Item[0].Image = image_id[(g_link>>item)&0x01];
				g_info[i].Item[1].TextID = text_id[i];
			}
			#else
			g_info[i].Item[0].Image = image_id[(g_link>>i)&0x01];
			g_info[i].Item[1].TextID = text_id[i];
			#endif
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[1].ShowType = STYLE_CENTER;
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
	g_listctrl.Parent = hDlg;
	g_listctrl.Mode = LIST_MODE_COMMON;				// 抬起释放，没有高亮显示
	g_listctrl.Caption = 1;							// 是否有标题栏
	g_listctrl.Sidebar = 0;							// 侧边栏
	g_listctrl.SideBarMode = 1;
	g_listctrl.Drawcallback = NULL;					// 侧边栏
	g_listctrl.fonttype = Font16X16;				// 文字显示大小
	g_listctrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listctrl.HeadInfo.TextID = SID_Set_Alarm_AfLink;
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
				 LIST_CTRL_X, LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
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
	uint32 TextID[RIGHT_NUM_MAX] = {0, SID_Right_Save, 0, SID_Right_Sure,  0};

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

#if 0
/*************************************************
  Function:		change_key_function
  Description: 	将确认按钮改为保存按钮
  Input:		无
  Output:		无
  Return:		TRUE 是 FALSE 否
  Others:		
*************************************************/
static void change_key_function(HWND hDlg, WPARAM index)
{
	//uint32 Image1, TextID1;
	
	// 将确认按钮改为保存
	//TextID1 = SID_Right_Save;
	//SendMessage(g_rightwin, WM_Right_Set_TextID, index, &TextID1);
	SendMessage(g_listwin, WM_List_Set_HeadText, SID_Set_Alarm_AfLink, 0);
	//CurPage = 0;
	//show_win(hDlg);
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
	uint32 id;
	id = SendMessage(GetDlgItem(hDlg, IDC_RIGHT_BUTTON), WM_Right_Get_ImageOrTextID, lParam, 0);
	
	switch (id)
	{
		case SID_Right_Save:
			storage_set_link_param(g_link);
			storage_set_link_scene(ScenLink);
			show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Save_OK);
			break;
			
		case SID_Right_Sure:
			break;
			
		case BID_Right_Exit:	
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
			
		default:
			break;
	}
}

/*************************************************
  Function:		show_scene
  Description: 	显示情景模式
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_scene(void)
{
	uint8 i, max = JD_SCENE_MAX;
	//uint32 Image, TextID;
	PLISTITEM_INFO listItem = NULL;
	uint32 typeId[JD_SCENE_MAX] = {JD_SCENE_RECEPTION, JD_SCENE_MEALS,
		JD_SCENE_NIGHT, JD_SCENE_POWERSAVING, JD_SCENE_GENERAL};
	uint32 typeStrId[JD_SCENE_MAX] = {SID_Jd_Mode_Huike, SID_Jd_Mode_Jiucan,
		SID_Jd_Mode_Yejian, SID_Jd_Mode_Jiedian, SID_Jd_Mode_Putong};
	
	// 将保存按钮改为确认按钮
	//TextID = SID_Right_Sure;
	//SendMessage(g_rightwin, WM_Right_Set_TextID, 0, &TextID);

	// 清列表框内容
	SendMessage(g_listwin, WM_List_Set_HeadText, SID_Jd_Set_Scene, 0);
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
			if (ScenLink[CurPage-1] == typeId[i])
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
		SendMessage(g_listwin, WM_List_Set_Sel, ScenLink[CurPage - 1], 0);
		free(listItem);
		listItem = NULL;
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
	uint8 itemnum;
	uint8 BF_Mode, CF_Mode;
	uint32 max;
	
	#ifdef _JD_MODE_
	#ifdef _AUTO_CAPTURE_
	max = 8;
	#else
	max = 7;
	#endif
	#else
	#ifdef _AUTO_CAPTURE_
	max = 4;
	#else
	max = 3;
	#endif
	#endif
	
	if (0 == CurPage)
	{
		#ifdef _JD_MODE_
		#ifdef _AUTO_CAPTURE_
		BF_Mode = max - 4;
		CF_Mode = max - 2;
		if (lParam == BF_Mode + 1)
		{
			itemnum = lParam - 1;
		}
		else if (lParam == CF_Mode + 1)
		{
			itemnum = lParam - 2;
		}
		else
		{
			itemnum = lParam;
		}
		#else
		CF_Mode = max - 1;
		BF_Mode = max - 3;
		if (lParam == BF_Mode+1)
		{
			itemnum = lParam - 1;
		}
		else
		{
			itemnum = lParam;
		}
		#endif
		if (lParam == BF_Mode)
		{
			CurPage = 1;
			show_scene();
		}
		else if (lParam == CF_Mode)
		{
			CurPage = 2;
			show_scene();
		}
		#else
		itemnum = lParam;
		#endif

		#ifdef _JD_MODE_
		else if (1 == ((g_link >> itemnum) & 0x01))
		#else 
		if (1 == ((g_link >> itemnum) & 0x01))
		#endif
		{
			SET_ONE_VALUE(g_link, 0, itemnum);
			set_list_Image(g_listwin, lParam, 0, BID_Com_UnChose, CHECK_IMAGE);
		}
		else
		{
			SET_ONE_VALUE(g_link, 1, itemnum);
			set_list_Image(g_listwin, lParam, 0, BID_Com_Chose, CHECK_IMAGE);
		}
	}
	else
	{
		ScenLink[CurPage-1] = lParam;
		show_scene();
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
			RightCtrlCommand(hDlg, lParam);
			break;

		case IDC_LIST_CTRL:
			ListCtrlCommand(hDlg, lParam);
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		SetAlarmLink
  Description: 	联动参数主窗口
  Input: 		无    
  Output:		
  Return:		
  Others:
*************************************************/
uint32 SetAlarmLink(HWND hDlg)
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
	
	form_show(&FrmAlarmLink);
	g_link = storage_get_link_param();
	storage_get_link_scene(ScenLink);

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

