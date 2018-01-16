/********************************************************************************
 *                                                              								*
 * Copyright (c) Nuvoton Technology Corp. All rights reserved. 								*
 *                                                              								*
 ********************************************************************************/
#include "wav_record.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_WriteHeader(
	S_WavFileWriteInfo* psInfo
) {
	const BYTE abySignature1[4] = { 'R', 'I', 'F', 'F' };

	if (4 != write(psInfo->i32FileHandle, abySignature1, 4))
		return FALSE;

	psInfo->u32RiffDataLenFilePos = lseek(psInfo->i32FileHandle, 0, SEEK_CUR);
	if (-1 == psInfo->u32RiffDataLenFilePos)
		return FALSE;

	UINT32 u32Len = 0;

	if (4 != write(psInfo->i32FileHandle, &u32Len, 4))
		return FALSE;

	const BYTE abySignature2[4] = { 'W', 'A', 'V', 'E' };

	if (4 != write(psInfo->i32FileHandle, abySignature2, 4))
		return FALSE;
	return TRUE;
} // WavFileUtil_Write_WriteHeader()

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_SetFormatInternal(
	S_WavFileWriteInfo*	psInfo,
	void*				pWavFormat,
	UINT32				u32WavFormatDataLen
) {
	if (!psInfo || psInfo->i32FileHandle < 0)
		return FALSE;

	// "fmt " chunk
	static const BYTE abySignature1[4] = { 'f', 'm', 't', ' ' };

	if (4 != write(psInfo->i32FileHandle, abySignature1, 4))
		return FALSE;
	if (4 != write(psInfo->i32FileHandle, &u32WavFormatDataLen, 4))
		return FALSE;
	if (u32WavFormatDataLen != write(psInfo->i32FileHandle, pWavFormat, u32WavFormatDataLen))
		return FALSE;

	// "data" chunk
	static const BYTE abySignature2[4] = { 'd', 'a', 't', 'a' };

	if (4 != write(psInfo->i32FileHandle, abySignature2, 4))
		return FALSE;

	psInfo->u32DataChunkLenFilePos = lseek(psInfo->i32FileHandle, 0, SEEK_CUR);
	if (-1 == psInfo->u32DataChunkLenFilePos)
		return FALSE;

	UINT32 u32ChunkLen = 0;

	if (4 != write(psInfo->i32FileHandle, &u32ChunkLen, 4))
		return FALSE;
	return TRUE;
} // WavFileUtil_Write_SetFormatInternal()

//----------------------------------------------------------------------------
/*
BOOL
WavFileUtil_Write_SetFormat(
	S_WavFileWriteInfo*	psInfo,
	S_WavFormat*		psWavFormat
	)
{
	return WavFileUtil_Write_SetFormatInternal(
				psInfo, psWavFormat, WAV_FORMAT_LEN);
} // WavFileUtil_Write_SetFormat()
*/

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_SetFormatEx(
	S_WavFileWriteInfo*	psInfo,
	S_WavFormatEx*		psWavFormatEx
) {
	return WavFileUtil_Write_SetFormatInternal(
			   psInfo, psWavFormatEx, WAV_FORMAT_EX_LEN);
} // WavFileUtil_Write_SetFormatEx()


//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_SetIMAFormat(
	S_WavFileWriteInfo*	psInfo,
	S_IMAWavFormat*		psIMAWavFormat
) {
	return WavFileUtil_Write_SetFormatInternal(
			   psInfo, psIMAWavFormat, IMA_WAV_FORMAT_LEN);
} // WavFileUtil_Write_SetIMAFormat()

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_FillFormatEx(
	S_WavFormatEx*	psWavFormatEx,
	UINT16			u16Channels,
	UINT32			u32SamplingRate,
	UINT16			u16BitsPerSample	// 8 or 16
) {
	if ((8 != u16BitsPerSample) && (16 != u16BitsPerSample))
		return FALSE;

	psWavFormatEx->u16FormatTag = 0x0001;					// PCM
	psWavFormatEx->u16Channels = u16Channels;
	psWavFormatEx->u32SamplesPerSec = u32SamplingRate;
	psWavFormatEx->u16BlockAlign = u16BitsPerSample / 8;	// (N + 1) * 4 * nChannels
	psWavFormatEx->u16BitsPerSample = u16BitsPerSample;
	psWavFormatEx->u16CbSize = 0;
	psWavFormatEx->u32AvgBytesPerSec =
		(u32SamplingRate * psWavFormatEx->u16BlockAlign);
	return TRUE;
} // WavFileUtil_Write_FillFormatEx()

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_FillFormatG726(
	S_WavFormatEx*	psWavFormatEx,
	UINT16			u16Channels,
	UINT32			u32SamplingRate,
	UINT16			u16BitsPerSample	// 8 or 16
) {
//	if ((2 != u16BitsPerSample) && (3 != u16BitsPerSample))
//		return FALSE;

	psWavFormatEx->u16FormatTag = eWAVE_FORMAT_G726_ADPCM;	// G726
	psWavFormatEx->u16Channels = u16Channels;
	psWavFormatEx->u32SamplesPerSec = u32SamplingRate;
	psWavFormatEx->u16BlockAlign = u16Channels * u16BitsPerSample / 8;
	psWavFormatEx->u16BitsPerSample = u16BitsPerSample;
	psWavFormatEx->u16CbSize = 0;
	psWavFormatEx->u32AvgBytesPerSec =
		(u32SamplingRate * psWavFormatEx->u16BlockAlign);
	return TRUE;
} // WavFileUtil_Write_FillFormatG726()

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_FillIMAFormat(
	S_IMAWavFormat*	psIMAWavFormat,
	UINT16			u16Channels,
	UINT32			u32SamplingRate,
	UINT16			u16BitsPerSample
) {
	if ((3 != u16BitsPerSample) && (4 != u16BitsPerSample))
		return FALSE;

	UINT16 u16N;

	if (u32SamplingRate < 22050)
		u16N = 63;
	else if (u32SamplingRate < 44100) {
		if (3 == u16BitsPerSample)
			u16N = 126;
		else
			u16N = 127;
	}
	else
		u16N = 255;

	psIMAWavFormat->u16FormatTag = 0x0011;							// DVI ADPCM
	psIMAWavFormat->u16Channels = u16Channels;
	psIMAWavFormat->u32SamplesPerSec = u32SamplingRate;
	psIMAWavFormat->u16BlockAlign = (u16N + 1) * 4 * u16Channels;	// (N + 1) * 4 * nChannels
	psIMAWavFormat->u16BitsPerSample = u16BitsPerSample;
	psIMAWavFormat->u16CbSize = 2;
	if (3 == u16BitsPerSample)
		psIMAWavFormat->u16SamplesPerBlock = (u16N * 32) / 3 + 1;	// ((N * 4 * 8) / 3) + 1
	else
		psIMAWavFormat->u16SamplesPerBlock = (u16N * 8) + 1;		// ((N * 4 * 8) / 4) + 1

	psIMAWavFormat->u32AvgBytesPerSec =
		(u32SamplingRate * psIMAWavFormat->u16BlockAlign) / 
		psIMAWavFormat->u16SamplesPerBlock;
	return TRUE;
} // WavFileUtil_Write_FillIMAFormat()

//----------------------------------------------------------------------------
static BOOL
WavFileUtil_Write_FillFormatALaw(
	S_WavFormatEx*	psWavFormatEx,
	UINT16			u16Channels,
	UINT32			u32SamplingRate,
	UINT16			u16BitsPerSample	// 8 or 16
) {
	if ((8 != u16BitsPerSample) && (16 != u16BitsPerSample))
		return FALSE;

	psWavFormatEx->u16FormatTag = eWAVE_FORMAT_G711_ALAW;					// ALaw
	psWavFormatEx->u16Channels = u16Channels;
	psWavFormatEx->u32SamplesPerSec = u32SamplingRate;
	psWavFormatEx->u16BlockAlign = u16BitsPerSample / 8;
	psWavFormatEx->u16BitsPerSample = u16BitsPerSample;
	psWavFormatEx->u16CbSize = 0;
	psWavFormatEx->u32AvgBytesPerSec =
		(u32SamplingRate * psWavFormatEx->u16BlockAlign);
	return TRUE;
} // WavFileUtil_Write_FillFormatALaw()

//----------------------------------------------------------------------------
// Public functions
//----------------------------------------------------------------------------
BOOL
WavFileUtil_Write_Initialize(
	S_WavFileWriteInfo*	psInfo,
	PCSTR				pszOutFileName

) {
	if (NULL == psInfo)
		return FALSE;

	psInfo->i32FileHandle = creat(pszOutFileName, S_IWUSR | S_IRUSR);
	if (psInfo->i32FileHandle < 0)
		return FALSE;

	psInfo->u32DataChunkLenFilePos = 0;
	psInfo->u32RiffDataLenFilePos = 0;
	if (!WavFileUtil_Write_WriteHeader(psInfo)) {
		close(psInfo->i32FileHandle);
		psInfo->i32FileHandle = -1;
		return FALSE;
	}

	return TRUE;
} // WavFileUtil_Write_Initialize()

//----------------------------------------------------------------------------
BOOL
WavFileUtil_Write_SetFormat(
	S_WavFileWriteInfo*	psInfo,
	E_WavFormatTag		eWavFormatTag,
	UINT16				u16Channels,
	UINT32				u32SamplingRate,
	UINT16				u16BitsPerSample
) {
	S_WavFormatEx	sWavFormatEx;

	switch (eWavFormatTag) {
	case eWAVE_FORMAT_PCM:
		if (!WavFileUtil_Write_FillFormatEx(&sWavFormatEx, u16Channels,
											u32SamplingRate, u16BitsPerSample))
			return FALSE;
		if (!WavFileUtil_Write_SetFormatEx(psInfo, &sWavFormatEx))
			return FALSE;
		break;

	case eWAVE_FORMAT_IMA_ADPCM: {
		S_IMAWavFormat	sIMAWavFormat;

		if (!WavFileUtil_Write_FillIMAFormat(&sIMAWavFormat, u16Channels,
											 u32SamplingRate, u16BitsPerSample))
			return FALSE;
		if (!WavFileUtil_Write_SetIMAFormat(psInfo, &sIMAWavFormat))
			return FALSE;
		break;
	}

	case eWAVE_FORMAT_G726_ADPCM:{
		if (!WavFileUtil_Write_FillFormatG726(&sWavFormatEx, u16Channels,
											  u32SamplingRate, u16BitsPerSample))
			return FALSE;
		if (!WavFileUtil_Write_SetFormatEx(psInfo, &sWavFormatEx))
			return FALSE;
		break;
	}

	case eWAVE_FORMAT_G711_ALAW:{
		if (!WavFileUtil_Write_FillFormatALaw(&sWavFormatEx, u16Channels,
											  u32SamplingRate, u16BitsPerSample))
			return FALSE;
		if (!WavFileUtil_Write_SetFormatEx(psInfo, &sWavFormatEx))
			return FALSE;
		break;
	}


	default:
		return FALSE;
	} // switch eWavFormatTag

	return TRUE;
} // WavFileUtil_Write_SetFormat()

//----------------------------------------------------------------------------
BOOL
WavFileUtil_Write_WriteData(
	S_WavFileWriteInfo*	psInfo,
	const BYTE*			pbyData,
	UINT32				u32DataSize
) {
	if (!psInfo || psInfo->i32FileHandle < 0)
		return FALSE;

	if (u32DataSize != write(psInfo->i32FileHandle, pbyData, u32DataSize))
		return FALSE;

	return TRUE;
} // WavFileUtil_Write_WriteData()

//----------------------------------------------------------------------------
BOOL
WavFileUtil_Write_Finish(
	S_WavFileWriteInfo*	psInfo
) {
	if (!psInfo || psInfo->i32FileHandle < 0)
		return FALSE;

	UINT32 u32Pos;

	u32Pos = lseek(psInfo->i32FileHandle, 0, SEEK_CUR);
	if (-1 == u32Pos)
		return FALSE;

	if ((u32Pos > psInfo->u32RiffDataLenFilePos) &&
			(u32Pos > psInfo->u32DataChunkLenFilePos)) {
		UINT32	u32Len;

		u32Len = u32Pos - psInfo->u32RiffDataLenFilePos - 4;
		if (psInfo->u32RiffDataLenFilePos != lseek(psInfo->i32FileHandle, psInfo->u32RiffDataLenFilePos, SEEK_SET)
			|| 4 != write(psInfo->i32FileHandle, &u32Len, 4))
				return FALSE;

		u32Len = u32Pos - psInfo->u32DataChunkLenFilePos - 4;
		if (psInfo->u32DataChunkLenFilePos != lseek(psInfo->i32FileHandle, psInfo->u32DataChunkLenFilePos, SEEK_SET)
			|| 4 != write(psInfo->i32FileHandle, &u32Len, 4))
				return FALSE;
	}

	// add by chenbh 20180112
	fsync(psInfo->i32FileHandle);
	
	if (0 != close(psInfo->i32FileHandle))
		return FALSE;

	psInfo->i32FileHandle = -1;
	return TRUE;
} // WavFileUtil_Write_Finish()

