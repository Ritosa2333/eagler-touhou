from pathlib import Path
import hashlib,json,os,re
p=Path('D:/Temp/native-performance-20261128/client-info')
old=Path('C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128/webview-adapter')
stage=Path('D:/workspace/eagler/artifacts/.native-room-client-win')
repo=Path('C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native')
bundle=Path('D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf/preview')
for src in bundle.rglob('*'):
    if src.is_file():
        dest=p/'preview'/src.relative_to(bundle)
        dest.parent.mkdir(parents=True,exist_ok=True)
        if not dest.exists():
            os.link(src,dest)
        else:
            assert os.path.samefile(src,dest), str(dest)
if (p/'preview/touhouonline.exe').exists():
    (p/'preview/touhouonline.exe').unlink()
declarations=(p/'client-info.cpp').read_text(encoding='utf-8-sig').split('int main()')[0]
(p/'client-info-declarations.h').write_text(declarations,encoding='utf-8')
helper=r'''
#include "client-info-declarations.h"
#include <commctrl.h>
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
FILE* OpenLog() {
  FILE* file=nullptr;
  fopen_s(&file,"D:/Temp/native-performance-20261128/client-info/host-events.jsonl","a");
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
'''
src=(repo/'windows/runner/flutter_window.cpp').read_text(encoding='utf-8')
src=helper+'\n'+src
src=src.replace('this->Show();','QueryPerformanceFrequency(&frequency);\n    Mark("first_frame");\n    ShowWindow(GetHandle(), SW_SHOWNOACTIVATE);\n    SetTimer(GetHandle(), 7361, 5000, SampleTimer);')
src=src.replace('SetChildContent(flutter_controller_->view()->GetNativeWindow());','SetChildContent(flutter_controller_->view()->GetNativeWindow());\n  SetWindowSubclass(flutter_controller_->view()->GetNativeWindow(),ObserveChild,7362,0);')
src=src.replace('void FlutterWindow::OnDestroy() {','void FlutterWindow::OnDestroy() {\n  Mark("destroy_enter");')
src=src.replace('flutter_controller_ = nullptr;','flutter_controller_ = nullptr;\n    Mark("engine_destroyed");')
src=src.replace('Win32Window::OnDestroy();','Win32Window::OnDestroy();\n  Mark("destroy_exit");')
(p/'client-flutter-window.cpp').write_text(src,encoding='utf-8')
rsp=(old/'startup-compile.rsp').read_text(encoding='utf-16').replace(str(old).replace('\\','/'),str(p).replace('\\','/')).replace(str(old),str(p)).replace('startup-flutter-window','client-flutter-window')
rsp=rsp.replace('/W4 /WX ', '/W4 /WX- ')
(p/'compile.rsp').write_text(rsp)
link=(old/'startup-link.rsp').read_text(encoding='utf-16').replace(str(old).replace('\\','/'),str(p).replace('\\','/')).replace(str(old),str(p)).replace('startup-flutter-window','client-flutter-window').replace('touhouonline-startup.exe','preview/client-info-host.exe')
(p/'link.rsp').write_text(link)
cmd='@echo off\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\18\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x64 -host_arch=x64\nif errorlevel 1 exit /b 1\ncd /d "'+str(stage/'build/windows/x64/runner')+'"\ncl @"'+str(p/'compile.rsp')+'"\nif errorlevel 1 exit /b 1\nlink @"'+str(p/'link.rsp')+'"\nexit /b %errorlevel%\n'
(p/'build-host.cmd').write_text(cmd)
proof={'aotSha256':hashlib.sha256((bundle/'data/app.so').read_bytes()).hexdigest(),'diagnosticChanges':['native child WM_GETOBJECT observer','read-only client source snapshots every5s','nonactivating show','normal close after6samples','native shutdown markers'],'productionSourcesModified':False}
(p/'host-source.json').write_text(json.dumps(proof,indent=2))
print(json.dumps(proof))
