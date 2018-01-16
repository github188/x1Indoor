/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	storage_comm.c
  Author:    	luofl
  Version:   	2.0
  Date: 		2014-11-03
  Description:  存储模块公共接口
  				存储模块要用到的公共接口都在这实现
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/
#include "storage_include.h"
//#include "storage_path.h"

static uint8 ModelFlag[FLAG_COUNT];

/**************************************************************************/
/*								 音视频sdp参数							  */	
/**************************************************************************/
#define  VIDEO_FILE_NAME  "D:\\video_param.dat"	
#define  AUDIO_FILE_NAME  "D:\\audio_param.dat"	

/**************************************************************************/
/*								保存注册成功设备编号					  */	
/**************************************************************************/
//#define	PRE_DEVNO_FILENAME	"D:\\pre_devno.dat"

/*************************************************
  Function:		DelayMs_nops
  Description: 	
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void DelayMs_nops(uint32 count)
{
	usleep(count*1000);
	return;
}

/*************************************************
  Function:     is_fileexist
  Description:  判断指定路径文件是否存在
  Input:       	
   	1.path		指定文件的路径
  Output:       无
  Return:
				文件存在:TRUE
				文件不存在:FALSE
  Others:
*************************************************/
int32 is_fileexist(char * path)
{
	struct stat buf;
	int32 ret;
	
	if (NULL == path)
	{
		return FALSE;
	}
	ret = stat((const char *)path, &buf);
	if (-1 == ret)
	{
		if (errno == ENOENT)
		{
			return FALSE;
		}
	}
		
	return TRUE;	
}

/*************************************************
  Function:		IsHaveFile
  Description: 	是否有该文件
  Input:		
  	1.Filename	文件名
  Output:		无
  Return:		ECHO_VALUE (OK ERR)
  Others:
*************************************************/
ECHO_STORAGE IsHaveFile (char* Filename)
{
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	FILE* fPListFile = NULL;	
	
	if (Filename)
	{
		fPListFile = fopen(Filename, "r");
		if (fPListFile != NULL)
		{
			fclose(fPListFile);
			ret = ECHO_STORAGE_OK;
		}
	}
	return ret;
}

/*************************************************
  Function:		get_size
  Description: 	获得文件夹大小
  Input:		
  	1.Filename	文件夹名
  Output:		无
  Return:		SIZE
  Others:
*************************************************/
int get_size (char* srcname)
{
	#if 0
	FILE* fPListFile = NULL;	
	uint32 size = 0;
	
	if (NULL != srcname)
	{
		fPListFile = fopen(srcname, "r");
		if (fPListFile != NULL)
		{
			size = fPListFile->_bufsiz;
			fclose(fPListFile);
		}
	}
	return size;
	#else
	struct stat buf;
	int32 ret;
	
	if (NULL == srcname)
	{
	   	printf(" not find \n");
	   	return 0;
	}

	log_printf("file: %s\n", srcname);
	ret = stat(srcname, &buf);
	log_printf("fie ret: %d\n", ret);
	if (ret == -1)
	{
		return 0;
	}
    log_printf("buf.st_size: %d \n", (int)buf.st_size);
	return buf.st_size;
	#endif
}

/*************************************************
  Function:     	FSFlush
  Description:  	
  Input:       	
  Output:           无
  Return:
  Others:			文件数据写入磁盘
*************************************************/
void FSFlush(FILE* pFile)
{
	fflush(pFile);
	fsync(fileno(pFile));
}

/*************************************************
  Function:     	FSCheckSpareSpace
  Description:  	
  Input:       	
  Output:           
  Return:
  	freeDisk		可用空间 单位B
  Others:			磁盘空间
  cat /proc/mounts 可以查看文件系统中有几个可用盘
*************************************************/
uint64 FSCheckSpareSpace(const char* dir)
{

	char dirname[32];
	struct statfs diskInfo;

	if(strstr(dir, "nand") != NULL)
		strcpy(dirname, "/nand/");
	else if(strstr(dir, "tmp")!=NULL)
		strcpy(dirname, "/tmp/");

	statfs(dirname, &diskInfo);
	unsigned long long totalsize = diskInfo.f_bsize * diskInfo.f_blocks;		//总的字节数，f_blocks为block的数目
	unsigned long long freeDisk = diskInfo.f_bfree * diskInfo.f_bsize;			//剩余空间的大小
	printf("total = %llu KB Free =  %llu KB \n", totalsize>>10, freeDisk>>10);
		
	// 小于200K时会出现 fopen fail, 所以预留200K的空间
	if(freeDisk > 204800)
		freeDisk -= 204800;
	else
		freeDisk = 0;

	return (uint64)freeDisk;
}

/*************************************************
  Function:    		FSFileDelete
  Description: 		删除指定的文件
  Input: 
  	1.path			文件路径名
  Output:     		无
  Return:
					成功0:失败返回错误号
					错误代码  EROFS 文件存在于只读文件系统内
					EFAULT 参数pathname指针超出可存取内存空间
					ENAMETOOLONG 参数pathname太长
					ENOMEM 核心内存不足
					ELOOP 参数pathname 有过多符号连接问题
					EIO I/O 存取错误
  others:
*************************************************/
int32 FSFileDelete(char * path)
{	
	DIR * dir;
	struct dirent * ptr;
	char filepath[100];
	int32 ret;
	
	if (NULL == path)
	{
		return FALSE;
	}
	if (IsHaveFile(path) != ECHO_STORAGE_OK)
	{
		return FALSE;
	}
	
	if (path[strlen(path) - 1] == '/')
	{
		dir = opendir(path);
		while ((ptr = readdir(dir)) != NULL)
		{
			if ((strcmp(ptr->d_name,".") != 0) && (strcmp(ptr->d_name,"..") != 0))	
			{
				#if 0
				sprintf(filepath, "%s%s", path, ptr->d_name);
				printf("del_file filepath:%s\n",filepath);
				remove(filepath);
				#else
				memset(filepath, 0, sizeof(filepath));
				sprintf(filepath, "rm -rvf %s%s", path, ptr->d_name);
				system(filepath);
				system("sync");
				#endif
			}						
		}		
		return TRUE;
	}
	else
	{	
		#if 0
		ret = remove(path);
		if (0 != ret)
		{
			return FALSE;
		}
		#else
		memset(filepath, 0, sizeof(filepath));
		sprintf(filepath, "rm -rvf  %s ", path);
		log_printf("filepath: %s\n", filepath);
		system(filepath);
		system("sync");
		#endif
	}
	
	return TRUE;		
}

/*************************************************
  Function:		Fwrite_common
  Description: 	写数据
  Input:		
  	1.Filename	记录类型
  	2.Data		设备类型
  	3.DataSize	数据结构大小
  	4.DataNum	数据个数
  Output:		无
  Return:		ECHO_VALUE
  Others:
*************************************************/
//#define SMALL_BLOCK_SIZE		64
ECHO_STORAGE Fwrite_common (char * Filename, void * Data, int DataSize, int DataNum)
{	
	FILE* fPListFile = NULL;
	ECHO_STORAGE ret = ECHO_STORAGE_ERR;
	uint32 size = 0;
	uint32 maxsize = DataSize * DataNum;

	if (Filename == NULL || Data == NULL)
	{
		return ECHO_STORAGE_NOFILE;
	}
	
    fPListFile = fopen(Filename, "w+");
	if (maxsize == 0)
	{
		if (fPListFile != NULL )
		{
			fclose(fPListFile);
	   		fPListFile = NULL;
			ret = ECHO_STORAGE_OK;
		}		
		return ret;
	}
	
	if (fPListFile != NULL )
	{
		#if 0
		if (maxsize <= SMALL_BLOCK_SIZE)
		{
			size = fwrite((char *)Data, maxsize, 1, fPListFile);
			if (size == maxsize)
			{
				ret = ECHO_STORAGE_OK;
			}
		}
		else
		{	
			while (maxsize != size)
			{
				size = fwrite((char *)Data, maxsize, 1, fPListFile);
				maxsize -= size;
				size = 0;
			}
			ret = ECHO_STORAGE_OK;
		}
		#else
		size = fwrite((char *)Data, 1, maxsize, fPListFile);
		if (size == maxsize)
		{
			ret = ECHO_STORAGE_OK;
		}
		log_printf("Write Size  %d\n", size);
		FSFlush(fPListFile);
		#endif
		fclose(fPListFile);
	   	fPListFile = NULL;
	}	
	
	return ret;
}

#if 0
/*************************************************
  Function:    		get_filesize
  Description: 		获得指定路径文件的大小
  Input:                  
  	1.filepath		文件路径
  Output:			无
  Return:			成功:指定文件的大小(单位字节)
					不成功:FALSE
  Others:
*************************************************/
int32 get_filesize(int8 *filepath)
{
	struct stat buf;
	int32 ret;
	
	if (NULL == filepath)
	{
	   	printf(" not find \n");
	   	return -1;
	}

	ret = stat(filepath,&buf);
	if (ret == -1)
	{
		return -1;
	}
    printf("buf.st_size: %d \n", (int)buf.st_size);
	return buf.st_size;
}
#endif

/*************************************************
  Function:		Fread_common
  Description: 	读数据
  Input:		
  	1.Filename	记录类型
  	2.Data		设备类型
  	3.DataSize	数据结构大小
  	4.DataMaxNum 数据个数
  Output:		无
  Return:		数据个数
  Others:
*************************************************/
uint32 Fread_common (char* Filename, void* Data, int DataSize, int DataMaxNum)
{	
	#if 1
	FILE * fPListFile = NULL;
	int Num = 0;
	int Size = 0;
	
	if (Data == NULL)
	{
		return 0;
	}
	
	memset(Data, 0, DataSize*DataMaxNum);
    fPListFile = fopen(Filename, "r");
	if (fPListFile != NULL)
	{
		Size = get_size(Filename);
		if (Size > 0)
		{
			Num = Size/DataSize;
			if (Num > DataMaxNum)
			{
				Num = DataMaxNum;
			}
		}
	
		if (Num > 0)
		{
			fread(Data, DataSize*Num, 1, fPListFile);
		}
		fclose(fPListFile);
	   	fPListFile = NULL;
	}
	
	log_printf("Fread_common Num:  %d read size: %d DataSize: %d DataMaxNum: %d\n", 
		Num, Size, DataSize, DataMaxNum);
	return Num;
	#else
	int32 ret = 0;
	FILE * fPListFile = NULL;
	
	if (Data == NULL)
	{
		return 0;
	}
	
	memset(Data, 0, DataSize*DataMaxNum);
    fPListFile = fopen(Filename, "r");
	if (fPListFile != NULL)
	{
		ret = fread(Data, 1, DataSize*DataMaxNum, fPListFile);
	}
	log_printf("Fread_common ret:  %d DataSize*DataMaxNum = %d\n", ret, DataSize*DataMaxNum);
	
	return ret;
	#endif
}

/*************************************************
  Function:		storage_set_save_flag
  Description: 	设置存储标志
  Input:		
  	1.Flag		类型
  	2.state		状态(TURE -- 定时存储 FALSE -- 无须存储)
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_set_save_flag (FLAG_STORAGE Flag, uint8 state)
{
	if (Flag < FLAG_COUNT)
	{
		ModelFlag[Flag] = state;
	}
}

/*************************************************
  Function:		storage_get_save_flag
  Description: 	获取存储标志
  Input:		
  	1.Flag		类型
  Output:		无
  Return:		状态
  Others:
*************************************************/
uint8 storage_get_save_flag (FLAG_STORAGE Flag)
{
	if (Flag < FLAG_COUNT)
	{
		return ModelFlag[Flag];
	}
	else
	{
		return FALSE;
	}
}

/*************************************************
  Function:		storage_comm_model_save
  Description: 	公共定时存储
  Input:		无
  Output:		无
  Return:		无
  Others:		所有模块的定时存储都在这
*************************************************/
void storage_comm_model_save(void)
{
	// 报警模块定时存储
	 storage_af_timer_save();

	// 信息模块定时存储
}

/*********************************************************/
/*						语言设置						 */	
/*********************************************************/

/*************************************************
  Function:		stroage_get_language
  Description: 	获取语言
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
LANGUAGE_E storage_get_language(void)
{
	return gpSysParam->Language;
}

/*************************************************
  Function:		stroage_init_language
  Description: 	初始化语言
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
static void storage_init_language(LANGUAGE_E language)
{
	switch (language)
	{
		case CHNBIG5:
			g_China = 1;
			g_Big5 = 1;
			break;

		case ENGLISH:
			g_China = 0;
			g_Big5 = 0;
			break;

		case CHINESE:
		default:
			g_China = 1;
			g_Big5 = 0;
			break;
	}
}

/*************************************************
  Function:		stroage_set_language
  Description: 	设置语言
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_set_language(LANGUAGE_E language)
{
	if (language > ENGLISH)
	{
		language = CHINESE;
	}
	log_printf("language: %d\n", language);
	gpSysParam->Language = language;
	storage_init_language(language);
	storage_init_get_dev_desc(gpSysParam->DevDesc, language);
	log_printf("gpSysConfig->SysFlashData.Language %d\n", gpSysConfig->SysFlashData.Language);
	SaveRegInfo();
	
	return;
}

/*************************************************
  Function:		storage_get_openscreen
  Description: 	获取开屏状态
  Input:		无
  Output:		无
  Return:		0开屏 1关屏
  Others:
*************************************************/
uint8 storage_get_openscreen(void)
{
	return gpSysParam->OpenScreen;
}

/*************************************************
  Function:		stroage_set_openscreen
  Description: 	设置关屏状态
  Input:		无
  Output:		无
  Return:		0开屏 1关屏
  Others:
*************************************************/
void storage_set_openscreen(uint8 state)
{
	if (gpSysParam->OpenScreen != state)
	{
		gpSysParam->OpenScreen = state;
		SaveRegInfo();
	}
	
	return;
}

/*********************************************************/
/*						外部模块设置					 */	
/*********************************************************/

/*************************************************
  Function:		storage_get_extmode
  Description: 	获取外部模块
  Input:		无
  Output:		无
  Return:		TRUE 启用 FALSE 不启用
  Others:
*************************************************/
uint32 storage_get_extmode(EXT_MODE_TYPE type)
{
	return gpSysParam->Extparam[type];
}

/*************************************************
  Function:		storage_set_extmode
  Description: 	设置外部模块
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_extmode(uint8 save, EXT_MODE_TYPE type, uint8 state)
{
	gpSysParam->Extparam[type] = state;
	if (save)
	{
		SaveRegInfo();
	}
	return TRUE;	
}

/*********************************************************/
/*						 背光设置						 */	
/*********************************************************/

/*************************************************
  Function:		storage_get_bright
  Description: 	获取背光
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_bright(void)
{
	return gpSysParam->Bright;
}

/*************************************************
  Function:		storage_set_bright
  Description: 	设置背光
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_bright(uint8 save, uint8 Bright)
{
	if (gpSysParam->Bright != Bright)
	{
		gpSysParam->Bright = Bright;
		if (save)
		{
			SaveRegInfo();
		}
	}
	return TRUE;	
}

/*********************************************************/
/*						 桌面设置						 */	
/*********************************************************/

/*************************************************
  Function:		storage_get_desk
  Description: 	获取桌面ID
  Input:		无
  Output:		无
  Return:		桌面ID
  Others:
*************************************************/
uint32 storage_get_desk(void)
{
	return gpSysParam->DeskId;
}

/*************************************************
  Function:		storage_set_desk
  Description: 	设置桌面ID
  Input:		无
  Output:		无
  Return:		正确与否
  Others:
*************************************************/
uint32 storage_set_desk(uint8 save, uint8 ID)
{
	if (gpSysParam->DeskId != ID)
	{
		gpSysParam->DeskId = ID;
		if (save)
		{
			SaveRegInfo();
		}
	}
	return TRUE;	
}

/*********************************************************/
/*					 MAC码地址设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_mac
  Description: 	获取MAC
  Input:		
  	1.type		MAC类型
  Output:		无
  Return:		MAC码
  Others:
*************************************************/
uint8 * storage_get_mac(MAC_TYPE type)
{
	switch (type)
	{
		case HOUSE_MAC:								// 室内机MAC
			return gpSysParam->MacAddr.HouseMac;
			
		case DOOR1_MAC:								// 网络门前机1MAC
			return gpSysParam->MacAddr.Door1Mac;
			
		case DOOR2_MAC:								// 网络门前机2MAC
			return gpSysParam->MacAddr.Door2Mac;

		default:
			break;
	}
	return NULL;
}

/*************************************************
  Function:		storage_set_mac
  Description: 	设置MAC
  Input:		
  	1.type		MAC类型
  	2.data		MAC码
  Output:		无
  Return:		TRUE、FALSE
  Others:
*************************************************/
uint32 storage_set_mac(MAC_TYPE type, char* data)
{
	if (data == NULL)
	{
		return FALSE;
	}

	switch (type)
	{
		case HOUSE_MAC:
			memset(gpSysParam->MacAddr.HouseMac, 0, sizeof(gpSysParam->MacAddr.HouseMac));
			memcpy(gpSysParam->MacAddr.HouseMac, data, sizeof(gpSysParam->MacAddr.HouseMac));
			break;

		case DOOR1_MAC:
			memset(gpSysParam->MacAddr.Door1Mac, 0, sizeof(gpSysParam->MacAddr.Door1Mac));
			memcpy(gpSysParam->MacAddr.Door1Mac, data, sizeof(gpSysParam->MacAddr.Door1Mac));
			break;

		case DOOR2_MAC:
			memset(gpSysParam->MacAddr.Door2Mac, 0, sizeof(gpSysParam->MacAddr.Door2Mac));
			memcpy(gpSysParam->MacAddr.Door2Mac, data, sizeof(gpSysParam->MacAddr.Door2Mac));
			break;

		default:
			return FALSE;
	}

	
	SaveRegInfo();
	return TRUE;
}

/*********************************************************/
/*					 	密码设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_pass
  Description: 	获取密码
  Input:		
  	1.type		密码类型
  Output:		无
  Return:		密码
  Others:
*************************************************/
uint8 * storage_get_pass(PASS_TYPE type)
{
	switch (type)
	{
		case PASS_TYPE_ADMIN:					// 管理员密码
			return gpSysParam->Passparam.admin;
			
		case PASS_TYPE_USER:					// 用户密码
			return gpSysParam->Passparam.user;
			
		case PASS_TYPE_DOOR_USER:				// 用户开门密码
			return gpSysParam->Passparam.door_user;
			
		case PASS_TYPE_DOOR_SERVER:				// 家政开门密码	
			return gpSysParam->Passparam.door_server;
			
		case PASS_TYPE_SERVER:				    // 家政密码	
			return gpSysParam->Passparam.user_server;

		default:
			return NULL;
	}
}

/*************************************************
  Function:		storage_get_pass
  Description: 	获取密码
  Input:		
  	1.type		密码类型
  	2.data		密码
  Output:		无
  Return:		长度
  Others:
*************************************************/
uint32 storage_set_pass(PASS_TYPE type, char* data)
{
	int nlen = 0;
	int maxlen[PASS_TYPE_MAX] ={MAX_ADMIN_LEN,MAX_USER_LEN,MAX_DOOR_USER_LEN,MAX_DOOR_SERVER_LEN, MAX_DOOR_SERVER_LEN};
	
	if (NULL != data)
	{
		nlen = strlen(data);
		if (nlen > maxlen[type])
		{
			nlen = maxlen[type];
		}
		switch (type)
		{
			case PASS_TYPE_ADMIN:					// 管理员密码
				memset(gpSysParam->Passparam.admin,0,sizeof(gpSysParam->Passparam.admin));
				memcpy(gpSysParam->Passparam.admin, data, nlen);
				gpSysParam->Passparam.admin[nlen] = 0;
				break;
				
			case PASS_TYPE_USER:					// 用户密码
				memset(gpSysParam->Passparam.user,0,sizeof(gpSysParam->Passparam.user));
				memcpy( gpSysParam->Passparam.user, data, nlen);
				gpSysParam->Passparam.user[nlen] = 0;	
				break;
				
			case PASS_TYPE_DOOR_USER:				// 用户开门密码
				memset(gpSysParam->Passparam.door_user,0,sizeof(gpSysParam->Passparam.door_user));
				memcpy(gpSysParam->Passparam.door_user, data, nlen);
				gpSysParam->Passparam.door_user[nlen] = 0;	
				break;
				
			case PASS_TYPE_DOOR_SERVER:				// 家政开门密码	
				memset(gpSysParam->Passparam.door_server,0,sizeof(gpSysParam->Passparam.door_server));
				memcpy(gpSysParam->Passparam.door_server, data, nlen);
				gpSysParam->Passparam.door_server[nlen] = 0;	
				gpSysParam->Passparam.doorserverEnable = 1;
				break;
				
			case PASS_TYPE_SERVER:				    // 家政密码	
				memset(gpSysParam->Passparam.user_server,0,sizeof(gpSysParam->Passparam.user_server));
				memcpy(gpSysParam->Passparam.user_server, data, nlen);
				gpSysParam->Passparam.user_server[nlen] = 0;	
				gpSysParam->Passparam.userserverenb = 1;
				break;

			default:
				break;
		}
		SaveRegInfo();
		return TRUE;
	}

	return FALSE;
}

/*************************************************
  Function:		storage_get_doorserver
  Description: 	获取家政开门密码有效
  Input:		无
  Output:		无
  Return:		
  Others:
*************************************************/
uint8 storage_get_doorserver(uint8 pass_type)
{
	if (1 == pass_type)
	{
		return gpSysParam->Passparam.doorserverEnable;
	}
	else
	{
		return gpSysParam->Passparam.userserverenb;
	}
}

/*************************************************
  Function:		storage_set_doorserver
  Description: 	设置家政开门密码有效
  Input:		
  	1.enable
  Output:		无
  Return:		
  Others:
*************************************************/
uint32 storage_set_doorserver(uint8 enable, uint8 pass_type)
{
	if (1 == pass_type)
	{
		if (gpSysParam->Passparam.doorserverEnable != enable)
		{
			gpSysParam->Passparam.doorserverEnable = enable;
			SaveRegInfo();
		}
	}
	else
	{
		if (gpSysParam->Passparam.userserverenb!= enable)
		{
			gpSysParam->Passparam.userserverenb = enable;
			SaveRegInfo();
		}
	}
	return TRUE;
}

/*********************************************************/
/*					 	音量设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_ringvolume
  Description: 	获取铃声大小
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_ringvolume(void)
{
	return gpSysParam->RingVolume;
}

/*************************************************
  Function:		storage_get_talkvolume
  Description: 	获取通话大小
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_talkvolume(void)
{
	return gpSysParam->TalkVolume;
}

/*************************************************
  Function:		storage_get_keykeep
  Description: 	获取按键音
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_keykeep(void)
{
	return gpSysParam->KeyBeep;
}

/*************************************************
  Function:		storage_set_volume
  Description: 	设置声音大小
  Input:		
  	1.ring
  	2.talk
  	3.key
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_volume(uint8 ring, uint8 talk, uint8 key)
{
	gpSysParam->RingVolume = ring;
	gpSysParam->TalkVolume = talk;
	gpSysParam->KeyBeep = key;

	if (gpSysParam->RingVolume > MAX_RING_VOLUME)
	{
		gpSysParam->RingVolume = MAX_RING_VOLUME;
	}

	if (gpSysParam->TalkVolume > MAX_TALK_VOLUME)
	{
		gpSysParam->TalkVolume = MAX_TALK_VOLUME;
	}
	SaveRegInfo();
	
	return TRUE;
}

/*************************************************
  Function:		storage_get_micvolume
  Description: 	获取咪头增益大小
  Input:		无
  Output:		无
  Return:		无
  Others:		add by luowf 2011.12.2  咪头增益
*************************************************/ 			
uint8 storage_get_micvolume(void)
{
	return gpSysParam->MicVolume;
}

/*************************************************
  Function:		storage_set_micvolume
  Description: 	设置咪头音量大小
  Input:		无
  Output:		无
  Return:		无
  Others:		add by luowf 2011.12.2  咪头增益
*************************************************/
uint8 storage_set_micvolume(uint8 mic)
{
	gpSysParam->MicVolume = mic;
	if (gpSysParam->MicVolume > MAX_MIC_VOLUME)
	{
		gpSysParam->MicVolume = MAX_MIC_VOLUME;
	}
	SaveRegInfo();
	return TRUE;
}

/*********************************************************/
/*					 	免打扰设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_noface
  Description: 	免打扰状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_noface(void)
{
	return gpSysParam->Nofaceparam.noface;
}

/*************************************************
  Function:		storage_set_noface_enable
  Description: 	设置免打扰状态
  Input:		
  	1.enable
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_set_noface_enable(uint8 enable)
{
	gpSysParam->Nofaceparam.noface = enable;
//	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:		storage_get_noface_index
  Description: 	免打扰时间
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_noface_index(void)
{
	return gpSysParam->Nofaceparam.time;
}

/*************************************************
  Function:		storage_get_noface_time
  Description: 	免打扰时间
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_noface_time(void)
{
	uint32 time = 30;
	switch (gpSysParam->Nofaceparam.time)
	{
		case NOFACE_TIME_30:
			time = 30;
			break;
			
		case NOFACE_TIME_60:
			time = 60;
			break;
			
		case NOFACE_TIME_120:
			time = 120;
			break;
			
		case NOFACE_TIME_180:
			time = 180;
			break;
			
		case NOFACE_TIME_300:
			time = 300;
			break;
			
		case NOFACE_TIME_480:
			time = 480;
			break;

		default:
			break;
	}

	return (time*60);
}

/*************************************************
  Function:		storage_set_noface
  Description: 	设置免打扰状态
  Input:		
  	1.enable
  	2.index
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_noface(uint8 enable, uint8 index)
{
	gpSysParam->Nofaceparam.noface = enable;
	gpSysParam->Nofaceparam.time = index;
	if (gpSysParam->Nofaceparam.time > NOFACE_TIME_480)
	{
		gpSysParam->Nofaceparam.time = NOFACE_TIME_480;
	}
	SaveRegInfo();
	
	return TRUE;
}

/*********************************************************/
/*					 铃声提示音设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_ring_id
  Description: 	获取铃声ID
  Input:		
  	1.index		(梯口 区口 门口 中心 住户 分机 信息 预警)
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_ring_id(uint8 index)
{
	return gpSysParam->RingID[index];
}

/*************************************************
  Function:		storage_get_ring_id
  Description: 	获取铃声ID
  Input:		
  	1.index		(梯口 区口 门口 中心 住户 分机 信息 预警)
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_ring_id(uint8 index, uint8 id)
{
	gpSysParam->RingID[index] = id;
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:		storage_get_ring_by_id
  Description: 	获取铃声全局路径
  Input:		
  	1.id		
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_by_id(uint8 id)
{
#if 1
	switch(id)
	{
		case 0:
			return CALL_RING1_PATH;

		case 1:
			return CALL_RING2_PATH;	

		case 2:
			return CALL_RING3_PATH;	

		case 3:
			return CALL_RING4_PATH;	

		case 4:
			return CALL_RING5_PATH;	

		case 5:
			return CALL_RING6_PATH;	

		case 6:
			return CALL_RING7_PATH;	

		case 7:
			return CALL_RING8_PATH;		

		default:
			return CALL_RING1_PATH;
	}
#else // 测试用先用同一个铃声
return CALL_RING1_PATH;
#endif
}

/*************************************************
  Function:		storage_get_ring_by_devtype
  Description: 	获取铃声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_by_devtype(DEVICE_TYPE_E type)
{
	uint8 index = 0;
	switch(type)
	{
		case DEVICE_TYPE_STAIR:
			index = 0;
			break;

		case DEVICE_TYPE_AREA:
			index = 1;
			break;

		case DEVICE_TYPE_DOOR_PHONE:
		case DEVICE_TYPE_DOOR_NET:
			index = 2;
			break;

		case DEVICE_TYPE_MANAGER:
			index = 3;
			break;

		case DEVICE_TYPE_ROOM:
			index = 4;
			break;

		case DEVICE_TYPE_FENJI_NET:
			index = 5;
			break;

		default:
			break;
	}
	
	return storage_get_ring_by_id(gpSysParam->RingID[index]);
}

/*************************************************
  Function:		storage_get_ring_fenji
  Description: 	获取分机铃声全局路径
  Input:		
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_fenji(void)
{
	return storage_get_ring_by_id(gpSysParam->RingID[5]); 
}

/*************************************************
  Function:		storage_get_ring_out
  Description: 	获取回铃声全局路径
  Input:		
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_ring_out(void)
{
	return CALL_RINGOUT_PATH;
}

/*************************************************
  Function:		storage_get_yj_path_byID
  Description: 	获取预警声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_yj_path_byID(uint8 id)
{
	switch(id)
	{
		case 0:
			return AF_YJ1_PATH;

		case 1:
			return AF_YJ2_PATH;	

		case 2:
			return AF_YJ3_PATH;	

		case 3:
			return AF_YJ4_PATH;	

		default:
			return AF_YJ1_PATH;
	}
}

/*************************************************
  Function:		storage_get_yj_path
  Description: 	获取预警声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_yj_path(void)
{
	switch(gpSysParam->RingID[7])
	{
		case 0:
			return AF_YJ1_PATH;

		case 1:
			return AF_YJ2_PATH;	

		case 2:
			return AF_YJ3_PATH;	

		case 3:
			return AF_YJ4_PATH;	

		default:
			return AF_YJ1_PATH;
	}
}

/*************************************************
  Function:		storage_get_msg_hit_byID
  Description: 	获取信息提示声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_msg_hit_byID(uint8 id)
{
	switch(id)
	{
		case 0:
			return MSG_HIT1_PATH;

		case 1:
			return MSG_HIT2_PATH;	

		case 2:
			return MSG_HIT3_PATH;	

		case 3:
			return MSG_HIT4_PATH;	

		default:
			return MSG_HIT1_PATH;
	}
}

/*************************************************
  Function:		storage_get_msg_hit
  Description: 	获取信息提示声全局路径
  Input:		
  	1.type		设备类型
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_msg_hit(void)
{
	switch(gpSysParam->RingID[6])
	{
		case 0:
			return MSG_HIT1_PATH;

		case 1:
			return MSG_HIT2_PATH;	

		case 2:
			return MSG_HIT3_PATH;	

		case 3:
			return MSG_HIT4_PATH;	

		default:
			return MSG_HIT1_PATH;
	}
}

/*********************************************************/
/*					 留影留言设置						 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_lyly_param
  Description: 	获取LYLY使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
PLYLY_PARAM storage_get_lyly_param(void)
{
	return &(gpSysParam->Lylyparam);
}

/*************************************************
  Function:		storage_get_lyly_param
  Description: 	设置LYLY使能
  Input:		
  	1.param		结构
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_lyly_param(PLYLY_PARAM param)
{
	if (param)
	{
		memcpy(&gpSysParam->Lylyparam,param,sizeof(LYLY_PARAM));
		SaveRegInfo();
		return TRUE;
	}
	return FALSE;
}

/*************************************************
  Function:		storage_get_lyly_mode
  Description: 	获取LYLY模式
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
LYLY_TYPE storage_get_lyly_mode(void)
{
	return gpSysParam->Lylyparam.Mode;
}

/*************************************************
  Function:		storage_get_lyly_enable
  Description: 	获取LYLY使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_lyly_enable(void)
{
	return gpSysParam->Lylyparam.Enable;
}

/*************************************************
  Function:		storage_get_lyly_Linkmode
  Description: 	获取LYLY联动状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_lyly_Linkmode(LYLY_LINK_TYPE type)
{
	return gpSysParam->Lylyparam.Link[type];
}

/*************************************************
  Function:		storage_get_lyly_tip
  Description: 	获取LYLY提示音状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_lyly_tip(void)
{
	return gpSysParam->Lylyparam.default_tip;
}

/*************************************************
  Function:		storage_set_lyly_tip
  Description: 	设置LYLY提示音状态
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_set_lyly_tip(uint8 tip)
{
	gpSysParam->Lylyparam.default_tip = tip;
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:		storage_get_lyly_tip_path_bytip
  Description: 	获取LYLY提示音路径
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_lyly_tip_path_bytip(void)
{
	if(ENGLISH == storage_get_language())
	{
		return LYLY_HIT_EN_PATH;
	}
	else
	{
		return LYLY_HIT_CN_PATH;
	}
}

/*************************************************
  Function:		storage_get_lyly_tip_path
  Description: 	获取LYLY提示音路径
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
char* storage_get_lyly_tip_path(void)
{
	if (0 == gpSysParam->Lylyparam.default_tip)
	{
		return storage_get_lyly_tip_path_bytip();
	}
	else
	{
		return LYLY_HIT_RE_PATH;
	}
}

/*********************************************************/
/*						屏幕保护设置					 */	
/*********************************************************/
/*************************************************
  Function:		storage_get_screenparam
  Description: 	获得屏幕保护数据
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
PEPHOTO_PARAM storage_get_screenparam(void)
{
	return &(gpSysParam->Ephotoparam);
}

/*************************************************
  Function:		storage_set_screenparam
  Description: 	设置屏幕保护数据
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_set_screenparam(PEPHOTO_PARAM param)
{
	memcpy(&(gpSysParam->Ephotoparam), param, sizeof(EPHOTO_PARAM));
	SaveRegInfo();
	return TRUE;
}

/*************************************************
  Function:		storage_get_screen_enable
  Description: 	屏幕保护使能
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_get_screen_enable(void)
{
	return gpSysParam->Ephotoparam.used;
}

/*************************************************
  Function:		storage_get_screen_intime
  Description: 	进入屏幕保护
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_screen_intime(void)
{
	switch(gpSysParam->Ephotoparam.intime)
	{
		case EPHOTO_TIME_HALF:
			return 30;
			
		case EPHOTO_TIME_1:
			return 60;
			
		case EPHOTO_TIME_3:
			return (60*3);
			
		case EPHOTO_TIME_5:
			return (60*5);
			
		case EPHOTO_TIME_10:
			return (60*10);
			
		case EPHOTO_TIME_30:
			return (60*30);

		case EPHOTO_TIME_60:
		default:
			return (60*60);
	}
}

/*************************************************
  Function:		storage_get_closelcd_time
  Description: 	关屏幕时间
  Input:		无
  Output:		无
  Return:		无
  Others:		此函数简化版的和经济版的返回值不同
*************************************************/
uint32 storage_get_closelcd_time(void)
{
	switch(gpSysParam->Ephotoparam.holdtime)
	{
		case EPHOTO_TIME_HALF:
			return 30;
			
		case EPHOTO_TIME_1:
			return 60;
			
		case EPHOTO_TIME_3:
			return (60*3);
			
		case EPHOTO_TIME_5:
			return 5;
			
		case EPHOTO_TIME_10:
			return 10;

		case EPHOTO_TIME_15:
			return 15;
			
		case EPHOTO_TIME_30:
			return 30;
			
		case EPHOTO_TIME_60:
			return 60;

		case EPHOTO_TIME_120:
			return 120;
			
		default:
			return (60*60);
	}
}

/*************************************************
  Function:		storage_get_regcode
  Description: 	获得平台注册码
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint32 storage_get_regcode(void)
{
	return gpSysParam->RegCode;
}

/*************************************************
  Function:		storage_get_regcode
  Description: 	设置平台注册码
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
uint8 storage_set_regcode(uint32 code)
{
	gpSysParam->RegCode = code;
	SaveRegInfo();
	return TRUE;
}

#ifdef _USE_NEW_CENTER_

/*************************************************
  Function:			storage_set_predevno
  Description:		获取注册成功设备编号
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
DEVICE_NO storage_get_predevno(void)
{
	FILE *fp;
	char path[50];
	DEVICE_NO pre_devno;
	
	memset(path, 0, sizeof(path));
	memset(&pre_devno, 0, sizeof(DEVICE_NO));
	memcpy(path, PRE_DEVNO_FILENAME, sizeof(PRE_DEVNO_FILENAME));
	fp = fopen(path, "r+");
	if (fp != NULL)
	{
		fread(&pre_devno, sizeof(DEVICE_NO), 1, fp);
		fclose(fp);
	}
	
	return pre_devno;
}

/*************************************************
  Function:			storage_set_predevno
  Description:		保存注册成功设备编号
  Input: 	
  Output:			无
  Return:			成功与否, TRUE/FALSE
  Others:	
*************************************************/
int storage_set_predevno(PDEVICE_NO DeviceNo)
{
	FILE *fp;
	char path[50];
	DEVICE_NO pre_devno = {0};
	
	memset(path, 0, sizeof(path));
	memset(&pre_devno, 0, sizeof(DEVICE_NO));
	memcpy(path, PRE_DEVNO_FILENAME, sizeof(PRE_DEVNO_FILENAME));
	fp = fopen(path, "w+");
	if(fp == NULL)
	{
		return FALSE;
	}
	if (DeviceNo != 0)
	{
		memcpy(&pre_devno, DeviceNo, sizeof(DEVICE_NO));
	}
	fwrite(&pre_devno, sizeof(DEVICE_NO), 1, fp);
	FSFlush(fp);
	fclose(fp);
}
#endif

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
	for (i = 3; i >= 0; i--)
	{
		t = uIp >> (i * 8);
		t = t & 0xFF;
		sprintf(temp, "%d", (uint32)t);
		strcat(strIp, temp);
		if (i != 0)
		{
			strcat(strIp, ".");
		}
	}
	
	return strIp;
}

/*************************************************
Function:   	 	storage_set_ipaddr
Description: 		初始化ip
Input:              无
Output:				无
Return:				成功－ture 失败－false
Others:
*************************************************/
uint32 storage_set_ipaddr(void)
{
	char buf[255];	
	PNET_PARAM NetParam = &gpSysParam->Netparam;

	system("ifconfig eth0 down");

	// 设置MAC码
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "ifconfig eth0 hw ether %02x:%02x:%02x:%02x:%02x:%02x",
		gpSysParam->MacAddr.HouseMac[0], gpSysParam->MacAddr.HouseMac[1],
		gpSysParam->MacAddr.HouseMac[2], gpSysParam->MacAddr.HouseMac[3],
		gpSysParam->MacAddr.HouseMac[4], gpSysParam->MacAddr.HouseMac[5]);
	log_printf("storage_set_ipaddr MAC:%s\n", buf);
	system(buf);

	#ifdef _ETHERNET_DHCP_
	log_printf("Enter Ethernet DHCP.........\n");
	if (storage_get_ethernetdhcp_enable())
	{
		//system("kill -15 $(pidof udhcpc)");	// 系统没有pidof命令
		system("udhcpc -i eth0 -b");
	}
	else
	#endif
	{
		// 设置IP地址
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "ifconfig %s %s ", "eth0", UlongtoIP1(NetParam->IP));
		log_printf("storage_set_ipaddr IP:%s\n", buf);
		system(buf);

		// 设置子网掩码
		memset(buf, 0, sizeof(buf));
		sprintf(buf,"ifconfig %s netmask %s ","eth0", UlongtoIP1(NetParam->SubNet));	
		log_printf("storage_set_ipaddr NETMASK:%s\n", buf);
		system(buf);

		// 设置网关
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "route add default gw  %s ", UlongtoIP1(NetParam->DefaultGateway));	
		log_printf("storage_set_ipaddr GW:%s\n", buf);	
		system(buf);
	}

	system("ifconfig eth0 up");
	return 0;
}

/*************************************************
Function:   	 	storage_netdoor_set_ipaddr
Description: 		初始化ip
Input:              无
Output:				无
Return:				成功－ture 失败－false
Others:
*************************************************/
uint32 storage_netdoor_set_ipaddr(uint32 ip, uint32 netmack, uint32 gateway)
{
	char buf[255];	
	PNET_PARAM NetParam = &gpSysParam->Netparam;

	system("ifconfig eth0 down");

	// 设置MAC码
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "ifconfig eth0 hw ether %02x:%02x:%02x:%02x:%02x:%02x",
		gpSysParam->MacAddr.HouseMac[0], gpSysParam->MacAddr.HouseMac[1],
		gpSysParam->MacAddr.HouseMac[2], gpSysParam->MacAddr.HouseMac[3],
		gpSysParam->MacAddr.HouseMac[4], gpSysParam->MacAddr.HouseMac[5]);
	log_printf("storage_set_ipaddr MAC:%s\n", buf);
	system(buf);

	// 设置IP地址
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "ifconfig %s %s ", "eth0", UlongtoIP1(ip));
	log_printf("storage_set_ipaddr IP:%s\n", buf);
	system(buf);

	// 设置子网掩码
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"ifconfig %s netmask %s ","eth0", UlongtoIP1(netmack));	
	log_printf("storage_set_ipaddr NETMASK:%s\n", buf);
	system(buf);

	// 设置网关
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "route add default gw  %s ", UlongtoIP1(gateway));	
	log_printf("storage_set_ipaddr GW:%s\n", buf);	
	system(buf);

	system("ifconfig eth0 up");
	return 0;
}

/*************************************************
  Function:		storage_init_video_param
  Description: 	
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_init_video_param(void)
{
	if(FALSE == is_fileexist(VIDEO_PARAM_PATH))
	{	
		// 3视频亮度 4视频对比度 5视频饱和度 6视频色彩度
		write_ini(VIDEO_PARAM_PATH, "参数说明", "亮度", "0 - 63");
		write_ini(VIDEO_PARAM_PATH, "参数说明", "对比度", "0 - 63");
		write_ini(VIDEO_PARAM_PATH, "参数说明", "饱和度", "0 - 63");
		write_ini(VIDEO_PARAM_PATH, "参数说明", "色彩度", "0 - 100");
		
		write_ini(VIDEO_PARAM_PATH, "视频参数", "亮度", "32");
		write_ini(VIDEO_PARAM_PATH, "视频参数", "对比度", "32");
		write_ini(VIDEO_PARAM_PATH, "视频参数", "饱和度", "32");
		write_ini(VIDEO_PARAM_PATH, "视频参数", "色彩度", "50");
	}
}

/*************************************************
  Function:		storage_init
  Description: 	存储初始化
  Input:		无
  Output:		无
  Return:		无
  Others:
*************************************************/
void storage_init(void)
{
	LANGUAGE_E lang;

	// 初始化系统参数
	storage_init_sysconfig();
	
	// 初始化语言
	lang = storage_get_language();
	storage_init_language(lang);

	// 设置IP
	storage_set_ipaddr();

	// 初始化对讲模块存储
	storage_call_init();

	// 初始化报警模块存储
	storage_af_init();

	// 初始化家电模块存储
	storage_jd_init();

	#ifdef _YUYUE_MODE_
	// 初始化预约模块存储
	storage_yuyue_init();
	#endif

	// 初始化视频参数
	storage_init_video_param();
}

