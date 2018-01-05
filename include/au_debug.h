/*********************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	au_debug.h
  Author:		chenbh
  Version:  	2.0
  Date: 
  Description:  定义打印信息宏

  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*********************************************************/
#ifndef __AU_DEBUG_H__
#define __AU_DEBUG_H__
#include <stdio.h>
#include "AppConfig.h"

#define CLR_RED       	 		"\033[0;31m"    // "\033[0;31;40m"
#define CLR_GREEN       		"\033[0;32m"    // "\033[0;32;40m"
#define CLR_YELLOW        		"\033[0;33m"    // "\033[0;33;40m"
#define CLR_BLUE        		"\033[0;34m"    // "\033[0;34;40m"
#define CLR_MAGENTA        		"\033[0;35m"    // "\033[0;35;40m"
#define CLR_CYAN        		"\033[0;36m"    // "\033[0;36;40m"
#define CLR_WHITE        		"\033[0;37m"    // "\033[0;37;40m"
#define CLR_RED_BOLD        	"\033[1;31m"    // "\033[1;31;40m"
#define CLR_YELLOW_BOLD    		"\033[1;33m"    // "\033[1;33;40m"
#define CLR_BLUE_BOLD    		"\033[1;34m"    // "\033[1;34;40m"
#define CLR_DEFAULT        		"\033[0m"


#define err_printf(...)			do{	printf(CLR_RED);\
									printf("%s [%d] :", __FUNCTION__,__LINE__);\
									printf(__VA_ARGS__);\
									printf(CLR_DEFAULT);\
									fflush(stdout);\
									}while(0)

#define warn_printf(...)			do{	printf(CLR_YELLOW);\
									printf("%s [%d] :", __FUNCTION__,__LINE__);\
									printf(__VA_ARGS__);\
									printf(CLR_DEFAULT);\
									fflush(stdout);\
									}while(0)							
									
#if DEBUG_PRINT
#define log_printf(...)			do{	printf("%s [%d] :", __FUNCTION__,__LINE__);\
									printf(__VA_ARGS__);\
									fflush(stdout);\
									}while(0)

#else
#define log_printf(...)
#endif

#endif

