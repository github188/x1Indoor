/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	gui_include.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-01
  Description:  
				GUI窗体头文件
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __GUI_INCLUDE_H__
#define __GUI_INCLUDE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
//#include <minigui/mywindows.h>
//#include <minigui/mgext.h>

#include "au_types.h"
#include "AppConfig.h"
#include "comm/ui_mainwin.h"
//#include "comm/au_language.h"
#include "comm/au_pic_winres.h"
#include "comm/au_other_winres.h"
#include "comm/au_model_func.h"

#include "ctrl/ui_right_ctrl.h"
#include "ctrl/ui_keybord_ctrl.h"
#include "ctrl/ui_list.h"
#include "ctrl/ui_list_ctrl.h"
#include "ctrl/ui_input_ctrl.h"
#include "ctrl/ui_eightarea_ctrl.h"
#include "ctrl/ui_msg_ctrl.h"
#include "ctrl/ui_spin_ctrl.h"
#include "ctrl/ui_procress_ctrl.h"

//#include "SysGlobal.h"

#ifdef __cplusplus
extern "C" {
#endif

// 窗体定义
extern GUI_FORM FrmMain;
extern GUI_FORM FrmBeCall;      	// 被叫界面
extern GUI_FORM FrmCall;			// 主叫界面
extern GUI_FORM FrmMonitor;			// 监视界面
extern GUI_FORM FrmMsg;				// 信息界面
extern GUI_FORM FrmMsgView;			// 信息查看
extern GUI_FORM FrmSet;				// 设置界面
extern GUI_FORM	FrmSetLanguage;		// 语言设置
extern GUI_FORM	FormSetTimer;		// 时间设置
extern GUI_FORM FrmSetOffScreen;	// 关屏时间设置
extern GUI_FORM FrmSetstorage;		// 存储信息界面
extern GUI_FORM	FrmSysInfo;			// 系统信息界面
extern GUI_FORM FrmAlarm;			// 报警界面
extern GUI_FORM FrmAlarmWholeAttr;	// 防区属性界面
extern GUI_FORM FrmAlarmPara;		// 安防参数界面
extern GUI_FORM FrmAlarmTime;		// 安防时间参数界面
extern GUI_FORM FrmAlarmAreaList;	// 防区设置界面
extern GUI_FORM	FrmAlarmSingleAttr; // 安防设置防区属性单界面	
extern GUI_FORM FrmDemo;
extern GUI_FORM	FrmMsgCtrl;			//消息控件
//extern GUI_FORM FrmMsgHit;			// 新来电、新信息等消息界面
extern GUI_FORM FrmDianti;			// 电梯召唤界面
extern GUI_FORM FrmlylyMode;		// 留影留言模式界面
extern GUI_FORM	FrmPhoto;			// 照片界面
extern GUI_FORM	FrmLyLy;			// 留影留言界面
#ifdef _JRLY_MODE_
extern GUI_FORM FormSetjrlyrecord;	// 家人留言
#endif

// 外部函数定义
extern uint32 ui_photo_view(HWND hDlg, uint8 index, PPHOTOLIST_INFO photolist);
extern uint32 ui_language_win(HWND hDlg);
extern uint32 ui_screensave_win(HWND hDlg);
extern uint32 ui_storage_win(HWND hDlg);
extern uint32 ui_sysinfo_win(HWND hDlg);
extern uint32 ui_password_win(PASS_TYPE type);
extern uint32 SetAlarmWholeAttr(HWND hDlg);
extern uint32 ui_netparam_win(HWND hDlg);
extern uint32 ui_extparam_win(HWND hDlg);
extern uint32 ui_devno_win(HWND hDlg);
extern uint32 SetLockPwd(HWND hDlg, uint8 pass_type);
extern uint32 SetAlarmAreaList(HWND hDlg, uint8 type);
extern uint32 SetAlarmLink(HWND hDlg);
extern uint32 SetLockPwd(HWND hDlg, uint8 pass_type);
extern uint32 ui_ring_win(HWND hDlg);
extern uint32 ui_volume_win(HWND hDlg);
extern uint32 ui_lyly_win(HWND hDlg);
extern uint32 SetAlarmSingleAttr(HWND hDlg, uint8 num);
extern uint32 ui_lyly_view(HWND hDlg, uint8 index, PLYLYLIST_INFO lylylist);
extern void   ui_monitor_list_appstart(DEVICE_TYPE_E DevType);
extern uint32 AlarmWindow(HWND hDlg);
extern uint32 ui_lylymode_win(HWND hDlg , uint32 mode);
extern uint32 ui_ringtype_win(HWND hDlg, uint8 page);
extern uint32 ui_DevNoDialog_win(HWND hDlg, uint32 textid, PFULL_DEVICE_NO devtype);
extern uint32 ui_netparam_dialog_win(HWND hDlg, IP_TYPE IPtype, uint32 *IP, uint8 dev_type);
extern uint32 SetAlarmPara(HWND hDlg);
extern uint32 SetAlarmTime(HWND hDlg, uint8 type, uint8 time);
extern uint32 ui_msg_view(HWND hDlg, uint8 index, PMSGLIST pmsg_list);
extern uint32 ui_passin_win(PMSG_INFO info);
extern void ui_msg_win(PMSG_INFO info);
extern void ui_msg_reboot(void);
extern void set_curkey(void);
//extern void ui_msg_hit_win(PMSG_INFO info);
extern uint32 ui_becall_win(HWND hDlg, void * param);
extern uint32 ui_set_netdoor_win(HWND hDlg, HWND doormun);
extern uint32 ui_netdoor_netparam_win(HWND hDlg,HWND doornum);
extern uint32 ui_netdoor_lock_win(HWND hDlg);
extern uint32 ui_netdoor_card_win(HWND hDlg);
extern uint32 ui_netdoorsysinfo_win(HWND hDlg);
extern uint32 ui_set_netdoor_other_win(HWND hDlg);
extern void set_netdoorparam_Page(uint8 pageIndex);
#ifdef _JRLY_MODE_
extern uint32 ui_jrlyrecord_win(HWND hDlg);
#endif

#ifdef __cplusplus
}
#endif
#endif

