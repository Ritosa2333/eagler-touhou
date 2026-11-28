from pathlib import Path
import hashlib,json,re
p=Path(__file__).resolve().parent
base=Path('D:/workspace/eagler/artifacts/.native-room-client-win/build/windows/x64/plugins/flutter_inappwebview_windows')
log=base/'flutter_inappwebview_windows_plugin.dir/Release/flutter_.2E7768A0.tlog/link.command.1.tlog'
lines=log.read_text(encoding='utf-16').splitlines()
assert lines[0].startswith('^') and lines[1].startswith('/OUT:')
assert all(not line.startswith('^') for line in lines[1:])
command='\n'.join(lines[1:])
for option,name in [('OUT','flutter_inappwebview_windows_plugin.dll'),('IMPLIB','flutter_inappwebview_windows_plugin.lib'),('PDB','flutter_inappwebview_windows_plugin.pdb')]:
    command,count=re.subn(r'/'+option+r':(?:"[^"]*"|\S+)',lambda m:'/'+option+':"'+str(p/name)+'"',command,flags=re.I)
    if option in ('OUT','IMPLIB'):assert count==1,(option,count)
command+=' /MAP:"'+str(p/'webview.map')+'"'
(p/'link.rsp').write_text(command)
(p/'link.cmd').write_text('@echo off\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\18\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x64 -host_arch=x64\nif errorlevel 1 exit /b 1\ncd /d "'+str(base)+'"\nlink @"'+str(p/'link.rsp')+'"\nexit /b %errorlevel%\n')
print(json.dumps({'commandBytes':len(command),'objectInputs':len(lines)-2,'target':str(p),'originalDllSha256':hashlib.sha256(Path('D:/Temp/native-performance-20261128/gpu-preference/preview/flutter_inappwebview_windows_plugin.dll').read_bytes()).hexdigest()}))
