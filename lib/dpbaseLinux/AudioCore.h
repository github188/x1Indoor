#ifndef	_AUDIOCORE_H_
#define	_AUDIOCORE_H_

#ifndef __cplusplus
#ifndef bool
typedef enum { false, true } bool;
#endif
#endif

typedef int (*AudioCbFunc)(char *DataBuffer, int DataSize, void *pContext);


#ifdef __cplusplus
extern "C" {
#endif


// Core
void AudioCore_SetLogLevel(int level);
void AudioCore_SetJavaVM(void *vm);
void AudioCore_SetAecMode(bool enable);
void AudioCore_SetPlayVolume(int volume);
void AudioCore_SetRecordVolume(int volume);


// Play
int  Audio_Player_Open();
bool Audio_Player_Close(int StreamID);
bool Audio_Player_Start(int StreamID);
bool Audio_Player_Stop(int StreamID);
bool Audio_Player_Play(int StreamID);
bool Audio_Player_Pause(int StreamID);
bool Audio_Player_Flush(int StreamID);
bool Audio_Player_Write(int StreamID, const char *DataBuffer, int BufferSize, int Timeout);
bool Audio_Player_SetDevice(int StreamID, const char *DeviceName);
bool Audio_Player_SetFormat(int StreamID, int Channels, int SampleBits, int SampleRate);
bool Audio_Player_SetCache(int StreamID, int Blocks, int SamplesPerBlock);
bool Audio_Player_SetMode(int StreamID, bool SyncMode, AudioCbFunc CallbackFunc, void *pContext);
bool Audio_Player_SetMute(int StreamID, bool mute);
bool Audio_Player_SetVolume(int StreamID, int vol);


// Record
int  Audio_Recorder_Open();
bool Audio_Recorder_Close(int StreamID);
bool Audio_Recorder_Start(int StreamID);
bool Audio_Recorder_Stop(int StreamID);
bool Audio_Recorder_Read(int StreamID, char *DataBuffer, int BufferSize, int Timeout);
bool Audio_Recorder_SetDevice(int StreamID, const char *DeviceName);
bool Audio_Recorder_SetFormat(int StreamID, int Channels, int SampleBits, int SampleRate);
bool Audio_Recorder_SetCache(int StreamID, int Blocks, int SamplesPerBlock);
bool Audio_Recorder_SetMode(int StreamID, bool SyncMode, AudioCbFunc CallbackFunc, void *pContext);


#ifdef __cplusplus
} 
#endif

#endif//_AUDIOCORE_H_
