@echo off
setlocal
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0Open-th09.ps1" %*
if errorlevel 1 (
    pause
    exit /b 1
)
