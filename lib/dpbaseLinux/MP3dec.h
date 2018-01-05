#ifndef __MP3DEC_H__
#define __MP3DEC_H__

#include "AudioLib.h"
#ifdef __cplusplus
extern "C" {
#endif

//MP3 decoder return error type
#define MP3DEC_ERR_NONE         0
#define MP3DEC_ERR_PARAMETER    1
#define MP3DEC_ERR_FILEOPEN     2
#define MP3DEC_ERR_FILETYPE     3
#define MP3DEC_ERR_VERSION      4
#define MP3DEC_ERR_SUPPORT      5
#define MP3DEC_ERR_NOMOREDATA   6
#define MP3DEC_ERR_DECODE       7


typedef void*  MP3File;

int MP3File_Open(MP3File *mp3file, char *filename);
int MP3File_Seek   (MP3File  mp3file, int target_time, int *actual_time);
int MP3File_Decode (MP3File  mp3file, short *pcmbuf, int *nsamples, int *timestamp);
extern int MP3File_Close  (MP3File  mp3file);




//MP3 decoder return error type
#define MP3_ERR_NONE       0      // decoder one frame successfully
#define MP3_ERR_SYNC       1      // not find SYNC word, so need more data
#define MP3_ERR_CRC        2      // SYNC OK, but CRC error
#define MP3_ERR_LENGTH     3      // SYNC OK, but need more data
#define MP3_ERR_PTR        4      // involid input or output buffer pointer
#define MP3_ERR_DECODE     5      // fatal error when decode one frame

#define MP3FRAMESIZE       1152           // MP3 one frame max 1152 sample
#define MP3CACHESIZE      10240           // MP3 bit stream cache > 8*1024 + 1728

typedef struct
{
	int     skip_flag;              // (i)
	char   *input_buffer;           // (io)
	int     input_length;           // (io)
	short  *output_buffer;          // (i)
	int     output_sample;          // (o)
	int     channel_number;         // (o)
	int     sample_bit;             // (o)
	int     sample_rate;            // (o)
	int     bit_rate;               // (o)
	int     frame_length;           // (o)
	void   *decoder;
} MP3Decoder;

typedef struct 
{
	AudioInfo info;
	void  *pfile;
	int    offset;
	int    vbrflag;
	int    currtime;
	int    index[100];
	char   cache[MP3CACHESIZE];
	MP3Decoder  *decoder;
} MP3Handle;

extern int  MP3Decoder_Init    (MP3Decoder **mp3dec);
extern int  MP3Decoder_Release (MP3Decoder  *mp3dec);
extern int  MP3Decoder_Frame   (MP3Decoder  *mp3dec);

#ifdef __cplusplus
} 
#endif

#endif//__MP3DEC_H__
