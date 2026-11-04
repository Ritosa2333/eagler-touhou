@echo off
chcp 65001 >nul
cd /d "%~dp0"
echo TH19: http://127.0.0.1:8130/
echo Keep this window open while playing.

node serve.mjs 8130 --open
pause



