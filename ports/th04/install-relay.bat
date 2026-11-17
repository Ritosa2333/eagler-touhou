@echo off
cd /d "%~dp0"
echo Installing WebSocket relay dependencies for this computer...
python -m pip install -r requirements-relay.txt
if errorlevel 1 goto failed
echo Installation finished. Restart start-lan.bat.
goto done
:failed
echo Installation failed. Keep this window open and copy the error above.
:done
pause
