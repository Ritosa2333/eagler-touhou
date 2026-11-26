#include "DisplaySettings.hpp"
#include <windows.h>
#include <fstream>
#include <stdexcept>

namespace kaikeidou {
std::string DisplaySettings::encode() const {
    std::string result;for(auto v:values)result.push_back(char('0'+v));return result;
}
DisplaySettings DisplaySettings::read(const std::filesystem::path& path) {
    DisplaySettings settings;std::ifstream stream(path,std::ios::binary);std::string text;std::getline(stream,text);
    constexpr unsigned limits[]{2,4,6,2,2};
    for(unsigned i=0;i<5;++i)if(i<text.size()&&text[i]>='0'&&unsigned(text[i]-'0')<limits[i])settings.values[i]=unsigned(text[i]-'0');
    return settings;
}
void DisplaySettings::save(const std::filesystem::path& path) const {
    std::filesystem::create_directories(path.parent_path());auto temporary=path;temporary+=L".tmp";
    {std::ofstream out(temporary,std::ios::binary|std::ios::trunc);out<<encode();out.flush();if(!out)throw std::runtime_error("Cannot save display settings");}
    if(!MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("Cannot replace display settings");
}
namespace {
struct SettingsWindow {
    std::filesystem::path path;
    DisplaySettings settings;
    std::array<HWND,5> selectors{};
    HWND status=nullptr;
    HFONT font=nullptr;
};
LRESULT CALLBACK settings_proc(HWND window,UINT message,WPARAM wparam,LPARAM lparam) {
    auto* state=reinterpret_cast<SettingsWindow*>(GetWindowLongPtrW(window,GWLP_USERDATA));
    if(message==WM_NCCREATE){state=static_cast<SettingsWindow*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(state));}
    if(message==WM_CREATE){
        state->font=CreateFontW(-18,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Microsoft YaHei UI");
        const wchar_t* labels[]{L"显示模式",L"窗口尺寸",L"输出分辨率",L"颜色位深",L"绘图方式"};
        const wchar_t* choices[5][6]={{L"窗口",L"全屏（保持桌面分辨率）"},{L"640 × 480",L"960 × 720",L"1024 × 768",L"1280 × 960"},{L"320 × 240",L"640 × 480",L"800 × 600",L"1024 × 768",L"1280 × 800",L"1280 × 1024"},{L"16 位",L"32 位（推荐）"},{L"硬件加速（推荐）",L"软件兼容"}};
        const unsigned count[]{2,4,6,2,2};
        auto child=[&](const wchar_t* cls,const wchar_t* text,DWORD style,int x,int y,int w,int h,int id){auto handle=CreateWindowExW(0,cls,text,WS_CHILD|WS_VISIBLE|style,x,y,w,h,window,reinterpret_cast<HMENU>(INT_PTR(id)),GetModuleHandleW(nullptr),nullptr);SendMessageW(handle,WM_SETFONT,reinterpret_cast<WPARAM>(state->font),TRUE);return handle;};
        for(unsigned i=0;i<5;++i){child(L"STATIC",labels[i],0,24,24+44*i,120,28,0);state->selectors[i]=child(L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,150,20+44*i,286,220,100+i);for(unsigned j=0;j<count[i];++j)SendMessageW(state->selectors[i],CB_ADDSTRING,0,reinterpret_cast<LPARAM>(choices[i][j]));SendMessageW(state->selectors[i],CB_SETCURSEL,state->settings.values[i],0);}
        child(L"STATIC",L"F11 或 Alt+Enter 可在游戏中切换全屏。\n画面保持 4:3 比例，设置保存后下次启动生效。",0,24,250,418,50,0);
        child(L"BUTTON",L"恢复默认",WS_TABSTOP,24,314,105,34,201);
        child(L"BUTTON",L"保存设置",WS_TABSTOP|BS_DEFPUSHBUTTON,215,314,105,34,202);
        child(L"BUTTON",L"关闭",WS_TABSTOP,331,314,105,34,IDCANCEL);
        state->status=child(L"STATIC",L"",0,24,363,412,28,0);SetFocus(state->selectors[0]);return 0;
    }
    if(message==WM_COMMAND){
        if(LOWORD(wparam)==IDCANCEL){DestroyWindow(window);return 0;}
        if(LOWORD(wparam)==201){state->settings=DisplaySettings{};for(unsigned i=0;i<5;++i)SendMessageW(state->selectors[i],CB_SETCURSEL,state->settings.values[i],0);SetWindowTextW(state->status,L"已恢复默认值，点击保存设置即可应用。");return 0;}
        if(LOWORD(wparam)==202){
            for(unsigned i=0;i<5;++i)state->settings.values[i]=unsigned(SendMessageW(state->selectors[i],CB_GETCURSEL,0,0));
            try{state->settings.save(state->path);SetWindowTextW(state->status,L"设置已保存。");}
            catch(const std::exception&){MessageBoxW(window,L"保存失败，请确认游戏目录可写后重试。",L"显示设置",MB_OK|MB_ICONERROR);}
            return 0;
        }
    }
    if(message==WM_CLOSE){DestroyWindow(window);return 0;}
    if(message==WM_DESTROY){if(state&&state->font)DeleteObject(state->font);PostQuitMessage(0);return 0;}
    return DefWindowProcW(window,message,wparam,lparam);
}
}
int show_display_settings(const std::filesystem::path& userdata) {
    SettingsWindow state;state.path=userdata/"config.txt";state.settings=DisplaySettings::read(state.path);
    WNDCLASSW type{};type.lpfnWndProc=settings_proc;type.hInstance=GetModuleHandleW(nullptr);type.lpszClassName=L"ThkkdDisplaySettings";type.hCursor=LoadCursorW(nullptr,IDC_ARROW);type.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_BTNFACE+1);RegisterClassW(&type);
    RECT rect{0,0,462,404};AdjustWindowRect(&rect,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);
    auto window=CreateWindowExW(WS_EX_CONTROLPARENT,type.lpszClassName,L"东方海惠堂显示设置",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,rect.right-rect.left,rect.bottom-rect.top,nullptr,nullptr,type.hInstance,&state);
    if(!window)return 1;ShowWindow(window,SW_SHOW);UpdateWindow(window);
    MSG message{};while(GetMessageW(&message,nullptr,0,0)>0){if(!IsDialogMessageW(window,&message)){TranslateMessage(&message);DispatchMessageW(&message);}}
    return 0;
}
}
