/*********************************************************
  Copyright (C), 2006-2016
  File name:	ui_set_ip_protocol.c
  Author:   	caogw
  Version:   	1.0
  Date: 		2015-07-15
  Description:  
  				IP协议转换器界面
  History:            
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"
#include "storage_include.h"

#ifdef _IP_MODULE_
/************************变量定义************************/
static HWND g_rightwin;
static HWND g_listwin;
static uint32 g_ip[1]= {0};
static uint32 g_maincode = 0;					// 存放主机绑定码
static uint32 g_extcode = 0;
static uint32 g_bindstatus; 					// 0 : 未绑定; 1 :已绑定 
static IPMODULE_INFO ipmoduleinfo;
static RIGHTCTRL_INFO g_rightCtrl;
static LISTCTRL_INFO g_listCtrl;
static IPAD_EXTENSION_LIST g_IpadList;

/************************函数定义************************/
CREATE_FORM(FrmIpProtocol);

/*************************************************
  Function:		show_win_1
  Description: 	显示的具体信息
  Input:		无
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
static void show_win(HWND hDlg)
{
	uint32 extenNo = 0;
	char * cOnlineID;
	uint32 i, j, line = 0;
	PLISTITEM_INFO g_info = NULL;	
	char pExtIp[20] = {0};
	char tmp[20] = {0};
	char cExtBuf[50];
	
	SendMessage(g_listwin, WM_List_Clear_Item, 0, 0);
	g_bindstatus = get_ipmodule_bindstate();
	// 获取IPAD分机信息
	get_ipad_extension(&g_IpadList);
	line = 3 + g_IpadList.count;
	g_info = malloc((sizeof(LISTITEM_INFO))*line);

	if (g_info)
	{
		memset(g_info,0,(sizeof(LISTITEM_INFO))*line);
		for (i = 0; i < line; i++)
		{	
			g_info[i].Num = 2;
			g_info[i].Icon = 0;	
			g_info[i].Item[0].Change = FALSE;
			g_info[i].Item[0].ShowType = STYLE_LEFT;
			g_info[i].Item[1].ShowType = STYLE_CENTER;
			if (i < 3)
			{
				g_info[i].Item[0].TextID = SID_Set_binding_status+i;
			}
			
			switch(i)
			{
				case 0:	
					if(g_bindstatus)
					{
						g_info[i].Item[1].TextID = SID_Set_bounded;
					}
					else
					{
						g_info[i].Item[1].TextID = SID_Set_unbounded;
					}
					break;
				
				case 1:
					memset(g_info[i].Item[1].Text ,0,sizeof(g_info[i].Item[1].Text));
					if(!is_main_DeviceNo())
					{
						g_info[i].Icon = BID_ListIcon;
						if(g_bindstatus == 1)
						{
							g_maincode = get_ipmodule_bindcode();
							sprintf(g_info[i].Item[1].Text, "%d", g_maincode);
							break;
						}
						sprintf(g_info[i].Item[1].Text, "%d", g_extcode); 
					}
					else
					{
						g_maincode = get_ipmodule_bindcode();
						sprintf(g_info[i].Item[1].Text, "%d", g_maincode);
					}
					break;
				
				case 2:
					memset(g_info[i].Item[1].Text ,0,sizeof(g_info[i].Item[1].Text));
					memset(tmp, 0, sizeof(tmp));
					if(!is_main_DeviceNo())	
					{	
					 	g_info[i].Icon = BID_ListIcon;				
						if(g_bindstatus == 1)
						{
							change_ip_to_str((get_ipmodule_addr()), tmp);
						}
						else
						{
							change_ip_to_str(g_ip[0], tmp);
						}
					}
					else
					{
						change_ip_to_str((get_ipmodule_addr()), tmp);
					}
					strcpy(g_info[i].Item[1].Text, tmp);
					break;

				case 3:
					for(j = 0; j < g_IpadList.count; j++)
					{
						//获取ipad分机状态
						g_info[j+i].Item[0].TextID = SID_Set_extension_information;
						extenNo = g_IpadList.ipadData[j].devno;	
						memset(pExtIp, 0, sizeof(pExtIp));
						change_ip_to_str(g_IpadList.ipadData[j].ipAddr, pExtIp); 
						if (g_IpadList.ipadData[j].state == 1)
						{
							cOnlineID = get_str(SID_Set_Online);	
						}
						else
						{
							cOnlineID = get_str(SID_Set_Offline);	
						}
						memset(cExtBuf, 0, sizeof(cExtBuf));
						sprintf(cExtBuf, "%s:%d IP:%s %s", get_str(SID_Set_ExtenNo),extenNo, pExtIp, cOnlineID);
						memcpy(g_info[j+i].Item[1].Text, cExtBuf, sizeof(cExtBuf));
					}
					break;

				default:
					break;
			}
		}
		SendMessage(g_listwin, WM_List_Add_Item, line, (LPARAM)g_info);
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
	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(FONT_20));
	show_win(hdc);
	EndPaint(hDlg, hdc);
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
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, SID_Set_generate_binding_code, 0, SID_Right_Sure, 0};
	if(is_main_DeviceNo())
	{
		TextID[1] =  SID_Set_generate_binding_code;
	}
	else
	{
		TextID[1] = SID_Set_binding;
	}
	change_rightctrl_allimagetext(g_rightwin, ImageID, TextID);
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
	int ret;
	uint32 id = SendMessage(g_rightwin, WM_Right_Get_ImageOrTextID, lParam, 0);
	switch (id)
	{	
		//生成绑定码
		case SID_Set_generate_binding_code: 
			start_change_rightkey(hDlg);
			show_msg(hDlg, IDC_Set_Back, MSG_QUERY, SID_Set_generate_binding_code);
			break;
			
		case SID_Set_binding:
			set_ipmodule_bindcode(g_extcode);
			set_ipmodule_addr(g_ip[0]);
			set_ipmodule_bindstate(0);
			ret = ipmodule_request_bind();
			if (ret)
			{
				show_msg(hDlg, IDC_MSG_CTRL, MSG_RIGHT, SID_Set_Bind_OK);
			}
			else
			{
				show_msg(hDlg, IDC_MSG_CTRL, MSG_ERROR, SID_Set_Bind_Fail);
			}
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
	uint32 TextID = get_list_text(g_listwin, lParam, 0, 0, CHECK_TEXTID);
	switch(TextID)
	{	
		case SID_Set_ip_ptcAddr:
			if (!is_main_DeviceNo())
			{
				ui_netparam_dialog_win(hDlg, 10, &g_ip[0], 0);
			}
			break;
			
		case SID_Set_binding_code:
			if (!is_main_DeviceNo())
			{
				ui_binding_code_win(hDlg, &g_extcode);
			}
			break;
			
		default:
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
	uint64 code;
	switch (wParam)
	{
		case IDC_RIGHT_BUTTON:
			RightCtrlCommand(hDlg, lParam);
			break;

		case IDC_LIST_CTRL:
			ListCtrlCommand(hDlg, lParam);
			break;

		case IDC_Set_Back:
			if (lParam == MSG_EVENT_YES)
			{
				srand(time(NULL));
				code = rand()%6553600;		
				if (code < 100000)
				{
					while (code < 100000)
					{
						code = code*2;
					}
				}
				
				if (code > 1000000)
				{
					while (code > 1000000)
					{
						code = code/2;
					}
				}
				g_maincode = code;
				ipmoduleinfo.BindCode = g_maincode;
				ipmoduleinfo.State = 0;
				ipmoduleinfo.IpAddr = 0;
				set_ipmodule_info(&ipmoduleinfo);	
				InvalidateRect(hDlg, NULL, FALSE);
			}
			break;
			
		default:
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
	uint32 i;	
	uint32 ImageID[RIGHT_NUM_MAX] = {0, 0, BID_Right_Table, 0, BID_Right_Exit};
	uint32 TextID[RIGHT_NUM_MAX] = {0, SID_Set_generate_binding_code, 0, SID_Right_Sure, 0};
	
	memset(&g_rightCtrl,0,sizeof(RIGHTCTRL_INFO));
	g_rightCtrl.parent = hDlg;
	g_rightCtrl.VideoMode = 0;
	g_rightCtrl.MaxNum = 5;
	g_rightCtrl.BmpBk = BID_RightCtrlBK;
	g_rightCtrl.TextSize = Font16X16;
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
	g_listCtrl.Drawcallback = (SidebarCallBack)draw_Sidebar; // 侧边栏
	g_listCtrl.fonttype = Font16X16;				// 文字显示大小
	g_listCtrl.Sidebar = 1;							// 不显示侧边栏
	#ifdef _TFT_7_
	g_listCtrl.PageCount = 8;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK8_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK8_0;
	#else
	g_listCtrl.PageCount = 5;						// 显示个数
	g_listCtrl.ImageTopBk = BID_ListCtrl_TopBK5_0;
	g_listCtrl.ImageBk = BID_ListCtrl_BK5_0;
	#endif
	g_listCtrl.HeadInfo.Image = BID_ListCtrl_Title;
	g_listCtrl.HeadInfo.TextID = SID_Set_Prj_Set_IPModule;
	g_listCtrl.Width[0] = 110;
	#ifdef _TFT_7_
	g_listCtrl.Width[1] = 450;
	#else
	g_listCtrl.Width[1] = 200;
	#endif
	
	g_listwin = CreateWindow(AU_LIST_CTRL, "", 
		 		 WS_VISIBLE | BS_NOTIFY | BS_DEFPUSHBUTTON, 
				 IDC_LIST_CTRL, 
				 LIST_CTRL_X,LIST_CTRL_Y, LIST_CTRL_W, LIST_CTRL_H, 
				 hDlg, (DWORD)&g_listCtrl);	
}

/*************************************************
  Function:		ui_ring_win
  Description: 	设置铃声界面
  Input:		
  	1.hDlg
  Output:		无
  Return:		TRUE 成功 FALSE 失败
  Others:		
*************************************************/
uint32 ui_Set_IP_protocol_win(HWND hDlg)
{ 
	g_extcode = get_ipmodule_bindcode();
	g_ip[0] = get_ipmodule_addr();
	form_show(&FrmIpProtocol);
	
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
			if (wParam == SW_SHOWNORMAL)
			{
				uint32 TextID;
				if(is_main_DeviceNo())
				{
					TextID =  SID_Set_generate_binding_code;
				}
				else
				{
					TextID = SID_Set_binding;
				}
				SendMessage(g_rightwin, WM_Right_Set_TextID, 1, (LPARAM)TextID);
				SendMessage(hDlg, WM_Change_RightKey, 0, 0);
			}
			else if (wParam == SW_HIDE)
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

		case WM_Change_RightKey:
			finish_change_rightkey(hDlg);
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
#endif

