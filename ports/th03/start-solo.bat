@echo off
setlocal
cd /d "%~dp0"
title TH03 DOSBox-X Solo
python "%~dp0server.py" --page solo.html
if errorlevel 1 pause
endlocal
