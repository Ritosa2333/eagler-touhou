
#include "client-info-declarations.h"
#include <commctrl.h>
#include <map>
#include <string>
#pragma comment(lib,"comctl32.lib")
namespace {
unsigned long long query_count=0,query_ticks=0,query_max_ticks=0;
unsigned long long query_msaa=0,query_uia=0,query_other=0;
int sample_count=0;
LARGE_INTEGER frequency;
std::string Escape(BSTR name) {
  if(!name) return "";
  int length=WideCharToMultiByte(CP_UTF8,0,name,SysStringLen(name),nullptr,0,nullptr,nullptr);
  std::string value(length,'\0');
  WideCharToMultiByte(CP_UTF8,0,name,SysStringLen(name),value.data(),length,nullptr,nullptr);
  std::string out;
  for(unsigned char c:value) {
    if(c=='"'||c=='\\') out+='\\';
    if(c>=32) out+=static_cast<char>(c);
  }
  return out;
}

struct MessageCounts { unsigned long long count=0; };
std::map<std::pair<UINT,HWND>,MessageCounts> called_messages;
HHOOK message_hook=nullptr;
LRESULT CALLBACK CountThreadMessages(int code,WPARAM wp,LPARAM lp) {
  if(code>=0) {
    auto* event=reinterpret_cast<CWPSTRUCT*>(lp);
    ++called_messages[{event->message,event->hwnd}].count;
  }
  return CallNextHookEx(message_hook,code,wp,lp);
}
void WriteMessages(FILE* file) {
  fprintf(file,",\"threadCalledMessages\":[");
  bool first=true;
  for(const auto& entry:called_messages) {
    wchar_t name[256]={};GetClassNameW(entry.first.second,name,256);
    BSTR class_name=SysAllocString(name);
    auto text=Escape(class_name);SysFreeString(class_name);
    fprintf(file,"%s{\"message\":%u,\"hwnd\":%llu,\"class\":\"%s\",\"count\":%llu}",first?"":",",entry.first.first,reinterpret_cast<unsigned long long>(entry.first.second),text.c_str(),entry.second.count);
    first=false;
  }
  fprintf(file,"]");
}

FILE* OpenLog() {
  FILE* file=nullptr;
  fopen_s(&file,"D:/Temp/native-performance-20261128/client-info/thread-events.jsonl","a");
  return file;
}
void Mark(const char* label) {
  if(FILE* file=OpenLog()) {
    fprintf(file,"{\"event\":\"%s\",\"tick\":%llu}\n",label,GetTickCount64());
    fclose(file);
  }
}
void Snapshot() {
  FILE* file=OpenLog(); if(!file) return;
  FILETIME create,exit,kernel,user;
  GetProcessTimes(GetCurrentProcess(),&create,&exit,&kernel,&user);
  ULARGE_INTEGER k,u; k.LowPart=kernel.dwLowDateTime;k.HighPart=kernel.dwHighDateTime;
  u.LowPart=user.dwLowDateTime;u.HighPart=user.dwHighDateTime;
  DWORD foreground=0;GetWindowThreadProcessId(GetForegroundWindow(),&foreground);
  fprintf(file,"{\"event\":\"snapshot\",\"tick\":%llu,\"pid\":%lu,\"foregroundPid\":%lu,\"cpu100ns\":%llu,\"getobjectCount\":%llu,\"msaa\":%llu,\"uia\":%llu,\"other\":%llu,\"getobjectTotalMs\":%.6f,\"getobjectMaxMs\":%.6f,",GetTickCount64(),GetCurrentProcessId(),foreground,k.QuadPart+u.QuadPart,query_count,query_msaa,query_uia,query_other,1000.0*query_ticks/frequency.QuadPart,1000.0*query_max_ticks/frequency.QuadPart);
  Microsoft::WRL::ComPtr<IUIAutomationClientInfoSource> source;
  HRESULT hr=CoCreateInstance(__uuidof(CUIAutomationClientInfoSource),nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&source));
  fprintf(file,"\"createHr\":%ld",hr);
  if(SUCCEEDED(hr)) {
    SAFEARRAY* clients=nullptr;hr=source->GetConnectedClients(&clients);
    fprintf(file,",\"clientsHr\":%ld,\"nullArray\":%s,\"clients\":[",hr,clients?"false":"true");
    bool first=true;
    if(SUCCEEDED(hr)&&clients) {
      LONG lo=0,hi=-1;SafeArrayGetLBound(clients,1,&lo);SafeArrayGetUBound(clients,1,&hi);
      for(LONG i=lo;i<=hi;++i) {
        Microsoft::WRL::ComPtr<IUnknown> item;
        Microsoft::WRL::ComPtr<IUIAutomationClientInfo> info;
        HRESULT item_hr=SafeArrayGetElement(clients,&i,item.GetAddressOf());
        if(SUCCEEDED(item_hr)&&item&&SUCCEEDED(item.As(&info))) {
          DWORD id=0;BSTR name=nullptr;
          HRESULT pid_hr=info->get_ProcessId(&id),name_hr=info->get_ProcessName(&name);
          fprintf(file,"%s{\"pid\":%lu,\"pidHr\":%ld,\"nameHr\":%ld,\"name\":\"%s\"}",first?"":",",id,pid_hr,name_hr,Escape(name).c_str());
          first=false;SysFreeString(name);
        }
      }
      SafeArrayDestroy(clients);
    }
    fprintf(file,"]");
  }
  WriteMessages(file);
  fprintf(file,"}\n");fclose(file);
}
LRESULT CALLBACK ObserveChild(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR id,DWORD_PTR) {
  if(msg==WM_NCDESTROY) RemoveWindowSubclass(hwnd,ObserveChild,id);
  if(msg!=WM_GETOBJECT) return DefSubclassProc(hwnd,msg,wp,lp);
  LARGE_INTEGER start,stop;QueryPerformanceCounter(&start);
  LRESULT value=DefSubclassProc(hwnd,msg,wp,lp);
  QueryPerformanceCounter(&stop);
  ++query_count;
  if(static_cast<DWORD>(lp)==static_cast<DWORD>(OBJID_CLIENT)) ++query_msaa;
  else if(static_cast<DWORD>(lp)==static_cast<DWORD>(-25)) ++query_uia;
  else ++query_other;
  auto ticks=static_cast<unsigned long long>(stop.QuadPart-start.QuadPart);
  query_ticks+=ticks;query_max_ticks=(ticks>query_max_ticks)?ticks:query_max_ticks;
  return value;
}
void CALLBACK SampleTimer(HWND hwnd,UINT,UINT_PTR id,DWORD) {
  Snapshot();
  if(++sample_count==6) {
    KillTimer(hwnd,id);Mark("close_requested");PostMessage(hwnd,WM_CLOSE,0,0);
  }
}
}

#include "flutter_window.h"

#include <optional>

#include "flutter/generated_plugin_registrant.h"

FlutterWindow::FlutterWindow(const flutter::DartProject& project)
    : project_(project) {}

FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }

  SetWindowLongPtr(GetHandle(),GWL_EXSTYLE,GetWindowLongPtr(GetHandle(),GWL_EXSTYLE)|WS_EX_NOACTIVATE);
  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  flutter_controller_ = std::make_unique<flutter::FlutterViewController>(
      frame.right - frame.left, frame.bottom - frame.top, project_);
  // Ensure that basic setup of the controller was successful.
  if (!flutter_controller_->engine() || !flutter_controller_->view()) {
    return false;
  }
  RegisterPlugins(flutter_controller_->engine());
  SetChildContent(flutter_controller_->view()->GetNativeWindow());
  SetWindowSubclass(flutter_controller_->view()->GetNativeWindow(),ObserveChild,7362,0);

  flutter_controller_->engine()->SetNextFrameCallback([&]() {
    QueryPerformanceFrequency(&frequency);
    Mark("first_frame");
    message_hook=SetWindowsHookEx(WH_CALLWNDPROC,CountThreadMessages,nullptr,GetCurrentThreadId());
    ShowWindow(GetHandle(), SW_SHOWNOACTIVATE);
    SetTimer(GetHandle(), 7361, 5000, SampleTimer);
  });

  // Flutter can complete the first frame before the "show window" callback is
  // registered. The following call ensures a frame is pending to ensure the
  // window is shown. It is a no-op if the first frame hasn't completed yet.
  flutter_controller_->ForceRedraw();

  return true;
}

void FlutterWindow::OnDestroy() {
  Mark("destroy_enter");
  if(message_hook) {UnhookWindowsHookEx(message_hook);message_hook=nullptr;}
  if (flutter_controller_) {
    flutter_controller_ = nullptr;
    Mark("engine_destroyed");
  }

  Win32Window::OnDestroy();
  Mark("destroy_exit");
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) noexcept {
  // Give Flutter, including plugins, an opportunity to handle window messages.
  if (flutter_controller_) {
    std::optional<LRESULT> result =
        flutter_controller_->HandleTopLevelWindowProc(hwnd, message, wparam,
                                                      lparam);
    if (result) {
      return *result;
    }
  }

  switch (message) {
    case WM_FONTCHANGE:
      flutter_controller_->engine()->ReloadSystemFonts();
      break;
  }

  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
