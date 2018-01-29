/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name:	ui_spin_ctrl.c
  Author:   	caogw	
  Version:   	2.0
  Date: 		2014-11-27
  Description:  
                IP控件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#include "gui_include.h"

#if (_LCD_DPI_ == _LCD_800480_)
#define SPIN_ICON_W				50
#define SPIN_ICON_DISX			16
#define SPIN_ICON_XPOS			36 					// 窗口起始坐标到第一个输入框的距离 
#define SP_TEXT_XPOS			47
#define SP_TEXT_YPOS			5
#elif (_LCD_DPI_ == _LCD_1024600_)
#define SPIN_ICON_W				64
#define SPIN_ICON_DISX			24
#define SPIN_ICON_XPOS			38 					// 窗口起始坐标到第一个输入框的距离 
#define	 SP_TEXT_XPOS			53
#define SP_TEXT_YPOS			10
#endif

/*************************************************
  Function:		ui_reflash_ctrl
  Description: 	刷新控件
  Input:	
  	1.hDlg		窗口句柄
  	2.wParam 	参数1
   	3.lParam    参数2	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash_ctrl(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hDlg, NULL, FALSE);
}

/*************************************************
  Function:		draw_mode_ip
  Description: 	无
  Input:		
    1.hDlg		句柄
   	2.hdc       句柄	
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 draw_mode_ip (HWND hDlg, HDC hdc)
{
	uint8 i;
	RECT TextRc;
	char Text[30];
	uint32 xpos = 0;

	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	SetTextColor(hdc, COLOR_BLACK);
	SelectFont(hdc, GetFont(WIN_COMM_FONT));
	
	#if 0
	Image[0] = pData->ImageId[0];
	Image[1] = pData->ImageId[1];
	
	if(pData->Enable)
	{
		if(pData->index != 0 && pData->state == KEYDOWN)
		{
			Image[pData->index] += BIDON_NUM; 
		}
	}
	else
	{
		Image[0] = pData->ImageId[0] + BIDUNENABLE_NUM;
		Image[1] = pData->ImageId[1] + BIDUNENABLE_NUM;
	}
	#endif
	DrawBmpID(hdc, 0, 0, 0, 0, pData->ImageBK);

	for (i = 0; i < 4; i++)
	{
		if (pData->Value == i)
		{
			xpos =  SPIN_ICON_XPOS +(i*(SPIN_ICON_W + SPIN_ICON_DISX));
			if (pData->Value == 3)
			{
				#if (_LCD_DPI_ == _LCD_800480_)
				DrawBmpID(hdc, xpos - 1, 0, 0, 0, pData->ImageIpSel);
				#elif (_LCD_DPI_ == _LCD_1024600_)
				DrawBmpID(hdc, xpos - 1, 0, 0, 0, pData->ImageIpSel);
				#endif
			}
			else
			{
				DrawBmpID(hdc, xpos, 0, 0, 0, pData->ImageIpSel);
			}
		}

		memset(Text,0,sizeof(Text));
		sprintf(Text, "%d", pData->IP[i]);
		TextRc.left = SP_TEXT_XPOS + (i*(SPIN_ICON_W + SPIN_ICON_DISX));
		TextRc.top = SP_TEXT_YPOS;
		TextRc.right = TextRc.left + 30;
		TextRc.bottom = TextRc.top + 30;
		
		DrawTextFont(hdc, Text, -1, &TextRc, DT_CALCRECT|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		DrawTextFont(hdc, Text, -1, &TextRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE);	
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_SpinCtrl_paint
  Description: 	无
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_SpinCtrl_paint(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	HDC hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rect);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	switch (pData->Mode)
	{
		case SPIN_MODE_TEXT:
			break;

		case SPIN_MODE_NUM:
			// 万年历
			break;
			
		case SPIN_MODE_IP:
			draw_mode_ip(hDlg, hdc);
			break;
			
		default:
			break;
	}
    EndPaint(hDlg, hdc);
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_init
  Description: 	控件初始化
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_init(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg); 
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	if (pData)
	{
		pData->Enable = 1;
		pData->index = -1;
		pData->Value = 0;
		pData->active = 0;
		
		if (SPIN_MODE_TEXT == pData->Mode)
		{
			pData->ListList = ListCreate(LS_NONE, NULL); 
			pData->MaxNum = 0;
			pData->MinNum = 0;
		}
		else
		{
			pData->ListList = NULL;
		}
	}
	
	return TRUE;
}
/*************************************************
  Function:		ui_spin_get_value
  Description: 	从keybord中获得值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2   		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_get_value(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case IDC_KEYBOARD_CTRL:
			
			switch(lParam)
			{	
			    case 0:
			    case 1:
			    case 2:
			    case 3:
			    case 4:
			    case 5:
			    case 6:
			    case 7:
			    case 8:
			    case 9:
					SendMessage(hDlg, WM_Spin_Set_Value, 0, lParam);
					break;

				case 10:
					SendMessage(hDlg, WM_Spin_Set_SelAll, 0, 1);
					break;

				case 11:
					SendMessage(hDlg, WM_Spin_Del, 0, 0);
					break;
					
		    	default:
		  			break;			   	
			}
			break;

    	default:
		  	break;
	}
	ui_reflash_ctrl(hDlg, wParam, lParam);
	 
	return TRUE;
}

/*************************************************
  Function:		ui_spin_set_value
  Description: 	输入ip
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_value(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint32 Index = lParam;
	uint32 tmp;
	
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData)
	{
		if (pData->Mode == SPIN_MODE_IP)
		{
			if (pData->selall)
			{
				pData->selall = 0;
				pData->IP[pData->Value] = Index;
			}
			else
			{
				if (pData->IP[pData->Value] >= 100)
				{
					pData->IP[pData->Value] = Index;
				}
				else
				{
					tmp = pData->IP[pData->Value]*10+Index;
					if (tmp > 255)
					{
						pData->IP[pData->Value] = 255;
					}
					else
					{
						pData->IP[pData->Value] = tmp;
					}
				}
			}
			if (pData->IP[pData->Value] > 100)
			{
				if (pData->Value < pData->MaxNum)
				{
					pData->Value++;
					//WindowSendMessage(win->Parent, WM_COMMAND, ((UINT32)(win->id)) << 16 | pData->index, &(pData->state));
				}
			}
		}
		else
		{
			if(pData->Value != Index)
			{
				pData->Value = Index;
			}
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_get_ipvalue
  Description: 	返回每段IP值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	某段的下标   		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_get_ipvalue(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint8 Index = (uint8)lParam;
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData)
	{
		if (pData->Mode == SPIN_MODE_IP)
		{
			if (Index < 4)
			{
				return pData->IP[Index];
			}
		}
		else
		{
			return pData->Value;
		}
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_get_maxvalue
  Description: 	获取最大值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_get_maxvalue(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
   	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData)
	{
		return pData->MaxNum;
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_set_enable
  Description: 	设置使能
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_enable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint8 Index = lParam;
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData)
	{
		if(pData->Enable != Index)
		{
			pData->Enable = Index;
			ui_reflash_ctrl(hDlg, wParam, lParam);
		}
	}
	 
	return TRUE;
}

/*************************************************
  Function:		ui_spin_get_enable
  Description: 	获取使能
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_get_enable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	if (pData)
	{
		return pData->Enable;
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_set_setall
  Description: 	TAB键进行段IP切换
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_setall(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData)
	{
		pData->selall = lParam;
		if (pData->Value < pData->MaxNum)
		{
			pData->Value++;
		}
		else
		{
			pData->Value = pData->MinNum;
		}
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}


/*************************************************
  Function:		ui_spin_set_maxvalue
  Description: 	设置最大值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_maxvalue(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	uint32 Index = lParam;
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData)
	{
		if(pData->MaxNum != Index)
		{
			pData->MaxNum = Index;
			if (pData->Value > pData->MaxNum)
			{
				pData->Value = pData->MaxNum;
			}
			ui_reflash_ctrl(hDlg, wParam, lParam);
		}
		return TRUE;
	}
	 
	return TRUE;
}

/*************************************************
  Function:		ui_spin_set_del
  Description: 	逐个删除IP值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_del(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg); 
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData && pData->Mode == SPIN_MODE_IP)
	{
		pData->selall = 0;
		pData->IP[pData->Value] = pData->IP[pData->Value]/10;
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_set_clear
  Description: 	清空某段IP值
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_clear(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg); 
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	
	if (pData && pData->Mode == SPIN_MODE_IP)
	{
		pData->IP[pData->Value] = 0;
		ui_reflash_ctrl(hDlg, wParam, lParam);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_spin_set_active
  Description: 	设置激活状态
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_set_active(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg); 
	if (NULL == pData)
	{
		log_printf("pData is NULL!!!");
		return FALSE;
	}
	if (pData && pData->active != lParam)
	{
		pData->active = lParam;
		ui_reflash_ctrl(hDlg, wParam, lParam);
		log_printf("pData->active .....%d\n",pData->active);
	}
	
	return TRUE;
}

/*************************************************
  Function:		ui_KeyboardCtrl_destroy
  Description: 	无
  Input:		
  	1.hDlg		窗体句柄
  	2.wParam	参数1
  	3.lParam 	参数2  		
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_spin_destroy(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PSPIN_INFO pData = (PSPIN_INFO)GetWindowAdditionalData(hDlg);
	if (pData)
	{
    	//free(pData);
		//pData = NULL;
	}
	
	return TRUE;
}

/*************************************************
  Function:			MyControlProc
  Description: 		模板-消息处理函数
  Input: 		
	1.hwnd			句柄
	2.message		消息类型
	3.wParam		附加数据
	4.lParam		附加数据
  Output:			无
  Return:			无
  Others:
*************************************************/
static int MyControlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_CREATE:
			ui_spin_init(hWnd, wParam, lParam);
        	break;
						
		case MSG_PAINT:
			ui_SpinCtrl_paint(hWnd, wParam, lParam);
			break;				
			
		case WM_Spin_Set_Value:
		 	ui_spin_set_value(hWnd, wParam, lParam);
			break;
			
		case WM_Spin_Get_IPValue:
		 	return ui_spin_get_ipvalue(hWnd, wParam, lParam);
			break;
			
   		case WM_Spin_Get_MaxValue:
		 	return ui_spin_get_maxvalue(hWnd, wParam, lParam);
			break;
			
   		case WM_Spin_Set_MaxValue:
		 	ui_spin_set_maxvalue(hWnd, wParam, lParam);
			break;
			
		case WM_Spin_Set_enable:
		 	ui_spin_set_enable(hWnd, wParam, lParam);
			break;

		case WM_Spin_Get_enable:
		 	return ui_spin_get_enable(hWnd, wParam, lParam);
			break;
			
   		case WM_Spin_Set_SelAll:
		 	ui_spin_set_setall(hWnd, wParam, lParam);
			break;
			
   		case WM_Spin_Del:
		 	ui_spin_set_del(hWnd, wParam, lParam);
			break;

		case WM_Spin_Clear_Item:
			ui_spin_set_clear(hWnd, wParam, lParam);
			break;
			
		case WM_Spin_Active:
		 	ui_spin_set_active(hWnd, wParam, lParam);
			break;

		case WM_Spin_Get_Value:
		 	ui_spin_get_value(hWnd, wParam, lParam);
			break;
		 
   		case MSG_DESTROY:
		 	ui_spin_destroy(hWnd, wParam, lParam);
        	break;
	}
	
	return DefaultControlProc (hWnd, message, wParam, lParam);
}

/*************************************************
  Function:		register_num_keybord
  Description: 	注册控件
  Input: 		
	1.void		空	
  Output:		
  Return:		
  Others:
*************************************************/
BOOL register_spin_ctrl(void)
{
	WNDCLASS MyClass;

	MyClass.spClassName = AU_SPIN_CTRL;
	MyClass.dwStyle     = WS_NONE;
	MyClass.dwExStyle   = WS_EX_NONE;
	MyClass.hCursor     = GetSystemCursor (IDC_ARROW);
	MyClass.iBkColor    = PAGE_BK_COLOR;
	MyClass.WinProc     = MyControlProc;
	
	return RegisterWindowClass (&MyClass);
}

/*************************************************
  Function:		unregister_num_keybord
  Description: 	取消注册
  Input: 		
	1.void		空	
  Output:		
  Return:		
  Others:
*************************************************/
void unregister_spin_ctrl(void)
{
	UnregisterWindowClass (AU_SPIN_CTRL);
}

