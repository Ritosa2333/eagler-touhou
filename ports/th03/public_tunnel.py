"""Print complete TH03 game URLs from a user-started Cloudflare Quick Tunnel."""
import argparse
import re
import subprocess
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=9877)
    args = parser.parse_args()
    if not 1 <= args.port <= 65535:
        parser.error('port must be in 1..65535')
    destination = Path(__file__).resolve().parent / 'public-test-url.txt'
    destination.write_text('Waiting for a new tunnel URL. Keep this launcher open.\n', encoding='utf-8')
    process = None
    try:
        process = subprocess.Popen(
            ['cloudflared', 'tunnel', '--url', f'http://127.0.0.1:{args.port}'],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, encoding='utf-8', errors='replace',
        )
        last_url = None
        for line in process.stdout:
            print(line, end='', flush=True)
            match = re.search(r'https://[a-z0-9-]+\.trycloudflare\.com\b', line)
            if not match or match.group(0) == last_url:
                continue
            last_url = match.group(0)
            base = last_url + '/lan.html?network=public'
            links = (f'UDP (STUN + configured TURN):\n{base}\n\n'
                     f'TURN only (configure TURN first):\n{base}&ice=relay\n\n'
                     f'TCP (WebSocket):\n{base}&transport=ws\n')
            destination.write_text(links, encoding='utf-8')
            print(f'\nGAME URLS (both players use the same URL):\n{links}\nSaved to: {destination}\n', flush=True)
        return process.wait()
    except FileNotFoundError:
        print('cloudflared not found. Install Cloudflare.cloudflared and restart this launcher.', flush=True)
        return 1
    except KeyboardInterrupt:
        return 130
    finally:
        if process is not None and process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill(); process.wait()
        destination.write_text('Tunnel stopped. Restart start-public-tunnel.bat to get a new URL.\n', encoding='utf-8')


if __name__ == '__main__':
    raise SystemExit(main())
