/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name:	net_command.h
  Author:   	xiewr
  Version:   	1.0
  Date: 		
  Description:  冠林网络通讯协议_命令定义
  
  History:        
                  
    1. Date:	
       Author:	
       Modification:
    2. ...
*************************************************/
#ifndef __NET_COMMAND_H__
#define __NET_COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short NET_COMMAND;

/************************以下为公共部分命令**********************************/
#define CMD_SET_SYS_PASS	 		(SSC_PUBLIC<<8 | 0x01)		// 设置管理员密码
#define CMD_SET_ROOM_PARAM			(SSC_PUBLIC<<8 | 0x02)		// 室内机参数设置
#define CMD_SET_AREACODEAM			(SSC_PUBLIC<<8 | 0x04)		// 设置区号
#define CMD_TERMINAL_CMD			(SSC_PUBLIC<<8 | 0x05)		// 终端命令
#define CMD_DEVNO_RULE_CMD			(SSC_PUBLIC<<8 | 0x06)		// 设置编号规则

#define CMD_GET_SUIT_SERVER			(SSC_PUBLIC<<8 | 0x14)		// 获取最适合的服务器地址
#define CMD_GET_EVENT_SERVER		(SSC_PUBLIC<<8 | 0x15)		// 获取事件上报服务器地址
#define CMD_SEND_MAXDATA			(SSC_PUBLIC<<8 | 0x20)		// 分包发送
#define CMD_STOP_SEND_MAXDATA		(SSC_PUBLIC<<8 | 0x21)		// 中断分包发送

#define CMD_GET_MANAGER_IP			(SSC_PUBLIC<<8 | 0x4B)		// 获取管理员机地址
#define CMD_QUERY_DEVICE			(SSC_PUBLIC<<8 | 0x53)		// 设备查询
#define CMD_REQ_SYN_TIME			(SSC_PUBLIC<<8 | 0x54)		// 请求时间同步
#define CMD_SYN_TIME				(SSC_PUBLIC<<8 | 0x55)		// 时间同步
#define CMD_DNS_GETIP				(SSC_PUBLIC<<8 | 0x56)		// DNS：广播获取设备IP
#define CMD_SERVER_GETIP			(SSC_PUBLIC<<8 | 0x57)		// DNS：中心服务器获取设备IP

#define CMD_REGISTER_AURINE			(SSC_PUBLIC<<8 | 0x61)		// 室内终端注册	0x61
#define CMD_REGISTER				(SSC_PUBLIC<<8 | 0x62)		// 终端保持	0x62
#define CMD_RTSP_KEEP				(SSC_PUBLIC<<8 | 0x67)		// 向流媒体服务器发送终端保持

#define CMD_ELEVATOR_BECKON			(SSC_PUBLIC<<8 | 0xA0)		// 移动终端请求与室内机主机电梯召唤
#define CMD_ELEVATOR_BECKON_UNLOCK	(SSC_PUBLIC<<8 | 0xA1)		// 移动终端请求与室内主机开锁被动召唤电梯

#define CMD_SET_MAC					(SSC_PUBLIC<<8 | 0xB0)		// 设置MAC码
#define CMD_IPMODULE_HEART			(SSC_PUBLIC<<8 | 0xB1)		// IP模块向室内主机保持通讯
#define CMD_GET_PARAM				(SSC_PUBLIC<<8 | 0xB2)		// 获取室内主机的相关参数信息内容
#define CMD_GET_NETDOOR_LIST		(SSC_PUBLIC<<8 | 0xB3)		// 获取室内主机捆绑的网络门前机信息列表
#define CMD_GET_MANAGER_LIST		(SSC_PUBLIC<<8 | 0xB4)		// 获取室内主机捆绑的管理中心列表
#define CMD_IPMODULE_BIND_QUEST		(SSC_PUBLIC<<8 | 0xB5)		// 室内分机向IP模块请求捆绑注册
#define CMD_IPMODULE_BIND			(SSC_PUBLIC<<8 | 0xB6)		// IP模块向室内主机绑定
#define CMD_GET_EXTENSION_ADDR		(SSC_PUBLIC<<8 | 0xB9)		// 移动终端或室内分机向IP模块获取分机的IP地址
#define CMD_TUNNEL					(SSC_PUBLIC<<8 | 0xA2)		// 数据透传命令

#ifdef	_USE_AURINE_SET_
#define CMD_SOFT_SET_PARAM			(0xFF<<8 | 0xD0)			// 上位机设置室内机参数
#define CMD_SOFT_GET_PARAM			(0xFF<<8 | 0xD1)			// 上位机获取室内机参数
#define CMD_SOFT_TERMINAL_CMD		(0xFF<<8 | 0xD2)			// 终端操作
#else
#define CMD_SOFT_SET_PARAM			(SSC_PUBLIC<<8 | 0xD0)		// 上位机设置室内机参数
#define CMD_SOFT_GET_PARAM			(SSC_PUBLIC<<8 | 0xD1)		// 上位机获取室内机参数
#define CMD_SOFT_TERMINAL_CMD		(SSC_PUBLIC<<8 | 0xD2)		// 终端操作
#endif

/************************以上为公共部分命令**********************************/

/************************以下为安防部分命令**********************************/
#define CMD_QUERY_AREA_DEFINE		(SSC_ALARM<<8 | 0x10)		// 查询防区属性
#define CMD_RP_AREA_DEFINE			(SSC_ALARM<<8 | 0x11)		// 上报防区属性
#define CMD_QUERY_ALARM_STATUS		(SSC_ALARM<<8 | 0x12)		// 安防状态查询
#define CMD_RP_ALARM_STATE			(SSC_ALARM<<8 | 0x14)		// 安防状态上报

#define CMD_ALARM_SET				(SSC_ALARM<<8 | 0x20)		// 布防命令
#define CMD_ALARM_UNSET				(SSC_ALARM<<8 | 0x21)		// 撤防命令
#define CMD_ALARM_PARTSET			(SSC_ALARM<<8 | 0x22)		// 局防命令

#define CMD_RP_SET_EVENT			(SSC_ALARM<<8 | 0x30)		// 布防操作事件
#define CMD_RP_UNSET_EVENT			(SSC_ALARM<<8 | 0x31)		// 撤防操作事件
#define CMD_RP_PARTSET_EVENT		(SSC_ALARM<<8 | 0x32)		// 局防操作事件
#define CMD_RP_BYPASS_EVENT			(SSC_ALARM<<8 | 0x33)		// 旁路操作事件
// add by luofl 2011-4-27 begin
#define CMD_SYNC_SET_EVENT			(SSC_ALARM<<8 | 0x3D)		// 布防同步事件
#define CMD_SYNC_UNSET_EVENT		(SSC_ALARM<<8 | 0x3E)		// 撤防同步事件
#define CMD_SYNC_PARTSET_EVENT		(SSC_ALARM<<8 | 0x3F)		// 局防同步事件
// add by luofl 2011-4-27 end

#define CMD_RP_FORCE_UNSET_ALARM	(SSC_ALARM<<8 | 0x41)		// 挟持撤防报警
#define CMD_RP_SOS_ALARM			(SSC_ALARM<<8 | 0x42)		// 紧急求救报警
#define CMD_RP_AREA_BREAK_ALARM		(SSC_ALARM<<8 | 0x43)		// 防区触发报警
#define CMD_RP_AREA_FAULT_ALARM		(SSC_ALARM<<8 | 0x44)		// 防区故障报警

//add by fanfj
#ifdef _IP_MODULE_ALARM
#define CMD_CLEAR_ALARM				(SSC_ALARM<<8 | 0x23)		// 清除警示
#define CMD_SOS_REMOTECTRL			(SSC_ALARM<<8 | 0x25)		// 远程SOS控制
#define CMD_TRANSMIT_ALARMRECORD	(SSC_ALARM<<8 | 0x45)		// 转发报警记录
#define CMD_QUERY_ALARMRECORD		(SSC_ALARM<<8 | 0x46)		// 查询报警记录
#endif
/************************以上为安防部分命令**********************************/

/************************以下为对讲部分命令**********************************/
#define CMD_QUERY_ACCESS_PASS		(SSC_INTERPHONE<<8 | 0x10)	// 查询开门密码
#define CMD_CLEAR_ACCESS_PASS		(SSC_INTERPHONE<<8 | 0x11)	// 复位开门密码
#define CMD_RP_CARD_ACCESS_EVENT	(SSC_INTERPHONE<<8 | 0x31)	// 上报刷卡开门事件

#ifdef _AU_PROTOCOL_
#define CMD_UNLOCK					(SSC_INTERPHONE<<8 | 0x20)	// 开锁
#define CMD_MONITOR					(SSC_INTERPHONE<<8 | 0x21)	// 监视
#define CMD_MONITOR_TO_TALK			(SSC_INTERPHONE<<8 | 0x22)	// 监视进入通话
#define CMD_STOP_MONITOR			(SSC_INTERPHONE<<8 | 0x23)	// 中断监视
#define CMD_MONITOR_HEART			(SSC_INTERPHONE<<8 | 0x24)	// 监视握手（心跳）
#define CMD_RP_SNAPSHOT				(SSC_INTERPHONE<<8 | 0x25)	// 梯(区)口拍照上报

#define CMD_CALL_CALLING			(SSC_INTERPHONE<<8 | 0x50)	// 呼叫请求	0x50
#define CMD_CALL_ANSWER				(SSC_INTERPHONE<<8 | 0x51)	// 确认请求	0x51
#define CMD_CALL_HANDDOWN			(SSC_INTERPHONE<<8 | 0x52)	// 挂断请求	0x52
#define CMD_CALL_HEART				(SSC_INTERPHONE<<8 | 0x53)	// 通话心跳	0x53
#else
#define CMD_UNLOCK					(SSC_INTERPHONE<<8 | 0xA0)	// 开锁
#define CMD_MONITOR					(SSC_INTERPHONE<<8 | 0xA1)	// 监视
#define CMD_MONITOR_TO_TALK			(SSC_INTERPHONE<<8 | 0xA2)	// 监视进入通话
#define CMD_STOP_MONITOR			(SSC_INTERPHONE<<8 | 0xA3)	// 中断监视
#define CMD_MONITOR_HEART			(SSC_INTERPHONE<<8 | 0xA4)	// 监视握手（心跳）
#define CMD_RP_SNAPSHOT				(SSC_INTERPHONE<<8 | 0xA5)	// 梯(区)口拍照上报

#define CMD_CALL_CALLING			(SSC_INTERPHONE<<8 | 0xB0)	// 呼叫请求
#define CMD_CALL_ANSWER				(SSC_INTERPHONE<<8 | 0xB1)	// 确认请求
#define CMD_CALL_HANDDOWN			(SSC_INTERPHONE<<8 | 0xB2)	// 挂断请求
#define CMD_CALL_HEART				(SSC_INTERPHONE<<8 | 0xB3)	// 通话心跳
#endif
/************************以上为对讲部分命令**********************************/

/************************以下为信息部分命令**********************************/
#define CMD_SEND_INFO				(SSC_INFO<<8 | 0x10)		// 发送信息
#define CMD_SEND_SHORT_MSG			(SSC_INFO<<8 | 0x11)		// 短信发送(用户往室内机发)
#define CMD_SEND_MSG_VALIDATE		(SSC_INFO<<8 | 0x12)		// 发送短信功能验证码
#define CMD_SHORT_MSG_TRANSMIT		(SSC_INFO<<8 | 0x13)		// 短信转发
/************************以上为信息部分命令**********************************/

/************************以下为网络门前机命令(部份命令与网关共用)************/
#define CMD_GATEWAY_SET_NET_PARAM	(SSC_IPDOORPHONE<<8 | 0x10)		// 设置网络门前机络参数
#define CMD_GATEWAY_SET_DEV_INFO	(SSC_IPDOORPHONE<<8 | 0x11)		// 设置网络门前机设备编号以及规则
#define CMD_GATEWAY_SET_DATE_TIME	(SSC_IPDOORPHONE<<8 | 0x14)		// 设置日期、时间、时区
#define CMD_GATEWAY_SET_OTHERS		(SSC_IPDOORPHONE<<8 | 0x16)		// 其他设置

#define CMD_GATEWAY_GET_NET_PARAM	(SSC_IPDOORPHONE<<8 | 0x20)		// 查询网络门前机络参数
#define CMD_GATEWAY_GET_DEV_INFO	(SSC_IPDOORPHONE<<8 | 0x21)		// 查询网络门前机设备编号
#define CMD_GATEWAY_GET_DATE_TIME	(SSC_IPDOORPHONE<<8 | 0x24)		// 查询日期、时间、时区
#define CMD_GATEWAY_GET_OTHERS		(SSC_IPDOORPHONE<<8 | 0x26)		// 查询其他设置

#define CMD_GATEWAY_GET_MAC			(SSC_IPDOORPHONE<<8 | 0x30)		// 查询网络门前机MAC
#define CMD_GATEWAY_GET_VER			(SSC_IPDOORPHONE<<8 | 0x31)		// 查询网络门前机版本信息

#define CMD_NETDOOR_MNG_CARD		(SSC_IPDOORPHONE<<8 | 0x40)		// 网络门前机卡管理
#define CMD_NETDOOR_GET_CARD_INFO	(SSC_IPDOORPHONE<<8 | 0x41)		// 获得卡信息
#define CMD_NETDOOR_SET_LOCK		(SSC_IPDOORPHONE<<8 | 0x42)		// 设置锁信息
#define CMD_NETDOOR_GET_LOCK		(SSC_IPDOORPHONE<<8 | 0x43)		// 获得锁信息
#ifdef _ADD_BLUETOOTH_DOOR_
#define CMD_NETDOOR_24HOUR_RECORD	(SSC_IPDOORPHONE<<8 | 0xA0)		// 查看门前机最近24小时记录
#define CMD_NETDOOR_HISTORY_RECORD	(SSC_IPDOORPHONE<<8 | 0xA1)		// 查看门前机历史记录
#define CMD_NETDOOR_SNAP_REPORT		(SSC_IPDOORPHONE<<8 | 0xA3)		// 通知室内机有新的告警抓拍图片
#define CMD_NETDOOR_ROOMLIST		(SSC_IPDOORPHONE<<8 | 0xA4)		// 获取门前机的室内绑定列表
#define CMD_NETDOOR_SYSINFO			(SSC_IPDOORPHONE<<8 | 0xA5)		// 获取门前机信息
#endif

/************************以上为网络门前机命令(部份命令与网关共用)************/

/************************以下为便利功能命令**********************************/
#define CMD_ELEVATOR_CALL_AUTHORIZE	(SSC_OTHER<<8 | 0x16)		// 电梯授权 呼叫类型设备1B+主叫号码串30B+被叫号码串30B
#define CMD_ELEVATOR_OPEN_TK_LOCK	(SSC_OTHER<<8 | 0x17)		// 通知室内机开梯口机锁
#define CMD_ELEVATOR_OTHER_CTRL		(SSC_OTHER<<8 | 0x19)		// 第三方电梯控制
/************************以上为便利功能命令**********************************/

/************************以下为远程家电命令**********************************/
#define CMD_JD_DEV_OPEN				(SSC_EHOME<<8 | 0x10)		// 家电设备开
#define CMD_JD_DEV_CLOSE			(SSC_EHOME<<8 | 0x11)		// 家电设备关
#define CMD_JD_SCENE_MODE			(SSC_EHOME<<8 | 0x12)		// 家电情景模式
#define CMD_JD_GET_JD_INFO			(SSC_EHOME<<8 | 0xA0)		// 获取家电信息
#define CMD_JD_GET_DEVINFO			(SSC_EHOME<<8 | 0xA1)		// 向IP模块获取家电设备信息列表
#ifdef _AURINE_ELEC_NEW_
#define CMD_JD_STATE_QUERY 			(SSC_EHOME<<8 | 0x0F)		// 家电查询命令
#endif
/************************以上为远程家电命令**********************************/

/************************以下为视频监视部分命令******************************/
#define CMD_RTSP_PTZ_CONTROL		(SSC_VIDEOMONITORING<<8 | 0x01)		// 云台控制 add by fanfj
#define CMD_GET_AREA_CAMERALIST		(SSC_VIDEOMONITORING<<8 | 0x03)		// 获取小区安防监控设备的信息列表 add by fanfj
#define CMD_GET_HOME_CAMERALIST		(SSC_VIDEOMONITORING<<8 | 0xD0)		// 获取室内摄像头信息列表 add by fanfj

#define CMD_GET_AREA_CAMERALIST_EXT	(SSC_VIDEOMONITORING<<8 | 0x06)		// 获取小区安防监控设备的信息列表 add by chenbh 2016-03-04
#define CMD_GET_HOME_ONLINE_LIST_EXT (SSC_VIDEOMONITORING<<8 | 0x07)	// 获取室内机IPC在线列表
#define CMD_GET_HOME_CAMERALIST_EXT	(SSC_VIDEOMONITORING<<8 | 0xD1)		// 获取室内摄像头信息列表 add by chenbh 2016-03-04
/************************以下为视频监视部分命令******************************/


#ifdef __cplusplus
}
#endif

#endif

