#pragma once
#include "windows.h"
using HWAVEOUT=struct WebWave*;
using MMRESULT=unsigned;
struct WAVEHDR {LPSTR lpData;DWORD dwBufferLength,dwFlags;};
struct WAVEFORMATEX {WORD wFormatTag,nChannels;DWORD nSamplesPerSec,nAvgBytesPerSec;WORD nBlockAlign,wBitsPerSample;};
enum : UINT {WAVE_MAPPER=0xffffffffu,WAVE_FORMAT_PCM=1,CALLBACK_EVENT=0x50000,
             WHDR_DONE=1,WHDR_INQUEUE=16,MMSYSERR_NOERROR=0};
MMRESULT waveOutOpen(HWAVEOUT*,UINT,const WAVEFORMATEX*,DWORD_PTR,DWORD_PTR,DWORD);
MMRESULT waveOutPrepareHeader(HWAVEOUT,WAVEHDR*,UINT);
MMRESULT waveOutUnprepareHeader(HWAVEOUT,WAVEHDR*,UINT);
MMRESULT waveOutWrite(HWAVEOUT,WAVEHDR*,UINT);
MMRESULT waveOutPause(HWAVEOUT);
MMRESULT waveOutRestart(HWAVEOUT);
MMRESULT waveOutReset(HWAVEOUT);
MMRESULT waveOutClose(HWAVEOUT);
