#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "echo_wrap.h"
#include "ace_echo.h"
#include "ace_preprocess.h"
#include "include/echo_control_mobile.h"
#include "aecm_defines.h"

static   ACEEchoState *gst;
static   ACEPreprocessState *gden;
static   int gsampleRate = 8000;
static   int  framesize= 160;


void *aecmInst;
AecmConfig aecmconfig;
int AudioPktSize;


#define AEC_DELAY	200

//int DiscardedMicPacketsForAECAlign=7;
//int DiscardedMicPacketsForAECAlign=9;
int DiscardedMicPacketsForAECAlign=0;

#define PLAYBACK_DELAY 15

short *play_buf;
int play_buf_pos;
int play_buf_started;

int aec_started; 
int processedcount;

void echo_cancel_init(int size, int tail, int noiselevel, int echolevel)
{
   int ren;
   int denoise_enabled=1;
   AudioPktSize=size;
   printf("echo_cancel_init	AudioPktSize=%d\r\n",AudioPktSize);

//   gst = ace_echo_state_init(framesize, tail);
//   gden = ace_preprocess_state_init(framesize, noiselevel, echolevel);

   gst = ace_echo_state_init(size, tail);
   gden = ace_preprocess_state_init(size, noiselevel, echolevel);
   ace_echo_ctl(gst, ACE_ECHO_SET_SAMPLING_RATE, &gsampleRate);
   ace_preprocess_ctl(gden, ACE_PREPROCESS_SET_ECHO_STATE, gst);	
   ace_preprocess_ctl(gden, ACE_PREPROCESS_SET_DENOISE, &denoise_enabled);
   

///////
//for aecm
 	aecmInst = 0;
	ren=WebRtcAecm_Create(&aecmInst);
	if(ren)
	{
		printf("WebRtcAec_Create Error!");
		return;
	}
	ren=WebRtcAecm_Init(aecmInst,gsampleRate);
	if(ren)
	{
		printf("WebRtcAec_Create Error!");
		return;
	}
	printf("aecmInst=%p\r\n",aecmInst);


	WebRtcAecm_get_config(aecmInst,&aecmconfig);
	printf("aecmconfig.cngMode=%d,aecmconfig.echoMode=%d\r\n",aecmconfig.cngMode,aecmconfig.echoMode);
	aecmconfig.echoMode= 4;
	WebRtcAecm_set_config(aecmInst,aecmconfig);
	WebRtcAecm_get_config(aecmInst,&aecmconfig);
	printf("aecmconfig.cngMode=%d,aecmconfig.echoMode=%d\r\n",aecmconfig.cngMode,aecmconfig.echoMode);


//////////
	DiscardedMicPacketsForAECAlign=0;


   play_buf = (short*)malloc((PLAYBACK_DELAY+1)*AudioPktSize*sizeof(short));
   play_buf_pos = 0;
   play_buf_started = 0;

   aec_started=1; 
   processedcount=0;


}

void echo_cancel_free(void)
{
   aec_started=0; 

	usleep(100000);

//	return ;
//	printf("delay 100ms \n");
   ace_echo_state_destroy(gst);
   ace_preprocess_state_destroy(gden);

	if (aecmInst!=0){
 	    WebRtcAecm_Free(aecmInst);
 		aecmInst=0;
	}

   free(play_buf);


}
///////////////////////////////////////////////////////////
// by zxf
/*---------------------------------------
method name : AmplifyPCMData
comment  : 对PCM数据的音量进行调整
parameter : 
   pData PCM数据
   nLen PCM数据的长度
   nBitsPerSample 每个Sample的位数，一般为8的整数
   multiple 放大倍数
result  : S_OK 成功
---------------------------------------*/
int AmplifyPCMData(unsigned char * pData, int nLen, int nBitsPerSample, float multiple)
{
 int nCur = 0;
 if (16 == nBitsPerSample)
 {
  while (nCur < nLen)
  {
   short* volum = (short*)(pData + nCur);
   *volum = (*volum) * multiple;
   if (*volum > 32767)
   {
    *volum = 32767;
   }
   if (*volum < -32768)
   {
    *volum = -32768;
   }
   *(short*)(pData + nCur) = *volum  ;
   nCur += 2;
  }
  
 }
 else if (8 == nBitsPerSample)
 {
  while (nCur < nLen)
  {
   unsigned char* volum = pData + nCur;
   *volum = (*volum) * multiple;
   if (*volum > 255)
   {
    *volum = 255;
   }
   *pData  = *volum  ;
   nCur ++;
  }
  
 }
 return 0;
 
}
/*
void ace_echo_capture( const short *rec, short *out)
{
   int i;
    play_buf_started = 1;
   if (play_buf_pos>=AudioPktSize)
   {
      ace_echo_cancellation(st, rec, play_buf, out);
      play_buf_pos -= AudioPktSize;
      for (i=0;i<play_buf_pos;i++)
         play_buf[i] = play_buf[i+AudioPktSize];
   } else {
      printf("No playback frame available (your application is buggy and/or got xruns)");
      if (play_buf_pos!=0)
      {
         printf("internal playback buffer corruption?");
         play_buf_pos = 0;
      }
      for (i=0;i<AudioPktSize;i++)
         out[i] = rec[i];
   }
}
*/
void ace_echo_playback_aec(  short *play)
{
   /*printf_int("playback with fill level ", play_buf_pos/frame_size);*/
	int i;
/////////
//调整远端音量大小
   
	int volum;
	for(i=0;i<AudioPktSize;i++)
	{
	   volum = play[i] ;
	   volum=volum<<3;
	   if (volum > 32767)
	   {
		volum = 32767;
	   }
	   if (volum < -32768)
	   {
		volum = -32768;
	   }
	   play[i]=(short)volum ;

	}
	
/////////////////////

	//printf("ace_echo_playback_aec play_buf_pos=%d\n",play_buf_pos);

   if (play_buf_pos<=PLAYBACK_DELAY*AudioPktSize)
   {
//      int i;
//      for (i=0;i<AudioPktSize;i++)
//         play_buf[play_buf_pos+i] = play[i];
	  memcpy(play_buf+play_buf_pos,play,AudioPktSize*sizeof(short));
      play_buf_pos += AudioPktSize;
 
   } else {
      printf("Had to discard a playback frame (your application is buggy and/or got xruns)");
   }
}

void ace_echo_playback_aec2(  short * play, int samplecount)
{
   /*printf_int("playback with fill level ", play_buf_pos/frame_size);*/
	int i;
   
	#if 1
	//调整远端音量大小
	int volum;
	for (i = 0; i < samplecount; i++)
	{
		volum = play[i] ;
		// volum = volum << 2;
		volum = volum << 1;
		if (volum > 32767)
		{
			volum = 32767;
		}
		else if (volum < -32768)
		{
			volum = -32768;
		}
		play[i] = (short)volum ;
	}
	#endif
	
	//printf("ace_echo_playback_aec play_buf_pos=%d\n",play_buf_pos);

   if (play_buf_pos<=PLAYBACK_DELAY*AudioPktSize)
   {
//      int i;
//      for (i=0;i<AudioPktSize;i++)
//         play_buf[play_buf_pos+i] = play[i];
	  memcpy(play_buf+play_buf_pos,play,samplecount*sizeof(short));
      play_buf_pos += samplecount;
 
   } 
   else {
     // printf("Had to discard a playback frame (your application is buggy and/or got xruns)\r\n");
   }
}

void echo_cancel_exe( short *spk_buf,  short *echo_buf, short *out_buf)
{
	int ren;
	int i;
//ace_echo_playback(gst,spk_buf);	
//ace_echo_capture(gst,echo_buf,out_buf);

//	ace_echo_cancellation(gst, echo_buf, spk_buf, out_buf);
//	ace_preprocess_run(gden, out_buf);	

   short nearbuf[160], farbuf[160], cleanbuf[160];
   int FrmCount=0;

/////////
//调整远端音量大小
/*   
	int volum;
	for(i=0;i<AudioPktSize;i++)
	{
	   volum = spk_buf[i] ;
	   volum=volum<<3;
	   if (volum > 32767)
	   {
		volum = 32767;
	   }
	   if (volum < -32768)
	   {
		volum = -32768;
	   }
	   spk_buf[i]=(short)volum ;

	}
*/	
/////////////////////
/////////
//调整近端音量大小
/*
	for(i=0;i<AudioPktSize;i++)
	{
	   volum = echo_buf[i] ;
	   volum=volum>>1;
	   if (volum > 32767)
	   {
		volum = 32767;
	   }
	   if (volum < -32768)
	   {
		volum = -32768;
	   }
	   echo_buf[i]=(short)volum ;

	}  
*/
/////////////////////	
//远端音量大小放大   
	/*
	int meanvalue=0;
	int volum;
	int shiftbits;
	
	for(i=0;i<AudioPktSize;i++)
	{
		if(spk_buf[i]>=0)
			meanvalue+=spk_buf[i];
		else
			meanvalue-=spk_buf[i];
	}
	meanvalue/=AudioPktSize;	
	if(meanvalue<=5000)
	{
		shiftbits=2;

	}
	else if(meanvalue>5000 && meanvalue<=10000)
	{
		shiftbits=1;

	}
	else
		shiftbits=0;

	if(shiftbits>0)
	{
		for(i=0;i<AudioPktSize;i++)
		{
		   volum = spk_buf[i] ;
		   volum=volum<<shiftbits;
		   if (volum > 32767)
		   {
			volum = 32767;
		   }
		   if (volum < -32768)
		   {
			volum = -32768;
		   }
		   spk_buf[i]=(short)volum ;

		}
	}
	*/	
/////////
/*

	ren=WebRtcAecm_BufferFarend(aecmInst,spk_buf,AudioPktSize);
	if(ren<0)
		printf("WebRtcAecm_BufferFarend	return=%d\r\n",ren);
 	ren=WebRtcAecm_Process(aecmInst,echo_buf,0,out_buf,AudioPktSize,0);
	if(ren<0)
		printf("WebRtcAecm_Process	return=%d\r\n",ren);

//	memcpy(echo_buf,out_buf,AudioPktSize*2);
//	ace_echo_cancellation(gst, echo_buf, play_buf, out_buf);	
//	ace_preprocess_run(gden, out_buf);	



	return;

*/


/////////
//调整远端音量大小	
   /*
	int meanvalue=0;
	int volum;
	
	for(i=0;i<AudioPktSize;i++)
	{
		if(spk_buf[i]>=0)
			meanvalue+=spk_buf[i];
		else
			meanvalue-=spk_buf[i];
	}
	meanvalue/=AudioPktSize;	
	if(meanvalue<=5000)
	{
		for(i=0;i<AudioPktSize;i++)
		{
		   volum = spk_buf[i] ;
		   volum=volum<<2;
		   if (volum > 32767)
		   {
			volum = 32767;
		   }
		   if (volum < -32768)
		   {
			volum = -32768;
		   }
		   spk_buf[i]=(short)volum ;

		}

	}

	*/
/////////

	ace_echo_playback_aec(spk_buf);

	if(DiscardedMicPacketsForAECAlign>0)
	{
		DiscardedMicPacketsForAECAlign--;
		return;
	}


///////////////////	
   play_buf_started = 1;
   if (play_buf_pos>=AudioPktSize)
   {
////////

		ren=WebRtcAecm_BufferFarend(aecmInst,play_buf,AudioPktSize);
		if(ren<0)
			printf("WebRtcAecm_BufferFarend	return=%d\r\n",ren);
 		ren=WebRtcAecm_Process(aecmInst,echo_buf,0,out_buf,AudioPktSize,0);
		if(ren<0)
			printf("WebRtcAecm_Process	return=%d\r\n",ren);


	   /*
		FrmCount=AudioPktSize/framesize;
		for(i=0;i<FrmCount;i++)
		{
			memcpy(nearbuf,echo_buf+i*framesize,framesize*2);
			memcpy(farbuf,play_buf+i*framesize,framesize*2);

			//AmplifyPCMData( farbuf,framesize*2 ,16,2.0);
			
			ren=WebRtcAecm_BufferFarend(aecmInst,farbuf,framesize);
			if(ren<0)
				printf("WebRtcAecm_BufferFarend	return=%d\r\n",ren);
 			ren=WebRtcAecm_Process(aecmInst,nearbuf,nearbuf,cleanbuf,framesize,0);
			if(ren<0)
				printf("WebRtcAecm_Process	return=%d\r\n",ren);
 			
			memcpy(out_buf+i*framesize,cleanbuf,framesize*2);

		}
		*/
/////////////////////
/////////
//调整近端音量大小
		/*
		for(i=0;i<AudioPktSize;i++)
		{
		   volum = out_buf[i] ;
		   volum=volum>>1;
		   if (volum > 32767)
		   {
			volum = 32767;
		   }
		   if (volum < -32768)
		   {
			volum = -32768;
		   }
		   out_buf[i]=(short)volum ;

		} 
		*/
/////////////////////	


//		memcpy(echo_buf,out_buf,AudioPktSize*2);
//		ace_echo_cancellation(gst, echo_buf, play_buf, out_buf);	
//		ace_preprocess_run(gden, out_buf);	


////////
      play_buf_pos -= AudioPktSize;
      for (i=0;i<play_buf_pos;i++)
         play_buf[i] = play_buf[i+AudioPktSize];
   } else {
      //printf("No playback frame available (your application is buggy and/or got xruns)");
	  /*
      if (play_buf_pos!=0)
      {
         printf("internal playback buffer corruption?");
         play_buf_pos = 0;
      }
      for (i=0;i<AudioPktSize;i++)
         out_buf[i] = echo_buf[i];
	  */
   }
}

void echo_cancel_exe2( short *echo_buf, short *out_buf)
{
	int ren;
	int i;
//ace_echo_playback(gst,spk_buf);	
//ace_echo_capture(gst,echo_buf,out_buf);

//	ace_echo_cancellation(gst, echo_buf, spk_buf, out_buf);
//	ace_preprocess_run(gden, out_buf);	

   short nearbuf[160], farbuf[160], cleanbuf[160];
   int FrmCount=0;

	if(!aec_started)
		return;

/////////
//调整远端音量大小
   /*
	int volum;
	for(i=0;i<AudioPktSize;i++)
	{
	   volum = spk_buf[i] ;
	   volum=volum<<2;
	   if (volum > 32767)
	   {
		volum = 32767;
	   }
	   if (volum < -32768)
	   {
		volum = -32768;
	   }
	   spk_buf[i]=(short)volum ;

	}
	*/
/////////////////////
/////////
//调整近端音量大小
/*
	for(i=0;i<AudioPktSize;i++)
	{
	   volum = echo_buf[i] ;
	   volum=volum>>1;
	   if (volum > 32767)
	   {
		volum = 32767;
	   }
	   if (volum < -32768)
	   {
		volum = -32768;
	   }
	   echo_buf[i]=(short)volum ;

	}  
*/
/////////////////////	
//远端音量大小放大   
	/*
	int meanvalue=0;
	int volum;
	int shiftbits;
	
	for(i=0;i<AudioPktSize;i++)
	{
		if(spk_buf[i]>=0)
			meanvalue+=spk_buf[i];
		else
			meanvalue-=spk_buf[i];
	}
	meanvalue/=AudioPktSize;	
	if(meanvalue<=5000)
	{
		shiftbits=2;

	}
	else if(meanvalue>5000 && meanvalue<=10000)
	{
		shiftbits=1;

	}
	else
		shiftbits=0;

	if(shiftbits>0)
	{
		for(i=0;i<AudioPktSize;i++)
		{
		   volum = spk_buf[i] ;
		   volum=volum<<shiftbits;
		   if (volum > 32767)
		   {
			volum = 32767;
		   }
		   if (volum < -32768)
		   {
			volum = -32768;
		   }
		   spk_buf[i]=(short)volum ;

		}
	}
	*/	

   play_buf_started = 1;
   if (play_buf_pos>=AudioPktSize)
   {
////////

		ren=WebRtcAecm_BufferFarend(aecmInst,play_buf,AudioPktSize);
		if(ren<0)
			printf("WebRtcAecm_BufferFarend	return=%d\r\n",ren);
 		ren=WebRtcAecm_Process(aecmInst,echo_buf,0,out_buf,AudioPktSize,0);
		if(ren<0)
			printf("WebRtcAecm_Process	return=%d\r\n",ren);
		
//		if(processedcount%4==0)
		{
//		    ace_preprocess_ctl(gden, ACE_PREPROCESS_SET_ECHO_STATE, gst);	
			memcpy(echo_buf,out_buf,AudioPktSize*2);
			ace_echo_cancellation(gst, echo_buf, play_buf, out_buf);	
		}
//		else
		{
//			ace_preprocess_ctl(gden, ACE_PREPROCESS_SET_ECHO_STATE, 0);	

		}
		ace_preprocess_run(gden, out_buf);	
		
		processedcount++;

	   /*
		FrmCount=AudioPktSize/framesize;
		for(i=0;i<FrmCount;i++)
		{
			memcpy(nearbuf,echo_buf+i*framesize,framesize*2);
			memcpy(farbuf,play_buf+i*framesize,framesize*2);

			//AmplifyPCMData( farbuf,framesize*2 ,16,2.0);
			
			ren=WebRtcAecm_BufferFarend(aecmInst,farbuf,framesize);
			if(ren<0)
				printf("WebRtcAecm_BufferFarend	return=%d\r\n",ren);
 			ren=WebRtcAecm_Process(aecmInst,nearbuf,nearbuf,cleanbuf,framesize,0);
			if(ren<0)
				printf("WebRtcAecm_Process	return=%d\r\n",ren);
 			
			memcpy(out_buf+i*framesize,cleanbuf,framesize*2);

		}
		*/
/////////////////////
/////////
//调整近端音量大小
		/*
		for(i=0;i<AudioPktSize;i++)
		{
		   volum = out_buf[i] ;
		   volum=volum>>1;
		   if (volum > 32767)
		   {
			volum = 32767;
		   }
		   if (volum < -32768)
		   {
			volum = -32768;
		   }
		   out_buf[i]=(short)volum ;

		} 
		*/
/////////////////////	


//		memcpy(echo_buf,out_buf,AudioPktSize*2);
//		ace_echo_cancellation(gst, echo_buf, play_buf, out_buf);	
//		ace_preprocess_run(gden, out_buf);	


////////
      play_buf_pos -= AudioPktSize;
      for (i=0;i<play_buf_pos;i++)
         play_buf[i] = play_buf[i+AudioPktSize];
   } else {
      //printf("No playback frame available (your application is buggy and/or got xruns)\r\n");
	  /*
      if (play_buf_pos!=0)
      {
         printf("internal playback buffer corruption?");
         play_buf_pos = 0;
      }
      for (i=0;i<AudioPktSize;i++)
         out_buf[i] = echo_buf[i];
	  */
   }
}

