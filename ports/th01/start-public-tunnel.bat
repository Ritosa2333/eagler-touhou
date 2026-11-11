@echo off
setlocal
cd /d "%~dp0"
where cloudflared >nul 2>nul
if errorlevel 1 goto missing
echo Keep start-lan.bat running on port 9874 first.
echo This exposes the TH01 web directory and room API through a temporary URL.
echo UDP, TURN-only and TCP game URLs will be printed and saved to public-test-url.txt.
echo All players must open the same public URL, including the host.
echo TURN requires configure-turn.bat. Quick Tunnel is not a TURN server.
echo Keep both windows open. Ctrl+C stops the public tunnel.
python -u public_tunnel.py
goto done
:missing
echo cloudflared was not found in PATH.
echo Install it with: winget install --id Cloudflare.cloudflared --exact
echo After installing, open this launcher again.
:done
pause
endlocal
