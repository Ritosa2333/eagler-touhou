@echo off
chcp 65001 >nul
setlocal
cd /d "%~dp0"
set "TH15_NODE=%~dp0..\th10_web\tools\node.exe"
if not exist "%TH15_NODE%" set "TH15_NODE=node"
echo 请打开 http://127.0.0.1:3007/ ，保持本窗口开启。
"%TH15_NODE%" "%~dp0artifacts\sdl-release\scripts\serve.mjs" --port 3007
echo 服务已停止。如果启动失败，请保留上方错误信息。
pause
