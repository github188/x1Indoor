#pragma once
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/stat.h>

#define __in
#define __out
#define	WINAPI
#define __declspec(dllexport) 

#define CONST       const
#define	SOCKET_ERROR	-1
#define	TRUE	1
#define	FALSE	0
#define	INVALID_HANDLE_VALUE	(void*)0xffffffff
#define	INVALID_SOCKET			-1
#define MAX_PATH				256
#define	INFINITE				0xffffffff

typedef int SOCKET;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int BOOL;
typedef unsigned long long UINT64;
typedef void* HANDLE;
typedef unsigned char UINT8;
typedef int LONG;
typedef DWORD COLORREF;
typedef	socklen_t SOCKET_LEN;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr	SOCKADDR;
typedef void VOID;
typedef pthread_mutex_t CRITICAL_SECTION;
typedef WORD DPCHAR;

typedef struct
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME;

typedef struct
{
	int wSecond;
	int wMinute;
	int wHour;
	int wDay;
	int wMonth;
	int wYear;
	int wDayOfWeek;
	int tm_yday;
	int tm_isdst;
}SYSTEMTIME;

typedef struct
{
	int x;
	int y;
} POINT;

typedef struct
{
	int cx;
	int cy;
} SIZE;

typedef struct 
{ 
	LONG left; 
	LONG top; 
	LONG right; 
	LONG bottom; 
} RECT; 

typedef struct tagRGBQUAD { 
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;



