@echo off
cd /d "%~dp0"
if not exist "web\game.wasm" (
  echo Please run: python tools\build.py
  pause
  exit /b 1
)
python server.py
