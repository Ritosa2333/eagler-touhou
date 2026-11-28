@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1
cl /nologo /O2 /EHsc /MD /LD device_lifetime.cpp /link /OUT:device_lifetime.dll d3d11.lib runtimeobject.lib CoreMessaging.lib
if errorlevel 1 exit /b 1
cl /nologo /O2 /EHsc /MD device_host.cpp /link /OUT:device_host.exe
exit /b %errorlevel%
