/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name:  	au_types.h
  Author:     	luofl
  Version:    	2.0
  Date: 		2014-09-01
  Description:  
				类型定义
  History:        
    1. Date:	
       Author:	
       Modification:
*************************************************/
#ifndef __AU_TYPES_H__
#define __AU_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#define TRUE  1
#endif

/*
#ifndef true
#define true  TRUE
#endif
*/

#ifndef FALSE
#define FALSE 0
#endif

/*
#ifndef false
#define false FALSE
#endif
*/

#ifndef bool
typedef enum { false, true } bool;
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef null
#define null 0
#endif

typedef enum
{
	HI_FALSE = 0,
	HI_TRUE = 1,
}HI_BOOL;

#define HI_NULL		0L
#define HI_SUCCESS	0
#define HI_FAILURE	(-1)

typedef int					NT_BOOL;	
typedef int					Socket;
typedef unsigned char		uint8;					/* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char		int8;					/* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short		uint16;					/* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short		int16;					/* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int		uint32;					/* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int		int32;					/* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef long long			int64;					/* defined for signed 64-bits integer variable 		有符号64位整型变量 */
typedef unsigned long long	uint64;					/* defined for unsigned 64-bits integer variable 	有符号64位整型变量 */
typedef float				fp32;					/* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double				fp64;					/* double precision floating point variable (64bits) 双精度浮点数（64位长度） */
typedef struct { unsigned char octet[16]; }  uint128;

#ifdef __cplusplus
}
#endif
#endif

