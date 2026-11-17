@echo off
cd /d "%~dp0"
where cloudflared >nul 2>nul
if errorlevel 1 goto missing
echo Keep start-lan.bat running on port 9868 first.
echo This exposes the lab web directory and room API through a temporary URL.
echo The complete WebSocket game URL will be printed as GAME URL.
echo It will also be saved to public-test-url.txt in this folder.
echo Both players must open that full URL. Use different networks to test WAN.
echo Keep both windows open. Ctrl+C stops the public tunnel.
python -u public_tunnel.py
goto done
:missing
echo cloudflared was not found in PATH.
echo Install it with: winget install --id Cloudflare.cloudflared --exact
echo After installing, open this launcher again.
:done
pause
