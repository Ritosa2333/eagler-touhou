import argparse
import mimetypes
import re
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import unquote, urlsplit


PROJECT = Path(__file__).resolve().parents[1]
SITE_FILES = {'index.html', 'app.mjs', 'inspector.mjs', 'style.css', 'play.html', 'play.mjs', 'play.css',
              'runtime/core.mjs', 'runtime/core.wasm', 'runtime/draw.mjs', 'runtime/timing.mjs', 'runtime/renderer.mjs',
              'runtime/replay.mjs', 'runtime/build.json', 'runtime/sound.mjs', 'runtime/background.mjs',
              'runtime/last-name-field.mjs', 'runtime/status.json',
              'runtime/native-ending-browser-host-v4.mjs', 'runtime/native-ending-bridge-v6.mjs',
              'runtime/native-ending-pixels.mjs', 'runtime/renderer-owned-ending-v2.mjs'}


def resolve_request(request, site, local):
    try:
        path = unquote(urlsplit(request).path, errors='strict')
    except (UnicodeError, ValueError):
        return None
    if '\\' in path or '\0' in path or '%' in path or '..' in path.split('/'):
        return None
    site, local = site.resolve(), local.resolve()
    if path == '/':
        return site / 'index.html'
    if path.removeprefix('/') in SITE_FILES:
        return site / path[1:]
    runtime_build = re.fullmatch(r'/runtime/builds/[a-f0-9]{64}/core\.(?:mjs|wasm)', path)
    if runtime_build:
        candidate = (site / path[1:]).resolve()
        return candidate if candidate.is_relative_to(site) else None
    if path == '/assets/catalog.json':
        return local / 'catalog.json'
    if path in ('/assets/runtime/bullet-profile-8.bin','/assets/runtime/bullet-profile-18.bin','/assets/runtime/bullet-profile-20.bin','/assets/runtime/sound-bindings.json'):
        candidate = (local / 'runtime' / path.rsplit('/',1)[1]).resolve()
        return candidate if candidate.is_relative_to(local) else None
    match = re.fullmatch(r'/assets/(generations/[a-f0-9]{32}/(?:ja|zh)/)(.+)', path)
    if not match:
        return None
    suffix = match[2]
    runtime_resource = re.fullmatch(r'runtime/([A-Za-z0-9_-]+\.(?:anm|ecl|std|msg|sht|rpy))', suffix)
    if runtime_resource:
        candidate = (local / match[1] / 'raw' / runtime_resource[1]).resolve()
        return candidate if candidate.is_relative_to(local) else None
    allowed = suffix in ('manifest.json', 'native-ending-resources.json') or bool(re.fullmatch(
        r'(?:textures/[A-Za-z0-9_-]+\.png|animations/[A-Za-z0-9_-]+\.json|'
        r'ecl/[A-Za-z0-9_-]+\.(?:json|txt)|(?:sounds|music)/[A-Za-z0-9_-]+\.wav|'
        r'native-ending-glyphs/e0[0-5]-line[0-9]{3}\.a4r4g4b4)', suffix))
    if not allowed:
        return None
    candidate = (local / match[1] / suffix).resolve()
    return candidate if candidate.is_relative_to(local) else None


def parse_range(header, length):
    if not header:
        return 0, length - 1, False
    match = re.fullmatch(r'bytes=([0-9]*)-([0-9]*)', header)
    if not match or not any(match.groups()) or length <= 0:
        raise ValueError('unsupported range')
    if not match[1]:
        count = int(match[2])
        if count <= 0:
            raise ValueError('empty suffix')
        start, end = max(0, length - count), length - 1
    else:
        start = int(match[1])
        end = min(int(match[2]), length - 1) if match[2] else length - 1
    if start >= length or end < start:
        raise ValueError('unsatisfiable range')
    return start, end, True


def make_handler(site, local):
    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            self.respond(False)

        def do_HEAD(self):
            self.respond(True)

        def respond(self, head_only):
            path = resolve_request(self.path, site, local)
            if path is None or not path.is_file():
                self.send_error(404, 'Only the inspector and prepared public assets are available')
                return
            length = path.stat().st_size
            try:
                start, end, partial = parse_range(self.headers.get('Range'), length)
            except ValueError:
                self.send_response(416)
                self.send_header('Content-Range', f'bytes */{length}')
                self.send_header('Content-Length', '0')
                self.end_headers()
                return
            content_type = {'.mjs': 'text/javascript; charset=utf-8', '.wasm': 'application/wasm', '.json': 'application/json; charset=utf-8',
                            '.txt': 'text/plain; charset=utf-8'}.get(path.suffix)
            content_type = content_type or mimetypes.guess_type(path.name)[0] or 'application/octet-stream'
            self.send_response(206 if partial else 200)
            self.send_header('Content-Type', content_type)
            self.send_header('Content-Length', str(max(0, end - start + 1)))
            self.send_header('Accept-Ranges', 'bytes')
            self.send_header('Cache-Control', 'no-cache')
            self.send_header('X-Content-Type-Options', 'nosniff')
            self.send_header('Content-Security-Policy', "default-src 'self'; script-src 'self' 'wasm-unsafe-eval'; style-src 'self'; "
                             "img-src 'self' data:; media-src 'self'; connect-src 'self'; object-src 'none'; base-uri 'none'")
            if partial:
                self.send_header('Content-Range', f'bytes {start}-{end}/{length}')
            self.end_headers()
            if head_only:
                return
            try:
                with path.open('rb') as source:
                    source.seek(start)
                    remaining = end - start + 1
                    while remaining > 0:
                        block = source.read(min(remaining, 65536))
                        if not block:
                            break
                        self.wfile.write(block)
                        remaining -= len(block)
            except (BrokenPipeError, ConnectionResetError):
                return

    return Handler


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only TH128 reconstruction workbench (gameplay incomplete).')
    parser.add_argument('--port', type=int, default=8128)
    parser.add_argument('--local', type=Path, default=PROJECT / 'local')
    arguments = parser.parse_args(argv)
    if not (arguments.local / 'catalog.json').is_file():
        parser.error('No prepared catalog; run tools/prepare.py first.')
    server = ThreadingHTTPServer(('127.0.0.1', arguments.port), make_handler(PROJECT / 'site', arguments.local))
    print(f'TH128 workbench: http://127.0.0.1:{server.server_port}/ (game incomplete; animation validation at /play.html)', flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == '__main__':
    main()
