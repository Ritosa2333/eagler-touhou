"""为 Runtime 联调提供固定 MIME 的 loopback HTTP 服务。"""
from pathlib import Path
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import argparse

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', type=int, default=8173)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    class Handler(SimpleHTTPRequestHandler):
        extensions_map = {**SimpleHTTPRequestHandler.extensions_map, '.mjs': 'text/javascript', '.js': 'text/javascript', '.wasm': 'application/wasm', '.data': 'application/octet-stream', '.otf': 'font/otf'}
        def __init__(self, *values, **options):
            super().__init__(*values, directory=str(root), **options)
    print(f'Local: http://127.0.0.1:{args.port}/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data', flush=True)
    ThreadingHTTPServer(('127.0.0.1', args.port), Handler).serve_forever()

if __name__ == '__main__':
    main()
