@echo off
cd /d "%~dp0"
echo TH02 public play - keep this window open.
python -u public_tunnel.py
pause
