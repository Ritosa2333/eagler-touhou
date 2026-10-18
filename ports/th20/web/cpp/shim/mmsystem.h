// WinMM surface for the Emscripten/SDL3 (TH_SDL3) build. Declarations only;
// the wasm platform-services host (Phase 2/3) supplies implementations.
#pragma once

#include "windows.h"

typedef UINT MMRESULT;

#define MMSYSERR_NOERROR 0
#define JOYERR_NOERROR 0
#define JOYERR_PARMS 165
#define JOYERR_UNPLUGGED 167
#define TIMERR_NOERROR 0

#define MAXPNAMELEN 32
#define MAX_JOYSTICKOEMVXDNAME 260

typedef struct joyinfoex_tag {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwXpos;
    DWORD dwYpos;
    DWORD dwZpos;
    DWORD dwRpos;
    DWORD dwUpos;
    DWORD dwVpos;
    DWORD dwButtons;
    DWORD dwButtonNumber;
    DWORD dwPOV;
    DWORD dwReserved1;
    DWORD dwReserved2;
} JOYINFOEX, *LPJOYINFOEX;

typedef struct joycapsw_tag {
    WORD wMid;
    WORD wPid;
    WCHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    DWORD wNumButtons;
    DWORD wPeriodMin;
    DWORD wPeriodMax;
    DWORD wRmin;
    DWORD wRmax;
    DWORD wUmin;
    DWORD wUmax;
    DWORD wVmin;
    DWORD wVmax;
    DWORD wCaps;
    DWORD wMaxAxes;
    DWORD wNumAxes;
    DWORD wMaxButtons;
    WCHAR szRegKey[MAXPNAMELEN];
    WCHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYCAPSW, *LPJOYCAPSW;

#define JOY_RETURNX 0x00000001L
#define JOY_RETURNY 0x00000002L
#define JOY_RETURNZ 0x00000004L
#define JOY_RETURNR 0x00000008L
#define JOY_RETURNU 0x00000010L
#define JOY_RETURNV 0x00000020L
#define JOY_RETURNPOV 0x00000040L
#define JOY_RETURNBUTTONS 0x00000080
#define JOY_RETURNALL (JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | JOY_RETURNPOV | JOY_RETURNBUTTONS)
#define JOY_USEDEADZONE 0x00000800L

#define JOYSTICKID1 0
#define JOYSTICKID2 1

extern "C" {
MMRESULT WINAPI joyGetPosEx(UINT, LPJOYINFOEX);
MMRESULT WINAPI joyGetDevCapsW(UINT, LPJOYCAPSW, UINT);
DWORD WINAPI timeGetTime();
MMRESULT WINAPI timeBeginPeriod(UINT);
MMRESULT WINAPI timeEndPeriod(UINT);
MMRESULT WINAPI timeSetEvent(UINT, UINT, void*, DWORD_PTR, UINT);
MMRESULT WINAPI timeKillEvent(UINT);
}
