@echo off
cd /d "%~dp0"
if not exist "build\th01-native.exe" (
  echo Please run: python tools\build.py
  pause
  exit /b 1
)
start "" "build\th01-native.exe"
