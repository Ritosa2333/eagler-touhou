@echo off
setlocal
cd /d "%~dp0"
python -m pip install -r requirements-relay.txt
pause
endlocal
