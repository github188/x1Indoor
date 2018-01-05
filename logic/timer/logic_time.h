/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	logic_time.h
  Author:		chenbh
  Version:  	1.0
  Date: 		2014-12-11
  Description:  时钟处理
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#ifndef _AU_TIME_H
#define _AU_TIME_H

#include "au_types.h"

#ifdef  __cplusplus
extern "C"
{
#endif



/*************************************************
  Function:			init_reboot_timer
  Description: 		定时重启初始化
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int32 init_reboot_timer(void);

/*************************************************
  Function:			init_reboot_timer
  Description: 		定时重启初始化
  Input: 		
  Output:		
  Return:		 
  Others:
*************************************************/
int32 test_reboot_timer(void);

#ifdef __cplusplus
};
#endif

#endif 





