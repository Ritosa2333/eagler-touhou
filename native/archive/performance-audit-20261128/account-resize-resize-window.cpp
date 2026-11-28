
#include <windows.h>
#include <cstdio>
namespace {
int samples=0;
FILE* OpenLog() {
  char path[1024]={};if(!GetEnvironmentVariableA("TOUHOU_PROBE_LOG",path,1024)) return nullptr;
  FILE* file=nullptr;fopen_s(&file,path,"a");return file;
}
void Mark(const char* event,HWND hwnd) {
  RECT client{};GetClientRect(hwnd,&client);DWORD foreground=0;
  GetWindowThreadProcessId(GetForegroundWindow(),&foreground);
  if(FILE* file=OpenLog()) {
    fprintf(file,"{\"event\":\"%s\",\"tick\":%llu,\"pid\":%lu,\"foregroundPid\":%lu,\"clientPixels\":[%ld,%ld],\"dpi\":%u}\n",event,GetTickCount64(),GetCurrentProcessId(),foreground,client.right,client.bottom,GetDpiForWindow(hwnd));fclose(file);
  }
}
void CALLBACK Observe(HWND hwnd,UINT,UINT_PTR id,DWORD) {
  Mark("snapshot",hwnd);
  ++samples;
  int width=0;
  if(samples==1) width=1280;
  if(samples==25) width=390;
  if(samples==55) width=320;
  if(samples==85) width=1280;
  if(width) {
    RECT frame{},client{}; GetWindowRect(hwnd,&frame);GetClientRect(hwnd,&client);
    const int pixels=MulDiv(width,GetDpiForWindow(hwnd),96);
    if(!SetWindowPos(hwnd,nullptr,0,0,pixels+(frame.right-frame.left)-client.right,frame.bottom-frame.top,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE)) {
      Mark("resize_failed",hwnd);
    }
    Mark("resized",hwnd);
  }
  if(samples==115) {KillTimer(hwnd,id);Mark("close_requested",hwnd);PostMessage(hwnd,WM_CLOSE,0,0);}
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

  flutter_controller_->engine()->SetNextFrameCallback([&]() {
    Mark("first_frame",GetHandle());
    ShowWindow(GetHandle(),SW_SHOWNOACTIVATE);
    SetTimer(GetHandle(),7461,1000,Observe);
  });

  // Flutter can complete the first frame before the "show window" callback is
  // registered. The following call ensures a frame is pending to ensure the
  // window is shown. It is a no-op if the first frame hasn't completed yet.
  flutter_controller_->ForceRedraw();

  return true;
}

void FlutterWindow::OnDestroy() {
  Mark("destroy_enter",GetHandle());
  if (flutter_controller_) {
    flutter_controller_ = nullptr;
    Mark("engine_destroyed",GetHandle());
  }

  Win32Window::OnDestroy();
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
