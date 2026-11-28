import argparse
import mimetypes
from pathlib import Path
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import unquote, urlsplit
import json
import re

ROOT = Path(__file__).resolve().parent / 'site'

def handler(root):
    root = root.resolve()
    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            self.respond(False)
        def do_HEAD(self):
            self.respond(True)
        def respond(self, head):
            try:
                request = unquote(urlsplit(self.path).path, errors='strict')
            except (ValueError, UnicodeError):
                self.send_error(400)
                return
            if '\\' in request or '\0' in request or '%' in request or '..' in request.split('/'):
                self.send_error(400)
                return
            if request == '/':
                self.send_response(302)
                self.send_header('Location', '/play.html')
                self.end_headers()
                return
            target = (root / request.lstrip('/')).resolve()
            if not target.is_relative_to(root) or not target.is_file() or target.suffix not in ['.mjs', '.css', '.html', '.json', '.wasm']:
                self.send_error(404)
                return
            length = target.stat().st_size
            start, end, partial = 0, length - 1, False
            requested_range = self.headers.get('Range')
            if requested_range:
                match = re.fullmatch(r'bytes=(\d*)-(\d*)', requested_range)
                if not match or not any(match.groups()):
                    self.send_error(416)
                    return
                if match[1]:
                    start = int(match[1])
                    if match[2]:
                        end = min(int(match[2]), end)
                else:
                    start = max(0, length - int(match[2]))
                if start >= length or end < start:
                    self.send_error(416)
                    return
                partial = True
            self.send_response(206 if partial else 200)
            content_type = {'.mjs': 'text/javascript; charset=utf-8', '.wasm': 'application/wasm', '.json': 'application/json; charset=utf-8'}.get(target.suffix, mimetypes.guess_type(target.name)[0] or 'application/octet-stream')
            self.send_header('Content-Type', content_type)
            self.send_header('Content-Length', str(end - start + 1))
            self.send_header('Accept-Ranges', 'bytes')
            self.send_header('Cache-Control', 'no-cache')
            self.send_header('X-Content-Type-Options', 'nosniff')
            self.send_header('Content-Security-Policy', "default-src 'self'; script-src 'self' 'wasm-unsafe-eval'; style-src 'self'; img-src 'self' data: blob:; media-src 'self' blob:; connect-src 'self'; object-src 'none'; base-uri 'none'")
            if partial:
                self.send_header('Content-Range', f'bytes {start}-{end}/{length}')
            self.end_headers()
            if head:
                return
            try:
                with target.open('rb') as stream:
                    stream.seek(start)
                    remaining = end - start + 1
                    while remaining > 0:
                        chunk = stream.read(min(65536, remaining))
                        if not chunk:
                            break
                        self.wfile.write(chunk)
                        remaining -= len(chunk)
            except (BrokenPipeError, ConnectionResetError):
                return
        def log_message(self, *args):
            pass
    return Handler

def main():
    parser = argparse.ArgumentParser(description='TH128 DAT + OGG Web Runtime')
    parser.add_argument('--port', type=int, default=8140)
    args = parser.parse_args()
    server = ThreadingHTTPServer(('127.0.0.1', args.port), handler(ROOT))
    server.daemon_threads = True
    build = json.loads((ROOT / 'runtime' / 'build.json').read_text(encoding='utf-8'))
    print(json.dumps({'ready': True, 'url': f'http://127.0.0.1:{server.server_port}/play.html', 'core': build['privateCore'], 'generation': build['generation'], 'resourceImport': 'DAT + OGG ZIP'}, ensure_ascii=False), flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()

if __name__ == '__main__':
    main()
