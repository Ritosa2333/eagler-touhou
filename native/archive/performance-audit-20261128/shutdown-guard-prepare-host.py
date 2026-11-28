from pathlib import Path
import hashlib,json,os,re
p=Path(__file__).resolve().parent
account=p.parent/'account-release';client=p.parent/'client-info'
stage=Path('D:/workspace/eagler/artifacts/.native-room-client-win')
bundle=Path('D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf/preview')
for source in bundle.rglob('*'):
    if not source.is_file() or source.suffix.lower()=='.exe' or source.name=='flutter_inappwebview_windows_plugin.dll':continue
    dest=p/'preview'/source.relative_to(bundle);dest.parent.mkdir(parents=True,exist_ok=True)
    if not dest.exists():os.link(source,dest)
    else:assert os.path.samefile(source,dest)
target=p/'preview/flutter_inappwebview_windows_plugin.dll'
if not target.exists():os.link(p/target.name,target)
else:assert os.path.samefile(p/target.name,target)
source=(account/'window-320.cpp').read_text()
old='FILE* file=nullptr;fopen_s(&file,"D:/Temp/native-performance-20261128/account-release/events-320.jsonl","a");return file;'
new='char path[1024]={};if(!GetEnvironmentVariableA("TOUHOU_PROBE_LOG",path,1024)) return nullptr;\n  FILE* file=nullptr;fopen_s(&file,path,"a");return file;'
assert source.count(old)==1
source=source.replace(old,new).replace('++samples==24','++samples==6')
(p/'host-window.cpp').write_text(source)
main=(account/'main-320.cpp').read_text().replace('TouhouOnline | account release 320','TouhouOnline | shutdown guard')
(p/'host-main.cpp').write_text(main)
rc=(account/'runner-320.rc').read_text().replace('account-release-320','shutdown-guard-20261128')
(p/'host.rc').write_text(rc)
for kind in ['window','main']:
    text=(account/f'{kind}-320.rsp').read_text().replace(str(account/f'{kind}-320.obj').replace('\\','/'),str(p/f'host-{kind}.obj').replace('\\','/')).replace(str(account/f'{kind}-320.cpp'),str(p/f'host-{kind}.cpp'))
    (p/f'host-{kind}.rsp').write_text(text)
link=(account/'link-320.rsp').read_text()
for old,new in [(str(account/'preview/account-320.exe').replace('\\','/'),str(p/'preview/guard-host.exe').replace('\\','/')),(str(account/'window-320.obj').replace('\\','/'),str(p/'host-window.obj').replace('\\','/')),(str(account/'main-320.obj'),str(p/'host-main.obj')),(str(account/'runner-320.res'),str(p/'host.res'))]:
    assert old in link,old;link=link.replace(old,new)
(p/'host-link.rsp').write_text(link)
commands=['@echo off','call "C:\\Program Files (x86)\\Microsoft Visual Studio\\18\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x64 -host_arch=x64','if errorlevel 1 exit /b 1','cd /d "'+str(stage/'build/windows/x64/runner')+'"',f'rc /nologo /i "{stage}/windows/runner" /fo "{p}/host.res" "{p}/host.rc"','if errorlevel 1 exit /b 1']
for kind in ('main','window'):commands += [f'cl @"{p}/host-{kind}.rsp"','if errorlevel 1 exit /b 1']
commands += [f'link @"{p}/host-link.rsp"','exit /b %errorlevel%']
(p/'build-host.cmd').write_text('\n'.join(commands)+'\n')
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
proof=json.loads((p/'source.json').read_text())
proof.update(aotSha256=sha(bundle/'data/app.so'),candidatePluginSha256=sha(target),hostSourceSha256={str(f.name):sha(f) for f in [p/'host-main.cpp',p/'host-window.cpp',p/'host.rc']})
(p/'host-source.json').write_text(json.dumps(proof,indent=2)+'\n')
print(json.dumps(proof,indent=2))
