@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1
cd /d "D:\workspace\eagler\artifacts\.native-room-client-win\build\windows\x64\runner"
rc /nologo /i "D:\workspace\eagler\artifacts\.native-room-client-win/windows/runner" /fo "D:\Temp\native-performance-20261128\gpu-preference/host.res" "D:\Temp\native-performance-20261128\gpu-preference/host.rc"
if errorlevel 1 exit /b 1
cl @"D:\Temp\native-performance-20261128\gpu-preference/main.rsp"
if errorlevel 1 exit /b 1
cl @"D:\Temp\native-performance-20261128\gpu-preference/window.rsp"
if errorlevel 1 exit /b 1
link @"D:\Temp\native-performance-20261128\gpu-preference/link.rsp"
exit /b %errorlevel%
