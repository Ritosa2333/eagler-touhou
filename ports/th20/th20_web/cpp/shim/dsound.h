// DirectSound 8 surface for the Emscripten/SDL3 (TH_SDL3) build.
// SDK-faithful x86 struct layouts; interfaces declare only the members the
// reconstructed sources call. Implementations arrive with the wasm audio
// backend (Phase 2/3); nothing is implemented in this header.
#pragma once

#include "windows.h"

#pragma pack(push, 1)
typedef struct tWAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX* LPCWAVEFORMATEX;
#pragma pack(pop)
static_assert(sizeof(WAVEFORMATEX) == 18, "SDK x86 layout");

#define WAVE_FORMAT_PCM 1

#define DSBCAPS_PRIMARYBUFFER 0x00000001
#define DSBCAPS_STATIC 0x00000002
#define DSBCAPS_LOCHARDWARE 0x00000004
#define DSBCAPS_LOCSOFTWARE 0x00000008
#define DSBCAPS_CTRL3D 0x00000010
#define DSBCAPS_CTRLFREQUENCY 0x00000020
#define DSBCAPS_CTRLPAN 0x00000040
#define DSBCAPS_CTRLVOLUME 0x00000080
#define DSBCAPS_CTRLPOSITIONNOTIFY 0x00000100
#define DSBCAPS_STICKYFOCUS 0x00004000
#define DSBCAPS_GLOBALFOCUS 0x00008000
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000
#define DSBCAPS_MUTE3DATMAXDISTANCE 0x00020000

#define DSSCL_NORMAL 1
#define DSSCL_PRIORITY 2
#define DSSCL_EXCLUSIVE 3
#define DSSCL_WRITEPRIMARY 4

#define DSBSTATUS_PLAYING 0x00000001
#define DSBSTATUS_BUFFERLOST 0x00000002
#define DSBSTATUS_LOOPING 0x00000004

#define DSBPLAY_LOOPING 0x00000001

#define DSBPN_OFFSETSTOP 0xFFFFFFFF

#define DS_OK S_OK
#define DSERR_ALLOCATED ((HRESULT)0x8878000AL)
#define DSERR_CONTROLUNAVAIL ((HRESULT)0x8878001EL)
#define DSERR_INVALIDPARAM E_INVALIDARG
#define DSERR_INVALIDCALL ((HRESULT)0x88780032L)
#define DSERR_GENERIC E_FAIL
#define DSERR_PRIOLEVELNEEDED ((HRESULT)0x88780046L)
#define DSERR_OUTOFMEMORY E_OUTOFMEMORY
#define DSERR_BADFORMAT ((HRESULT)0x88780064L)
#define DSERR_UNSUPPORTED E_NOTIMPL
#define DSERR_NODRIVER ((HRESULT)0x88780078L)
#define DSERR_ALREADYINITIALIZED ((HRESULT)0x88780082L)
#define DSERR_NOAGGREGATION ((HRESULT)0x8878008CL)
#define DSERR_BUFFERLOST ((HRESULT)0x88780096L)
#define DSERR_OTHERAPPHASPRIO ((HRESULT)0x887800A0L)
#define DSERR_UNINITIALIZED ((HRESULT)0x887800AAL)

#define DSBVOLUME_MIN (-10000)
#define DSBVOLUME_MAX 0

typedef struct _DSBUFFERDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    LPWAVEFORMATEX lpwfxFormat;
    GUID guid3DAlgorithm;
} DSBUFFERDESC, *LPDSBUFFERDESC;

typedef struct _DSBPOSITIONNOTIFY {
    DWORD dwOffset;
    HANDLE hEventNotify;
} DSBPOSITIONNOTIFY, *LPDSBPOSITIONNOTIFY;

typedef struct _DSCAPS {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwMinSecondarySampleRate;
    DWORD dwMaxSecondarySampleRate;
    DWORD dwPrimaryBuffers;
    DWORD dwMaxHwMixingAllBuffers;
    DWORD dwMaxHwMixingStaticBuffers;
    DWORD dwMaxHwMixingStreamingBuffers;
    DWORD dwFreeHwMixingAllBuffers;
    DWORD dwFreeHwMixingStaticBuffers;
    DWORD dwFreeHwMixingStreamingBuffers;
    DWORD dwMaxHw3DAllBuffers;
    DWORD dwMaxHw3DStaticBuffers;
    DWORD dwMaxHw3DStreamingBuffers;
    DWORD dwFreeHw3DAllBuffers;
    DWORD dwFreeHw3DStaticBuffers;
    DWORD dwFreeHw3DStreamingBuffers;
    DWORD dwTotalHwMemBytes;
    DWORD dwFreeHwMemBytes;
    DWORD dwMaxContigFreeHwMemBytes;
    DWORD dwUnlockTransferRateHwBuffers;
    DWORD dwPlayCpuOverheadSwBuffers;
    DWORD dwReserved1;
    DWORD dwReserved2;
} DSCAPS, *LPDSCAPS;

struct IDirectSound8;
struct IDirectSoundBuffer8;
struct IDirectSoundBuffer;
struct IDirectSoundNotify8;
typedef IDirectSoundBuffer* LPDIRECTSOUNDBUFFER;
typedef IDirectSound8* LPDIRECTSOUND8;

struct IDirectSound8 : IUnknown {
    virtual HRESULT WINAPI CreateSoundBuffer(const DSBUFFERDESC*, IDirectSoundBuffer**, IUnknown*) = 0;
    virtual HRESULT WINAPI GetCaps(DSCAPS*) = 0;
    virtual HRESULT WINAPI DuplicateSoundBuffer(IDirectSoundBuffer*, IDirectSoundBuffer**) = 0;
    virtual HRESULT WINAPI SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT WINAPI Compact() = 0;
    virtual HRESULT WINAPI GetSpeakerConfig(DWORD*) = 0;
    virtual HRESULT WINAPI SetSpeakerConfig(DWORD) = 0;
    virtual HRESULT WINAPI Initialize(const GUID*) = 0;
};

struct IDirectSoundBuffer : IUnknown {
    virtual HRESULT WINAPI GetCaps(void*) = 0;
    virtual HRESULT WINAPI GetCurrentPosition(DWORD*, DWORD*) = 0;
    virtual HRESULT WINAPI GetFormat(WAVEFORMATEX*, DWORD, DWORD*) = 0;
    virtual HRESULT WINAPI GetVolume(LONG*) = 0;
    virtual HRESULT WINAPI GetPan(LONG*) = 0;
    virtual HRESULT WINAPI GetFrequency(DWORD*) = 0;
    virtual HRESULT WINAPI GetStatus(DWORD*) = 0;
    virtual HRESULT WINAPI Initialize(IDirectSound8*, const DSBUFFERDESC*) = 0;
    virtual HRESULT WINAPI Lock(DWORD, DWORD, void**, DWORD*, void**, DWORD*, DWORD) = 0;
    virtual HRESULT WINAPI Play(DWORD, DWORD, DWORD) = 0;
    virtual HRESULT WINAPI SetCurrentPosition(DWORD) = 0;
    virtual HRESULT WINAPI SetFormat(const WAVEFORMATEX*) = 0;
    virtual HRESULT WINAPI SetVolume(LONG) = 0;
    virtual HRESULT WINAPI SetPan(LONG) = 0;
    virtual HRESULT WINAPI SetFrequency(DWORD) = 0;
    virtual HRESULT WINAPI Stop() = 0;
    virtual HRESULT WINAPI Unlock(void*, DWORD, void*, DWORD) = 0;
    virtual HRESULT WINAPI Restore() = 0;
};

struct IDirectSoundBuffer8 : IDirectSoundBuffer {};

struct IDirectSoundNotify8 : IUnknown {
    virtual HRESULT WINAPI SetNotificationPositions(DWORD, const DSBPOSITIONNOTIFY*) = 0;
};
typedef IDirectSoundNotify8 IDirectSoundNotify;

extern "C" {
HRESULT WINAPI DirectSoundCreate8(const GUID*, IDirectSound8**, IUnknown*);
extern const GUID IID_IDirectSoundNotify;
}
