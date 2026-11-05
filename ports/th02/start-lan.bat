@echo off
cd /d "%~dp0"
echo TH02 LAN / public room server - port 19822
python -c "import websockets" >nul 2>nul
if errorlevel 1 (
  echo Install the relay dependency first: python -m pip install -r requirements-relay.txt
  pause
  exit /b 1
)
python -u lan_server.py --bind 0.0.0.0 --port 19822
pause
