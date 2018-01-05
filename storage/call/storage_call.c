/*********************************************************
  Copyright (C), 2009-2012
  File name:	storage_call.c
  Author:   	唐晓磊
  Version:   	1.0
  Date: 		09.4.7
  Description:  存储模块--对讲
  History:            
*********************************************************/
#include "storage_include.h"

#ifdef _ETHERNET_DHCP_
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>      
#include <linux/rtnetlink.h>  
#include <net/if.h>					//获取本机ip函数用到

struct route_info{
    u_int dstAddr;
    u_int srcAddr;
    u_int gateWay;
    char ifName[IF_NAMESIZE];
};

#define BUFSIZE 		8192
#define MAXINTERFACES	6
#endif

#define CALLINFO_SIZE			(sizeof(MCALLINFO))
#define MONITORINFO_SIZE			(sizeof(MONITORINFO))

/*************************************************
  Function:		free_call_memory
  Description: 	释放对讲存储内存
  Input:		
  	1.CallList
  Output:		无
  Return:		无
  Others:
*************************************************/
static void free_call_memory(PMCALLLISTINFO* CallList)
{
	if ((*CallList))
	{
		if ((*CallList)->CallInfo)
		{
			free((*CallList)->CallInfo);
			(*CallList)->CallInfo = NULL;
		}
		free((*CallList));	
		(*CallList) = NULL;
	}
}

/*************************************************
  Function:		malloc_call_memory
  Description: 	申请对讲存储内存
  Input:
  	1.CallList
  	2.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
static void malloc_call_memory (PMCALLLISTINFO *CallList,uint32 MaxNum)
{
	*CallList = (PMCALLLISTINFO)malloc(sizeof(MCALLLISTINFO));	

	if (*CallList)
	{
		(*CallList)->CallCount = 0;
		(*CallList)->CallInfo = (PMCALLINFO)malloc(CALLINFO_SIZE * MaxNum);
	}
}		

/*************************************************
  Function:		get_callrecord_from_storage
  Description: 	获得通话记录
  Input:		
  	1.CallList
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_callrecord_from_storage(FLAG_STORAGE mode, PMCALLLISTINFO CallList)
{
	if (CallList)
	{
		switch (mode)
		{
			case FLAG_CALLIN:
				CallList->CallCount = Fread_common(CALL_IN_PATH, CallList->CallInfo, CALLINFO_SIZE, MAX_RECORD_NUM);
				break;

			case FLAG_CALLOUT:
				CallList->CallCount = Fread_common(CALL_OUT_PATH, CallList->CallInfo, CALLINFO_SIZE, MAX_RECORD_NUM);
				break;	

			case FLAG_CALLMISS:
				CallList->CallCount = Fread_common(CALL_MISS_PATH, CallList->CallInfo, CALLINFO_SIZE, MAX_RECORD_NUM);
				break;
				
			default:
				break;
		}
	}
}

/*************************************************
  Function:		save_call_storage
  Description: 	按模块存储
  Input:	
  	1.mode
  	2.CallList	
  Output:		无
  Return:		无
  Others:
*************************************************/
static void save_call_storage(FLAG_STORAGE mode, PMCALLLISTINFO CallList)
{
	switch (mode)
	{
		case FLAG_CALLIN:
			Fwrite_common(CALL_IN_PATH, CallList->CallInfo, CALLINFO_SIZE, CallList->CallCount);
			break;

		case FLAG_CALLOUT:
			Fwrite_common(CALL_OUT_PATH, CallList->CallInfo, CALLINFO_SIZE, CallList->CallCount);
			break;	

		case FLAG_CALLMISS:
			Fwrite_common(CALL_MISS_PATH, CallList->CallInfo, CALLINFO_SIZE, CallList->CallCount);
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		storage_get_callrecord
  Description: 	获得通话记录
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PMCALLLISTINFO storage_get_callrecord (CALL_TYPE Calltype)
{
	PMCALLLISTINFO info = NULL;
	FLAG_STORAGE mode = FLAG_CALLMISS;

	switch (Calltype)
	{
		case INCOMING:
			mode = FLAG_CALLIN;
			break;

		case OUTGOING:
			mode = FLAG_CALLOUT;
			break;	

		case MISSED:
			mode = FLAG_CALLMISS;
			break;
			
		default:
			break;
	}
	
	malloc_call_memory(&info, MAX_RECORD_NUM);
	if (NULL == info)
	{
		return NULL;
	}
	get_callrecord_from_storage(mode, info);

	return info;
}

/*************************************************
  Function:		storage_add_callrecord
  Description: 	添加通话记录
  Input:		
  	1.Calltype	记录类型
  	2.Type		设备类型
  	3.devno		设备号码
  	3.Time		时间
  	4.LylyFlag	图片状态
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_callrecord (CALL_TYPE Calltype, DEVICE_TYPE_E Type, char *devno, ZONE_DATE_TIME Time, uint8 LylyFlag)
{
	PMCALLLISTINFO calllist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	MCALLINFO buf[MAX_RECORD_NUM];
	uint32 num = 0;
	FLAG_STORAGE flag;
	int nlen = 0;

	switch(Calltype)
	{
		case MISSED:
			flag = FLAG_CALLMISS;
			break;

		case OUTGOING:
			flag = FLAG_CALLOUT;
			break;
			
		case INCOMING:
			flag = FLAG_CALLIN;
			break;
			
		default:
			break;
	}

	calllist = storage_get_callrecord(Calltype);

	// 新记录加在头部
	if (calllist && calllist->CallInfo)
	{
		if (0 == calllist->CallCount)
		{
			calllist->CallInfo[0].Calltype = Calltype;
			calllist->CallInfo[0].ReadFlag = TRUE;
			calllist->CallInfo[0].Type = Type;
			calllist->CallInfo[0].LylyFlag = LylyFlag;
			memset(calllist->CallInfo[0].devno,0,sizeof(calllist->CallInfo[0].devno));
			if (devno)
			{
				nlen = strlen(devno);
			}
			
			if (nlen > 0)
			{
				memcpy(calllist->CallInfo[0].devno,devno,nlen);
			}
			memcpy(&(calllist->CallInfo[0].Time), &Time, sizeof(ZONE_DATE_TIME));			
			calllist->CallCount = 1;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			buf[0].Calltype = Calltype;
			buf[0].ReadFlag= TRUE;
			buf[0].Type= Type;
			buf[0].LylyFlag = LylyFlag;
			memset(buf[0].devno,0,sizeof(buf[0].devno));
			if (devno)
			{
				nlen = strlen(devno);
			}
			
			if (nlen > 0)
			{
				memcpy(buf[0].devno,devno,nlen);
			}
			memcpy(&(buf[0].Time), &Time, sizeof(ZONE_DATE_TIME));	
			num = calllist->CallCount;
			if (num >= MAX_RECORD_NUM)
			{
				num = MAX_RECORD_NUM-1;
			}
			// 将原有记录加在新记录后面
			memcpy(&(buf[1]), calllist->CallInfo, num*CALLINFO_SIZE);
			calllist->CallCount++;
			if (calllist->CallCount > MAX_RECORD_NUM)
			{
				calllist->CallCount = MAX_RECORD_NUM;
			}

			memset(calllist->CallInfo, 0, sizeof(calllist->CallInfo));
			memcpy(calllist->CallInfo, buf, CALLINFO_SIZE*calllist->CallCount);
		}
		
		save_call_storage(flag, calllist);
		ret = ECHO_STORAGE_OK;
	}

	free_call_memory(&calllist);
	return ret;
}

/*************************************************
  Function:		storage_del_callrecord
  Description: 	删除通话记录
  Input:		
  	1.Calltype	记录类型
  	2.Index		索引
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_del_callrecord (CALL_TYPE Calltype, uint8 Index)
{
	uint8 i, j;
	PMCALLLISTINFO calllist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	MCALLINFO buf[MAX_RECORD_NUM];
	uint32 num = 0;
	FLAG_STORAGE flag;

	switch(Calltype)
	{
		case MISSED:
			flag = FLAG_CALLMISS;
			break;
			
		case OUTGOING:
			flag = FLAG_CALLOUT;
			break;
			
		case INCOMING:
			flag = FLAG_CALLIN;
			break;
			
		default:
			break;
	}

	calllist = storage_get_callrecord(Calltype);
	
	if (calllist && calllist->CallInfo)
	{
		num = calllist->CallCount;
		if (num > 0 && Index <= (num-1))
		{
			if (Index == (num-1))
			{
				memset((calllist->CallInfo+Index), 0, CALLINFO_SIZE);
			}
			else
			{	
				memset(buf, 0, CALLINFO_SIZE*MAX_RECORD_NUM);
				memcpy(buf, calllist->CallInfo, CALLINFO_SIZE*num);
				memset(calllist->CallInfo, 0, CALLINFO_SIZE*MAX_RECORD_NUM);
				j = 0;
				for(i = 0; i < num; i++)
				{
					if (Index != i)	
					{
						memcpy(calllist->CallInfo+j, buf+i, CALLINFO_SIZE);
						j++;
					}
				}
			}
			calllist->CallCount--;
		}
		
		save_call_storage(flag, calllist);
		ret = ECHO_STORAGE_OK;
	}

	free_call_memory(&calllist);
	return ret;
}

/*************************************************
  Function:		storage_clear_callrecord
  Description: 	清空通话记录
  Input:		
  	1.Calltype	记录类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_clear_callrecord (CALL_TYPE Calltype)
{
	FLAG_STORAGE flag;
	PMCALLLISTINFO calllist = NULL;
	
	switch(Calltype)
	{
		case MISSED:
			flag = FLAG_CALLMISS;
			break;
			
		case OUTGOING:
			flag = FLAG_CALLOUT;
			break;
			
		case INCOMING:
			flag = FLAG_CALLIN;
			break;
			
		default:
			break;
	}

	calllist = storage_get_callrecord(Calltype);
	
	if (calllist)
	{	
		calllist->CallCount = 0;
		memset(calllist->CallInfo, 0, CALLINFO_SIZE*MAX_RECORD_NUM);
		save_call_storage(flag, calllist);
		free_call_memory(&calllist);
		return ECHO_STORAGE_OK;
	}
	else
	{
		free_call_memory(&calllist);
		return ECHO_STORAGE_ERR;
	}
}

/*************************************************
  Function:		storage_set_callrecord_lylyflag
  Description: 	设置该记录是否有关联的留影留言
  Input:		
  	1.time
  	2.LylyFlag
  Output:		无
  Return:		无
  Others:
*************************************************/
FLAG_STORAGE storage_set_callrecord_lylyflag(ZONE_DATE_TIME time, uint8 LylyFlag)
{	
	//FLAG_STORAGE flag;
	PMCALLLISTINFO calllist = NULL;
	uint8 i;
	
	calllist = storage_get_callrecord(MISSED);
	
	if (calllist && calllist->CallCount > 0)
	{	
		for (i = 0; i < calllist->CallCount; i++)
		{
			if (time.year == calllist->CallInfo[i].Time.year && time.month == calllist->CallInfo[i].Time.month
					&& time.day == calllist->CallInfo[i].Time.day && time.hour == calllist->CallInfo[i].Time.hour
					&& time.min == calllist->CallInfo[i].Time.min && time.sec == calllist->CallInfo[i].Time.sec)
			{
				calllist->CallInfo[i].LylyFlag = LylyFlag;
			}
		}
		
		save_call_storage(FLAG_CALLMISS, calllist);
		free_call_memory(&calllist);
		return ECHO_STORAGE_OK;
	}
	else
	{
		free_call_memory(&calllist);
		return ECHO_STORAGE_ERR;
	}
}

/*************************************************
  Function:		storage_clear_all_callrecord
  Description:  清空对讲模块所有通话记录
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_clear_all_callrecord(void)
{
	storage_clear_callrecord(MISSED);
	storage_clear_callrecord(OUTGOING);
	storage_clear_callrecord(INCOMING);
}

/*************************************************
  Function:		storage_get_callrecord_state
  Description:  获取是否有未接来电
  Input:		无
  Output:		无
  Return:		TRUE -- 有未接来电
  				FALSE -- 没有未接来电
  Others:		
*************************************************/
uint8 storage_get_callrecord_state(void)
{
	uint8 i;
	uint8 ret = FALSE;
	
	PMCALLLISTINFO calllist = storage_get_callrecord(MISSED);
	if (calllist && calllist->CallCount > 0)
	{
		for(i = 0; i < calllist->CallCount; i++)
		{
			if (1 == calllist->CallInfo[i].ReadFlag)
			{
				ret = TRUE;
				break;
			}
		}
		free_call_memory(&calllist);
	}
	
	return ret;
}

/*************************************************
  Function:		storage_set_callrecord_state
  Description:  将未接来电设置为已读状态
  Input:		无
  Output:		无
  Return:		无
  Others:		
*************************************************/
void storage_set_callrecord_state(void)
{
	uint8 i;
	//uint8 ret = FALSE;
	
	PMCALLLISTINFO calllist = storage_get_callrecord(MISSED);
	
	if (calllist && calllist->CallCount > 0)
	{
		for(i = 0; i < calllist->CallCount; i++)
		{
			calllist->CallInfo[i].ReadFlag = 0;
		}
	}
	save_call_storage(FLAG_CALLMISS, calllist);
	free_call_memory(&calllist);
}

/*********************************************************/
/*						网络设置						 */	
/*********************************************************/

/*************************************************
  Function:		storage_get_netparam
  Description: 	获取IP信息结构
  Input:		
  Output:		无
  Return:		网络参数
  Others:
*************************************************/
PNET_PARAM storage_get_netparam(void)
{
	return &(gpSysParam->Netparam);
}

/*************************************************
  Function:		storage_get_netparam_bytype
  Description: 	获取IP信息
  Input:		
  	1.type		要获取的IP类型
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_netparam_bytype(IP_TYPE type)
{
	switch(type)
	{
		case HOST_IPADDR:		return gpSysParam->Netparam.IP;
		case HOST_NETMASK:		return gpSysParam->Netparam.SubNet;
		case HOST_GATEWAY:		return gpSysParam->Netparam.DefaultGateway;
		#ifdef _AURINE_REG_
		case AURINE_IPADDR:		return gpSysParam->Netparam.AurineServer;
		#endif
		case CENTER_IPADDR:		return gpSysParam->Netparam.CenterIP;
		case MANAGER1_IPADDR:	return gpSysParam->Netparam.ManagerIP;
		#ifdef _MULTI_CENTER_
		case MANAGER2_IPADDR:	return gpSysParam->Netparam.ManagerIP1;	
		case MANAGER3_IPADDR:	return gpSysParam->Netparam.ManagerIP2;	
		#endif
		#ifdef _USE_ELEVATOR_
		case DIANTI_IPADDR:		return gpSysParam->Netparam.DiantiServer;
		#endif
		#ifdef _LIVEMEDIA_
		case RTSP_IPADDR: 		return gpSysParam->RtspServer;
		#endif	
		#ifdef _SEND_EVENT_TO_STAIR_
		case STUN_IPADDR:	    return gpSysParam->Netparam.StunServer;
		#endif
		default:				return 0;
	}
}

/*************************************************
  Function:		storage_set_netparam
  Description: 	设置IP信息
  Input:			
  	1.save		是否要保存
  	1.type		要设置的IP类型
  	2.IP		IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_netparam(uint8 save, IP_TYPE type, uint32 IP)
{
	uint32 ret = FALSE;
	
	switch(type)
	{
		case HOST_IPADDR:		
			gpSysParam->Netparam.IP = IP;
			ret = TRUE;
			break;
			
		case HOST_NETMASK:		
			gpSysParam->Netparam.SubNet = IP;
			ret = TRUE;
			break;
			
		case HOST_GATEWAY:		
			gpSysParam->Netparam.DefaultGateway = IP;
			ret = TRUE;
			break;
			
		case CENTER_IPADDR:		
			gpSysParam->Netparam.CenterIP = IP;
			ret = TRUE;
			break;
			
		case MANAGER1_IPADDR:
			gpSysParam->Netparam.ManagerIP = IP;	
			ret = TRUE;
			break;

		case MANAGER2_IPADDR:	
			gpSysParam->Netparam.ManagerIP1 = IP;	
			ret = TRUE;
			break;

		case MANAGER3_IPADDR:	
			gpSysParam->Netparam.ManagerIP2 = IP;	
			ret = TRUE;
			break;
		
		#ifdef _AURINE_REG_
		case AURINE_IPADDR:		
			gpSysParam->Netparam.AurineServer = IP;
			ret = TRUE;
			break;
		#endif

		#ifdef _USE_ELEVATOR_
		case DIANTI_IPADDR:		
			gpSysParam->Netparam.DiantiServer = IP;
			ret = TRUE;
			break;
		#endif

		#ifdef _LIVEMEDIA_
		case RTSP_IPADDR:		
			gpSysParam->RtspServer = IP;
			ret = TRUE;
			break;
		#endif

		#ifdef _SEND_EVENT_TO_STAIR_
		case STUN_IPADDR:	
		gpSysParam->Netparam.StunServer = IP;
		ret = TRUE;
		break;
		#endif
		
		default:				
			break;
	}

	if (save && ret)
	{
		SaveRegInfo();
	}
	return ret;
}

#ifdef _ETHERNET_DHCP_
/*************************************************
  Function:		storage_set_ip_enable
  Description: 	设置静态IP
  Input:		无	
  Output:		无
  Return:		无
  Others:		无
*************************************************/
void storage_set_ethernetdhcp_enable(uint8 save, uint8 enable)
{
	gpSysParam->Netparam.Enable = enable;
	if (save)
	{
		SaveRegInfo();
	}
}

/*************************************************
  Function:		storage_get_ip_enable
  Description:  获取静态IP
  Input:		无	
  Output:		无
  Return:		无
  Others:		无
*************************************************/
uint8 storage_get_ethernetdhcp_enable(void)
{
	return gpSysParam->Netparam.Enable;
}

/*************************************************
  Function:    		get_local_ipaddress
  Description:		获得指定网络设备的IP地址
  Input: 
	1.ifname		设备名称, 如eth0 ifname=null 获得任意地址
  Output:			无
	1.ip			获得的IP地址
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int get_local_ipaddress(const char * ifname, uint32 *ip)
{
	register int fd, intrface;
	int ret = FALSE;
	struct ifreq buf[MAXINTERFACES];
	//struct arpreq arp;
	struct ifconf ifc;
	if (ip==NULL)
	{
		return false;
	}
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t)buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		{
			//interface num
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface--)
			{
				//net device name				
				if (ifname!=NULL && 0!=strcmp(buf[intrface].ifr_name, ifname))
					continue;
				//Jugde whether the net card status is promisc
				/*
				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface]))) 
				{
					if (buf[intrface].ifr_flags & IFF_PROMISC)
					{
						puts ("the interface is PROMISC");
						retn++;
					}
				}
				else 
				{
					char str[256];

					sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name);
					perror (str);
				}

				//Jugde whether the net card status is up
				if (buf[intrface].ifr_flags & IFF_UP) 
				{
					puts("the interface status is UP");
				}
				else 
				{
					puts("the interface status is DOWN");
				}
				*/
				/*Get IP of the net card */
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
				{
					memcpy(ip, &(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr), 4);
					*ip = ntohl(*ip);
					ret = TRUE;
					break;
				}
			}
		}
		close (fd);
	} 
	return ret;
}

/*************************************************
  Function:    		get_local_subnetMask
  Description:		获得指定网络设备的子网掩码
  Input: 
	1.ifname		设备名称, 如eth0 ifname=null 获得任意地址
  Output:			无
	1.ip			获得的IP地址
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
int get_local_subnetMask(const char * ifname, uint32 *ip)
{
	register int fd, intrface;
	int ret = FALSE;
	struct ifreq buf[MAXINTERFACES];
	//struct arpreq arp;
	struct ifconf ifc;
	if (ip==NULL)
	{
		return false;
	}
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t)buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		{
			//interface num
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface--)
			{
				//net device name				
				if (ifname!=NULL && 0!=strcmp(buf[intrface].ifr_name, ifname))
					continue;
				//Jugde whether the net card status is promisc
				/*
				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface]))) 
				{
					if (buf[intrface].ifr_flags & IFF_PROMISC)
					{
						puts ("the interface is PROMISC");
						retn++;
					}
				}
				else 
				{
					char str[256];

					sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name);
					perror (str);
				}

				//Jugde whether the net card status is up
				if (buf[intrface].ifr_flags & IFF_UP) 
				{
					puts("the interface status is UP");
				}
				else 
				{
					puts("the interface status is DOWN");
				}
				*/
				/*Get IP of the net card */
				if (!(ioctl (fd, SIOCGIFNETMASK, (char *) &buf[intrface])))
				{
					memcpy(ip, &(((struct sockaddr_in*)(&buf[intrface].ifr_netmask))->sin_addr), 4);
					*ip = ntohl(*ip);
					ret = TRUE;
					break;
				}
			}
		}
		close (fd);
	} 
	return ret;
}

/*************************************************
  Function:    		readNlSock
  Description:	
  Input: 	
  Output:			
  Return:			
  Others:	
*************************************************/
static int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;
    do{
        //收到内核的应答
        if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
        {
            perror("SOCK READ: ");
            return -1;
        }
        
        nlHdr = (struct nlmsghdr *)bufPtr;
        //检查header是否有效
        if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
        {
            perror("Error in recieved packet");
            return -1;
        }
		
         /* Check if the its the last message */
        if(nlHdr->nlmsg_type == NLMSG_DONE)
        {
            break;
        }
        else
        {
            /* Else move the pointer to buffer appropriately */
            bufPtr += readLen;
            msgLen += readLen;
        }
        
        /* Check if its a multi part message */
        if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
        {
            /* return if its not */
            break;
        }
    } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
    return msgLen;
}

/*************************************************
  Function:    		parseRoutes
  Description:		//分析返回的路由信息  
  Input: 
  Output:		
  Return:			
  Others:	
*************************************************/
static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;
    char *tempBuf = NULL;
    //2007-12-10
    struct in_addr dst;
    struct in_addr gate;
    
    tempBuf = (char *)malloc(100);
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
    // If the route is not for AF_INET or does not belong to main routing table
    //then return.
    if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
        return;
    /* get the rtattr field */
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
        switch(rtAttr->rta_type) {
            case RTA_OIF:
                if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
                break;
            case RTA_GATEWAY:
                rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
                break;
            case RTA_PREFSRC:
                rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
                break;
            case RTA_DST:
                rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
                break;
        }
    }
    //2007-12-10
    dst.s_addr = rtInfo->dstAddr;
    if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
    {
        gate.s_addr = rtInfo->gateWay;
        sprintf(gateway, (char *)inet_ntoa(gate));
    }
    free(tempBuf);
    return;
}

/*************************************************
  Function:    		get_local_DefaultGateway
  Description:		获得指定网络设备的网关
  Input: 
	1.gateway		
  Output:			
  Return:			
  Others:
*************************************************/
int get_local_DefaultGateway(char *gateway)
{
	struct nlmsghdr *nlMsg;
	struct rtmsg *rtMsg;
	struct route_info *rtInfo;
	char msgBuf[BUFSIZE];
	
	int sock, len, msgSeq = 0;
	//创建 Socket
	if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
	{
		perror("Socket Creation: ");
		return -1;
	}
	
	/* Initialize the buffer */
	memset(msgBuf, 0, BUFSIZE);
	
	/* point the header and the msg structure pointers into the buffer */
	nlMsg = (struct nlmsghdr *)msgBuf;
	rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);
	
	/* Fill in the nlmsg header*/
	nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
	nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
	
	nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
	nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
	nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.
	
	/* Send the request */
	if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
		printf("Write To Socket Failed...\n");
		return -1;
	}
	
	/* Read the response */
	if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
		printf("Read From Socket Failed...\n");
		return -1;
	}
	/* Parse and print the response */
	rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
	for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
		memset(rtInfo, 0, sizeof(struct route_info));
		parseRoutes(nlMsg, rtInfo,gateway);
	}
	free(rtInfo);
	close(sock);
	return 0;
}

#endif

/*************************************************
  Function:		storage_get_netdoor_ip
  Description: 	设置IP信息
  Input:			
 	1.Index		索引
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_netdoor_ip(uint8 Index)
{
	if (0 == Index)
	{
		return gpSysParam->NetDoor.DoorIP1;
	}
	else
	{
		return gpSysParam->NetDoor.DoorIP2;
	}
}

/*************************************************
  Function:		storage_set_netdoor_ip
  Description: 	设置IP信息
  Input:			
  	1.save		是否要保存
 	2.Index		索引
  	3.IP		IP地址
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_netdoor_ip(uint8 save, uint8 Index, uint32 IP)
{
	if (0 == Index)
	{
		gpSysParam->NetDoor.DoorIP1 = IP;
	}
	else
	{
		gpSysParam->NetDoor.DoorIP2 = IP;
	} 
	if (save)
	{
		SaveRegInfo();
	}
	
	return TRUE;
}

/*********************************************************/
/*					设备号设备规则设置					 */	
/*********************************************************/

/*************************************************
  Function:    		str_to_deviceno
  Description:		将字符串转换为,设备号
  Input: 
	1.devno			设备号参数
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:
*************************************************/
static uint32 str_to_deviceno(PFULL_DEVICE_NO devno)
{	
	int n;
	
	if (!devno)
	{
		return FALSE;
	}
	
	devno->DeviceNo.DeviceNo1 = 0;
	devno->DeviceNo.DeviceNo2 = 0;
	n = strlen(devno->DeviceNoStr);
	if (0 == n)
	{
		return FALSE;
	}

	if (n != devno->DevNoLen)
	{
		return FALSE;
	}
	if (devno->DevNoLen > 9)
	{
		char temp[10];
		strncpy(temp, devno->DeviceNoStr, devno->DevNoLen-9);
		devno->DeviceNo.DeviceNo1 = atoi(temp);
		strncpy(temp, devno->DeviceNoStr+devno->DevNoLen-9, 9);
		devno->DeviceNo.DeviceNo2 = atoi(temp);
	}
	else
	{
		devno->DeviceNo.DeviceNo2 = atoi(devno->DeviceNoStr);
	}
	#ifndef _AU_PROTOCOL_
	devno->DeviceNo.DeviceType = devno->DeviceType;
	#endif
	
	return TRUE;
}

/*************************************************
  Function:		storage_get_devparam
  Description: 	获取设备编号结构
  Input:		
  Output:		无
  Return:		设备编号结构
  Others:
*************************************************/
PFULL_DEVICE_NO storage_get_devparam(void)
{
	return &(gpSysParam->Devparam);
}

/*************************************************
  Function:		storage_get_devrule
  Description: 	获取设备规则结构
  Input:		
  Output:		无
  Return:		设备编号结构
  Others:
*************************************************/
DEVICENO_RULE* storage_get_devrule(void)
{
	return &(gpSysParam->Devparam.Rule);
}

/*************************************************
  Function:    		storage_get_devno
  Description:		获得设备编号
  Input: 			无
  Output:			无
  Return:			设备编号
  Others:
*************************************************/
DEVICE_NO storage_get_devno(void)
{
	return gpSysParam->Devparam.DeviceNo;
}

/*************************************************
  Function:    		get_devno_str
  Description:		获得设备编号字符串
  Input: 			无
  Output:			无
  Return:			设备编号字符串
  Others:
*************************************************/
char * storage_get_devno_str(void)
{
	return gpSysParam->Devparam.DeviceNoStr;
}

/*************************************************
  Function:    		storage_get_use_cell
  Description:		读取是否启用单元号
  Input: 			无
  Output:			无
  Return:			1:启用0:未启用
  Others:
*************************************************/
uint32 storage_get_use_cell (void)
{
	return gpSysParam->Devparam.Rule.UseCellNo;
} 

/*************************************************
  Function:    		storage_set_areano
  Description:		设置区号
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 storage_set_areano (uint32 areano)
{
	if (gpSysParam->Devparam.AreaNo != areano)
	{
		gpSysParam->Devparam.AreaNo = areano;
		#ifndef _AU_PROTOCOL_
		gpSysParam->Devparam.DeviceNo.AreaNo = areano;
		#endif
		SaveRegInfo();
	}
	return TRUE;
}

/*************************************************
  Function:    		storage_get_areano
  Description:		获取区号
  Input: 			无
  Output:			无
  Return:			无
  Others:
*************************************************/
uint32 storage_get_areano (void)
{
	return gpSysParam->Devparam.AreaNo;
}

/*************************************************
  Function:    		set_devno_rule
  Description:		保存设备编号规则
  Input: 
  	1.save			是否保存
	2.rule			设备编号规则
  Output:			无
  Return:			0-成功 1-单元长度非法 2-梯号长度非法 3-房号长度非法
  					4-总长度不能大于18
  Others:
*************************************************/
uint32 storage_set_devno_rule(uint8 save, DEVICENO_RULE Rule)
{
	//合法判断
	if (Rule.CellNoLen > 2)
	{
		return 1;
	}
	if (Rule.StairNoLen < Rule.CellNoLen || Rule.StairNoLen > 9)
	{
		return 2;
	}
	if (Rule.RoomNoLen < 3 || Rule.RoomNoLen > 9)
	{
		return 3;
	}
	if ((Rule.StairNoLen+Rule.RoomNoLen) > 17) 
	{
		return 4;
	}
	if (Rule.CellNoLen == 0)
	{
		Rule.UseCellNo = FALSE;
	}
	memcpy(&gpSysParam->Devparam.Rule, &Rule, sizeof(DEVICENO_RULE));
	//gpSysParam->Devparam.Rule.Subsection = (Rule.StairNoLen -Rule.CellNoLen)*100+Rule.CellNoLen*10+Rule.RoomNoLen;
	gpSysParam->Devparam.DevNoLen = Rule.StairNoLen + Rule.RoomNoLen + 1;
	gpSysParam->Devparam.IsRight = FALSE;
	if (save)
	{
		SaveRegInfo();
	}
	return 0;
}

/*************************************************
  Function:    		storage_set_devno
  Description:		保存设备号
  Input: 
  	1.DEVICE_TYPE_E	设备类型
	2.devno			设备号字符串
  Output:			无
  Return:			
  	0-成功 
  	1-长度非法 
  	2-房号段全为0   
  	3-区口机编号必须在1-40之间 
  Others:
*************************************************/
uint32 storage_set_devno(uint8 save, DEVICE_TYPE_E DeviceType, char* DevNo)
{
	char temp[20];
	int nlen = 0;

	if (NULL == DevNo)
	{
		return 1;
	}
	
	nlen = strlen(DevNo);
	if (nlen != gpSysParam->Devparam.DevNoLen)
	{
		return 2;
	}

	strncpy(temp, DevNo+gpSysParam->Devparam.Rule.StairNoLen, gpSysParam->Devparam.Rule.RoomNoLen);
	temp[gpSysParam->Devparam.Rule.RoomNoLen] = 0;
	if (0 == atoi(temp))
	{
		return 3;
	}
	gpSysParam->Devparam.DeviceType = DeviceType;
	memset(gpSysParam->Devparam.DeviceNoStr,0,sizeof(gpSysParam->Devparam.DeviceNoStr));
	strcpy(gpSysParam->Devparam.DeviceNoStr, DevNo); 
	gpSysParam->Devparam.IsRight = str_to_deviceno(storage_get_devparam());
	if (save)
	{
		SaveRegInfo();
	}
	return 0;
}

/*************************************************
  Function:    		storage_save_devno
  Description:		保存设备编号
  Input: 
  	1.save			是否保存
  	2.rule			设备编号规则
	3.devno			设备号字符串
  Output:			无
  Return:			无
  Others:			无
*************************************************/
void storage_save_devno(uint8 save, DEVICENO_RULE Rule, char* DevNo)
{
	memcpy(&gpSysParam->Devparam.Rule, &Rule, sizeof(DEVICENO_RULE));
	gpSysParam->Devparam.DevNoLen = Rule.StairNoLen + Rule.RoomNoLen + 1;
	gpSysParam->Devparam.IsRight = FALSE;

	gpSysParam->Devparam.DeviceType = DEVICE_TYPE_ROOM;
	memset(gpSysParam->Devparam.DeviceNoStr, 0, sizeof(gpSysParam->Devparam.DeviceNoStr));
	strcpy(gpSysParam->Devparam.DeviceNoStr, DevNo); 
	gpSysParam->Devparam.IsRight = str_to_deviceno(storage_get_devparam());
	if (save)
	{
		SaveRegInfo();
	}
}

/*************************************************
  Function:    		storage_get_dev_desc
  Description:		读取设备描述符
  Input: 			无
  Output:			无
  Return:			设备描述符指针
  Others:
*************************************************/
char * storage_get_dev_desc(void)
{
	return gpSysParam->DevDesc;
}

/*************************************************
  Function:    		storage_set_dev_desc
  Description:		读取设备描述符
  Input: 			无
  Output:			无
  Return:			设备描述符指针
  Others:
*************************************************/
uint8 storage_set_dev_desc(char * str)
{
	if (str == NULL)
	{
		return FALSE;
	}

	memset(gpSysParam->DevDesc, 0, sizeof(gpSysParam->DevDesc));
	memcpy(gpSysParam->DevDesc, str, sizeof(gpSysParam->DevDesc));
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:    		storage_init_get_dev_desc
  Description:		初始化时读取设备描述
  Input:
  	1.DevDesc		描述地址
  	2.language		语言类型
  Output:			无
  Return:			设备描述符指针
  Others:
*************************************************/
void storage_init_get_dev_desc(char * DevDesc, LANGUAGE_E language)
{
	char DevDesc_China[70] = {0xb6,0xb0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0xb5,0xa5,0xd4,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0xb7,
				0xbf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	char DevDesc_Big5[70] = {0xb4,0xc9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0xb3,0xe6,0xa4,0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0xa9,
				0xd0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	char DevDesc_En[7][10] ={"Build ","Unit ","Room "};
	

	if (language == CHNBIG5)
	{
		memcpy(DevDesc, DevDesc_Big5, 70);
	}
	else if (language == ENGLISH)
	{
		memcpy(DevDesc, DevDesc_En, 70);
	}
	else
	{
		memcpy(DevDesc, DevDesc_China, 70);
	}
}

/*************************************************
  Function:		storage_call_init
  Description: 	对讲模块存储初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_call_init(void)
{
}

/*************************************************
  Function:		save_call_storage
  Description: 	按模块存储
  Input:	
  	1.mode
  	2.CallList	
  Output:		无
  Return:		无
  Others:
*************************************************/
void save_monitorlist_storage(DEVICE_TYPE_E Type, PMONITORLISTINFO monitorlist)
{
	switch (Type)
	{
		case DEVICE_TYPE_DOOR_NET:
		case DEVICE_TYPE_DOOR_PHONE:
			Fwrite_common(MONITOR_DOOR_PATH, monitorlist->pMonitorInfo, MONITORINFO_SIZE, monitorlist->MonitorCount);
			break;

		case DEVICE_TYPE_STAIR:
			Fwrite_common(MONITOR_STAIR_PATH, monitorlist->pMonitorInfo, MONITORINFO_SIZE, monitorlist->MonitorCount);
			break;	

		case DEVICE_TYPE_AREA:
			Fwrite_common(MONITOR_AREA_PATH, monitorlist->pMonitorInfo, MONITORINFO_SIZE, monitorlist->MonitorCount);
			break;
			
		default:
			break;
	}
}

/*************************************************
  Function:		free_monitorlist_memory
  Description: 	释放对讲存储内存
  Input:		
  	1.CallList
  Output:		无
  Return:		无
  Others:
*************************************************/
void free_monitorlist_memory(PMONITORLISTINFO* monitorlist)
{
	if ((*monitorlist))
	{
		if ((*monitorlist)->pMonitorInfo)
		{
			free((*monitorlist)->pMonitorInfo);
			(*monitorlist)->pMonitorInfo = NULL;
		}
		free((*monitorlist));	
		(*monitorlist) = NULL;
	}
}

/*************************************************
  Function:		get_callrecord_from_storage
  Description: 	获得通话记录
  Input:		
  	1.CallList
  Output:		无
  Return:		无
  Others:
*************************************************/
static void get_monitorinfo_from_storage(DEVICE_TYPE_E devtype, PMONITORLISTINFO MonitorList)
{
	if (MonitorList)
	{
		switch (devtype)
		{
			case DEVICE_TYPE_DOOR_NET:
			case DEVICE_TYPE_DOOR_PHONE:
				MonitorList->MonitorCount = Fread_common(MONITOR_DOOR_PATH, MonitorList->pMonitorInfo, MONITORINFO_SIZE, MAX_MONITOR_NUM);
				break;

			case DEVICE_TYPE_STAIR:
				MonitorList->MonitorCount = Fread_common(MONITOR_STAIR_PATH, MonitorList->pMonitorInfo, MONITORINFO_SIZE, MAX_MONITOR_NUM);
				break;	

			case DEVICE_TYPE_AREA:
				MonitorList->MonitorCount = Fread_common(MONITOR_AREA_PATH, MonitorList->pMonitorInfo, MONITORINFO_SIZE, MAX_MONITOR_NUM);
				break;
				
			default:
				break;
		}
		
	}
}

/*************************************************
  Function:		malloc_call_memory
  Description: 	申请对讲存储内存
  Input:
  	1.CallList
  	2.MaxNum	最大记录数
  Output:		无
  Return:		无
  Others:
*************************************************/
void malloc_monitorlist_memory (PMONITORLISTINFO *MonitorList,uint32 MaxNum)
{
	*MonitorList = (PMONITORLISTINFO)malloc(sizeof(MONITORLISTINFO));	

	if (*MonitorList)
	{
		(*MonitorList)->MonitorCount = 0;
		(*MonitorList)->pMonitorInfo = (PMONITORINFO)malloc(MONITORINFO_SIZE * MaxNum);
	}
}	

/*************************************************
  Function:		storage_get_monitorlist
  Description: 	 获取监视列表信息
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PMONITORINFO storage_get_monitorinfo (DEVICE_TYPE_E devtype, int8 index)
{
	PMONITORINFO info = NULL;
	PMONITORLISTINFO monitorlist = NULL;
	monitorlist = storage_get_monitorlist(devtype);
	if (monitorlist->MonitorCount == 0)
	{
		log_printf(" storage_get_monitorlist return 0 \n");
		free_monitorlist_memory(&monitorlist);
		return NULL;
	}

	// 记得使用完 内存释放
	info = (PMONITORINFO)malloc(MONITORINFO_SIZE);
	if (NULL == info)
	{
		free_monitorlist_memory(&monitorlist);
		return NULL;
	}
	info->DeviceType = monitorlist->pMonitorInfo[index].DeviceType;
	info->index = monitorlist->pMonitorInfo[index].index;
	info->IP = monitorlist->pMonitorInfo[index].IP;
	
	free_monitorlist_memory(&monitorlist);
	return info;
}

/*************************************************
  Function:		storage_get_monitorlist
  Description: 	 获取监视列表信息
  Input:		
  	1.Calltype
  Output:		无
  Return:		无
  Others:		
  	1.该指针外部释放
*************************************************/
PMONITORLISTINFO storage_get_monitorlist(DEVICE_TYPE_E devtype)
{
	PMONITORLISTINFO info = NULL;
		
	malloc_monitorlist_memory(&info, MAX_MONITOR_NUM);
	if (NULL == info)
	{
		return NULL;
	}
	get_monitorinfo_from_storage(devtype,info);

	return info;
}

/*************************************************
  Function:		storage_add_monitorlist
  Description: 	增加监视设备
  Input:		
  	1.Type		设备类型
  	2.index		设备索引
  	3.ip		设备ip
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
ECHO_STORAGE storage_add_monitorlist (DEVICE_TYPE_E Type, int8 index, uint32 ip)
{
	PMONITORLISTINFO monitorlist = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	MONITORINFO buf[MAX_MONITOR_NUM];
	uint32 num = 0;

	monitorlist = storage_get_monitorlist(Type);

	// 新记录加在头部
	if (monitorlist && monitorlist->pMonitorInfo)
	{
		if (0 == monitorlist->MonitorCount)
		{
			monitorlist->pMonitorInfo[0].IP = ip;
			monitorlist->pMonitorInfo[0].index = index;
			monitorlist->pMonitorInfo[0].DeviceType = Type;	
			monitorlist->MonitorCount = 1;
		}
		else
		{
			memset(buf, 0, sizeof(buf));
			buf[0].IP = ip;
			buf[0].index= index;
			buf[0].DeviceType= Type;
			
			num = monitorlist->MonitorCount;
			if (num >= MAX_MONITOR_NUM)
			{
				num = MAX_MONITOR_NUM-1;
			}
			// 将原有记录加在新记录后面
			memcpy(&(buf[1]), monitorlist->pMonitorInfo, num*MONITORINFO_SIZE);
			monitorlist->MonitorCount++;
			if (monitorlist->MonitorCount > MONITORINFO_SIZE)
			{
				monitorlist->MonitorCount = MONITORINFO_SIZE;
			}

			memset(monitorlist->pMonitorInfo, 0, sizeof(monitorlist->pMonitorInfo));
			memcpy(monitorlist->pMonitorInfo, buf, MONITORINFO_SIZE*monitorlist->MonitorCount);
		}
		
		save_monitorlist_storage(Type, monitorlist);
		ret = ECHO_STORAGE_OK;
	}

	free_monitorlist_memory(&monitorlist);
	return ret;
}


/*************************************************
  Function:		storage_clear_monitorlist
  Description: 	清空存储文件内容
  Input:		
  	1.Type		设备类型
  Output:		无
  Return:		ECHO_STORAGE
  Others:
*************************************************/
void storage_clear_monitorlist(DEVICE_TYPE_E Type)
{
	FILE* fPListFile = NULL;

	switch (Type)
	{
		case DEVICE_TYPE_DOOR_NET:
		case DEVICE_TYPE_DOOR_PHONE:	
			fPListFile = fopen(MONITOR_DOOR_PATH, "w");
			//Fwrite_common(MONITOR_DOOR_PATH, monitorlist->pMonitorInfo, MONITORINFO_SIZE, monitorlist->MonitorCount);
			break;

		case DEVICE_TYPE_STAIR:
			fPListFile = fopen(MONITOR_STAIR_PATH, "w");
			//Fwrite_common(MONITOR_STAIR_PATH, monitorlist->pMonitorInfo, MONITORINFO_SIZE, monitorlist->MonitorCount);
			break;	

		case DEVICE_TYPE_AREA:
			fPListFile = fopen(MONITOR_AREA_PATH, "w");
			//Fwrite_common(MONITOR_AREA_PATH, monitorlist->pMonitorInfo, MONITORINFO_SIZE, monitorlist->MonitorCount);
			break;
			
		default:
			break;
	}
    
	fclose(fPListFile);
	system("sync");
}

#if 0
/************* 以下为视频参数设置部分 *****************/
////////////////////////////////////////////////////////////

/*************************************************
  Function:			storage_get_video_param
  Description:		从文件中获取视频参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static uint8 storage_set_video_param(void)
{
	FILE *fp;
	fp = fopen(VIDEO_FILE_NAME,"w+");
	if(fp == NULL)
	{
		return FALSE;
	}
	g_venc_parm.encMode = VIDEO_ENCMODE_H264;
	g_venc_parm.frameRate = 15;
	g_venc_parm.imgSize = VIDEO_SIZE_CIF; // 352*288
	g_venc_parm.resver0 = 0;
	g_venc_parm.bitRate = 512000;//128000;//512000;            // 512kbps
	g_venc_parm.resver1 = 0;
	g_venc_parm.resver2 = 0;
	fwrite(&g_venc_parm, sizeof(VIDEO_SDP_PARAM), fp);
	fclose(fp);
	
}

/*************************************************
  Function:			storage_get_video_param
  Description:		从文件中获取视频参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static uint8 storage_set_audio_param(void)
{
	FILE *fp;
	fp = fopen(AUDIO_FILE_NAME,"w+");
	if(fp == NULL)
	{
		return FALSE;
	}
	g_audio_parm.encMode = AUDIO_ENCMODE_PCMA;
	g_audio_parm.resver0 = 0;
	g_audio_parm.resver1 = 0;
	g_audio_parm.resver2 = 0;
	fwrite(&g_audio_parm, sizeof(AUDIO_SDP_PARAM), fp);
	fclose(fp);
}

/*************************************************
  Function:			storage_get_video_param
  Description:		从文件中获取视频参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void storage_get_video_param(void)
{
	FILE *fp;
	uint8 filebuf[50];
	fp = fopen(VIDEO_FILE_NAME, "r+");
	if (fp != NULL)
	{
		fread(filebuf, sizeof(VIDEO_SDP_PARAM), fp);
		memcpy(&g_venc_parm, (VIDEO_SDP_PARAM *)filebuf, sizeof(VIDEO_SDP_PARAM));
		fclose(fp);
		return;
	}
	else
	{
		storage_set_video_param();
	}
	return;
	
}

/*************************************************
  Function:			storage_get_audio_param
  Description:		从文件中获取音频参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void storage_get_audio_param(void)
{
	FILE *fp;
	uint8 filebuf[50];
	fp = fopen(AUDIO_FILE_NAME, "r+");
	if (fp != NULL)
	{
		fread(filebuf, sizeof(AUDIO_SDP_PARAM), fp);
		memcpy(&g_audio_parm, (AUDIO_SDP_PARAM *)filebuf, sizeof(AUDIO_SDP_PARAM));
		fclose(fp);
		return;
	}
	else
	{
		storage_set_audio_param();
	}
	return;
}

/*************************************************
  Function:			stroage_sdp_param
  Description:		初始化视频编码参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void stroage_sdp_param_init(void)
{
	storage_get_video_param();
	storage_get_audio_param();
	return;
}

#else
/*************************************************
  Function:			storage_get_video_param
  Description:		从文件中获取视频参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static uint8 storage_set_video_param(void)
{
#if 0
	g_venc_parm.encMode = VIDEO_ENCMODE_H264;
	g_venc_parm.frameRate = 30;
	g_venc_parm.imgSize = VIDEO_SIZE_D1; 		
	g_venc_parm.resver0 = 0;
	g_venc_parm.bitRate = 512000;    
	g_venc_parm.resver1 = 0;
	g_venc_parm.resver2 = 0;
#endif
	memset(&g_venc_parm, 0, sizeof(VIDEO_SDP_PARAM));
	return TRUE;
}

/*************************************************
  Function:			storage_get_video_param
  Description:		从文件中获取视频参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
static uint8 storage_set_audio_param(void)
{
	g_audio_parm.encMode = AUDIO_ENCMODE_PCMA;
	g_audio_parm.resver0 = 0;
	g_audio_parm.resver1 = 0;
	g_audio_parm.resver2 = 0;
	return TRUE;
}

/*************************************************
  Function:			stroage_sdp_param
  Description:		初始化视频编码参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void storage_sdp_param_init(void)
{
	storage_set_video_param();
	storage_set_audio_param();
	return;
}
#endif

#if 0
PSYS_FLASH_DATA g_SysParam;

/*************************************************
  Function:    		set_default_deviceno
  Description:		设置默认设备号
  Input: 
	1.devno			设备号参数
  Output:			无
  Return:			无
  Others:
*************************************************/
static void set_default_deviceno(PFULL_DEVICE_NO devno)
{
	devno->AreaNo = 1;
	//规则
	devno->Rule.StairNoLen = 4;
	devno->Rule.RoomNoLen = 4;
	devno->Rule.CellNoLen = 2;
	devno->Rule.UseCellNo = 1;
	devno->Rule.Subsection = 224;
	devno->DeviceNo.DeviceNo1 = 0;
	devno->DevNoLen = 9;
	devno->IsRight = true;

	devno->DeviceType = DEVICE_TYPE_ROOM;
	devno->DeviceNo.DeviceNo2 = 10102070;
	sprintf(devno->DeviceNoStr, "010102070");
}

/*************************************************
  Function:    		set_default_netparam
  Description:		设置默认网络参数 
  Input: 
	1.devno			设备号参数
  Output:			无
  Return:			无
  Others:
*************************************************/
static void set_default_netparam(PNET_PARAM NetParam)
{	
	#if 1
	//NetParam->IP 			= 0XA6E1222;//  10.110.18.34 ;	
	NetParam->IP 			= 0XA6E10CF;//DEFAULT_DEVICEIP;//0XA6E10B1  ;	
	NetParam->SubNet 		= 0XFFFFFF00;//DEFAULT_SUBNET;
	//NetParam->DefaultGateway = 0XA6E12FE;
	NetParam->DefaultGateway = 0XA6E10FE;//DEFAULT_GATEWAY;0XA6E10FE
//	NetParam->DNS1 			= DEFAULT_DNS1;
//	NetParam->DNS2 			= DEFAULT_DNS2;
	NetParam->CenterIP 		= DEFAULT_CENTERIP;
	NetParam->ManagerIP 	= DEFAULT_MANAGERIP;		

	NetParam->SipProxyServer = DEFAULT_SIPPROXYSERVER;
	NetParam->AurineServer   = DEFAULT_AURINESERVER;
	NetParam->StunServer     = DEFAULT_STUNSERVER;		
	#else
	NetParam->IP 			= DEFAULT_DEVICEIP;
	NetParam->SubNet 		= DEFAULT_SUBNET;
	NetParam->DefaultGateway = DEFAULT_GATEWAY;
	NetParam->DNS1 			= DEFAULT_DNS1;
	NetParam->DNS2 			= DEFAULT_DNS2;
	NetParam->CenterIP 		= DEFAULT_CENTERIP;
	NetParam->ManagerIP 	= DEFAULT_MANAGERIP;		

	NetParam->SipProxyServer = DEFAULT_SIPPROXYSERVER;
	NetParam->AurineServer   = DEFAULT_AURINESERVER;
	NetParam->StunServer     = DEFAULT_STUNSERVER;	
	#endif
}

/*************************************************
  Function:    		UlongtoIP
  Description:		将long型IP地址转化为IP地址字符串
  Input:
  	1.uIp			unsigned long型IP地址
  Output:			
  Return:			转化后的字符串指针
  Others:			
*************************************************/
static char* UlongtoIP1(uint32 uIp)
{
	static char strIp[20];
	char temp[20];
	int32 i;
	uint32 t;
	memset(strIp, 0, 20);
	for(i = 3; i >= 0; i--)
	{
		t = uIp >> (i * 8);
		t = t & 0xFF;
		sprintf(temp, "%d", (uint32)t);
		strcat(strIp, temp);
		if(i != 0)
		{
			strcat(strIp, ".");
		}
	}
	return strIp;
}

/*************************************************
Function:   	 	init_ipset
Description: 		初始化nfsip
Input:              无
Output:				无
Return:				成功－ture 失败－false
Others:
*************************************************/
static uint32 init_ipset(void)
{
	char buf[255];	
	PNET_PARAM NetParam = &gpSysParam->Netparam;

	system("ifconfig eth0 down");
	system("ifconfig eth0 hw ether 00:00:00:12:34:07");
	
	memset(buf,0,sizeof(buf));
	sprintf(buf,"ifconfig %s %s ", "eth0", UlongtoIP1(NetParam->IP));
	log_printf("1.init_ipset buf:%s\n", buf);
	system(buf);
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"ifconfig %s netmask %s ","eth0", UlongtoIP1(NetParam->SubNet));	
	log_printf("2.init_ipset buf:%s\n", buf);
	system(buf);
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "route add default gw  %s ", UlongtoIP1(NetParam->DefaultGateway));	
	log_printf("3.init_ipset buf:%s\n", buf);	
	system(buf);

	system("ifconfig eth0 up");
}

/*************************************************
  Function:			storage_init_sysparam
  Description:		初始化系统参数
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
void storage_init_sysparam(void)
{
	// 测试用先直接用初始值 不存储 gpSysParam
	g_SysParam = (SYS_FLASH_DATA *)malloc(sizeof(SYS_FLASH_DATA));
	memset(g_SysParam, 0, sizeof(SYS_FLASH_DATA));
	set_default_deviceno(&g_SysParam->Devparam);
	set_default_netparam(&g_SysParam->Netparam);
	gpSysParam = g_SysParam;
	init_ipset();
}
#endif

