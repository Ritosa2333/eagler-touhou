"""Serve only the independent lab web directory, on localhost."""
import argparse
from functools import partial
from http.server import ThreadingHTTPServer
from web_server import AssetHandler
from pathlib import Path
import threading
import webbrowser

class Handler(AssetHandler):
    extensions_map = {**AssetHandler.extensions_map, '.js':'text/javascript', '.mjs':'text/javascript', '.wasm':'application/wasm'}
    def list_directory(self,path):
        self.send_error(404)

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--no-browser',action='store_true')
    parser.add_argument('--port',type=int,default=9867)
    parser.add_argument('--page',choices=['local.html','solo.html','index.html'],default='local.html')
    args=parser.parse_args()
    root=Path(__file__).resolve().parent/'web'
    for port in range(args.port,args.port+20):
        try:
            server=ThreadingHTTPServer(('127.0.0.1',port),partial(Handler,directory=str(root)))
            break
        except OSError as e:
            if e.errno not in (98,10048):raise
    else:raise SystemExit('No free port')
    url=f'http://127.0.0.1:{port}/{args.page}'
    print(f'TH04 player: {url} · web root: {Path(__file__).resolve().parent / "web"}',flush=True)
    if not args.no_browser:threading.Timer(.5,lambda:webbrowser.open(url)).start()
    try:server.serve_forever()
    except KeyboardInterrupt:pass
    finally:server.server_close()
