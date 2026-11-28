@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1
cd /d "D:\Temp\native-performance-20261128\symbols"
cl /nologo /O2 /EHsc /W4 resolve-engine.cpp /link /OUT:resolve-engine.exe
exit /b %errorlevel%
