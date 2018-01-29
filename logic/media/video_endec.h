/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	video_endec.h
  Author:		chenbh
  Version:  	1.0
  Date: 		
  Description:  
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#ifndef _VIDEO_ENDEC_H_
#define _VIDEO_ENDEC_H_


#ifdef __cplusplus
extern "C" {
#endif


int VideoEncFunc(char* pData, unsigned int len, int* property);
void VideoEncStop(void);
int VideoEncStart(unsigned int width, unsigned int height,
	unsigned int framerate, unsigned int streamrate, unsigned int gop, unsigned int qu);

void VideoDecFunc(char * pdata, unsigned int dlen, unsigned int ptr);
void VideoDecStop(void);
int VideoDecStart(int left, int top, int width, int height);
void VideoSetDisplayRect(int left, int top, int width, int height);

void VideoSetColorKey(unsigned int color);

#ifdef __cplusplus
}
#endif

#endif

