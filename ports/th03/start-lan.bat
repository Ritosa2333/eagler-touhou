@echo off
setlocal
cd /d "%~dp0"
title TH03 LAN
echo TH03 LAN edition - port 9877
python "%~dp0lan_server.py" --port 9877
if errorlevel 1 pause
endlocal
