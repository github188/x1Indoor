/*********************************************************
  Copyright (C), 2009-2012
  File name:	ui_procress_ctrl.h
  Author:   	zhengxc
  Version:   	1.0
  Date: 		15.7.15
  Description:  进度控件头文件
  History:                   
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __UI_PROCRESS_CTRL_H__
#define __UI_PROCRESS_CTRL_H__

/********************************************************/
/*						控件结构						*/
/********************************************************/

typedef struct 
{
	uint8 proc;										// 当前进度
	uint8 allproc;									// 总进度
	uint32 ImageBK;								// 图片ID	
	uint32 Image[2];								// 左右图片ID		
	uint32 ImageProc;								// 图片进度
}PROCRESS_CTRL,*PPROCRESS_CTRL;

#endif /* __UI_PROCRESS_CTRL_H__ */



