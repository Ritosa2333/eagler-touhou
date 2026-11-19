@echo off
cd /d "%~dp0.."
set "NATIVE_RUNTIME=build/native-scheduler/runtime-tablet"
python tools/native_scheduler_server.py --bind 0.0.0.0 --port 19872
pause
