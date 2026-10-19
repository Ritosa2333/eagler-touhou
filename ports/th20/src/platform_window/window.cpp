#include "platform_window.hpp"
#include "data_constants.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"

namespace th20::source::platform_window {
namespace pe=program_entry;
#ifndef TH_SDL3
bool is_japanese_user_locale() { return GetUserDefaultLCID()==0x411; }
#else
bool is_japanese_user_locale();
#endif

int acquire_single_instance() {
#ifdef TH_SDL3
    return 0; // one runtime per page; the launcher owns process singularity
#else
    single_instance_mutex=CreateMutexW(nullptr,TRUE,L"th20 App");
    if(GetLastError()==ERROR_ALREADY_EXISTS) {
        pe::unrecovered::log_error(pe::log_buffer,data::already_running);
        return -1; // 0x0041c066
    }
    STARTUPINFOW startup{};startup.cb=sizeof(startup);
    wchar_t module_name[262],console_title[262];
    GetModuleFileNameW(nullptr,module_name,260);
    GetConsoleTitleW(console_title,260);
    GetStartupInfoW(&startup);
    return single_instance_mutex?0:-1; // 0x0041c0be..cc
#endif
}

int create_game_window(WindowStatePrefix& w,HINSTANCE instance) {
#ifdef TH_SDL3
    // Preserve the original 640x480 logical coordinate system. The graphics
    // adapter allocates higher resolution GPU surfaces independently.
    auto& c=pe::graphics_state.configuration;
    c.saved_display_mode=3;
    w.display_mode=3;
    pe::graphics_state.presentation.Windowed=true;
    if(c.frame_skip==0 && c.presentation_mode==2) w.flags|=4;
    else w.flags&=~4u;
    // 0x0041cc90 writes exactly three int32 fields.
    w.repeat[0]={15,15,0};w.repeat[1]={12,12,0};w.repeat[2]={12,12,0};w.repeat[3]={8,8,0};
    w.input_latch=0;calculate_layout(w,1);
    w.active=1;w.cursor_latch=0;
    w.window=nullptr;
    pe::graphics_state.window_handle=nullptr;
    (void)instance;
    return 0;
#else
    const wchar_t* title=is_japanese_user_locale()?
        L"東方錦上京　～ Fossilized Wonder. ver 1.00c":L"TH20 - Fossilized Wonder. ver 1.00c";
    WNDCLASSW klass{};
    klass.hbrBackground=static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    klass.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));
    klass.hInstance=instance;klass.lpfnWndProc=window_proc;klass.lpszClassName=L"BASE";
    w.active=1;w.cursor_latch=0;
    RegisterClassW(&klass);
    const auto& c=pe::graphics_state.configuration;
    w.display_mode=c.saved_display_mode;
    pe::graphics_state.presentation.Windowed=w.display_mode>2;
    if(c.frame_skip==0 && c.presentation_mode==2) w.flags|=4;
    else w.flags&=~4u;
    // 0x0041cc90 writes exactly three int32 fields.
    w.repeat[0]={15,15,0};w.repeat[1]={12,12,0};w.repeat[2]={12,12,0};w.repeat[3]={8,8,0};
    w.input_latch=0;calculate_layout(w,1);
    if(!pe::graphics_state.presentation.Windowed) {
        w.window=CreateWindowExW(0,L"BASE",title,0x90000000u,0,0,w.client_width,w.client_height,
            nullptr,nullptr,instance,nullptr);
    } else if(pe::window_state.display_mode==8 || pe::window_state.display_mode==9) {
        w.window=CreateWindowExW(0x40000,L"BASE",title,0x80000000u,0,0,w.client_width,w.client_height,
            nullptr,nullptr,instance,nullptr);
        SetWindowLongW(w.window,GWL_STYLE,0);
        ShowWindow(w.window,SW_SHOW);InvalidateRect(w.window,nullptr,TRUE);UpdateWindow(w.window);
    } else {
        RECT rectangle{0,0,w.client_width,w.client_height};
        AdjustWindowRectEx(&rectangle,0x10cb0000,FALSE,0);
        w.window=CreateWindowExW(0,L"BASE",title,0x10cb0000,c.saved_window_x,c.saved_window_y,
            rectangle.right-rectangle.left,rectangle.bottom-rectangle.top,nullptr,nullptr,instance,nullptr);
        ShowWindow(w.window,SW_SHOW);InvalidateRect(w.window,nullptr,TRUE);UpdateWindow(w.window);
    }
    GetWindowRect(w.window,&pe::graphics_state.window_rectangle);
    pe::graphics_state.window_handle=w.window;
    return w.window?0:1; // original 0x0041d058..67
#endif
}

#ifndef TH_SDL3
LRESULT CALLBACK window_proc(HWND hwnd,UINT message,WPARAM wp,LPARAM lp) {
    auto& w=pe::window_state;auto& c=pe::graphics_state.configuration;
    switch(message) {
    case WM_SETCURSOR:
        if(!pe::graphics_state.presentation.Windowed || w.display_mode==8 || w.display_mode==9) {
            if(w.cursor_latch==0) {
                while(ShowCursor(FALSE)>=0) {}
                SetCursor(nullptr);
            } else {
                SetCursor(LoadCursorW(nullptr,MAKEINTRESOURCEW(32512)));
                while(ShowCursor(TRUE)<0) {}
            }
        } else {SetCursor(LoadCursorW(nullptr,MAKEINTRESOURCEW(32512)));ShowCursor(TRUE);}
        return 1;
    case WM_DISPLAYCHANGE:
        if(!pe::graphics_state.presentation.Windowed) Sleep(3000);
        w.startup_status=300;break;
    case WM_SYSKEYDOWN:
        if(wp==VK_RETURN) {
            pe::set_device_reset(w,1);
            switch(w.display_mode) {
            case 0:w.display_mode=3;break;case 1:w.display_mode=4;break;
            case 2:w.display_mode=c.scale_choice==4?7:c.scale_choice==3?6:5;break;
            case 3:w.display_mode=0;break;case 4:w.display_mode=1;break;
            case 5:case 6:case 7:w.display_mode=2;break;
            case 8:case 9:
                switch(c.scale_choice) {
                case 0:w.display_mode=3;break;case 1:w.display_mode=4;break;
                case 2:case 5:w.display_mode=3;break;case 3:w.display_mode=6;break;
                case 4:w.display_mode=7;break;
                }break;
            }
        }break;
    case WM_SYSCOMMAND:
        if((wp&0xfff0)==0xf090 || (wp&0xfff0)==0xf100) return 1;
        break;
    case WM_ACTIVATEAPP:
        w.active=wp!=0;w.cursor_latch=w.active?0:1;
        SetWindowPos(hwnd,w.active?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,3);break;
    case WM_SIZE:
        if((w.flags&1) && wp==SIZE_MAXIMIZED) {
            pe::set_device_reset(w,1);
            w.display_mode=w.display_mode==3?0:w.display_mode==4?1:2;
        }break;
    case WM_PAINT:
        if(!(w.flags&0x80)) {
            PAINTSTRUCT paint;HDC dc=BeginPaint(hwnd,&paint);
            HBRUSH brush=CreateSolidBrush(0);FillRect(dc,&paint.rcPaint,brush);
            DeleteObject(brush);EndPaint(hwnd,&paint);w.flags|=0x80;
        }break;
    case WM_CLOSE:pe::graphics_event_flags=(pe::graphics_event_flags&~0x60u)|0x20;return 1;
    case WM_ERASEBKGND:return 1;
    }
    return DefWindowProcW(hwnd,message,wp,lp);
}
#endif
}

namespace th20::source::program_entry::unrecovered {
int fn_0041c020(HINSTANCE) {return platform_window::acquire_single_instance();}
void fn_004117a0(GraphicsStatePrefix& g,HINSTANCE instance) {g.unknown_0000=instance;}
int create_window(WindowStatePrefix& w,HINSTANCE instance) {return platform_window::create_game_window(w,instance);}
int fn_0041d0f0(GraphicsStatePrefix& g) {
    return g.configuration.saved_display_mode==8 || g.configuration.saved_display_mode==9;
}
}
