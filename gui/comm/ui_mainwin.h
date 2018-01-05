/*********************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	ui_mainwin.h
  Author:   	
  Version:   	2.0
  Date: 		08.3.26
  Description:  公共函数头文件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UI_MAINWIN_H__
#define __UI_MAINWIN_H__

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************
  Function:		InitGui
  Description: 	初始化GUI
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void InitGui(void);

/*************************************************
  Function:		FreeGUI
  Description: 	释放GUI程序资源
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void FreeGUI(void);

#ifdef __cplusplus
}
#endif

#endif

