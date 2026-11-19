@echo off
setlocal
cd /d "%~dp0"
set "PODD_ROOT=%~dp0..\..\..\..\PoDD-Port-Preview\PoDD-Port-Fixed"
set "PODD_BUILD=%~dp0..\.cache\native-solo"
if not exist "%PODD_BUILD%\classic.wasm" (
  echo Missing native browser build: %PODD_BUILD%
  pause
  exit /b 1
)
if not exist "%PODD_ROOT%\content\MAIN.EXE" (
  echo Missing TH03 content: %PODD_ROOT%\content
  pause
  exit /b 1
)
echo Open http://127.0.0.1:18332/
echo Keep this window open while playing.
node "%~dp0serve.mjs" "--build=%PODD_BUILD%" "--content=%PODD_ROOT%\content" "--ui=%PODD_ROOT%\fixed-trial\PODDUI" --port=18332
pause
