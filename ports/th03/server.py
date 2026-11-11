"""Standalone loopback HTTP server for the TH03 local multiplayer project."""
import argparse
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import socket
import threading
from urllib.parse import unquote, urlsplit
import webbrowser

ROOT = Path(__file__).resolve().parent / 'web'


class Server(ThreadingHTTPServer):
    allow_reuse_address = False

    def server_bind(self):
        if hasattr(socket, 'SO_EXCLUSIVEADDRUSE'):
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)
        super().server_bind()


class Handler(SimpleHTTPRequestHandler):
    extensions_map = {**SimpleHTTPRequestHandler.extensions_map, '.js': 'text/javascript', '.mjs': 'text/javascript',
                      '.wasm': 'application/wasm', '.gz': 'application/octet-stream'}

    def send_head(self):
        path = unquote(urlsplit(self.path).path)
        if '\\' in path or any(part.startswith('.') for part in path.split('/') if part):
            self.send_error(404)
            return None
        return super().send_head()

    def list_directory(self, path):
        self.send_error(404)
        return None

    def end_headers(self):
        self.send_header('Cache-Control', 'no-cache')
        self.send_header('X-Content-Type-Options', 'nosniff')
        super().end_headers()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=9876)
    parser.add_argument('--no-browser', action='store_true')
    parser.add_argument('--page', choices=('index.html', 'solo.html', 'local.html'), default='index.html')
    args = parser.parse_args()
    for name in ('index.html', 'vendor/dosbox-x/wdosbox-x-fast.wasm', 'vendor/dosbox-x/wdosbox-x-rollback.wasm', 'disks/3-jp.hdi.gz', 'disks/3-cn.hdi.gz',
                 'native-patch.json', 'native/main.exe', 'native/start.com',
                 'native/game-jp.bat', 'native/game-cn.bat'):
        if not (ROOT / name).is_file():
            raise SystemExit('Missing asset: ' + name)
    for port in range(args.port, min(args.port + 20, 65536)):
        try:
            server = Server(('127.0.0.1', port), partial(Handler, directory=str(ROOT)))
            break
        except OSError as error:
            if error.errno not in (98, 10048):
                raise
    else:
        raise SystemExit('No free local port')
    url = f'http://127.0.0.1:{port}/{args.page}'
    print(f'服务目录：{ROOT.parent}', flush=True)
    print(f'TH03 local multiplayer\n{url}\nKeep this window open. Ctrl+C to stop.', flush=True)
    if not args.no_browser:
        threading.Timer(0.5, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == '__main__':
    main()
