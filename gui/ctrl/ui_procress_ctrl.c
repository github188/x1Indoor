/*********************************************************
  Copyright (C), 2009-2012
  File name:	ui_procress_ctrl.c
  Author:   	zhengxc
  Version:   	1.0
  Date: 		15.7.15
  Description:  进度控件
  History:            
*********************************************************/

#include "gui_include.h"

/*************************************************
  Function:		ui_reflash_ctrl
  Description: 	刷新控件
  Input:	
  	1.hDlg		窗体
  	2.WButObj 	控件结构体		
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_reflash_ctrl(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hDlg, NULL, TRUE);
}

/*************************************************
  Function:		procctrlPaint
  Description: 	按钮画屏消息
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam 	参数2
  Output:		无
  Return:		无
  Others:
*************************************************/
static void ui_procctrl_paint(HDC hdc, RECT * rc, PPROCRESS_CTRL WButObj)
{
	int endpos, startpos, oper;
	PBITMAP bmpBK = NULL;
	uint32 bmpW[2];
	uint32 bmpProcW;
	
	bmpBK = get_bmp(WButObj->ImageBK);
	bmpW[0] = get_bmp(WButObj->Image[0])->bmWidth;
	bmpW[1] = get_bmp(WButObj->Image[1])->bmWidth;
	bmpProcW = get_bmp(WButObj->ImageProc)->bmWidth;	
	
	DrawBmpID(hdc, 0, 0, bmpBK->bmWidth, bmpBK->bmHeight, WButObj->ImageBK);
	if (WButObj->allproc > 0)
	{
		if ((WButObj->proc > 0)&&(WButObj->proc <= WButObj->allproc))
		{
			if (1 == WButObj->proc)
			{
				DrawBmpID(hdc, 0, 0, bmpW[0], bmpBK->bmHeight, WButObj->Image[0]);
			}
			else
			{
				endpos = bmpW[0] + ((bmpBK->bmWidth - bmpW[0] - bmpW[1]) * WButObj->proc) / WButObj->allproc;
				DrawBmpID(hdc, 0, 0, bmpW[0], bmpBK->bmHeight, WButObj->Image[0]);
				oper = TRUE;
				startpos = bmpW[0];
				while(oper)
				{
					startpos += bmpProcW;
					if (startpos < endpos)
					{
						DrawBmpID(hdc, (startpos - bmpProcW), 0, bmpProcW, bmpBK->bmHeight, WButObj->ImageProc); 	
					}
					else
					{
						startpos = endpos - bmpProcW;
						DrawBmpID(hdc, startpos, 0, bmpProcW, bmpBK->bmHeight, WButObj->ImageProc); 
						oper = FALSE;
					}
				}
				DrawBmpID(hdc, endpos, 0, endpos + bmpW[1], bmpBK->bmHeight, WButObj->Image[1]); 
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
	PLISTCTRL_INFO WObj = (PLISTCTRL_INFO)GetWindowAdditionalData(hDlg);
	if (NULL == WObj)
	{
		log_printf("WButObj is NULL!!!");
		return;
	}
	
	hdc = BeginPaint(hDlg);
	GetClientRect(hDlg, &rc);  	
	SetBkMode(hdc, BM_TRANSPARENT);
	//SetBrushColor(hdc, PAGE_BK_COLOR);
	//FillBox(hdc, rc.left, rc.top, rc.right, rc.bottom);
	ui_procctrl_paint(hdc, &rc, WObj);
	EndPaint(hDlg, hdc);
}

/*************************************************
  Function:		ui_set_proc
  Description: 	
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam	PPROCRESS_CTRL 结构
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_proc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PPROCRESS_CTRL WButObj = (PPROCRESS_CTRL)GetWindowAdditionalData(hDlg);
	if (WButObj && WButObj->proc != wParam)
	{
		WButObj->proc = wParam;		
		ui_reflash_ctrl(hDlg, wParam, lParam);
		return TRUE;
	}
	return FALSE;
}

/*************************************************
  Function:		ui_set_allproc
  Description: 	
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam	PPROCRESS_CTRL 结构
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_set_allproc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PPROCRESS_CTRL WButObj = (PPROCRESS_CTRL)GetWindowAdditionalData(hDlg);
	if (WButObj && WButObj->allproc != wParam)
	{
		WButObj->allproc = wParam;
		ui_reflash_ctrl(hDlg, wParam, lParam);
		return TRUE;
	}
	return FALSE;
}

/*************************************************
  Function:		ui_procctrl_creat
  Description: 	控件创建
  Input:		
  	1.win		窗体
  	2.wParam	参数1
  	3.lParam	PPROCRESS_CTRL 结构
  Output:		无
  Return:		无
  Others:
*************************************************/
static uint32 ui_procctrl_creat(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	PPROCRESS_CTRL WObj = (PPROCRESS_CTRL)GetWindowAdditionalData(hDlg);

	if (NULL == WObj)
	{
	    return FALSE;
	}
	
	WObj->proc = NULL;
	ui_reflash_ctrl(hDlg, wParam, lParam);
	
	return TRUE;
}

/*************************************************
  Function:			MyControlProc
  Description: 		控制消息处理函数
  Input: 		
	1.hDlg			句柄
	2.message		消息类型
	3.wParam		附加数据
	4.lParam		附加数据
  Output:			无
  Return:			无
  Others:
*************************************************/
static int MyControlProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
		case MSG_CREATE:
			ui_procctrl_creat(hDlg, wParam, lParam);
			break;

		case MSG_INITDIALOG:
			SetWindowBkColor(hDlg, PAGE_BK_COLOR);
			break;	
			
		case MSG_PAINT:
			OnPaint(hDlg);
			break;

		case WM_Procress_Set_Proc:
			ui_set_proc(hDlg, wParam, lParam);
			break;

		case WM_Procress_Set_Allproc:
			 ui_set_allproc(hDlg, wParam, lParam);
			 break;
	}

	return DefaultControlProc(hDlg, message, wParam, lParam);
}	

/*************************************************
  Function:			register_procress_ctrl
  Description: 		注册进度控件
  Input: 		
	1.void			无
  Output:			无
  Return:			无
  Others:
*************************************************/
BOOL register_procress_ctrl(void)
{
	WNDCLASS MyClass;

	MyClass.spClassName = AU_PROC_CTRL;
	MyClass.dwStyle = WS_NONE;
	MyClass.dwExStyle = WS_EX_NONE;
	MyClass.hCursor = GetSystemCursor(IDC_ARROW);
	MyClass.iBkColor = COLOR_lightwhite;
	MyClass.WinProc = MyControlProc;
	
	return RegisterWindowClass(&MyClass);
}

/*************************************************
  Function:			unregister_procress_ctrl
  Description: 		删除进度控件
  Input: 		
	1.void			无
  Output:			无
  Return:			无
  Others:
*************************************************/
void unregister_procress_ctrl(void)
{
	UnregisterWindowClass(AU_PROC_CTRL);
}

