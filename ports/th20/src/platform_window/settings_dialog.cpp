#include "platform_window.hpp"
#include "data_constants.hpp"
#include <memory>
#ifndef TH_SDL3

namespace th20::source::platform_window {
namespace pe=program_entry;
void register_dialog_raw_input(HWND hwnd) {
    RAWINPUTDEVICE device{1,4,RIDEV_INPUTSINK,hwnd};
    RegisterRawInputDevices(&device,1,sizeof(device));
}
void apply_settings_dialog() {
    auto& c=pe::graphics_state.configuration;HWND hwnd=pe::window_state.previous_window;
    if(IsDlgButtonChecked(hwnd,0xca)==BST_CHECKED) c.flags|=0x80;
    else c.flags&=~0x80u;
    for(int i=0;i<10;++i) if(IsDlgButtonChecked(hwnd,data::mode_controls[i])==BST_CHECKED) {
        c.saved_display_mode=i;c.scale_choice=static_cast<std::uint8_t>(data::mode_scales[i]);return;
    }
}
INT_PTR CALLBACK settings_dialog_proc(HWND hwnd,UINT message,WPARAM wp,LPARAM lp) {
    auto& w=pe::window_state;auto& c=pe::graphics_state.configuration;
    switch(message) {
    case WM_INITDIALOG:
        if(c.flags&0x80) SendMessageW(GetDlgItem(hwnd,0xca),BM_SETCHECK,BST_CHECKED,0);
        SendMessageW(GetDlgItem(hwnd,data::mode_controls[c.saved_display_mode]),BM_SETCHECK,BST_CHECKED,0);
        w.flags=(w.flags&~0x18u)|0x10;register_dialog_raw_input(hwnd);return 1;
    case WM_CLOSE:
        if(((w.flags>>3)&3)==2) w.flags=(w.flags&~0x18u)|8;
        DestroyWindow(w.previous_window);w.previous_window=nullptr;return 1;
    case WM_INPUT: {
        UINT size;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lp),RID_INPUT,nullptr,&size,sizeof(RAWINPUTHEADER));
        // Original allocation helper 0x00542a01 and deallocation 0x0054278d
        // are the executable's C++ new/delete runtime, replaced by source CRT.
        auto bytes=std::make_unique<unsigned char[]>(size);
        const UINT got=GetRawInputData(reinterpret_cast<HRAWINPUT>(lp),RID_INPUT,bytes.get(),&size,sizeof(RAWINPUTHEADER));
        if(got!=size || reinterpret_cast<RAWINPUT*>(bytes.get())->header.dwType!=RIM_TYPEHID || bytes[0x19]==0) return 1;
        break;
    }
    case WM_COMMAND:if(LOWORD(wp)!=0xce) return 1;break;
    default:return DefWindowProcW(hwnd,message,wp,lp);
    }
    apply_settings_dialog();w.flags&=~0x18u;
    if(IsWindow(w.previous_window)) DestroyWindow(w.previous_window);
    w.previous_window=nullptr;return 1;
}
void show_startup_settings() {
    auto& w=pe::window_state;const auto& c=pe::graphics_state.configuration;
    // The original explicitly ANDs 0x80, not the conventional 0x8000.
    if(!(c.flags&0x80) && !(GetKeyState(VK_SHIFT)&0x80) && !(GetKeyState(VK_CONTROL)&0x80)) return;
    int disabled[10]{};int selected=0;
    w.previous_window=CreateDialogParamW(w.instance,MAKEINTRESOURCEW(is_japanese_user_locale()?203:204),
        nullptr,settings_dialog_proc,0);
    DEVMODEW mode{};mode.dmPelsWidth=0;
    // 0x0041af89..b1 zeros all 220 bytes, including dmSize. Preserve it.
    EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&mode);
    auto disable=[&](int index,int control) {disabled[index]=1;EnableWindow(GetDlgItem(w.previous_window,control),FALSE);};
    if(mode.dmPelsWidth<2560 || mode.dmPelsHeight<1920) disable(5,0xd4);
    if(mode.dmPelsWidth<1920 && mode.dmPelsHeight<1440) disable(6,0xd5);
    if(mode.dmPelsWidth<1280 && mode.dmPelsHeight<960) {disable(7,0xd6);disable(2,0xd1);}
    if(mode.dmPelsWidth<960 && mode.dmPelsHeight<720) {disable(8,0xd7);disable(3,0xd2);}
    for(int i=0;i<10;++i) if(IsDlgButtonChecked(w.previous_window,data::navigation_controls[i])==1) {selected=i;break;}
    // 0x0040c6b0 on the invalid-handle path is a verified no-op.
    if(!IsWindow(w.previous_window)) return;
    ShowWindow(w.previous_window,SW_SHOW);
    for(;;) {
        unrecovered::sample_input();
        MSG message;
        if(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)) {
            if(IsDialogMessageW(w.previous_window,&message)) continue;
            TranslateMessage(&message);DispatchMessageW(&message);
        }
        if(input && pressed(*input,0x80001)) {
            apply_settings_dialog();w.flags&=~0x18u;DestroyWindow(w.previous_window);w.previous_window=nullptr;return;
        }
        if(input && repeated_or_pressed(*input,0x20) && selected<9) {
            do {++selected;} while(disabled[selected]);
            CheckRadioButton(w.previous_window,0xcf,0xd8,data::navigation_controls[selected]);
        }
        if(input && repeated_or_pressed(*input,0x10) && selected>0) {
            do {--selected;} while(disabled[selected]);
            CheckRadioButton(w.previous_window,0xcf,0xd8,data::navigation_controls[selected]);
        }
        if(!w.previous_window) return;
        Sleep(6);
    }
}
}
#else
namespace th20::source::platform_window {
void show_startup_settings() {} // launcher configure injects the same fields
}
#endif
namespace th20::source::program_entry::unrecovered {
void fn_0041ae70(HINSTANCE) {platform_window::show_startup_settings();}
}
