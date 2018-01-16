/*************************************************
  Copyright (C), 2006-2010, Aurine
  File name: 	logic_pubsetcmd.c
  Author:    	wufn
  Version:   	1.0
  Date: 
  Description:  公共设置命令
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
//#include "logic_pubcmd.h"
//#include "logic_ipmodule.h"
#include "logic_include.h"

/*************************************************
  Function:			public_distribute
  Description:		公共命令接收包回调函数
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 get_param_cmd_deal(const PRECIVE_PACKET recPacket)
{	
	char sendata[1024];
	#ifdef	_USE_AURINE_SET_
	char * data = recPacket->data + AU_NET_HEAD_SIZE;
	#else
	char * data = recPacket->data + NET_HEAD_SIZE;
	#endif
	uint32 func = *(uint32 *)data;

	switch(func)
	{			

		case DATA_ID_SYSPARAM:			//系统参数
			{
		
			SYSTEM_SETINFO send_system_info;
			memset(&send_system_info, 0, sizeof(SYSTEM_SETINFO));

			//默认的参数填充
			send_system_info.IsCallConvert = 0;
			send_system_info.LockType = 0;
			send_system_info.LockTime = 1;
			send_system_info.NetComm = 1;

			//网络参数
			PNET_PARAM netparam = storage_get_netparam();
			send_system_info.NetParam.IP = netparam->IP;
			send_system_info.NetParam.SubNet = netparam->SubNet;
			send_system_info.NetParam.DefaultGateway = netparam->DefaultGateway;
			send_system_info.NetParam.CenterIP = netparam->CenterIP;
			send_system_info.NetParam.ManagerIP[0] = netparam->ManagerIP;
			send_system_info.NetParam.ManagerIP[1] = netparam->ManagerIP1;
			send_system_info.NetParam.ManagerIP[2] = netparam->ManagerIP2;	
			send_system_info.NetParam.NetdoorIP[0] = netparam->IP1;
			
			//设备编号
			DEVICE_NO device_no = storage_get_devno();
			#ifdef	_USE_AURINE_SET_
			DEVICE_SET_NO device_set_no;			
			memset(&device_set_no, 0, sizeof(DEVICE_SET_NO));
			device_set_no.DeviceNo1 = device_no.DeviceNo1;
			device_set_no.DeviceNo2 = device_no.DeviceNo2;
			device_set_no.DeviceType = DEVICE_TYPE_ROOM;
			memcpy(&send_system_info.DeviceNo , &device_set_no, sizeof(DEVICE_SET_NO));
			#else
			memcpy(&send_system_info.DeviceNo, &device_no, sizeof(DEVICE_NO));
			#endif
			
			//编号规则
			DEVICENO_RULE *rule = storage_get_devrule();
			memcpy(&send_system_info.DeviceRule, rule, sizeof(DEVICENO_RULE));

			//工程密码
			memcpy(send_system_info.ProjectPwd, storage_get_pass(PASS_TYPE_ADMIN), 6);

			//外部模块
			send_system_info.ExtModule[0] = storage_get_extmode(EXT_MODE_JD);			//家电模块
			send_system_info.ExtModule[1] = storage_get_extmode(EXT_MODE_NETDOOR1);		//网络门前机1模块
			send_system_info.ExtModule[2] = storage_get_extmode(EXT_MODE_NETDOOR2);		//网络门前机2模块
			send_system_info.ExtModule[5] = storage_get_extmode(EXT_MODE_GENERAL_STAIR);//通用梯口

			//系统版本和mac 	
			strcpy(send_system_info.HardwareVer, HARD_VER);		
			strcpy(send_system_info.SoftwareVer, SOFT_VER);	
			memcpy(send_system_info.Mac, storage_get_mac(HOUSE_MAC), 6);
			
			//语言
			LANGUAGE_E language = storage_get_language();
			switch (language)
			{
				case CHINESE:
					send_system_info.language = LAN_CHINESE;
					break;

				case ENGLISH:
					send_system_info.language = LAN_ENGLISH;
					break;

				case CHNBIG5:
					send_system_info.language = LAN_BIG5;
					break;

				default:
					send_system_info.language = LAN_CHINESE;
					break;
			}

			// 数据: 设备分段个数4B +设备分段描述 +系统参数长度4B+系统参数内容


			//设备分段个数
			char ssub[10] = {0};
			sprintf(ssub, "%d",	rule->Subsection);
			uint32 subnum = strlen(ssub);
			memcpy(sendata, &subnum, 4);

			//设备分段描述
			int32 subsize = 10*strlen(ssub);				
			memcpy(sendata+4, storage_get_dev_desc(), subsize);

			//系统参数长度
			uint32 sysparamlen = (sizeof(SYSTEM_SETINFO));
			memcpy(sendata+4+subsize, &sysparamlen, 4);
			
			//系统参数	
			memcpy(sendata+4+subsize+4, (char *)&send_system_info, sizeof(SYSTEM_SETINFO));
			
			#ifdef	_USE_AURINE_SET_
			au_net_send_echo_packet_ext(recPacket, ECHO_OK, sendata, (4 + subsize + 4 + sizeof(SYSTEM_SETINFO)));	
			#else
			net_send_echo_packet_ext(recPacket, ECHO_OK, sendata, (4 + subsize + 4 + sizeof(SYSTEM_SETINFO)));	
			#endif
			return TRUE;
			
			}

		case DATA_ID_SECUPARAM:			//安防参数
			{

			//报警时间2B
			memcpy(sendata, &gpAfParam->alarm_time, 2);
			//退出预警时间2B
			memcpy(sendata+2, &gpAfParam->exit_alarm_time, 2);

			//增加防区属性
			SECU_ZONEINFO zone_info;
			int i;
			for(i = 0; i < 8; i++)
			{
				memset(&zone_info, 0, sizeof(SECU_ZONEINFO));
				
				zone_info.Enable =  (1 & ((gpAfParam->enable) >> i));
				zone_info.ProbeType = gpAfParam->area_type[i];			//这里的探头类型是什么
				zone_info.DelayTime = gpAfParam->delay_time[i];
				zone_info.IsPartValid = (1  & ((gpAfParam->part_valid) >> i));			
				zone_info.Is24Hour = (1 & ((gpAfParam->is_24_hour) >> i));	
				zone_info.IsSee = (1 & ((gpAfParam->can_see) >> i ));
				zone_info.IsSound = (1 & ((gpAfParam->can_hear) >> i));
				zone_info.ProbeState = (1 & ((gpAfParam->finder_state) >> i));
				
				memcpy(sendata+4 + (i*sizeof(SECU_ZONEINFO)), &zone_info, sizeof(SECU_ZONEINFO));
			}

			#ifdef	_USE_AURINE_SET_
			au_net_send_echo_packet_ext(recPacket, ECHO_OK, sendata, (4 +  sizeof(SYSTEM_SETINFO)*8) );	;	
			#else
			net_send_echo_packet_ext(recPacket, ECHO_OK, sendata, (4 +  sizeof(SYSTEM_SETINFO)*8) );	
			#endif
			
			return TRUE;
			
			}
			
		case DATA_ID_ELECPARAM:  		//家电参数
			{
			#if 0
			uint32 jdtype = *(uint32 *)(data+4);
			PJD_DEV_LIST_INFO g_jdList = storage_get_jddev(jdtype, 0);
			uint16 jdfactory = storage_get_extmode(EXT_MODE_JD_FACTORY);
			//家电设备信息
			ELEC_DEVINFO elec_info;

			//家电厂商2B
			memcpy(sendata, &jdfactory, 2);
			//设备个数2B
			uint16 count = g_jdList->nCount;
			memcpy(sendata+2, &count, 2);
			int i;
			
			if (g_jdList) 
			{
				for(i = 0; i < g_jdList->nCount; i++)
				{
					memset(&elec_info, 0, sizeof(ELEC_DEVINFO));
					
					elec_info.JdType = g_jdList->pjd_dev_info[i].JdType;
					elec_info.Index = g_jdList->pjd_dev_info[i].Index;
					elec_info.Address = g_jdList->pjd_dev_info[i].Address;
					memcpy(elec_info.Name, g_jdList->pjd_dev_info[i].Name, 30);
					elec_info.IsUsed = g_jdList->pjd_dev_info[i].IsUsed;
					elec_info.IsTune = g_jdList->pjd_dev_info[i].IsTune;
					
					memcpy(sendata+4+(sizeof(ELEC_DEVINFO)*i), &elec_info, sizeof(ELEC_DEVINFO));
				
				}
					free_jd_memory(&g_jdList);
			}
			net_send_echo_packet_ext(recPacket, ECHO_OK, sendata, (4 +  sizeof(ELEC_DEVINFO)*count) );	
			return TRUE;
			#endif
			return FALSE;
			
			}
			
		case DATA_ID_CAMERALIST:		//摄像头列表
			{
			#if 0	
			HOMEDEVICE Homedev[MAX_HOME_NUM+1];  
			uint32 size;
			size = get_homedev_from_storage(Homedev);
			NEWHOMECAMERA camera;
			int i;
			memcpy(sendata, &size, 4);
			if (0 < size)
			{
				for (i = 0; i < size; i++)
				{
					memset(&camera, 0, sizeof(NEWHOMECAMERA));
					
					camera.Enable = Homedev[i].EnableOpen;
					memcpy(camera.DeviceName, Homedev[i].DeviceName, sizeof(Homedev[i].DeviceName));
					camera.DeviceIP = Homedev[i].DeviceIP;
					camera.DevPort = Homedev[i].DevPort;
					camera.Channel = Homedev[i].ChannelNumber;
					memcpy(camera.FactoryName, Homedev[i].FactoryName, sizeof(Homedev[i].FactoryName));
					memcpy(camera.UserName, Homedev[i].UserName, sizeof(Homedev[i].UserName));
					memcpy(camera.Password, Homedev[i].Password, sizeof(Homedev[i].Password));
					camera.CanControlPTZ = Homedev[i].CanControlPTZ;
					camera.IsOnline = 1;		// 默认都是在线的
					
					memcpy(sendata+4+(sizeof(NEWHOMECAMERA)*i), &camera, sizeof(NEWHOMECAMERA));
				}
			}
			
			net_send_echo_packet_ext(recPacket, ECHO_OK, sendata, 4+(sizeof(NEWHOMECAMERA)*size)); 	
			return TRUE;
			#endif
			return FALSE;
			
			}
		
		/*
		case DATA_ID_ELEVATORLIST:
			return TRUE;
			
		case DATA_ID_STAIRPARAM:
			return TRUE;
		*/
		
		default:
			//功能类型不正确应答
			log_printf("func ECHO_ERROR\n");
			
			#ifdef	_USE_AURINE_SET_			
			au_net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
			#else			
			net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
			#endif
			return  FALSE;
			
	}

}

/*************************************************
  Function:			set_param_cmd_deal
  Description:		
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 set_param_cmd_deal(const PRECIVE_PACKET recPacket)
{	
	#ifdef	_USE_AURINE_SET_
	char * data = recPacket->data + AU_NET_HEAD_SIZE;
	#else
	char * data = recPacket->data + NET_HEAD_SIZE;
	#endif
	
	uint32 func = *(uint32 *)data;

	switch(func)
	{
		
		case DATA_ID_SYSPARAM:									//设置系统参数
			{
				//分段描述
				char sub_des[70] = {0};
				uint32 sublen = *(uint32 *)(data+4);			// 前面四字节功能代码	
				uint32 subsize = 10*sublen; 	
				memset(sub_des, 0, sizeof(sub_des));
				memcpy(sub_des, (char *)(data+8), subsize); 
				log_printf("subsize : %d\n", subsize);
				
				//系统参数	
				SYSTEM_SETINFO send_system_info;	
				memset(&send_system_info, 0, sizeof(SYSTEM_SETINFO));
				memcpy(&send_system_info, data+12+subsize, sizeof(send_system_info));	
					
				//网络参数
				NET_PARAM netparam;
				memset(&netparam, 0, sizeof(NET_PARAM));
				netparam.IP = send_system_info.NetParam.IP;
				netparam.SubNet = send_system_info.NetParam.SubNet;
				netparam.DefaultGateway = send_system_info.NetParam.DefaultGateway;
				netparam.CenterIP = send_system_info.NetParam.CenterIP;
				netparam.ManagerIP = send_system_info.NetParam.ManagerIP[0];
				netparam.ManagerIP1 = send_system_info.NetParam.ManagerIP[1];
			    netparam.ManagerIP2 = send_system_info.NetParam.ManagerIP[2];
			 	netparam.IP1 = send_system_info.NetParam.NetdoorIP[0];
				
				//设备编号
				FULL_DEVICE_NO devno;
				memset(&devno, 0, sizeof(FULL_DEVICE_NO));
				
				#ifdef	_USE_AURINE_SET_
				devno.DeviceNo.DeviceNo1 = send_system_info.DeviceNo.DeviceNo1;
				devno.DeviceNo.DeviceNo2 = send_system_info.DeviceNo.DeviceNo2;
				//memcpy(&devno.DeviceNo, &send_system_info.DeviceNo + 4, sizeof(DEVICE_NO));
				#else				
				memcpy(&devno.DeviceNo, &send_system_info.DeviceNo, sizeof(DEVICE_NO));
				#endif
				
				memcpy(&devno.Rule, &send_system_info.DeviceRule, sizeof(DEVICENO_RULE));
				
				//设置分段描述
				char ssub[10] = {0};
				memset(ssub, 0, sizeof(ssub));
				sprintf(ssub, "%d", devno.Rule.Subsection);
				if (sublen != strlen(ssub))
				{
					log_printf("rule subsection ECHO_ERROR !!! \n");			
					#ifdef	_USE_AURINE_SET_	
					au_net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
					#else
					net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
					#endif
					return FALSE;
				}
				
				//工程密码
				storage_set_pass(PASS_TYPE_ADMIN, send_system_info.ProjectPwd);

				//外部模块
				storage_set_extmode(TRUE, EXT_MODE_JD, send_system_info.ExtModule[0]); 
				storage_set_extmode(TRUE, EXT_MODE_NETDOOR1, send_system_info.ExtModule[1]); 
				storage_set_extmode(TRUE, EXT_MODE_NETDOOR2, send_system_info.ExtModule[2]); 
				storage_set_extmode(TRUE, EXT_MODE_GENERAL_STAIR, send_system_info.ExtModule[5]); 
				
				if (storage_save_setdevno(TRUE, devno.Rule, &devno.DeviceNo))
				{
					log_printf("storage_save_devno ECHO_ERROR !!! \n");		
					#ifdef	_USE_AURINE_SET_	
					au_net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
					#else
					net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
					#endif
					return FALSE;
				}
				
				//设备号正确后再设置分段描述
				storage_set_dev_desc(sub_des);
				
				//log_printf("solf_set_areano : CMD_SOFT_SET_PARAM : AreaNo:%d\n", send_system_info.DeviceNo.AreaNo);	
				#ifdef	_USE_AURINE_SET_	
				au_net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);
				#else
				net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);
				#endif
				
				//设置网络参数
				memcpy(&gpSysParam->Netparam, &netparam, sizeof(NET_PARAM));
				net_set_local_param(storage_get_netparam()); 	
				SaveRegInfo();
				net_change_comm_deviceno();
				
				//设置语言			
				LANGUAGE_E language = storage_get_language();
				uint16 Lan;
				switch(language)
				{
					case CHINESE:
						Lan = LAN_CHINESE;
						break;
					case ENGLISH:
						Lan = LAN_ENGLISH;
						break;
					case CHNBIG5:
						Lan = LAN_BIG5;
						break;
				}
				
				if(Lan != send_system_info.language)
				{
					switch (send_system_info.language)
					{
						case LAN_CHINESE:
							storage_set_language(CHINESE);
							break;
					
						case LAN_ENGLISH:
							storage_set_language(ENGLISH);
							break;
					
						case LAN_BIG5:
							storage_set_language(CHNBIG5);
							break;
					
						default:
							storage_set_language(CHINESE);
							break;
					}	
					
					hw_stop_feet_dog();
				}
				return TRUE;

			}

		case DATA_ID_SECUPARAM:
			{
				uint16 alarm_time,exit_time;
				alarm_time = *(uint16*)(data+4);
				exit_time = *(uint16*)(data+6);

				#ifdef	_USE_AURINE_SET_	
				au_net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0 );	
				#else
				net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0 );	
				#endif
				
				SECU_ZONEINFO zone_info;
				int i;
				uint8 enable = 0;
				uint8 area_type[8];
				uint8 delay_time[8];
				uint8 part_valid = 0;
				uint8 is_24_hour = 0;
				uint8 can_see = 0;
				uint8 can_hear = 0;
				uint8 finder_state = 0;
				
				for(i = 0; i < 8; i++)
				{
				
					memset(&zone_info, 0, sizeof(SECU_ZONEINFO));
					memcpy(&zone_info, data+8+i*(sizeof(SECU_ZONEINFO)), sizeof(SECU_ZONEINFO));

					enable |= (zone_info.Enable << i);
					area_type[i] = zone_info.ProbeType;
					delay_time[i] = zone_info.DelayTime;
					part_valid |= (zone_info.IsPartValid << i);
					is_24_hour |= (zone_info.Is24Hour << i);
					can_see |= (zone_info.IsSee << i);
					can_hear |= (zone_info.IsSound << i);
					finder_state |= (zone_info.ProbeState << i);
				}
				
				gpAfParam->enable = enable;
				memcpy(gpAfParam->area_type, area_type, sizeof(area_type));
				memcpy(gpAfParam->delay_time, delay_time, sizeof(delay_time));
				gpAfParam->part_valid = part_valid;
				gpAfParam->is_24_hour = is_24_hour;
				gpAfParam->can_see = can_see;
				gpAfParam->can_hear = can_hear;
				gpAfParam->finder_state = finder_state;
				gpAfParam->exit_alarm_time = exit_time;
				gpAfParam->alarm_time = alarm_time;
				SaveRegInfo();
				
				return TRUE;
			}
		#if 0
		case DATA_ID_ELECPARAM:
			{
				//家电设备信息
				ELEC_DEVINFO elec_info;

				uint16 jdfactory = *(uint16*)(data+4);  //家电厂商
				uint16 count = *(uint16*)(data+6);		//设备个数

				storage_set_extmode(1, EXT_MODE_JD_FACTORY, jdfactory);
				
				PJD_DEV_LIST_INFO g_jdList = NULL;
				malloc_jd_memory(&g_jdList, count);

				net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);	
				
				int i;
				
				for(i = 0; i < count; i++)
				{
					memset(&elec_info, 0, sizeof(ELEC_DEVINFO));
					memcpy(&elec_info, data+8+i*(sizeof(ELEC_DEVINFO)), sizeof(ELEC_DEVINFO));

					g_jdList->pjd_dev_info[i].JdType = elec_info.JdType;
					g_jdList->pjd_dev_info[i].Index = elec_info.Index;
					g_jdList->pjd_dev_info[i].Address = elec_info.Address;
					memcpy(g_jdList->pjd_dev_info[i].Name, elec_info.Name, 30);
					g_jdList->pjd_dev_info[i].IsUsed = elec_info.IsUsed;
					g_jdList->pjd_dev_info[i].IsTune = elec_info.IsTune;
					
				}
				
				if(count > 0)
				{
					save_jd_storage(g_jdList->pjd_dev_info[0].JdType, g_jdList->pjd_dev_info, count);
				}
				
				free_jd_memory(&g_jdList);
				
				return TRUE;	
			}

		case DATA_ID_CAMERALIST:
			{
				PMonitorDeviceList g_list = NULL;				
				storage_free_monitordev_memory(&g_list);
				storage_get_home_monitordev(&g_list);

				NEWHOMECAMERA camera;
				uint32 canera_num = *(uint32*)(data+4); 			//摄像头个数
				int i;
				net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);	
				
				if (0 < canera_num)
				{
					for (i = 0; i < canera_num; i++)
					{
						memset(&camera, 0, sizeof(NEWHOMECAMERA));
						memcpy(&camera, data+8+i*(sizeof(NEWHOMECAMERA)), sizeof(NEWHOMECAMERA));

						g_list->Homedev[i].EnableOpen = camera.Enable;
						memcpy(g_list->Homedev[i].DeviceName, camera.DeviceName, sizeof(camera.DeviceName));
						g_list->Homedev[i].DeviceIP = camera.DeviceIP;
						g_list->Homedev[i].DevPort = camera.DevPort;
						g_list->Homedev[i].ChannelNumber = camera.Channel;
						memcpy(g_list->Homedev[i].FactoryName, camera.FactoryName, sizeof(camera.FactoryName));
						memcpy(g_list->Homedev[i].UserName, camera.UserName, sizeof(camera.UserName));
						memcpy(g_list->Homedev[i].Password, camera.Password, sizeof(camera.Password));
						g_list->Homedev[i].CanControlPTZ = camera.CanControlPTZ;
						
					}
					
					storage_save_monitordev(HOME_CAMERA, g_list);
					storage_free_monitordev_memory(&g_list);		
				}
				
				return TRUE;	

			}
		#endif

		default:
			// 功能类型不正确应答
			log_printf("func ECHO_ERROR\n");
			#ifdef	_USE_AURINE_SET_	
			au_net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
			#else
			net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
			#endif
			return  FALSE;
	}
	
}

/*************************************************
  Function:			set_param_cmd_deal
  Description:		
  Input: 	
  	1.recPacket		接收包
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int32 terminal_cmd_deal(const PRECIVE_PACKET recPacket)
{
	#ifdef	_USE_AURINE_SET_
	char * data = recPacket->data + AU_NET_HEAD_SIZE;
	#else
	char * data = recPacket->data + NET_HEAD_SIZE;
	#endif

	uint32 style= *(uint32 *)data;

	if(OPER_REBOOT == style)					
	{
		#ifdef	_USE_AURINE_SET_	
		au_net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);
		#else
		net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);
		#endif
		hw_stop_feet_dog();							//让单片机来重启
	}
	else if(OPER_RECOVER == style)
	{
		#ifdef	_USE_AURINE_SET_	
		au_net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);
		#else
		net_send_echo_packet_ext(recPacket, ECHO_OK, NULL, 0);
		#endif
		storage_recover_factory();					//恢复出厂设置
		hw_stop_feet_dog();	
	}
	else
	{
		#ifdef	_USE_AURINE_SET_	
		au_net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
		#else
		net_send_echo_packet_ext(recPacket, ECHO_ERROR, NULL, 0);
		#endif
	}
	return TRUE;
}


