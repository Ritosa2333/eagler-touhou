"""Serve TH02 local cooperation on localhost, with the correct WASM MIME type."""
import argparse
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import threading
import webbrowser

class Handler(SimpleHTTPRequestHandler):
    extensions_map = {**SimpleHTTPRequestHandler.extensions_map, '.js':'text/javascript', '.mjs':'text/javascript', '.wasm':'application/wasm'}
    def end_headers(self):
        self.send_header('Cache-Control', 'no-store')
        super().end_headers()
    def list_directory(self, path):
        self.send_error(404)
    def do_GET(self):
        if self.path == '/':
            self.path = '/index.html'
        super().do_GET()

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--port',type=int,default=9872)
    parser.add_argument('--no-browser',action='store_true')
    args=parser.parse_args()
    root=Path(__file__).resolve().parent/'web'
    for port in range(args.port,args.port+20):
        try:
            server=ThreadingHTTPServer(('127.0.0.1',port),partial(Handler,directory=str(root)))
            break
        except OSError as error:
            if error.errno not in (98,10048):raise
    else:raise SystemExit('No free localhost port')
    url=f'http://127.0.0.1:{port}/index.html'
    print(f'TH02 游戏启动器：{url}',flush=True)
    if not args.no_browser:threading.Timer(.5,lambda:webbrowser.open(url)).start()
    try:server.serve_forever()
    except KeyboardInterrupt:pass
    finally:server.server_close()

if __name__=='__main__':main()
