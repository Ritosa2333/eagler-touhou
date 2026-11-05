@echo off
chcp 65001 >nul
setlocal
cd /d "%~dp0"
set "TH09_NODE=node"
if exist "..\th10_web\tools\node.exe" set "TH09_NODE=..\th10_web\tools\node.exe"
if exist "..\..\..\th10_web\tools\node.exe" set "TH09_NODE=..\..\..\th10_web\tools\node.exe"
set "TH09_SERVER=scripts\serve.mjs"
if exist "artifacts\sdl-release\scripts\serve.mjs" set "TH09_SERVER=artifacts\sdl-release\scripts\serve.mjs"
echo 花映塚启动后，请在浏览器打开 http://127.0.0.1:8096/app/th09.html
echo 此窗口需要保持开启。按 Ctrl+C 关闭服务。
"%TH09_NODE%" "%TH09_SERVER%" --port 8096
if errorlevel 1 echo 启动失败：请检查上面的错误信息。独立运行需要安装 Node.js 22 或更高版本。
pause
