
/*************************************************
  Copyright (C), 2006-2016, Aurine
  File name: 	wav_play.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include "wav_play.h"


t_wave_header wave_header;
t_wave_buf *wave_buf = NULL;


/*************************************************
  Function:			wav_play_init
  Description:		读取wav 文件信息
  Input: 	
  Output:			
  Return:			0 正确 -1 错误
  Others:
*************************************************/
void wav_play_uninit(void)
{
	if (wave_buf)
	{
		if (wave_buf->buffer)
		{
			free(wave_buf->buffer);
			wave_buf->buffer = NULL;
		}
		free(wave_buf);
		wave_buf = NULL;
	}
}

/*************************************************
  Function:			wav_play_init
  Description:		读取wav 文件信息
  Input: 	
  Output:			
  Return:			0 正确 -1 错误
  Others:
*************************************************/
int wav_play_init(char *filename)
{
	FILE* fp;
	int bytes_read;

	fp = fopen(filename,"rb");
	if(fp == NULL)
	{
	    printf("fopen %s error\n",filename);
	    return -1;
	}

	fread(wave_header.riff, sizeof(wave_header.riff), 1, fp);
	if((wave_header.riff[0] != 'R') ||(wave_header.riff[1] != 'I')
		||(wave_header.riff[2] != 'F') ||(wave_header.riff[3] != 'F'))
	{
	    printf("not riff!\n");
	    fclose(fp);
	    return  -1;
	}

	fread(&wave_header.riff_len, sizeof(wave_header.riff_len), 1, fp);
	fread(wave_header.wave, sizeof(wave_header.wave), 1, fp);
	if((wave_header.wave[0] != 'W') ||(wave_header.wave[1] != 'A')
		||(wave_header.wave[2] != 'V') ||(wave_header.wave[3] != 'E'))
	{
	    printf("not wave!\n");
	    fclose(fp);
	    return  -1;
	}

	fread(wave_header.fmt, sizeof(wave_header.fmt), 1, fp);
	if((wave_header.fmt[0] != 'f') ||(wave_header.fmt[1] != 'm')
		||(wave_header.fmt[2] != 't') ||(wave_header.fmt[3] != ' '))
	{
	    printf("not fmt \n");
	    fclose(fp);
	    return  -1;
	}

	printf("here1111\n");
	fread(&wave_header.fmt_len, sizeof(wave_header.fmt_len), 1, fp);
	fread(&wave_header.format_tag, sizeof(wave_header.format_tag), 1, fp);
	fread(&wave_header.channels, sizeof(wave_header.channels),1,fp);
	fread(&wave_header.sample_rate,sizeof(wave_header.sample_rate), 1,fp);
	fread(&wave_header.bytes_per_sec,sizeof(wave_header.bytes_per_sec), 1, fp);
	fread(&wave_header.bytes_per_sample, sizeof(wave_header.bytes_per_sample), 1,fp);
	fread(&wave_header.bits_per_sample, sizeof(wave_header.bits_per_sample), 1, fp);
	
	printf("wave_header.format_tag: %d\n", wave_header.format_tag);
	if (wave_header.format_tag != 0x01 && wave_header.format_tag != 0x06
		&& wave_header.format_tag != 0x07)
	{
		printf("wave_header.format_tag is not support!!!! \n");
		return -1;
	}

	fseek(fp, sizeof(wave_header.riff)+sizeof(wave_header.riff_len)+sizeof(wave_header.wave)
	        + sizeof(wave_header.fmt)+ sizeof(wave_header.fmt_len)+ wave_header.fmt_len, SEEK_SET);

	fread(wave_header.data, sizeof(wave_header.data), 1, fp);	

	///maybe don't have 'fact'
	if ((wave_header.data[0] == 'f')&&(wave_header.data[1] == 'a')&&(wave_header.data[2] == 'c')&&(wave_header.data[3] == 't'))
	{
		printf("is fact!\n");
	    fread(&wave_header.fact_len, sizeof(wave_header.fact_len), 1, fp);
	    fseek(fp, wave_header.fact_len, SEEK_CUR);
		//memset(wave_header.data, 0, sizeof(wave_header.data));
	    fread(wave_header.data, sizeof(wave_header.data), 1, fp);
	}

	if((wave_header.data[0] == 'd') &&(wave_header.data[1] == 'a')
		&&(wave_header.data[2] == 't') &&(wave_header.data[3]=='a'))
	{
	    fread(&wave_header.data_len, sizeof(wave_header.data_len), 1, fp);
	}
	printf("here444444\n");

	if (wave_buf)
	{
		if (wave_buf->buffer)
		{
			free(wave_buf->buffer);
			wave_buf->buffer = NULL;
		}
		free(wave_buf);
		wave_buf = NULL;
	}
	wave_buf = (t_wave_buf *)malloc(sizeof(t_wave_buf));
	wave_buf->iget = 0;
	wave_buf->iput = 0;
	wave_buf->buffer = (unsigned char*)malloc(wave_header.data_len);
	bytes_read = fread(wave_buf->buffer, wave_header.data_len, 1,fp);
	printf("data_len=%d\n",wave_header.data_len);
	printf("bytes_read=%d\n",bytes_read);
	if( bytes_read != 1)
	{
		if (wave_buf)
		{
	        if(wave_buf->buffer)
			{
	            free(wave_buf->buffer);
	            wave_buf->buffer = NULL;
	        }
			free(wave_buf);
			wave_buf = NULL;
		}
	    fclose(fp);
	    return -1;
	}
	printf("here3333\n");
	wave_buf->size = wave_header.data_len;
	fclose(fp);
	return 0;	
}



