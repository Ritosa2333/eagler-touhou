#include "flutter_window.h"
#include <windows.h>
#include <psapi.h>
#include <cstdio>
#include <optional>
#include "flutter/generated_plugin_registrant.h"

namespace {
int samples = 0;
unsigned long long Ticks() {
  FILETIME value{}; GetSystemTimePreciseAsFileTime(&value);
  ULARGE_INTEGER ticks{}; ticks.LowPart=value.dwLowDateTime; ticks.HighPart=value.dwHighDateTime;
  return ticks.QuadPart;
}
unsigned long long AsTicks(FILETIME value) {
  ULARGE_INTEGER ticks{}; ticks.LowPart=value.dwLowDateTime; ticks.HighPart=value.dwHighDateTime;
  return ticks.QuadPart;
}
FILE* OpenLog() {
  char path[1024] = {};
  if (!GetEnvironmentVariableA("TOUHOU_PROBE_LOG",path,1024)) return nullptr;
  FILE* file=nullptr; fopen_s(&file,path,"a"); return file;
}
void Mark(const char* event, HWND hwnd) {
  RECT client{}; GetClientRect(hwnd,&client);
  DWORD foreground=0; GetWindowThreadProcessId(GetForegroundWindow(),&foreground);
  PROCESS_MEMORY_COUNTERS_EX memory{}; memory.cb=sizeof(memory);
  GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory),sizeof(memory));
  FILETIME created{},exited{},kernel{},user{};
  GetProcessTimes(GetCurrentProcess(),&created,&exited,&kernel,&user);
  if (FILE* file=OpenLog()) {
    fprintf(file,"{\"event\":\"%s\",\"tick\":%llu,\"pid\":%lu,\"foregroundPid\":%lu,\"clientPixels\":[%ld,%ld],\"dpi\":%u,\"workingSet\":%llu,\"privateBytes\":%llu,\"cpuMs\":%.4f}\n",event,GetTickCount64(),GetCurrentProcessId(),foreground,client.right,client.bottom,GetDpiForWindow(hwnd),static_cast<unsigned long long>(memory.WorkingSetSize),static_cast<unsigned long long>(memory.PrivateUsage),static_cast<double>(AsTicks(kernel)+AsTicks(user))/10000.0);
    fclose(file);
  }
}
void CALLBACK Observe(HWND hwnd, UINT, UINT_PTR id, DWORD) {
  Mark("snapshot",hwnd);
  if (++samples==45) { KillTimer(hwnd,id); Mark("close_requested",hwnd); PostMessage(hwnd,WM_CLOSE,0,0); }
}
}

FlutterWindow::FlutterWindow(const flutter::DartProject& project) : project_(project) {}
FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate() {
  const auto createStart=Ticks();
  if (!Win32Window::OnCreate()) return false;
  SetWindowLongPtr(GetHandle(),GWL_EXSTYLE,GetWindowLongPtr(GetHandle(),GWL_EXSTYLE)|WS_EX_NOACTIVATE);
  RECT frame=GetClientArea();
  flutter_controller_=std::make_unique<flutter::FlutterViewController>(frame.right-frame.left,frame.bottom-frame.top,project_);
  if (!flutter_controller_->engine() || !flutter_controller_->view()) return false;
  const auto controllerReady=Ticks();
  RegisterPlugins(flutter_controller_->engine());
  const auto pluginsReady=Ticks();
  SetChildContent(flutter_controller_->view()->GetNativeWindow());
  flutter_controller_->engine()->SetNextFrameCallback([this,createStart,controllerReady,pluginsReady]() {
    const auto ready=Ticks();
    FILETIME created{},exited{},kernel{},user{};
    GetProcessTimes(GetCurrentProcess(),&created,&exited,&kernel,&user);
    char preference[32]={};GetEnvironmentVariableA("TOUHOU_GPU_PREFERENCE",preference,32);
    IDXGIAdapter* adapter=nullptr; DXGI_ADAPTER_DESC description{};
    const bool found=FlutterDesktopPluginRegistrarGetGraphicsAdapter(flutter_controller_->engine()->GetRegistrarForPlugin("GpuPreferenceAudit"),&adapter);
    HRESULT status=E_FAIL;
    if (found && adapter) {status=adapter->GetDesc(&description);adapter->Release();}
    if (FILE* file=OpenLog()) {
      fprintf(file,"{\"event\":\"startup\",\"pid\":%lu,\"tid\":%lu,\"gpuPreference\":\"%s\",\"processCreatedToFrameCallbackMs\":%.4f,\"processCreatedToWindowCreateMs\":%.4f,\"windowCreateToControllerReadyMs\":%.4f,\"pluginRegistrationMs\":%.4f,\"pluginsReadyToFrameCallbackMs\":%.4f,\"adapterFound\":%s,\"vendorId\":%u,\"deviceId\":%u,\"luidHigh\":%ld,\"luidLow\":%lu}\n",GetCurrentProcessId(),GetCurrentThreadId(),preference,static_cast<double>(ready-AsTicks(created))/10000.0,static_cast<double>(createStart-AsTicks(created))/10000.0,static_cast<double>(controllerReady-createStart)/10000.0,static_cast<double>(pluginsReady-controllerReady)/10000.0,static_cast<double>(ready-pluginsReady)/10000.0,SUCCEEDED(status)?"true":"false",description.VendorId,description.DeviceId,description.AdapterLuid.HighPart,description.AdapterLuid.LowPart);
      fclose(file);
    }
    Mark("first_frame",GetHandle());
    ShowWindow(GetHandle(),SW_SHOWNOACTIVATE);
    SetTimer(GetHandle(),7461,1000,Observe);
  });
  flutter_controller_->ForceRedraw();
  return true;
}

void FlutterWindow::OnDestroy() {
  Mark("destroy_enter",GetHandle());
  if (flutter_controller_) {flutter_controller_=nullptr;Mark("engine_destroyed",GetHandle());}
  Win32Window::OnDestroy();
}

LRESULT FlutterWindow::MessageHandler(HWND hwnd,UINT const message,WPARAM const wparam,LPARAM const lparam) noexcept {
  if (flutter_controller_) {
    std::optional<LRESULT> result=flutter_controller_->HandleTopLevelWindowProc(hwnd,message,wparam,lparam);
    if (result) return *result;
  }
  if (message==WM_FONTCHANGE && flutter_controller_) flutter_controller_->engine()->ReloadSystemFonts();
  return Win32Window::MessageHandler(hwnd,message,wparam,lparam);
}
