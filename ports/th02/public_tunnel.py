"""Start/reuse the TH02 backend and share a temporary HTTPS/WSS game URL."""
import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time
from urllib.error import URLError
from urllib.request import urlopen

ROOT=Path(__file__).resolve().parent

def health(port):
    try:
        with urlopen(f'http://127.0.0.1:{port}/api/health',timeout=2) as response:
            value=json.load(response)
        if value.get('game')!='th02' or value.get('protocol')!='th02-rollback/6':
            raise RuntimeError('Port belongs to another service. Choose --port or stop that service yourself.')
        if value.get('connectionFallback')!=1:
            raise RuntimeError('TH02 server is outdated. Close the old start-lan/start-public window, then run start-public.bat again.')
        return True
    except (URLError,TimeoutError,ConnectionError):
        return False

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--port',type=int,default=19822)
    args=parser.parse_args()
    cloudflared=shutil.which('cloudflared')
    if not cloudflared:
        raise SystemExit('cloudflared not found. Install: winget install --id Cloudflare.cloudflared --exact')
    try:import websockets  # noqa: F401
    except ImportError:
        raise SystemExit('Install relay dependency: python -m pip install -r requirements-relay.txt')
    destination=ROOT/'public-test-url.txt'
    destination.write_text('Waiting for this session to establish a public URL.\n',encoding='utf-8')
    backend=None
    process=None
    flags=subprocess.CREATE_NO_WINDOW if os.name=='nt' else 0
    log=None
    try:
        if not health(args.port):
            (ROOT/'reports').mkdir(parents=True,exist_ok=True)
            log=(ROOT/'reports/public-server.log').open('w',encoding='utf-8')
            backend=subprocess.Popen([sys.executable,'-u',str(ROOT/'lan_server.py'),'--bind','0.0.0.0','--port',str(args.port)],cwd=ROOT,stdout=log,stderr=subprocess.STDOUT,creationflags=flags)
            for _ in range(50):
                if backend.poll() is not None:raise RuntimeError('TH02 server failed; see reports/public-server.log')
                if health(args.port):break
                time.sleep(.1)
            else:raise RuntimeError('TH02 server did not become ready')
        print(f'TH02 backend ready on port {args.port}. Establishing HTTPS/WSS tunnel...',flush=True)
        process=subprocess.Popen([cloudflared,'tunnel','--url',f'http://127.0.0.1:{args.port}'],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace',creationflags=flags)
        last=None
        for line in process.stdout:
            print(line,end='',flush=True)
            match=re.search(r'https://[a-z0-9-]+\.trycloudflare\.com\b',line)
            if match and match[0]!=last:
                last=match[0]
                url=last+'/lan.html?network=public&transport=ws'
                destination.write_text(url+'\n',encoding='utf-8')
                print(f'\nGAME URL — all players open this complete URL:\n{url}\nSaved: {destination}\nKeep this window open. Ctrl+C stops public access.\n',flush=True)
        return process.wait()
    except KeyboardInterrupt:
        return 130
    finally:
        for child in (process,backend):
            if child and child.poll() is None:
                child.terminate()
                try:child.wait(timeout=5)
                except subprocess.TimeoutExpired:child.kill();child.wait()
        if log:log.close()
        destination.write_text('Tunnel stopped. Run start-public.bat for a new URL.\n',encoding='utf-8')

if __name__=='__main__':raise SystemExit(main())
