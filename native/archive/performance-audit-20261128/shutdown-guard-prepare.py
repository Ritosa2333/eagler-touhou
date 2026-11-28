from pathlib import Path
import hashlib,json,os,re
p=Path(__file__).resolve().parent
stage=Path('D:/workspace/eagler/artifacts/.native-room-client-win')
wrapper=stage/'windows/flutter/ephemeral/cpp_client_wrapper'
build=stage/'build/windows/x64'
source=wrapper/'core_implementations.cc'
text=source.read_text()
old='''    FlutterDesktopMessengerSetCallback(messenger_, channel.c_str(), nullptr,
                                       nullptr);'''
new='''    if (FlutterDesktopMessengerIsAvailable(messenger_)) {
      FlutterDesktopMessengerSetCallback(messenger_, channel.c_str(), nullptr,
                                         nullptr);
    }'''
assert text.count(old)==1
(p/'core_implementations.cc').write_text(text.replace(old,new))
log=build/'flutter/flutter_wrapper_plugin.dir/Release/flutter_.13566B7E.tlog'
lines=(log/'CL.command.1.tlog').read_text(encoding='utf-16').splitlines()
assert lines[0][1:].lower()==str(source).lower()
command=lines[1]
command,count=re.subn(r'/Fo(?:"[^"]*"|\S+)',lambda _: '/Fo"'+str(p/'core_implementations.obj')+'"',command,flags=re.I);assert count==1
command,count=re.subn(r'/Fd(?:"[^"]*"|\S+)',lambda _: '/Fd"'+str(p/'compile.pdb')+'"',command,flags=re.I);assert count==1
command=command.replace(str(source).upper(),str(p/'core_implementations.cc'))
assert str(source).upper() not in command
command+=' /I"'+str(wrapper)+'"'
(p/'compile.rsp').write_text(command)
lib='\n'.join((log/'Lib.command.1.tlog').read_text(encoding='utf-16').splitlines()[1:])
lib,count=re.subn(r'/OUT:(?:"[^"]*"|\S+)',lambda _: '/OUT:"'+str(p/'flutter_wrapper_plugin.lib')+'"',lib,flags=re.I);assert count==1
lib=lib.replace(r'FLUTTER_WRAPPER_PLUGIN.DIR\RELEASE\CORE_IMPLEMENTATIONS.OBJ','"'+str(p/'core_implementations.obj')+'"')
(p/'lib.rsp').write_text(lib)
plugin=build/'plugins/flutter_inappwebview_windows'
link='\n'.join((plugin/'flutter_inappwebview_windows_plugin.dir/Release/flutter_.2E7768A0.tlog/link.command.1.tlog').read_text(encoding='utf-16').splitlines()[1:])
for option,name in [('OUT','flutter_inappwebview_windows_plugin.dll'),('IMPLIB','flutter_inappwebview_windows_plugin.lib'),('PDB','plugin.pdb')]:
    link,count=re.subn(r'/'+option+r':(?:"[^"]*"|\S+)',lambda _: '/'+option+':"'+str(p/name)+'"',link,flags=re.I)
    if option in ('OUT','IMPLIB'):assert count==1
assert link.count(r'..\..\FLUTTER\RELEASE\FLUTTER_WRAPPER_PLUGIN.LIB')==1
link=link.replace(r'..\..\FLUTTER\RELEASE\FLUTTER_WRAPPER_PLUGIN.LIB','"'+str(p/'flutter_wrapper_plugin.lib')+'"')
(p/'link.rsp').write_text(link)
commands=['@echo off','call "C:\\Program Files (x86)\\Microsoft Visual Studio\\18\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x64 -host_arch=x64','if errorlevel 1 exit /b 1','cd /d "'+str(build/'flutter')+'"',f'cl @"{p}/compile.rsp"','if errorlevel 1 exit /b 1',f'lib @"{p}/lib.rsp"','if errorlevel 1 exit /b 1','cd /d "'+str(plugin)+'"',f'link @"{p}/link.rsp"','exit /b %errorlevel%']
(p/'build.cmd').write_text('\n'.join(commands)+'\n')
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest()
(p/'source.json').write_text(json.dumps({'scope':'Isolated component candidate: only null-handler callback cleanup checks the existing engine-availability API. Product files and SDK unchanged.','originalSource':str(source),'originalSha256':sha(source),'candidateSha256':sha(p/'core_implementations.cc'),'priorPluginSha256':sha(plugin/'Release/flutter_inappwebview_windows_plugin.dll')},indent=2)+'\n')
print('Prepared isolated wrapper candidate')
