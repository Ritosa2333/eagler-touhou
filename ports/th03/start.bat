@echo off
setlocal
cd /d "%~dp0"
title TH03 Local Multiplayer
where py >nul 2>nul
if not errorlevel 1 (
    py -3 -c "import sys" >nul 2>nul
    if not errorlevel 1 (
        py -3 "%~dp0server.py"
        goto end
    )
)
where python >nul 2>nul
if not errorlevel 1 (
    python "%~dp0server.py"
    goto end
)
if exist "%USERPROFILE%\miniconda3\python.exe" (
    "%USERPROFILE%\miniconda3\python.exe" "%~dp0server.py"
    goto end
)
echo Python 3 was not found.
:end
if errorlevel 1 pause
endlocal
