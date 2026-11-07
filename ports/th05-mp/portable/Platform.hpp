#pragma once
// Value types and file helpers used by the original game. Browser lifecycle,
// input, graphics and sound have concrete owners in the portable directory.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cstdarg>
#include <unistd.h>
#include <sys/stat.h>
#include <emscripten.h>
using BYTE=unsigned char; using WORD=uint16_t; using DWORD=uint32_t;
using UINT=unsigned; using BOOL=int; using HWND=void*; using HDC=void*;
using HGLRC=void*; using WPARAM=uintptr_t; using LPARAM=intptr_t; using LRESULT=intptr_t;
using TCHAR=char; using HANDLE=void*;
struct RECT { int left,top,right,bottom; };
struct LARGE_INTEGER { int64_t QuadPart; };
#define __forceinline inline
#define CALLBACK
#define WINAPI
#define TRUE 1
#define FALSE 0
#define MAX_PATH 1024
#define _T(x) x
#define WM_KEYDOWN 0x100
#define WM_KEYUP 0x101
#define WM_KILLFOCUS 8
#define VK_BACK 8
#define VK_TAB 9
#define VK_RETURN 13
#define VK_SHIFT 16
#define VK_CONTROL 17
#define VK_ESCAPE 27
#define VK_SPACE 32
#define VK_PRIOR 33
#define VK_NEXT 34
#define VK_END 35
#define VK_HOME 36
#define VK_LEFT 37
#define VK_UP 38
#define VK_RIGHT 39
#define VK_DOWN 40
#define VK_DELETE 46
#define VK_F1 112
#define VK_F2 113
#define VK_F3 114
#define VK_F4 115
#define VK_F5 116
#define VK_F6 117
#define VK_F7 118
#define VK_F8 119
#define VK_F9 120
#define VK_F10 121
#define VK_F11 122
#define VK_F12 123
#define VK_LSHIFT 160
#define VK_RSHIFT 161
#define VK_LCONTROL 162
#define VK_RCONTROL 163
#define INVALID_FILE_ATTRIBUTES UINT32_MAX
#define QueryPerformanceCounter(p) ((p)->QuadPart=int64_t(emscripten_get_now()*1000),1)
#define QueryPerformanceFrequency(p) ((p)->QuadPart=1000000,1)
inline DWORD GetTickCount(){return DWORD(emscripten_get_now());}
inline DWORD GetFileAttributesA(const char* path){struct stat s;return stat(path,&s)?INVALID_FILE_ATTRIBUTES:0;}
inline BOOL PathFileExists(const char* path){return GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES;}
inline BOOL CreateDirectory(const char* path,void*){return mkdir(path,0777)==0;}
template<size_t N,typename... T> int sprintf_s(char (&out)[N],const char* f,T... args){return snprintf(out,N,f,args...);}
template<size_t N> void strcpy_s(char (&out)[N],const char* value){snprintf(out,N,"%s",value);}
template<size_t N> void strcat_s(char (&out)[N],const char* value){strncat(out,value,N-strlen(out)-1);}
#define wsprintf sprintf
#define _stricmp strcasecmp
#define stricmp strcasecmp
#define _isnan std::isnan
int GetPrivateProfileIntA(const char*,const char*,int,const char*);
BOOL WritePrivateProfileStringA(const char*,const char*,const char*,const char*);
namespace th5w { class C2DImage; namespace web {
void TouchMotion(float& x,float& y,float speed);
void SetKey(int key,bool down); void ClearKeys(); void SampleKeys(BYTE* scans); void SampleUIKeys();
bool ReplayViewer();
void BeginUI(); void EndUI(); void InitializeUI(); void ShutdownUI();
C2DImage* WideText(const wchar_t* text);
void MusicTrack(const char* name); void MusicAction(int action,double value=0);
void PlaySound(int index); void ReportError(const char* message);
}}

#define MB_OK 0
#define MB_ICONERROR 0
inline int MessageBoxW(HWND,const wchar_t* text,const wchar_t*,unsigned){th5w::web::ReportError("TH05 scene resource initialization failed");return 0;}
