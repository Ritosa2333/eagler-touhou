"""Print and save the complete game URL from a user-started Quick Tunnel."""
import re
import subprocess
from pathlib import Path


def main():
    destination = Path(__file__).resolve().parent / 'public-test-url.txt'
    destination.write_text('Waiting for a new tunnel URL. Keep this launcher open.\n', encoding='utf-8')
    process = subprocess.Popen(
        ['cloudflared', 'tunnel', '--url', 'http://127.0.0.1:9868'],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, encoding='utf-8', errors='replace',
    )
    last_url = None
    try:
        for line in process.stdout:
            print(line, end='', flush=True)
            match = re.search(r'https://[a-z0-9-]+\.trycloudflare\.com\b', line)
            if not match:
                continue
            url = match.group(0) + '/lan.html?network=public&transport=ws'
            if url == last_url:
                continue
            last_url = url
            destination.write_text(url + '\n', encoding='utf-8')
            print(f'\nGAME URL (both players):\n{url}\nSaved to: {destination}\n', flush=True)
        return process.wait()
    except KeyboardInterrupt:
        return 130
    finally:
        if process.poll() is None:
            process.terminate()
            process.wait()
        destination.write_text('Tunnel stopped. Restart start-public-tunnel.bat to get a new URL.\n', encoding='utf-8')


if __name__ == '__main__':
    raise SystemExit(main())
