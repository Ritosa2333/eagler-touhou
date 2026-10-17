// Minimal Windows API surface for the Emscripten/SDL3 (TH_SDL3) build.
// Only what the reconstructed TH20 sources actually reference: honest type
// definitions with SDK-faithful layouts, constants with real values, and
// function/interface DECLARATIONS. No function is implemented here; anything
// the wasm host layer has not provided yet fails at link time, by design
// (see AGENTS.md: no fake behavior stubs).
#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifndef __cplusplus
#error "th20 shim headers require C++"
#endif

// --- base scalar types ------------------------------------------------------
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef int INT;
typedef long LONG;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef float FLOAT;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef unsigned char UCHAR;
typedef void* PVOID;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef WCHAR* PWSTR;
typedef char* PSTR;
typedef size_t SIZE_T;
typedef uintptr_t DWORD_PTR;
typedef uintptr_t ULONG_PTR;
typedef intptr_t LONG_PTR;
typedef intptr_t INT_PTR;
typedef uintptr_t UINT_PTR;
typedef uintptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t LRESULT;
typedef BYTE BOOLEAN;
typedef uint8_t UINT8;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int32_t INT32;
typedef uint32_t UINT32;
typedef int64_t INT64;
typedef uint64_t UINT64;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

// --- handles (opaque, STRICT-style distinct types) --------------------------
#define TH20_DECLARE_HANDLE(name) typedef struct name##__* name
TH20_DECLARE_HANDLE(HWND);
TH20_DECLARE_HANDLE(HINSTANCE);
TH20_DECLARE_HANDLE(HDC);
TH20_DECLARE_HANDLE(HICON);
TH20_DECLARE_HANDLE(HCURSOR);
TH20_DECLARE_HANDLE(HBRUSH);
TH20_DECLARE_HANDLE(HMENU);
TH20_DECLARE_HANDLE(HFONT);
TH20_DECLARE_HANDLE(HBITMAP);
typedef HINSTANCE HMODULE;
typedef void* HANDLE;
typedef void* HGDIOBJ;
typedef void* HKEY;
typedef void* HHOOK;
typedef void* HDWP;
typedef void* HRAWINPUT;
typedef void* HGLOBAL;
typedef void* HLOCAL;
typedef void* HPALETTE;
typedef int(WINAPI* FARPROC)();

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#ifndef NULL
#define NULL 0
#endif

// --- HRESULT / GUID / COM ---------------------------------------------------
typedef LONG HRESULT;
typedef struct _GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
typedef const GUID& REFGUID;
typedef const GUID& REFIID;
typedef GUID IID;
typedef GUID CLSID;
typedef const GUID& REFCLSID;

inline bool operator==(const GUID& a, const GUID& b) {
    return __builtin_memcmp(&a, &b, sizeof(GUID)) == 0;
}
inline bool operator!=(const GUID& a, const GUID& b) { return !(a == b); }

#define S_OK ((HRESULT)0L)
#define S_FALSE ((HRESULT)1L)
#define E_NOINTERFACE ((HRESULT)0x80004002L)
#define E_POINTER ((HRESULT)0x80004003L)
#define E_FAIL ((HRESULT)0x80004005L)
#define E_HANDLE ((HRESULT)0x80070006L)
#define E_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define E_INVALIDARG ((HRESULT)0x80070057L)
#define E_NOTIMPL ((HRESULT)0x80004001L)
#define DISP_E_PARAMNOTFOUND ((HRESULT)0x80020004L)
#define ERROR_SUCCESS 0L
#define ERROR_PROC_NOT_FOUND 127L

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define HRESULT_CODE(hr) ((hr)&0xFFFF)
#define HRESULT_FROM_WIN32(x) ((HRESULT)(x) <= 0 ? ((HRESULT)(x)) : ((HRESULT)(((x)&0x0000FFFF) | (7 << 16) | 0x80000000)))
#define E_UNEXPECTED ((HRESULT)0x8000FFFFL)
#define CO_E_NOTINITIALIZED ((HRESULT)0x800401F0L)
#define RPC_E_CHANGED_MODE ((HRESULT)0x80010106L)

typedef struct IUnknown IUnknown;
struct IUnknown {
    virtual HRESULT WINAPI QueryInterface(REFIID, void**) = 0;
    virtual ULONG WINAPI AddRef() = 0;
    virtual ULONG WINAPI Release() = 0;
};

#define CLSCTX_INPROC_SERVER 0x1
#define RPC_C_AUTHN_WINNT 10
#define RPC_C_AUTHZ_NONE 0
#define RPC_C_IMP_LEVEL_IMPERSONATE 3
#define RPC_C_AUTHN_LEVEL_CALL 3
#define EOAC_NONE 0

extern "C" {
HRESULT WINAPI CoInitialize(LPVOID);
HRESULT WINAPI CoInitializeEx(LPVOID, DWORD);
void WINAPI CoUninitialize();
HRESULT WINAPI CoCreateInstance(REFCLSID, IUnknown*, DWORD, REFIID, void**);
HRESULT WINAPI CoSetProxyBlanket(IUnknown*, DWORD, DWORD, WCHAR*, DWORD, DWORD, void*, DWORD);
}

// OLE Automation / VARIANT (used by the WMI query in input_win32.cpp)
typedef WCHAR* BSTR;
typedef unsigned short VARTYPE;
#define VT_EMPTY 0
#define VT_NULL 1
#define VT_BSTR 8
#define VT_BYREF 0x4000
#pragma pack(push, 8)
typedef struct tagVARIANT {
    VARTYPE vt;
    WORD wReserved1, wReserved2, wReserved3;
    union {
        LONGLONG llVal;
        LONG lVal;
        BSTR bstrVal;
        IUnknown* punkVal;
        PVOID byref;
    };
} VARIANT;
#pragma pack(pop)
static_assert(sizeof(VARIANT) == 16, "x86 VARIANT layout");

extern "C" {
BSTR WINAPI SysAllocString(const WCHAR*);
void WINAPI SysFreeString(BSTR);
void WINAPI VariantInit(VARIANT*);
HRESULT WINAPI VariantClear(VARIANT*);
}

// --- common structs ---------------------------------------------------------
typedef struct tagPOINT { LONG x, y; } POINT, *LPPOINT;
typedef struct tagSIZE { LONG cx, cy; } SIZE, *LPSIZE;
typedef struct tagRECT { LONG left, top, right, bottom; } RECT, *LPRECT;
typedef const RECT* LPCRECT;
typedef union _LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
typedef struct _SYSTEM_INFO {
    DWORD dwOemId;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;
typedef struct tagMSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
} MSG, *LPMSG;
typedef struct tagPAINTSTRUCT {
    HDC hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT, *LPPAINTSTRUCT;
typedef struct _RTL_CRITICAL_SECTION { void* reserved[6]; } CRITICAL_SECTION, *LPCRITICAL_SECTION;
typedef DWORD(WINAPI* LPTHREAD_START_ROUTINE)(LPVOID);
typedef LRESULT(WINAPI* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef LONG(WINAPI* PVECTORED_EXCEPTION_HANDLER)(void*);

typedef struct tagWNDCLASSW {
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCWSTR lpszMenuName;
    LPCWSTR lpszClassName;
} WNDCLASSW, *LPWNDCLASSW;

// --- GDI text/bitmap structs (x86 layouts) ----------------------------------
#define LF_FACESIZE 32
typedef struct tagLOGFONTW {
    LONG lfHeight, lfWidth, lfEscapement, lfOrientation, lfWeight;
    BYTE lfItalic, lfUnderline, lfStrikeOut, lfCharSet, lfOutPrecision,
        lfClipPrecision, lfQuality, lfPitchAndFamily;
    WCHAR lfFaceName[LF_FACESIZE];
} LOGFONTW, *LPLOGFONTW;
typedef struct tagTEXTMETRICW {
    LONG tmHeight, tmAscent, tmDescent, tmInternalLeading, tmExternalLeading,
        tmAveCharWidth, tmMaxCharWidth, tmWeight, tmOverhang, tmDigitizedAspectX,
        tmDigitizedAspectY;
    WCHAR tmFirstChar, tmLastChar, tmDefaultChar, tmBreakChar;
    BYTE tmItalic, tmUnderlined, tmStruckOut, tmPitchAndFamily, tmCharSet;
} TEXTMETRICW, *LPTEXTMETRICW;
typedef struct tagRGBQUAD { BYTE rgbBlue, rgbGreen, rgbRed, rgbReserved; } RGBQUAD;
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth, biHeight;
    WORD biPlanes, biBitCount;
    DWORD biCompression, biSizeImage;
    LONG biXPelsPerMeter, biYPelsPerMeter;
    DWORD biClrUsed, biClrImportant;
} BITMAPINFOHEADER;
typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO;
typedef struct tagBITMAPV4HEADER {
    DWORD bV4Size;
    LONG bV4Width, bV4Height;
    WORD bV4Planes, bV4BitCount;
    DWORD bV4V4Compression, bV4SizeImage;
    LONG bV4XPelsPerMeter, bV4YPelsPerMeter;
    DWORD bV4ClrUsed, bV4ClrImportant;
    DWORD bV4RedMask, bV4GreenMask, bV4BlueMask, bV4AlphaMask;
    DWORD bV4CSType;
    DWORD bV4Endpoints[9];
    DWORD bV4GammaRed, bV4GammaGreen, bV4GammaBlue;
} BITMAPV4HEADER, *LPBITMAPV4HEADER;
typedef DWORD COLORREF;
#define RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))

// --- constants used by the game sources -------------------------------------
#define MAX_PATH 260
#define CP_ACP 0
#define MB_PRECOMPOSED 0x1
#define WC_NO_BEST_FIT_CHARS 0x400

#define PM_NOREMOVE 0x0000
#define PM_REMOVE 0x0001
#define QS_KEY 0x0001
#define QS_MOUSEMOVE 0x0002
#define QS_MOUSEBUTTON 0x0004
#define QS_MOUSE (QS_MOUSEMOVE | QS_MOUSEBUTTON)
#define QS_INPUT (QS_MOUSE | QS_KEY)
#define QS_PAINT 0x0020
#define QS_TIMER 0x0010
#define QS_ALLINPUT (QS_INPUT | QS_PAINT | QS_TIMER | 0x0080 | 0x0400 | 0x0800)

#define WAIT_OBJECT_0 0x00000000L
#define WAIT_TIMEOUT 0x00000102L
#define WAIT_FAILED 0xFFFFFFFFL
#define INFINITE 0xFFFFFFFF

#define PAGE_READWRITE 0x04
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x8000
#define MEM_DECOMMIT 0x4000

#define GENERIC_READ 0x80000000L
#define GENERIC_WRITE 0x40000000L
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define OPEN_EXISTING 3
#define CREATE_ALWAYS 2
#define CREATE_NEW 1
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x00000200
#define FORMAT_MESSAGE_FROM_SYSTEM 0x00001000

#define SEM_FAILCRITICALERRORS 0x0001
#define SPI_GETSCREENSAVEACTIVE 0x0010
#define SPI_SETSCREENSAVEACTIVE 0x0011
#define SPI_GETPOWEROFFACTIVE 0x0054
#define SPI_SETPOWEROFFACTIVE 0x0055
#define SPI_GETLOWPOWERACTIVE 0x0053
#define SPI_SETLOWPOWERACTIVE 0x0049
#define SPIF_SENDCHANGE 0x0002

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_CXVSCROLL 2

#define WS_OVERLAPPEDWINDOW 0x00CF0000L
#define WS_POPUP 0x80000000L
#define WS_VISIBLE 0x10000000L
#define WS_CAPTION 0x00C00000L
#define WS_SYSMENU 0x00080000L
#define WS_EX_TOPMOST 0x00000008L
#define SW_SHOW 5
#define SW_HIDE 0
#define SW_RESTORE 9
#define SWP_NOMOVE 0x0002
#define SWP_NOSIZE 0x0001
#define SWP_NOZORDER 0x0004
#define SWP_SHOWWINDOW 0x0040
#define GWL_STYLE (-16)
#define GWL_EXSTYLE (-20)
#define HWND_TOP ((HWND)0)
#define CS_OWNDC 0x0020
#define CS_DBLCLKS 0x0008
#define IDC_ARROW ((LPCWSTR)(UINT_PTR)32512)
#define MB_OK 0x00000000L
#define MB_ICONERROR 0x00000010L
#define MB_ICONWARNING 0x00000030L
#define MB_SYSTEMMODAL 0x00001000L
#define MB_SETFOREGROUND 0x00010000L
#define IDOK 1
#define IDCANCEL 2

#define WM_NULL 0x0000
#define WM_DESTROY 0x0002
#define WM_QUIT 0x0012
#define WM_CLOSE 0x0010
#define WM_SIZE 0x0005
#define WM_ACTIVATEAPP 0x001C
#define WM_SETCURSOR 0x0020
#define WM_PAINT 0x000F
#define WM_ERASEBKGND 0x0014
#define WM_DISPLAYCHANGE 0x007E
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_INPUT 0x00FF
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_MOUSEMOVE 0x0200
#define SC_SCREENSAVE 0xF140
#define SC_MONITORPOWER 0xF170
#define SW_SHOWNORMAL 1
#define SIZE_MAXIMIZED 2
#define SIZE_RESTORED 0
#define HWND_TOPMOST ((HWND)(LONG_PTR)-1)
#define HWND_NOTOPMOST ((HWND)(LONG_PTR)-2)
#define MAKEINTRESOURCEW(i) ((LPWSTR)(ULONG_PTR)(WORD)(i))
#define MAKEINTRESOURCEA(i) ((LPSTR)(ULONG_PTR)(WORD)(i))
#define LOWORD(l) ((WORD)((DWORD_PTR)(l)&0xFFFF))
#define HIWORD(l) ((WORD)(((DWORD_PTR)(l)>>16)&0xFFFF))
#define ERROR_ALREADY_EXISTS 183L
#define BM_SETCHECK 0x00F1
#define BM_GETCHECK 0x00F0
#define BST_UNCHECKED 0x0000
#define BST_CHECKED 0x0001

#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_ESCAPE 0x1B
#define VK_RETURN 0x0D
#define VK_SPACE 0x20
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_F1 0x70
#define VK_F12 0x7B
#define VK_SNAPSHOT 0x2C

#define DIB_RGB_COLORS 0
#define BI_RGB 0L
#define BI_BITFIELDS 3L
#define TRANSPARENT 1
#define OPAQUE 2

// display settings / GDI device queries
#define ENUM_CURRENT_SETTINGS ((DWORD)-1)
#define ENUM_REGISTRY_SETTINGS ((DWORD)-2)
#define HORZRES 8
#define VERTRES 10
#define VREFRESH 116
#define SHIFTJIS_CHARSET 128
#define DEFAULT_CHARSET 1
#define BLACK_BRUSH 4
#define WHITE_BRUSH 0
#define NULL_BRUSH 5

typedef struct _devicemodeW {
    WCHAR dmDeviceName[32];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    union {
        struct {
            SHORT dmOrientation, dmPaperSize, dmPaperLength, dmPaperWidth,
                dmScale, dmCopies, dmDefaultSource, dmPrintQuality;
        };
        struct {
            POINT dmPosition;
            DWORD dmDisplayOrientation;
            DWORD dmDisplayFixedOutput;
        };
    };
    SHORT dmColor, dmDuplex, dmYResolution, dmTTOption, dmCollate;
    WCHAR dmFormName[32];
    WORD dmLogPixels;
    DWORD dmBitsPerPel;
    DWORD dmPelsWidth;
    DWORD dmPelsHeight;
    union {
        DWORD dmDisplayFlags;
        DWORD dmNup;
    };
    DWORD dmDisplayFrequency;
    DWORD dmICMMethod;
    DWORD dmICMIntent;
    DWORD dmMediaType;
    DWORD dmDitherType;
    DWORD dmReserved1;
    DWORD dmReserved2;
    DWORD dmPanningWidth;
    DWORD dmPanningHeight;
} DEVMODEW, *LPDEVMODEW;

typedef struct tagSTARTUPINFOW {
    DWORD cb;
    LPWSTR lpReserved;
    LPWSTR lpDesktop;
    LPWSTR lpTitle;
    DWORD dwX, dwY, dwXSize, dwYSize, dwXCountChars, dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    BYTE* lpReserved2;
    HANDLE hStdInput, hStdOutput, hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

// raw input (settings dialog)
#define RIDEV_INPUTSINK 0x00000100
#define RID_INPUT 0x10000003
#define RID_HEADER 0x10000005
#define RIM_TYPEMOUSE 0
#define RIM_TYPEKEYBOARD 1
#define RIM_TYPEHID 2
typedef struct tagRAWINPUTDEVICE {
    USHORT usUsagePage;
    USHORT usUsage;
    DWORD dwFlags;
    HWND hwndTarget;
} RAWINPUTDEVICE, *PRAWINPUTDEVICE;
typedef struct tagRAWINPUTHEADER {
    DWORD dwType;
    DWORD dwSize;
    HANDLE hDevice;
    WPARAM wParam;
} RAWINPUTHEADER, *PRAWINPUTHEADER;
typedef struct tagRAWMOUSE {
    USHORT usFlags;
    union {
        ULONG ulButtons;
        struct { USHORT usButtonFlags; USHORT usButtonData; };
    };
    ULONG ulRawButtons;
    LONG lLastX, lLastY;
    ULONG ulExtraInformation;
} RAWMOUSE;
typedef struct tagRAWKEYBOARD {
    USHORT MakeCode, Flags, Reserved, VKey;
    UINT Message;
    ULONG ExtraInformation;
} RAWKEYBOARD;
typedef struct tagRAWHID {
    DWORD dwSizeHid;
    DWORD dwCount;
    BYTE bRawData[1];
} RAWHID;
typedef struct tagRAWINPUT {
    RAWINPUTHEADER header;
    union {
        RAWMOUSE mouse;
        RAWKEYBOARD keyboard;
        RAWHID hid;
    } data;
} RAWINPUT, *PRAWINPUT;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    struct { DWORD dwLowDateTime, dwHighDateTime; } ftCreationTime, ftLastAccessTime, ftLastWriteTime;
    DWORD nFileSizeHigh, nFileSizeLow;
    DWORD dwReserved0, dwReserved1;
    WCHAR cFileName[MAX_PATH];
    WCHAR cAlternateFileName[14];
} WIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

#define DLL_PROCESS_ATTACH 1

// --- function declarations (link-time provided, Phase 2/3) ------------------
extern "C" {
// timing / threads / sync
BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER*);
BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER*);
void WINAPI Sleep(DWORD);
HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, DWORD*);
DWORD WINAPI WaitForSingleObject(HANDLE, DWORD);
DWORD WINAPI MsgWaitForMultipleObjects(DWORD, const HANDLE*, BOOL, DWORD, DWORD);
HANDLE WINAPI CreateEventW(LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCWSTR);
BOOL WINAPI SetEvent(HANDLE);
BOOL WINAPI ResetEvent(HANDLE);
BOOL WINAPI CloseHandle(HANDLE);
DWORD WINAPI GetCurrentThreadId();
void WINAPI ExitProcess(UINT);
PVOID WINAPI AddVectoredExceptionHandler(ULONG, PVECTORED_EXCEPTION_HANDLER);
ULONG WINAPI RemoveVectoredExceptionHandler(PVOID);

// modules / memory
HMODULE WINAPI LoadLibraryW(LPCWSTR);
HMODULE WINAPI GetModuleHandleW(LPCWSTR);
FARPROC WINAPI GetProcAddress(HMODULE, LPCSTR);
BOOL WINAPI FreeLibrary(HMODULE);
DWORD WINAPI GetModuleFileNameW(HMODULE, LPWSTR, DWORD);
DWORD WINAPI GetEnvironmentVariableW(LPCWSTR, LPWSTR, DWORD);
HANDLE WINAPI GetProcessHeap();
LPVOID WINAPI HeapAlloc(HANDLE, DWORD, SIZE_T);
BOOL WINAPI HeapFree(HANDLE, DWORD, LPVOID);
HLOCAL WINAPI LocalFree(HLOCAL);
DWORD WINAPI GetLastError();
void WINAPI SetLastError(DWORD);
UINT WINAPI SetErrorMode(UINT);
DWORD WINAPI FormatMessageW(DWORD, LPCVOID, DWORD, DWORD, LPWSTR, DWORD, va_list*);

// files
HANDLE WINAPI CreateFileW(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
BOOL WINAPI ReadFile(HANDLE, LPVOID, DWORD, DWORD*, void*);
BOOL WINAPI WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, void*);
DWORD WINAPI SetFilePointer(HANDLE, LONG, LONG*, DWORD);
DWORD WINAPI GetFileSize(HANDLE, DWORD*);

// windowing / messaging
HWND WINAPI CreateWindowExW(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
BOOL WINAPI DestroyWindow(HWND);
BOOL WINAPI ShowWindow(HWND, int);
BOOL WINAPI SetForegroundWindow(HWND);
BOOL WINAPI SetWindowPos(HWND, HWND, int, int, int, int, UINT);
LONG WINAPI SetWindowLongW(HWND, int, LONG);
LONG WINAPI GetWindowLongW(HWND, int);
BOOL WINAPI GetWindowRect(HWND, LPRECT);
BOOL WINAPI GetClientRect(HWND, LPRECT);
BOOL WINAPI MoveWindow(HWND, int, int, int, int, BOOL);
BOOL WINAPI SetWindowTextW(HWND, LPCWSTR);
int WINAPI MessageBoxW(HWND, LPCWSTR, LPCWSTR, UINT);
UINT_PTR WINAPI SetTimer(HWND, UINT_PTR, UINT, void*);
BOOL WINAPI KillTimer(HWND, UINT_PTR);
BOOL WINAPI PeekMessageW(LPMSG, HWND, UINT, UINT, UINT);
BOOL WINAPI GetMessageW(LPMSG, HWND, UINT, UINT);
BOOL WINAPI TranslateMessage(const MSG*);
LRESULT WINAPI DispatchMessageW(const MSG*);
void WINAPI PostQuitMessage(int);
BOOL WINAPI PostThreadMessageW(DWORD, UINT, WPARAM, LPARAM);
LRESULT WINAPI DefWindowProcW(HWND, UINT, WPARAM, LPARAM);
WORD WINAPI RegisterClassW(const WNDCLASSW*);
BOOL WINAPI UnregisterClassW(LPCWSTR, HINSTANCE);
HCURSOR WINAPI SetCursor(HCURSOR);
HCURSOR WINAPI LoadCursorW(HINSTANCE, LPCWSTR);
int WINAPI ShowCursor(BOOL);
int WINAPI GetSystemMetrics(int);
BOOL WINAPI SystemParametersInfoW(UINT, UINT, PVOID, UINT);
BOOL WINAPI GetKeyboardState(BYTE*);
BOOL WINAPI SetKeyboardState(BYTE*);
SHORT WINAPI GetAsyncKeyState(int);
SHORT WINAPI GetKeyState(int);
BOOL WINAPI ClientToScreen(HWND, LPPOINT);
BOOL WINAPI ScreenToClient(HWND, LPPOINT);
BOOL WINAPI AdjustWindowRect(LPRECT, DWORD, BOOL);
BOOL WINAPI AdjustWindowRectEx(LPRECT, DWORD, BOOL, DWORD);
BOOL WINAPI IsWindow(HWND);
BOOL WINAPI WINNLSEnableIME(HWND, BOOL);
BOOL WINAPI InvalidateRect(HWND, const RECT*, BOOL);
BOOL WINAPI UpdateWindow(HWND);
HANDLE WINAPI CreateMutexW(LPSECURITY_ATTRIBUTES, BOOL, LPCWSTR);
DWORD WINAPI GetConsoleTitleW(LPWSTR, DWORD);
void WINAPI GetStartupInfoW(LPSTARTUPINFOW);
DWORD WINAPI GetUserDefaultLCID();
LRESULT WINAPI SendMessageW(HWND, UINT, WPARAM, LPARAM);
HWND WINAPI GetDlgItem(HWND, int);
UINT WINAPI IsDlgButtonChecked(HWND, int);
BOOL WINAPI EnableWindow(HWND, BOOL);
HWND WINAPI CreateDialogParamW(HINSTANCE, LPCWSTR, HWND, INT_PTR(CALLBACK*)(HWND, UINT, WPARAM, LPARAM), LPARAM);
BOOL WINAPI EnumDisplaySettingsW(LPCWSTR, DWORD, DEVMODEW*);
HGDIOBJ WINAPI GetStockObject(int);
HBRUSH WINAPI CreateSolidBrush(COLORREF);
int WINAPI FillRect(HDC, const RECT*, HBRUSH);
HDC WINAPI BeginPaint(HWND, LPPAINTSTRUCT);
BOOL WINAPI EndPaint(HWND, const PAINTSTRUCT*);
BOOL WINAPI CheckRadioButton(HWND, int, int, int);
BOOL WINAPI IsDialogMessageW(HWND, LPMSG);
UINT WINAPI GetRawInputData(HRAWINPUT, UINT, LPVOID, UINT*, UINT);
BOOL WINAPI RegisterRawInputDevices(const RAWINPUTDEVICE*, UINT, UINT);

// GDI (text renderer)
HDC WINAPI GetDC(HWND);
int WINAPI ReleaseDC(HWND, HDC);
int WINAPI GetDeviceCaps(HDC, int);
HDC WINAPI CreateCompatibleDC(HDC);
BOOL WINAPI DeleteDC(HDC);
HBITMAP WINAPI CreateDIBSection(HDC, const BITMAPINFO*, UINT, void**, HANDLE, DWORD);
HBITMAP WINAPI CreateCompatibleBitmap(HDC, int, int);
HGDIOBJ WINAPI SelectObject(HDC, HGDIOBJ);
BOOL WINAPI DeleteObject(HGDIOBJ);
int WINAPI SetBkMode(HDC, int);
COLORREF WINAPI SetTextColor(HDC, COLORREF);
HFONT WINAPI CreateFontIndirectW(const LOGFONTW*);
HFONT WINAPI CreateFontW(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCWSTR);
BOOL WINAPI TextOutW(HDC, int, int, LPCWSTR, int);
BOOL WINAPI GetTextExtentPoint32W(HDC, LPCWSTR, int, LPSIZE);
typedef int(CALLBACK* FONTENUMPROCW)(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM);
int WINAPI EnumFontFamiliesExW(HDC, LPLOGFONTW, FONTENUMPROCW, LPARAM, DWORD);
BOOL WINAPI GetTextMetricsW(HDC, LPTEXTMETRICW);

// strings / codepages
int WINAPI MultiByteToWideChar(UINT, DWORD, LPCSTR, int, LPWSTR, int);
int WINAPI WideCharToMultiByte(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, BOOL*);

// file search
HANDLE WINAPI FindFirstFileW(LPCWSTR, LPWIN32_FIND_DATAW);
BOOL WINAPI FindNextFileW(HANDLE, LPWIN32_FIND_DATAW);
BOOL WINAPI FindClose(HANDLE);

// misc
void WINAPI GetSystemInfo(LPSYSTEM_INFO);
BOOL WINAPI IsProcessorFeaturePresent(DWORD);
void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION);
void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION);
void WINAPI EnterCriticalSection(LPCRITICAL_SECTION);
void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION);
}

// GUIDs actually referenced (definitions live in the wasm host layer / Phase 3)
extern "C" {
extern const GUID CLSID_WbemLocator;
extern const GUID IID_IWbemLocator;
}
#ifndef GUID_NULL
#define GUID_NULL GUID{}
#endif

// WMI client interfaces (input_win32.cpp XInput detection query)
struct IWbemClassObject : IUnknown {
    virtual HRESULT WINAPI Get(LPCWSTR, LONG, VARIANT*, LONG*, LONG*) = 0;
    virtual HRESULT WINAPI Put(LPCWSTR, LONG, VARIANT*, LONG) = 0;
};
struct IEnumWbemClassObject : IUnknown {
    virtual HRESULT WINAPI Reset() = 0;
    virtual HRESULT WINAPI Next(LONG, ULONG, IWbemClassObject**, ULONG*) = 0;
    virtual HRESULT WINAPI Clone(IEnumWbemClassObject**) = 0;
    virtual HRESULT WINAPI Skip(LONG, ULONG) = 0;
};
struct IWbemServices : IUnknown {
    virtual HRESULT WINAPI CreateInstanceEnum(BSTR, LONG, void*, IEnumWbemClassObject**) = 0;
};
struct IWbemLocator : IUnknown {
    virtual HRESULT WINAPI ConnectServer(BSTR, BSTR, BSTR, BSTR, LONG, BSTR, void*, IWbemServices**) = 0;
};
