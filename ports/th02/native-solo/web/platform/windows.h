#pragma once
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <string_view>
#include <emscripten.h>

using BOOL=int; using BYTE=unsigned char; using WORD=std::uint16_t;
using DWORD=std::uint32_t; using ULONG=std::uint32_t; using UINT=unsigned;
using ULONGLONG=std::uint64_t;
using LONG_PTR=std::intptr_t; using DWORD_PTR=std::uintptr_t;
using WPARAM=std::uintptr_t; using LPARAM=std::intptr_t; using LRESULT=std::intptr_t;
using HANDLE=void*; using HINSTANCE=void*; using HICON=void*;
using COLORREF=DWORD; using HWND=struct WebWindow*;
using HDC=struct WebDC*; using HGDIOBJ=struct WebGdi*;
using HBITMAP=HGDIOBJ; using HFONT=HGDIOBJ; using HBRUSH=HGDIOBJ;
using LPSTR=char*; using PUCHAR=unsigned char*;
#define CALLBACK
#define TRUE 1
#define FALSE 0

struct RECT {int left,top,right,bottom;};
struct POINT {int x,y;};
struct WINDOWPLACEMENT {UINT length;};
struct CREATESTRUCTW {void *lpCreateParams;};
struct PAINTSTRUCT {HDC hdc;};
struct BITMAPINFOHEADER {
    DWORD biSize; int biWidth,biHeight; WORD biPlanes,biBitCount;
    DWORD biCompression,biSizeImage; int biXPelsPerMeter,biYPelsPerMeter;
    DWORD biClrUsed,biClrImportant;
};
struct BITMAPINFO {BITMAPINFOHEADER bmiHeader;};
#pragma pack(push,2)
struct BITMAPFILEHEADER {WORD bfType; DWORD bfSize; WORD bfReserved1,bfReserved2; DWORD bfOffBits;};
#pragma pack(pop)
using WindowProc=LRESULT (*)(HWND, UINT, WPARAM, LPARAM);

enum : UINT {
    WM_DESTROY=2,WM_SIZE=5,WM_ACTIVATE=6,WM_SETFOCUS=7,WM_KILLFOCUS=8,WM_PAINT=15,
    WM_CLOSE=16,WM_ERASEBKGND=20,WM_NCCREATE=129,WM_KEYDOWN=256,WM_KEYUP=257,WM_CHAR=258,
    WM_DEADCHAR=259,WM_SYSKEYDOWN=260,WM_SYSKEYUP=261,WM_TIMER=275,WM_ENTERMENULOOP=529,
    WM_EXITMENULOOP=530,WM_MOUSEMOVE=512,WM_LBUTTONDOWN=513,WM_LBUTTONUP=514,WM_RBUTTONDOWN=516,
    WM_MBUTTONDOWN=519,WM_MOUSEWHEEL=522,WM_XBUTTONDOWN=523,WM_CAPTURECHANGED=533,
    WM_POWERBROADCAST=536,WM_ENTERSIZEMOVE=561,WM_EXITSIZEMOVE=562,WM_DPICHANGED=736,WM_CANCELMODE=31,
};
inline constexpr int
    VK_BACK=8,VK_TAB=9,VK_RETURN=13,VK_SHIFT=16,VK_CONTROL=17,VK_MENU=18,VK_ESCAPE=27,
    VK_SPACE=32,VK_PRIOR=33,VK_NEXT=34,VK_END=35,VK_HOME=36,VK_LEFT=37,VK_UP=38,
    VK_RIGHT=39,VK_DOWN=40,VK_SNAPSHOT=44,VK_DELETE=46,VK_LWIN=91,VK_RWIN=92,VK_APPS=93,
    VK_NUMPAD0=96,VK_NUMPAD1=97,VK_NUMPAD2=98,VK_NUMPAD3=99,VK_NUMPAD4=100,VK_NUMPAD6=102,
    VK_NUMPAD7=103,VK_NUMPAD8=104,VK_NUMPAD9=105,VK_SUBTRACT=109,VK_F1=112,VK_F2=113,
    VK_F3=114,VK_F4=115,VK_F5=116,VK_F7=118,VK_F8=119,VK_F10=121,VK_F12=123,
    VK_LSHIFT=160,VK_RSHIFT=161,VK_LCONTROL=162,VK_RCONTROL=163,VK_LMENU=164,VK_RMENU=165,
    VK_OEM_MINUS=189,VK_OEM_3=192,VK_OEM_4=219,VK_OEM_6=221;
enum : UINT {
    SIZE_MINIMIZED=1,WA_INACTIVE=0,PBT_APMSUSPEND=4,PBT_APMRESUMESUSPEND=7,PBT_APMRESUMEAUTOMATIC=18,
    CF_UNICODETEXT=13,MB_ERR_INVALID_CHARS=8,BI_RGB=0,DIB_RGB_COLORS=0,TRANSPARENT=1,OPAQUE=2,
    FW_NORMAL=400,FW_BOLD=700,SHIFTJIS_CHARSET=128,GB2312_CHARSET=134,CHINESEBIG5_CHARSET=136,
    OUT_DEFAULT_PRECIS=0,CLIP_DEFAULT_PRECIS=0,NONANTIALIASED_QUALITY=3,DEFAULT_PITCH=0,FIXED_PITCH=1,
    DT_LEFT=0,DT_RIGHT=2,DT_VCENTER=4,DT_BOTTOM=8,DT_SINGLELINE=32,DT_NOPREFIX=2048,
    DT_END_ELLIPSIS=32768,ETO_CLIPPED=4,SWP_NOZORDER=4,SWP_NOACTIVATE=16,WAIT_FAILED=0xffffffffu,
    COLORONCOLOR=3,BLACK_BRUSH=4,DEFAULT_GUI_FONT=17,SRCCOPY=0xcc0020u,GDI_ERROR=0xffffffffu,
};
inline constexpr int GWLP_USERDATA=-21;
inline HWND const HWND_TOPMOST=reinterpret_cast<HWND>(-1);
inline HGDIOBJ const HGDI_ERROR=reinterpret_cast<HGDIOBJ>(-1);
constexpr unsigned LOWORD(std::uintptr_t x){return x&65535;}
constexpr unsigned HIWORD(std::uintptr_t x){return (x>>16)&65535;}
constexpr COLORREF RGB(unsigned r,unsigned g,unsigned b){return r|(g<<8)|(b<<16);}
constexpr DWORD GetRValue(DWORD x){return x&255;}
constexpr DWORD GetGValue(DWORD x){return (x>>8)&255;}
constexpr DWORD GetBValue(DWORD x){return (x>>16)&255;}
constexpr LPARAM MAKELPARAM(unsigned a,unsigned b){return (a&65535)|((b&65535)<<16);}

HWND browser_window(WindowProc,void*);
void browser_frame_callback(double (*callback)(HWND,void*), HWND, void*);
void browser_fullscreen(bool);
void browser_pick_replay();
void browser_paint_cells(std::uint32_t*,int,int,std::wstring_view,int,int,unsigned);
bool browser_glyph_available(std::wstring_view);
extern "C" void soew_host_aspect(bool);
BOOL InvalidateRect(HWND,const RECT*,BOOL);
BOOL UpdateWindow(HWND);
BOOL DestroyWindow(HWND);
HWND GetForegroundWindow();
HWND SetFocus(HWND);
BOOL SetForegroundWindow(HWND);
BOOL GetClientRect(HWND,RECT*);
BOOL GetWindowRect(HWND,RECT*);
BOOL SetWindowPos(HWND,HWND,int,int,int,int,UINT);
LONG_PTR GetWindowLongPtrW(HWND,int);
LONG_PTR SetWindowLongPtrW(HWND,int,LONG_PTR);
BOOL SetWindowTextW(HWND,const wchar_t*);
BOOL IsWindowEnabled(HWND);
HWND SetCapture(HWND);
HWND GetCapture();
BOOL ReleaseCapture();
short GetKeyState(int);
short GetAsyncKeyState(int);
BOOL GetKeyboardState(BYTE*);
LRESULT DefWindowProcW(HWND,UINT,WPARAM,LPARAM);
LRESULT SendMessageW(HWND,UINT,WPARAM,LPARAM);
BOOL PostMessageW(HWND,UINT,WPARAM,LPARAM);
void PostQuitMessage(int);
std::uintptr_t SetTimer(HWND,std::uintptr_t,UINT,void*);
BOOL KillTimer(HWND,std::uintptr_t);
HDC BeginPaint(HWND,PAINTSTRUCT*);
BOOL EndPaint(HWND,const PAINTSTRUCT*);
HDC GetDC(HWND);
int ReleaseDC(HWND,HDC);
COLORREF GetPixel(HDC,int,int);
HANDLE CreateEventW(void*,BOOL,BOOL,const wchar_t*);
BOOL CloseHandle(HANDLE);
DWORD WaitForSingleObject(HANDLE,DWORD);
BOOL OpenClipboard(HWND);
BOOL CloseClipboard();
HANDLE GetClipboardData(UINT);
std::size_t GlobalSize(HANDLE);
void *GlobalLock(HANDLE);
BOOL GlobalUnlock(HANDLE);
int MultiByteToWideChar(UINT,DWORD,const char*,int,wchar_t*,int);

HDC CreateCompatibleDC(HDC);
HBITMAP CreateCompatibleBitmap(HDC,int,int);
HBITMAP CreateDIBSection(HDC,const BITMAPINFO*,UINT,void**,HANDLE,DWORD);
HFONT CreateFontW(int,int,int,int,int,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,const wchar_t*);
HGDIOBJ SelectObject(HDC,HGDIOBJ);
HGDIOBJ GetStockObject(int);
BOOL DeleteObject(HGDIOBJ);
BOOL DeleteDC(HDC);
int SetBkMode(HDC,int);
COLORREF SetBkColor(HDC,COLORREF);
COLORREF SetTextColor(HDC,COLORREF);
int SetStretchBltMode(HDC,int);
int FillRect(HDC,const RECT*,HGDIOBJ);
int StretchDIBits(HDC,int,int,int,int,int,int,int,int,const void*,const BITMAPINFO*,UINT,DWORD);
BOOL BitBlt(HDC,int,int,int,int,HDC,int,int,DWORD);
BOOL ExtTextOutW(HDC,int,int,UINT,const RECT*,const wchar_t*,UINT,const int*);
int DrawTextW(HDC,const wchar_t*,int,RECT*,UINT);
inline BOOL GdiFlush(){return TRUE;}
