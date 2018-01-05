#pragma once
#include "dpdef_linux.h"

BOOL JPGDecStart(int left, int top, int width, int height);
BOOL H264DecStart(int left, int top, int width, int height);
void H264WriteData(char * pdata,DWORD dlen, DWORD ptr = 0);
void H264SetDisplayRect(int left, int top, int width, int height);
void H264DecStop(void);
BOOL CheckH264DecStop();

HANDLE VideoEncStart(DWORD format, DWORD width, DWORD height, DWORD qu);
DWORD VideoEncRead(HANDLE hHandle, BYTE* pData, DWORD len, DWORD* property);
void VideoEncStop(HANDLE hHandle);

void VideoEncSetQuality(HANDLE hHandle, DWORD qu);
void VideoEncEnable(HANDLE hHandle, BOOL bOn);

// ÷ÿ±‡¬Î
HANDLE VideoReEncStart(DWORD dec_format, int decw, int dech, DWORD enc_format, int encw, int ench);
void VideoReEncStop(HANDLE hHandle);
void VideoReEncWrite(HANDLE hVideoEnc, BYTE* pdata, DWORD len);
DWORD VideoReEncRead(HANDLE hVideoEnc, BYTE* pdata, DWORD len, DWORD* property);
