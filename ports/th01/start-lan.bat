@echo off
cd /d "%~dp0"
if not exist "web\netplay\runtime.json" (
  echo Please run: python tools\build.py
  pause
  exit /b 1
)
python lan_server.py
pause
