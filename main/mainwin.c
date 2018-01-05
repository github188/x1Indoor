/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	mainwin.c
  Author:     	chenbh
  Version:    	2.0
  Date: 		2014-08-30
  Description:  
*************************************************/
#include "storage_include.h"
#include "logic_include.h"
#include "gui_include.h"

/*************************************************
  Function:		MiniGUIMain
  Description: 	系统main函数
  Input: 		无
  Output:		无
  Return:		无
  Others:
*************************************************/
int MiniGUIMain (int argc, const char* argv[])
{	
	#if defined(_LITE_VERSION)
	if (JoinLayer(argv[0], 0, 0) == INV_LAYER_HANDLE) 
	{
    	printf ("JoinLayer: invalid layer handle.\n");
		exit (1);
	}
	#endif

	logic_init();				// 初始化逻辑
	InitGui();					// 初始化GUI
	InitScreenTimer();
	form_showmodal(&FrmMain);

	FreeGUI();
	return 0;
}

