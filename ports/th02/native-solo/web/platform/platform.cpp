#include "windows.h"
#include "bcrypt.h"
#include "sha256.hpp"
#include "mmsystem.h"
#include <emscripten.h>
#include <algorithm>
#include <array>
#include <deque>
#include <string>
#include <tuple>
#include <vector>

EM_JS(void, host_present,(const std::uint32_t *p,int w,int h),{Module.host.present(p,w,h);});
EM_JS(void, host_text,(std::uint32_t *p,int w,int h,int x,int y,int l,int t,int r,int b,
                      const wchar_t *s,int n,const wchar_t *face,int size,int bold,
                      unsigned color,unsigned flags,int opaque,unsigned background),{
    Module.host.text(p,w,h,x,y,l,t,r,b,s,n,face,size,bold,color,flags,opaque,background);
});
EM_JS(void, host_title,(const wchar_t *s),{Module.host.title(s);});
EM_JS(void, host_fullscreen,(int value),{Module.host.fullscreen(value);});
EM_JS(void, host_replay,(),{Module.host.openReplay();});
EM_JS(void, host_exit,(),{Module.host.exit();});
EM_JS(int, host_decode,(const char *s,int n,wchar_t *out,int count),{return Module.host.decode(s,n,out,count);});
EM_JS(int, host_has_cell,(wchar_t cp,int face),{return Module.host.hasCell(cp,face);});
EM_JS(void, host_paint_cells,(std::uint32_t *p,int w,int h,const wchar_t *s,unsigned n,int x,int y,unsigned color,int face),{
    Module.host.paintCells(p,w,h,s,n,x,y,color,face);
});
EM_JS(void, host_audio_write,(unsigned bus,const WAVEHDR *h,const void *data,unsigned n),{Module.host.audioWrite(bus,h,data,n);});
EM_JS(void, host_audio_pause,(int paused),{Module.host.audioPause(paused);});
EM_JS(void, host_audio_reset,(unsigned bus),{Module.host.audioReset(bus);});
EM_JS(void, host_akyu_track,(int track),{Module.host.akyuTrack(track);});
EM_JS(void, host_akyu_gain,(float gain),{Module.host.akyuGain(gain);});
void soew_web_akyu_track(int track){host_akyu_track(track);}
void soew_web_akyu_gain(float gain){host_akyu_gain(gain);}

struct WebGdi {
    enum Kind {Bitmap, Font, Brush} kind;
    int w=0,h=0,size=16;
    bool bold=false;
    std::wstring face;
    std::vector<std::uint32_t> pixels;
};
struct WebDC {
    HGDIOBJ bitmap=nullptr,font=nullptr;
    COLORREF color=RGB(255,255,255),background=0;
    int mode=TRANSPARENT;
};
struct WebWindow {
    WindowProc proc=nullptr;
    void *data=nullptr;
    bool dirty=true,alive=true,active=true;
    int w=640,h=400;
    WebDC surface;
    std::deque<std::tuple<UINT,WPARAM,LPARAM>> messages;
};
struct WebWave {unsigned bus;};
namespace {
WebWindow window;
HWND capture;
std::array<BYTE,256> keys;
double (*frame_callback)(HWND,void*);
void *frame_state;
std::wstring clipboard;
WebGdi stock_bitmap{WebGdi::Bitmap}, stock_font{WebGdi::Font}, black_brush{WebGdi::Brush};
unsigned xrgb(COLORREF c){return (GetRValue(c)<<16)|(GetGValue(c)<<8)|GetBValue(c);}
void paint_text(HDC dc,int x,int y,RECT clip,const wchar_t *text,int n,unsigned flags){
    auto &b=*dc->bitmap;
    auto &f=*dc->font;
    host_text(b.pixels.data(),b.w,b.h,x,y,clip.left,clip.top,clip.right,clip.bottom,
              text,n,f.face.c_str(),f.size,f.bold,xrgb(dc->color),flags,dc->mode==OPAQUE,xrgb(dc->background));
}
}
HWND browser_window(WindowProc proc,void *data){
    window=WebWindow{};
    window.proc=proc;
    CREATESTRUCTW create{data};
    proc(&window,WM_NCCREATE,0,reinterpret_cast<LPARAM>(&create));
    proc(&window,WM_SETFOCUS,0,0);
    return &window;
}
void browser_frame_callback(double (*callback)(HWND,void*),HWND,void *state){frame_callback=callback;frame_state=state;}
void browser_fullscreen(bool yes){host_fullscreen(yes);}
void browser_pick_replay(){host_replay();}
bool browser_glyph_available(std::wstring_view text,int face){return host_has_cell(text.front(),face);}
void browser_paint_cells(std::uint32_t *pixels,int w,int h,std::wstring_view text,int x,int y,unsigned color,int face){
    host_paint_cells(pixels,w,h,text.data(),text.size(),x,y,color,face);
}
BOOL InvalidateRect(HWND hwnd,const RECT*,BOOL){hwnd->dirty=true;return TRUE;}
BOOL UpdateWindow(HWND hwnd){
    if(hwnd->alive&&hwnd->dirty){hwnd->dirty=false;hwnd->proc(hwnd,WM_PAINT,0,0);}
    return TRUE;
}
BOOL DestroyWindow(HWND hwnd){
    if(hwnd->alive){hwnd->alive=false;hwnd->proc(hwnd,WM_DESTROY,0,0);host_exit();}
    return TRUE;
}
HWND GetForegroundWindow(){return window.active?&window:nullptr;}
HWND SetFocus(HWND hwnd){window.active=true;return hwnd;}
BOOL SetForegroundWindow(HWND hwnd){SetFocus(hwnd);return TRUE;}
BOOL GetClientRect(HWND hwnd,RECT *r){*r={0,0,hwnd->w,hwnd->h};return TRUE;}
BOOL GetWindowRect(HWND hwnd,RECT *r){return GetClientRect(hwnd,r);}
BOOL SetWindowPos(HWND,HWND,int,int,int,int,UINT){return TRUE;}
LONG_PTR GetWindowLongPtrW(HWND hwnd,int){return reinterpret_cast<LONG_PTR>(hwnd->data);}
LONG_PTR SetWindowLongPtrW(HWND hwnd,int,LONG_PTR value){auto prev=GetWindowLongPtrW(hwnd,0);hwnd->data=reinterpret_cast<void*>(value);return prev;}
BOOL SetWindowTextW(HWND,const wchar_t *title){host_title(title);return TRUE;}
BOOL IsWindowEnabled(HWND hwnd){return hwnd->alive;}
HWND SetCapture(HWND hwnd){auto prev=capture;capture=hwnd;return prev;}
HWND GetCapture(){return capture;}
BOOL ReleaseCapture(){capture=nullptr;return TRUE;}
short GetKeyState(int key){return keys.at(key)?-32768:0;}
short GetAsyncKeyState(int key){return GetKeyState(key);}
BOOL GetKeyboardState(BYTE *out){std::copy(keys.begin(),keys.end(),out);return TRUE;}
LRESULT DefWindowProcW(HWND hwnd,UINT msg,WPARAM,LPARAM){if(msg==WM_CLOSE)DestroyWindow(hwnd);return 0;}
LRESULT SendMessageW(HWND hwnd,UINT msg,WPARAM w,LPARAM l){return hwnd->proc(hwnd,msg,w,l);}
BOOL PostMessageW(HWND hwnd,UINT msg,WPARAM w,LPARAM l){hwnd->messages.emplace_back(msg,w,l);return TRUE;}
void PostQuitMessage(int){}
std::uintptr_t SetTimer(HWND,std::uintptr_t id,UINT,void*){return id;}
BOOL KillTimer(HWND,std::uintptr_t){return TRUE;}
HDC BeginPaint(HWND hwnd,PAINTSTRUCT *ps){return ps->hdc=&hwnd->surface;}
BOOL EndPaint(HWND,const PAINTSTRUCT*){return TRUE;}
HDC GetDC(HWND hwnd){return &hwnd->surface;}
int ReleaseDC(HWND,HDC){return 1;}
COLORREF GetPixel(HDC dc,int x,int y){const auto p=dc->bitmap->pixels[y*dc->bitmap->w+x];return RGB((p>>16)&255,(p>>8)&255,p&255);}
HANDLE CreateEventW(void*,BOOL,BOOL,const wchar_t*){return &window;}
BOOL CloseHandle(HANDLE){return TRUE;}
DWORD WaitForSingleObject(HANDLE,DWORD){return 0;}
BOOL OpenClipboard(HWND){return !clipboard.empty();}
BOOL CloseClipboard(){clipboard.clear();return TRUE;}
HANDLE GetClipboardData(UINT){return &clipboard;}
std::size_t GlobalSize(HANDLE){return (clipboard.size()+1)*sizeof(wchar_t);}
void *GlobalLock(HANDLE){return clipboard.data();}
BOOL GlobalUnlock(HANDLE){return TRUE;}
int MultiByteToWideChar(UINT,DWORD,const char *s,int n,wchar_t *out,int count){return host_decode(s,n,out,count);}
int BCryptHash(BCRYPT_ALG_HANDLE,PUCHAR,ULONG,PUCHAR p,ULONG n,PUCHAR out,ULONG){
    const auto digest=soew::web::sha256({p,n});
    std::copy(digest.begin(),digest.end(),out);
    return 0;
}

HDC CreateCompatibleDC(HDC){return new WebDC{&stock_bitmap,&stock_font};}
HBITMAP CreateCompatibleBitmap(HDC,int w,int h){
    auto *b=new WebGdi{WebGdi::Bitmap,w,h};b->pixels.resize(std::size_t(w)*h);return b;
}
HBITMAP CreateDIBSection(HDC dc,const BITMAPINFO *info,UINT,void **bits,HANDLE,DWORD){
    auto b=CreateCompatibleBitmap(dc,info->bmiHeader.biWidth,-info->bmiHeader.biHeight);
    *bits=b->pixels.data();return b;
}
HFONT CreateFontW(int h,int,int,int,int weight,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,const wchar_t *face){
    return new WebGdi{WebGdi::Font,0,0,-h,weight==FW_BOLD,face};
}
HGDIOBJ SelectObject(HDC dc,HGDIOBJ obj){
    auto &selected=obj->kind==WebGdi::Bitmap?dc->bitmap:dc->font;
    auto prev=selected;selected=obj;return prev;
}
HGDIOBJ GetStockObject(int type){return type==BLACK_BRUSH?&black_brush:&stock_font;}
BOOL DeleteObject(HGDIOBJ obj){delete obj;return TRUE;}
BOOL DeleteDC(HDC dc){delete dc;return TRUE;}
int SetBkMode(HDC dc,int mode){auto prev=dc->mode;dc->mode=mode;return prev;}
COLORREF SetBkColor(HDC dc,COLORREF color){auto prev=dc->background;dc->background=color;return prev;}
COLORREF SetTextColor(HDC dc,COLORREF color){auto prev=dc->color;dc->color=color;return prev;}
int SetStretchBltMode(HDC,int){return COLORONCOLOR;}
int FillRect(HDC dc,const RECT *r,HGDIOBJ){
    auto &b=*dc->bitmap;
    for(int y=r->top;y<r->bottom;++y)std::fill_n(b.pixels.begin()+y*b.w+r->left,r->right-r->left,0);
    return 1;
}
int StretchDIBits(HDC dc,int x,int y,int w,int h,int,int,int sw,int sh,const void *raw,const BITMAPINFO*,UINT,DWORD){
    auto &b=*dc->bitmap;
    auto *read=static_cast<const std::uint32_t*>(raw);
    for(int row=0;row<h;++row){
        auto *out=b.pixels.data()+(y+row)*b.w+x;
        auto *in=read+row*sh/h*sw;
        if(w==sw)std::copy_n(in,w,out);
        else for(int col=0;col<w;++col)out[col]=in[col*sw/w];
    }
    return h;
}
BOOL BitBlt(HDC,int,int,int w,int h,HDC source,int,int,DWORD){
    host_present(source->bitmap->pixels.data(),w,h);return TRUE;
}
BOOL ExtTextOutW(HDC dc,int x,int y,UINT,const RECT *clip,const wchar_t *s,UINT n,const int*){
    paint_text(dc,x,y,*clip,s,n,0);return TRUE;
}
int DrawTextW(HDC dc,const wchar_t *s,int n,RECT *bounds,UINT flags){
    paint_text(dc,bounds->left,bounds->top,*bounds,s,n,flags);return dc->font->size;
}
MMRESULT waveOutOpen(HWAVEOUT *out,UINT device,const WAVEFORMATEX*,DWORD_PTR,DWORD_PTR,DWORD){*out=new WebWave{device==WAVE_MAPPER?0u:device};return 0;}
MMRESULT waveOutPrepareHeader(HWAVEOUT,WAVEHDR*,UINT){return 0;}
MMRESULT waveOutUnprepareHeader(HWAVEOUT,WAVEHDR*,UINT){return 0;}
MMRESULT waveOutWrite(HWAVEOUT wave,WAVEHDR *header,UINT){header->dwFlags=WHDR_INQUEUE;host_audio_write(wave->bus,header,header->lpData,header->dwBufferLength);return 0;}
MMRESULT waveOutPause(HWAVEOUT wave){if(!wave->bus)host_audio_pause(1);return 0;}
MMRESULT waveOutRestart(HWAVEOUT wave){if(!wave->bus)host_audio_pause(0);return 0;}
MMRESULT waveOutReset(HWAVEOUT wave){host_audio_reset(wave->bus);return 0;}
MMRESULT waveOutClose(HWAVEOUT wave){delete wave;return 0;}

extern "C" {
EMSCRIPTEN_KEEPALIVE double soew_host_frame(){
    if(!window.alive||!frame_callback)return 0;
    while(window.alive&&!window.messages.empty()){
        const auto [msg,w,l]=window.messages.front();window.messages.pop_front();
        SendMessageW(&window,msg,w,l);
    }
    const auto delay=window.alive?frame_callback(&window,frame_state):0;
    UpdateWindow(&window);
    return delay;
}
EMSCRIPTEN_KEEPALIVE void soew_host_key(unsigned key,bool down,bool repeat,unsigned scalar){
    if(!window.alive)return;
    keys.at(key)=down?128:0;
    keys[VK_SHIFT]=keys[VK_LSHIFT]|keys[VK_RSHIFT];
    keys[VK_CONTROL]=keys[VK_LCONTROL]|keys[VK_RCONTROL];
    keys[VK_MENU]=keys[VK_LMENU]|keys[VK_RMENU];
    SendMessageW(&window,down?WM_KEYDOWN:WM_KEYUP,key,repeat?(1u<<30):0);
    if(down&&scalar)SendMessageW(&window,WM_CHAR,scalar,0);
}
EMSCRIPTEN_KEEPALIVE void soew_host_focus(bool active){
    if(!window.alive||active==window.active)return;
    window.active=active;
    if(!active)keys.fill(0);
    SendMessageW(&window,active?WM_SETFOCUS:WM_KILLFOCUS,0,0);
}
EMSCRIPTEN_KEEPALIVE void soew_host_aspect(bool crt){
    const auto h=crt?480:400;
    if(h!=window.h){window.h=h;InvalidateRect(&window,nullptr,FALSE);}
}
EMSCRIPTEN_KEEPALIVE void soew_host_pointer(unsigned msg,int x,int y,int extra){
    if(window.alive)SendMessageW(&window,msg,MAKELPARAM(0,extra),MAKELPARAM(x,y));
}
EMSCRIPTEN_KEEPALIVE void soew_host_paste(const wchar_t *text){
    clipboard=text;
    soew_host_key(VK_LCONTROL,true,false,0);
    soew_host_key('V',true,false,0);
    soew_host_key('V',false,false,0);
    soew_host_key(VK_LCONTROL,false,false,0);
    clipboard.clear();
}
}
