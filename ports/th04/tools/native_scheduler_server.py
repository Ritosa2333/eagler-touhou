"""Serve an isolated native candidate through the normal LAN room service."""
import argparse
from functools import partial
import hashlib
from http.server import ThreadingHTTPServer
import json
import os
from pathlib import Path
import sys
from urllib.parse import urlparse

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))
import lan_server

RUNTIME = ROOT / os.environ.get('NATIVE_RUNTIME', 'build/native-scheduler/runtime-cpu-helpers')
ASSETS = {
    '/vendor/dosbox-x/' + name: RUNTIME / name
    for name in ['dosbox-rollback.mjs', 'dosbox-rollback-tracked.mjs',
                 'wdosbox-x-rollback.wasm', 'wdosbox-x-rollback-tracked.wasm',
                 'snapshot-diff-tracked.wasm']
}
ASSETS['/frame-presenter.js'] = ROOT / 'web/frame-presenter.js'


def digest(data):
    return hashlib.sha256(data).hexdigest()


def overlay():
    for file in ASSETS.values():
        if not file.is_file():
            raise SystemExit(f'Missing native candidate: {file}')
    data = {url: file.read_bytes() for url, file in ASSETS.items()}
    adapter = (ROOT / 'web/dosbox-adapter.js').read_text(encoding='utf-8')
    anchor = 'deterministic:net,'
    adapter = adapter.replace('nativeScheduling:true,nativeCpu:true,nativeFramePresentation:false,', '')
    if adapter.count(anchor) != 1:
        raise SystemExit('Native adapter anchor changed')
    adapter = adapter.replace(anchor, anchor +
        'nativeScheduling:true,nativeCpu:true,nativeFramePresentation:false,')
    data['/dosbox-adapter.js'] = adapter.encode()

    version = json.loads((ROOT / 'web/netplay/runtime.json').read_text(encoding='utf-8'))
    core = digest(data['/vendor/dosbox-x/wdosbox-x-rollback.wasm'])
    glue = digest(data['/vendor/dosbox-x/dosbox-rollback.mjs'])
    version.update(adapter='dosbox-x-native-scheduler-candidate-v1',
        wasm_sha256=core, mobile_wasm_sha256=core, generated_js_sha256=glue,
        dosbox_adapter_sha256=digest(data['/dosbox-adapter.js']),
        clock_sha256=digest(data['/dosbox-adapter.js']),
        frame_presenter_sha256=digest(data['/frame-presenter.js']))
    version['rollback_wasm'].update(sha256=core,
        continuation='Asyncify boundary with native frame scheduling and guarded CPU')
    version['write_journal'].update(
        wasm_sha256=digest(data['/vendor/dosbox-x/wdosbox-x-rollback-tracked.wasm']),
        glue_sha256=digest(data['/vendor/dosbox-x/dosbox-rollback-tracked.mjs']),
        diff_sha256=digest(data['/vendor/dosbox-x/snapshot-diff-tracked.wasm']))
    data['/netplay/runtime.json'] = json.dumps(version, ensure_ascii=False).encode()

    vendor = json.loads((ROOT / 'web/vendor/dosbox-x/runtime.json').read_text(encoding='utf-8'))
    for url, body in data.items():
        if url.startswith('/vendor/dosbox-x/'):
            vendor['files'][url.rsplit('/', 1)[1]] = digest(body)
    vendor['presentation']['sha256'] = digest(data['/frame-presenter.js'])
    vendor['network']['continuation'] = version['rollback_wasm']['continuation']
    vendor['network']['nativeFrameBridge'] = False
    data['/vendor/dosbox-x/runtime.json'] = json.dumps(vendor, ensure_ascii=False).encode()
    return data


class Handler(lan_server.Handler):
    candidate = {}

    def do_GET(self):
        path = urlparse(self.path).path
        body = self.candidate.get(path)
        if body is None:
            return super().do_GET()
        suffix = Path(path).suffix
        content_type = {'.wasm': 'application/wasm', '.js': 'text/javascript',
                        '.mjs': 'text/javascript', '.json': 'application/json'}[suffix]
        self.send_response(200)
        self.send_header('Content-Type', content_type)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--bind', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=19872)
    args = parser.parse_args()
    Handler.candidate = overlay()
    ThreadingHTTPServer.request_queue_size = 128
    server = ThreadingHTTPServer((args.bind, args.port),
        partial(Handler, directory=str(ROOT / 'web')))
    print(f'Native scheduler candidate (not accepted): http://127.0.0.1:{args.port}/lan.html?network=lan', flush=True)
    print('Every player must use this candidate server. Shipped assets are unchanged.', flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
