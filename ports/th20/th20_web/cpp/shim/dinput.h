// DirectInput 8 surface for the Emscripten/SDL3 (TH_SDL3) build.
// SDK-faithful x86 layouts for the structures the game sources embed by
// offset (DIDEVCAPS is load-bearing); interfaces declare called members only.
// Implementations arrive with the wasm input host (Phase 2/3).
#pragma once

#include "windows.h"

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#define DIENUM_STOP 0
#define DIENUM_CONTINUE 1
#define DIEDFL_ALLDEVICES 0x00000000
#define DIEDFL_ATTACHEDONLY 0x00000001
#define DIEDBS_RECENTDEVICE 0x00000010
#define DIEDBS_FORCEATTACHED 0x00000020

#define DI_OK S_OK
#define DIERR_INPUTLOST ((HRESULT)0x8007001EL)
#define DIERR_ACQUIRED ((HRESULT)0x800700AAL)
#define DIERR_NOTACQUIRED ((HRESULT)0x8007000CL)
#define DIERR_INVALIDPARAM E_INVALIDARG
#define DIERR_GENERIC E_FAIL
#define DIERR_DEVICENOTREG ((HRESULT)0x80040154L)
#define DIERR_NOINTERFACE E_NOINTERFACE

#define DIPH_DEVICE 0
#define DIPH_BYOFFSET 1
#define DIPH_BYID 2
#define DIPH_BYUSAGE 3

#define DI8DEVCLASS_ALL 0
#define DI8DEVCLASS_KEYBOARD 3
#define DI8DEVCLASS_GAMECTRL 4

#define DI8DEVTYPE_KEYBOARD 0x13
#define DI8DEVTYPEKEYBOARD_UNKNOWN 0x51
#define DI8DEVTYPE_GAMEPAD 0x15
#define DI8DEVTYPE_JOYSTICK 0x14
#define DI8DEVTYPESUPPLEMENTAL_UNKNOWN 0x00

#define DISCL_EXCLUSIVE 0x00000001
#define DISCL_NONEXCLUSIVE 0x00000002
#define DISCL_FOREGROUND 0x00000004
#define DISCL_BACKGROUND 0x00000008
#define DISCL_NOWINKEY 0x00000010

#define DIDFT_ALL 0x00000000
#define DIDFT_AXIS 0x00000003
#define DIDFT_POV 0x00000010
#define DIDFT_BUTTON 0x0000000C
#define DIDFT_ABSAXIS 0x00000002
#define DIDFT_ANYINSTANCE 0x00FFFF00
#define DIDFT_MAKEINSTANCE(n) ((WORD)(n) << 8)
#define DIDFT_GETINSTANCE(n) (((n) >> 8) & 0xFFFF)
#define DIDFT_GETTYPE(n) ((n) & 0xFF)

#define DIDOI_ASPECTPOSITION 0x00000100

#pragma pack(push, 4)
typedef struct _DIDEVCAPS {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwDevType;
    DWORD dwAxes;
    DWORD dwButtons;
    DWORD dwPOVs;
    DWORD dwFFSamplePeriod;
    DWORD dwFFMinTimeResolution;
    DWORD dwFirmwareRevision;
    DWORD dwHardwareRevision;
    DWORD dwFFDriverVersion;
} DIDEVCAPS, *LPDIDEVCAPS;
#pragma pack(pop)
static_assert(sizeof(DIDEVCAPS) == 44, "SDK x86 layout");

typedef struct _DIJOYSTATE2 {
    LONG lX, lY, lZ;
    LONG lRx, lRy, lRz;
    LONG rglSlider[2];
    DWORD rgdwPOV[4];
    BYTE rgbButtons[128];
    LONG lVX, lVY, lVZ;
    LONG lVRx, lVRy, lVRz;
    LONG rglVSlider[2];
    LONG lAX, lAY, lAZ;
    LONG lARx, lARy, lARz;
    LONG rglASlider[2];
    LONG lFX, lFY, lFZ;
    LONG lFRx, lFRy, lFRz;
    LONG rglFSlider[2];
} DIJOYSTATE2, *LPDIJOYSTATE2;

typedef struct _DIDEVICEINSTANCEW {
    DWORD dwSize;
    GUID guidInstance;
    GUID guidProduct;
    DWORD dwDevType;
    WCHAR tszInstanceName[MAX_PATH];
    WCHAR tszProductName[MAX_PATH];
    GUID guidFFDriver;
    WORD wUsagePage;
    WORD wUsage;
} DIDEVICEINSTANCEW, *LPDIDEVICEINSTANCEW;
typedef const DIDEVICEINSTANCEW* LPCDIDEVICEINSTANCEW;

typedef struct _DIDEVICEOBJECTINSTANCEW {
    DWORD dwSize;
    GUID guidType;
    DWORD dwOfs;
    DWORD dwType;
    DWORD dwFlags;
    WCHAR tszName[MAX_PATH];
    DWORD dwFFMaxForce;
    DWORD dwFFForceResolution;
    WORD wCollectionNumber;
    WORD wDesignatorIndex;
    WORD wUsagePage;
    WORD wUsage;
    DWORD dwDimension;
    WORD wExponent;
    WORD wReportId;
} DIDEVICEOBJECTINSTANCEW, *LPDIDEVICEOBJECTINSTANCEW;
typedef const DIDEVICEOBJECTINSTANCEW* LPCDIDEVICEOBJECTINSTANCEW;

typedef struct _DIPROPHEADER {
    DWORD dwSize;
    DWORD dwHeaderSize;
    DWORD dwObj;
    DWORD dwHow;
} DIPROPHEADER, *LPDIPROPHEADER;

typedef struct _DIPROPRANGE {
    DIPROPHEADER diph;
    LONG lMin;
    LONG lMax;
} DIPROPRANGE, *LPDIPROPRANGE;

typedef struct _DIOBJECTDATAFORMAT {
    const GUID* pguid;
    DWORD dwOfs;
    DWORD dwType;
    DWORD dwFlags;
} DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;

typedef struct _DIDATAFORMAT {
    DWORD dwSize;
    DWORD dwObjSize;
    DWORD dwFlags;
    DWORD dwDataSize;
    DWORD dwNumObjs;
    LPDIOBJECTDATAFORMAT rgodf;
} DIDATAFORMAT, *LPDIDATAFORMAT;

struct IDirectInput8W;
struct IDirectInputDevice8W;
typedef IDirectInput8W* LPDIRECTINPUT8W;
typedef IDirectInputDevice8W* LPDIRECTINPUTDEVICE8W;
typedef IDirectInput8W IDirectInputW;
typedef IDirectInputDevice8W IDirectInputDeviceW;

typedef BOOL(WINAPI* LPDIENUMDEVICESCALLBACKW)(LPCDIDEVICEINSTANCEW, LPVOID);
typedef BOOL(WINAPI* LPDIENUMDEVICEOBJECTSCALLBACKW)(LPCDIDEVICEOBJECTINSTANCEW, LPVOID);

struct IDirectInput8W : IUnknown {
    virtual HRESULT WINAPI CreateDevice(REFGUID, IDirectInputDevice8W**, IUnknown*) = 0;
    virtual HRESULT WINAPI EnumDevices(DWORD, LPDIENUMDEVICESCALLBACKW, LPVOID, DWORD) = 0;
    virtual HRESULT WINAPI GetDeviceStatus(REFGUID) = 0;
    virtual HRESULT WINAPI RunControlPanel(HWND, DWORD) = 0;
    virtual HRESULT WINAPI Initialize(HINSTANCE, DWORD) = 0;
};

struct IDirectInputDevice8W : IUnknown {
    virtual HRESULT WINAPI GetCapabilities(DIDEVCAPS*) = 0;
    virtual HRESULT WINAPI EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW, LPVOID, DWORD) = 0;
    virtual HRESULT WINAPI GetProperty(REFGUID, DIPROPHEADER*) = 0;
    virtual HRESULT WINAPI SetProperty(REFGUID, const DIPROPHEADER*) = 0;
    virtual HRESULT WINAPI Acquire() = 0;
    virtual HRESULT WINAPI Unacquire() = 0;
    virtual HRESULT WINAPI GetDeviceState(DWORD, LPVOID) = 0;
    virtual HRESULT WINAPI GetDeviceData(DWORD, void*, DWORD*, DWORD) = 0;
    virtual HRESULT WINAPI SetDataFormat(const DIDATAFORMAT*) = 0;
    virtual HRESULT WINAPI SetEventNotification(HANDLE) = 0;
    virtual HRESULT WINAPI SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT WINAPI GetObjectInfo(DIDEVICEOBJECTINSTANCEW*, DWORD, DWORD) = 0;
    virtual HRESULT WINAPI GetDeviceInfo(DIDEVICEINSTANCEW*) = 0;
    virtual HRESULT WINAPI RunControlPanel(HWND, DWORD) = 0;
    virtual HRESULT WINAPI Initialize(HINSTANCE, DWORD, REFGUID) = 0;
    virtual HRESULT WINAPI Poll() = 0;
};

extern "C" {
HRESULT WINAPI DirectInput8Create(HINSTANCE, DWORD, REFIID, void**, IUnknown*);
extern const GUID GUID_SysKeyboard;
extern const GUID GUID_Joystick;
extern const GUID GUID_XAxis;
extern const GUID GUID_YAxis;
extern const GUID GUID_ZAxis;
extern const GUID GUID_RzAxis;
extern const GUID DIPROP_RANGE;
extern const GUID DIPROP_DEADZONE;
extern const GUID DIPROP_SATURATION;
extern const GUID IID_IDirectInput8W;
extern const DIDATAFORMAT c_dfDIKeyboard;
extern const DIDATAFORMAT c_dfDIJoystick2;
}

#define GUID_NULL GUID{}
