@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1
cl /nologo /O2 /EHsc /MD device-adapter.cpp /link /OUT:adapter-default.exe d3d11.lib dxgi.lib
if errorlevel 1 exit /b 1
cl /nologo /O2 /EHsc /MD /DPREFER_NVIDIA device-adapter.cpp /link /OUT:adapter-nvidia.exe d3d11.lib dxgi.lib
exit /b %errorlevel%
