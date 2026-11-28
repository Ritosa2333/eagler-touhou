@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1
cd /d "D:\workspace\eagler\artifacts\.native-room-client-win\build\windows\x64\runner"
cl @"C:\Users\Ritosa\AppData\Local\Temp\native-process-proof-20261128\webview-adapter\startup-compile.rsp"
if errorlevel 1 exit /b 1
link @"C:\Users\Ritosa\AppData\Local\Temp\native-process-proof-20261128\webview-adapter\startup-link.rsp"
exit /b %errorlevel%
