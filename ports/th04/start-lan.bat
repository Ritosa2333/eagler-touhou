@echo off
cd /d "%~dp0"
echo TH04 DOSBox-X input-sync edition - port 9868
python lan_server.py --port 9868
pause
