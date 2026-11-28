@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1
cd /d "D:\Temp\native-performance-20261128\symbols"
cl /nologo /O2 /EHsc /MT /W4 /std:c++20 /I"D:/workspace/eagler/toolchains/flutter-stable/engine/src" conversion-probe.cpp "D:/workspace/eagler/toolchains/flutter-stable/engine/src/flutter/fml/string_conversion.cc" /link /OUT:conversion-probe.exe
exit /b %errorlevel%

