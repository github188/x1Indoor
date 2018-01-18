#ifndef __AUDIOLIB_H__
#define __AUDIOLIB_H__


#define WRATAG


typedef struct
{
	long  timestamp;
	char *string;
} SyncText;

typedef struct
{
	//id3v1 info
	char *title;
	char *artist;
	char *album;
	char *year;
	char *comment;
	char *track;
	char *genre;
	//id3v2 info
	char *length;
	char *textstr;
	SyncText    *synctext;
	int  syncmode;
	int  syncnumber;
} TagInfo;


typedef struct
{
	TagInfo *ContDesc;
	int   ChannelNumber;
	int   SampleBit;
	long  SampleRate;
	long  BitRate;
	long  TotalTime;          // ms
} AudioInfo;


typedef struct
{
	char  ID[6];              // "WRAI\0\0"
	char  ChannelNumber;
	char  SampleBit;
	long  SampleRate;
	long  BitRate;
	long  TotalTime;          // ms
	long  EncodeTime;         // seconds from 1970-01-01 00:00:00
	int   EncodeConfig;
	char  Zero;
	char  Track;
} WRAInfo;



// file operation interface
extern void *audio_fopen  (char *filename, char *mode);
extern int   audio_fclose (void *file);
extern int   audio_fread  (void *file, char *buffer, int readbytes);
extern int   audio_fwrite (void *file, char *buffer, int writebytes);
extern int   audio_fseek  (void *file, int offset, int mode);
extern int   audio_ftell  (void *file);

// memory operation interface
extern void *audio_malloc (int size);
extern void  audio_free   (void *ptr);


#endif//__AUDIOLIB_H__
