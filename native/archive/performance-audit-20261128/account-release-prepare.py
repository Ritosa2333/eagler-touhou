from pathlib import Path
import hashlib,json,os,re,sys
p=Path(__file__).resolve().parent
width=int(sys.argv[1]);assert width in (320,390,1280)
outer_width=width+(15 if '--client-width-144dpi' in sys.argv else 0)
repo=Path('C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native')
stage=Path('D:/workspace/eagler/artifacts/.native-room-client-win')
previous=p.parent/'client-info'
bundle=Path('D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf/preview')
for source in bundle.rglob('*'):
    if not source.is_file() or source.suffix.lower()=='.exe': continue
    dest=p/'preview'/source.relative_to(bundle)
    dest.parent.mkdir(parents=True,exist_ok=True)
    if not dest.exists():os.link(source,dest)
    else:assert os.path.samefile(source,dest)

helper=r'''
#include <windows.h>
#include <cstdio>
namespace {
int samples=0;
FILE* OpenLog() {
  FILE* file=nullptr;fopen_s(&file,"EVENT_PATH","a");return file;
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
  if(++samples==24) {KillTimer(hwnd,id);Mark("close_requested",hwnd);PostMessage(hwnd,WM_CLOSE,0,0);}
}
}
'''.replace('EVENT_PATH',str(p/f'events-{width}.jsonl').replace('\\','/'))
source=(repo/'windows/runner/flutter_window.cpp').read_text()
source=source.replace('RECT frame = GetClientArea();','SetWindowLongPtr(GetHandle(),GWL_EXSTYLE,GetWindowLongPtr(GetHandle(),GWL_EXSTYLE)|WS_EX_NOACTIVATE);\n  RECT frame = GetClientArea();')
source=source.replace('this->Show();','Mark("first_frame",GetHandle());\n    ShowWindow(GetHandle(),SW_SHOWNOACTIVATE);\n    SetTimer(GetHandle(),7461,5000,Observe);')
source=source.replace('void FlutterWindow::OnDestroy() {','void FlutterWindow::OnDestroy() {\n  Mark("destroy_enter",GetHandle());')
source=source.replace('flutter_controller_ = nullptr;','flutter_controller_ = nullptr;\n    Mark("engine_destroyed",GetHandle());')
(p/f'window-{width}.cpp').write_text(helper+'\n'+source)
main=(repo/'windows/runner/main.cpp').read_text().replace('Win32Window::Size size(1280, 720);',f'Win32Window::Size size({outer_width}, 720);').replace('L"TouhouOnline", origin',f'L"TouhouOnline | account release {width}", origin')
(p/f'main-{width}.cpp').write_text(main)
rc=(stage/'windows/runner/Runner.rc').read_text().replace('"com.touhouonline" "\\0"','"com.touhouonline.diagnostics" "\\0"').replace('"touhouonline" "\\0"',f'"account-release-{width}" "\\0"').replace('"resources\\\\app_icon.ico"','"'+str(stage/'windows/runner/resources/app_icon.ico').replace('\\','/')+'"')
(p/f'runner-{width}.rc').write_text(rc)
template=(previous/'thread-compile.rsp').read_text()
for kind in ('main','window'):
    rsp=template.replace(str(previous/'client-thread-window.obj').replace('\\','/'),str(p/f'{kind}-{width}.obj').replace('\\','/')).replace(str(previous/'client-thread-window.cpp'),str(p/f'{kind}-{width}.cpp'))
    (p/f'{kind}-{width}.rsp').write_text(rsp)
link=(previous/'thread-link.rsp').read_text().replace(str(previous/'preview/client-thread-host.exe').replace('\\','/'),str(p/f'preview/account-{width}.exe').replace('\\','/')).replace(str(previous/'client-thread-window.obj').replace('\\','/'),str(p/f'window-{width}.obj').replace('\\','/')).replace(r'TOUHOUONLINE.DIR\RELEASE\MAIN.OBJ','"'+str(p/f'main-{width}.obj')+'"').replace(r'TOUHOUONLINE.DIR\RELEASE\RUNNER.RES','"'+str(p/f'runner-{width}.res')+'"')
(p/f'link-{width}.rsp').write_text(link)
commands=['@echo off','call "C:\\Program Files (x86)\\Microsoft Visual Studio\\18\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x64 -host_arch=x64','if errorlevel 1 exit /b 1','cd /d "'+str(stage/'build/windows/x64/runner')+'"',f'rc /nologo /i "{stage}/windows/runner" /fo "{p}/runner-{width}.res" "{p}/runner-{width}.rc"','if errorlevel 1 exit /b 1']
for kind in ('main','window'):commands += [f'cl @"{p}/{kind}-{width}.rsp"','if errorlevel 1 exit /b 1']
commands += [f'link @"{p}/link-{width}.rsp"','exit /b %errorlevel%']
(p/f'build-{width}.cmd').write_text('\n'.join(commands)+'\n')
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
proof={'scope':'Ordinary production AOT. Native diagnostic runner adds fixed initial width, no-activate display, isolated Windows product metadata and normal-close timer. No Dart, plugin or engine changes.','nominalOuterWidth':outer_width,'requestedContentWidth':width if outer_width!=width else None,'widthPolicy':'15logical pixels added to compensate observed22physical border pixels at144DPI; actual client dimensions must be verified from native logs' if outer_width!=width else 'Initial outer window width; actual client dimensions recorded separately','isolatedCompany':'com.touhouonline.diagnostics','isolatedProduct':f'account-release-{width}','launchUrl':'https://touhouonline.com/login.html?lang=zh-CN','aotSha256':sha(bundle/'data/app.so'),'engineSha256':sha(bundle/'flutter_windows.dll'),'windowSourceSha256':sha(p/f'window-{width}.cpp'),'mainSourceSha256':sha(p/f'main-{width}.cpp')}
(p/f'source-{width}.json').write_text(json.dumps(proof,indent=2)+'\n')
print(json.dumps(proof))
