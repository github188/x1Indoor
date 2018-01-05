
/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	wav_play.h
  Author:		chenbh
  Version:  	1.0
  Date: 		2015-05-04
  Description:  wav文件播放
				
  History:        
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#ifndef _WAV_PLAY_H_
#define _WAV_PLAY_H_

// 音频数据格式
typedef enum
{
	AUDIO_UNUSE = 0,					// 未启用
	AUDIO_PCM   = 0x01,					// pcm
	AUDIO_A_LAW = 0x06,					// g.711a
	AUDIO_U_LAW = 0x07,					// g.711u
}AUDIO_FORMAT;	

typedef struct _wave_heade
{
    char riff[4];           	// 'RIFF' 
    int riff_len;           	// file length in bytes
    char wave[4];          	 	// 'WAVE'
    char fmt[4];                // 'FMT '
    int fmt_len;
    short int format_tag;    	// 1=PCM */
    short int channels;        	// 1=Mono 2=Stereo
    int sample_rate;   			// Sampling rate in samples per second 
    int  bytes_per_sec; 		// Bytes per second 
    short int bytes_per_sample; // 2= 16bit mono 4=16bit stereo 
    short int bits_per_sample;  // Number of bits per sample
    char fact[4];               // 'FACT' 
    int fact_len;
    char data[4];               // 'DATA' 
    int  data_len;
}t_wave_header;

typedef struct _wave_buf
{
    int iput;       			// the position of put
    int iget;      	 			// the position of get
    int size;
    unsigned char * buffer;
}t_wave_buf;

extern t_wave_header wave_header;
extern t_wave_buf *wave_buf;


/*************************************************
  Function:			wav_play_init
  Description:		读取wav 文件信息
  Input: 	
  Output:			
  Return:			0 正确 -1 错误
  Others:
*************************************************/
int wav_play_init(char *filename);

/*************************************************
  Function:			wav_play_init
  Description:		读取wav 文件信息
  Input: 	
  Output:			
  Return:			0 正确 -1 错误
  Others:
*************************************************/
void wav_play_uninit(void);
#endif

