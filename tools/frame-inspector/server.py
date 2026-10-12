"""Local-only frame inspector; launch an isolated original, never attach to a user's run."""
import argparse
import ctypes
import hashlib
import json
import mimetypes
import shutil
import sys
import threading
from http.server import ThreadingHTTPServer,BaseHTTPRequestHandler
from pathlib import Path
from urllib.parse import urlparse,unquote
from profile import COMMON_REVISION,prepare_config,verify_executable
from windows_debugger import FrameDebugger

HERE=Path(__file__).resolve().parent;TITLE=HERE.parents[1];WORKSPACE=TITLE.parent

def prepare(source,directory):
    source=source.resolve(); directory=directory.resolve()
    if not directory.is_relative_to(TITLE/'local/frame-inspector'):
        raise ValueError('Isolated game must stay inside local/frame-inspector.')
    verify_executable(source/'th12.exe')
    directory.mkdir(parents=True,exist_ok=True)
    for name in ('th12.exe','th12.dat','thbgm.dat'):
        origin=source/name; target=directory/name
        if not origin.is_file():raise ValueError('Missing original file: '+name)
        if not target.is_file() or target.stat().st_size!=origin.stat().st_size:
            shutil.copy2(origin,target)
    verify_executable(directory/'th12.exe')
    (directory/'th12.cfg').write_bytes(prepare_config((source/'th12.cfg').read_bytes()))
    replay=directory/'replay';replay.mkdir(exist_ok=True)
    for path in (source/'replay').glob('*.rpy'):shutil.copy2(path,replay/path.name)
    # Score/settings writes belong to the isolated copy.
    if not (directory/'scoreth12.dat').exists() and (source/'scoreth12.dat').exists():
        shutil.copy2(source/'scoreth12.dat',directory/'scoreth12.dat')
    return directory/'th12.exe'

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--original',type=Path,default=WORKSPACE/'th12')
    parser.add_argument('--common',type=Path,default=WORKSPACE/'reference/eagler-common')
    parser.add_argument('--port',type=int,default=8113)
    args=parser.parse_args()
    common=args.common.resolve()
    # Pin imported source bytes, not just a moving branch label.
    pin=json.loads((HERE/'common-pin.json').read_text(encoding='utf8'))
    for name,digest in pin['files'].items():
        if hashlib.sha256((common/name).read_bytes()).hexdigest()!=digest:raise ValueError('eagler-common source pin mismatch: '+name)
    exe=prepare(args.original,TITLE/'local/frame-inspector/retail')
    output=TITLE/'artifacts/frame-inspector'
    debugger=None
    def launch():
        nonlocal debugger
        if debugger is not None and not debugger.closed:raise ValueError('检查副本已经运行。')
        k=ctypes.WinDLL('kernel32',use_last_error=True)
        k.OpenMutexW.argtypes=[ctypes.c_uint32,ctypes.c_int,ctypes.c_wchar_p];k.OpenMutexW.restype=ctypes.c_void_p
        k.CloseHandle.argtypes=[ctypes.c_void_p]
        handle=k.OpenMutexW(0x100000,False,'Touhou 12 App')
        if handle:
            k.CloseHandle(handle)
            raise ValueError('原版只允许一个实例：请先关闭已有星莲船窗口，再启动检查副本。')
        debugger=FrameDebugger(exe,common,output)
    class Handler(BaseHTTPRequestHandler):
        def log_message(self,*_):pass
        def allowed(self):
            return self.headers.get('Host') in (f'127.0.0.1:{args.port}',f'localhost:{args.port}')
        def send(self,status,body,kind='application/json'):
            self.send_response(status);self.send_header('Content-Type',kind);self.send_header('Cache-Control','no-store')
            self.send_header('X-Content-Type-Options','nosniff');self.send_header('Content-Length',str(len(body)))
            self.end_headers();self.wfile.write(body)
        def do_GET(self):
            if not self.allowed():self.send(403,b'{}');return
            path=unquote(urlparse(self.path).path)
            if path=='/api/state':self.send(200,json.dumps(debugger.state() if debugger else {'status':'idle','commonRevision':COMMON_REVISION}).encode());return
            if path=='/api/frame.png':file=output/'latest.png'
            elif path=='/api/snapshot.json':file=output/'latest.json'
            elif path=='/':file=HERE/'index.html'
            elif path=='/inspector.mjs':file=HERE/'inspector.mjs'
            elif path=='/auto-scan.mjs':file=HERE/'auto-scan.mjs'
            elif path=='/candidate':file=TITLE/'site/index.html'
            else:
                root=TITLE/'src' if path.startswith('/src/') else TITLE/'site'
                relative=path[5:] if path.startswith('/src/') else path[1:]
                file=(root/relative).resolve()
                if not file.is_relative_to(root.resolve()):self.send(404,b'{}');return
            if not file.is_file():self.send(404,b'{}');return
            kind={'.mjs':'text/javascript','.wasm':'application/wasm'}.get(file.suffix,mimetypes.guess_type(file)[0] or 'application/octet-stream')
            self.send(200,file.read_bytes(),kind)
        def do_POST(self):
            if not self.allowed() or self.headers.get('Origin')!=f'http://{self.headers.get("Host")}':self.send(403,b'{}');return
            if self.path!='/api/command':self.send(404,b'{}');return
            try:
                length=int(self.headers.get('Content-Length','0'))
                if not 0<length<=256*1024:raise ValueError('Invalid request size')
                command=json.loads(self.rfile.read(length));kind=command.get('kind')
                if kind=='launch':launch();self.send(202,b'{"queued":true}');return
                if kind not in ('pause','resume','step','until','capture','close','scan-batch'):raise ValueError('Unknown command')
                if debugger is None:raise ValueError('请先启动检查副本。')
                if kind=='scan-batch':
                    if debugger.state().get('status')!='paused':raise ValueError('先停住原版再开始检查')
                    from scan import ScanBatch
                    ScanBatch(command['base'],command['rows'])
                    if not isinstance(command.get('batchId'),str) or len(command['batchId'])>80:raise ValueError('检查批次无效')
                    debugger.command(kind,base=command['base'],rows=command['rows'],identity=command['identity'],batchId=command['batchId'])
                    self.send(202,b'{"queued":true}');return
                if kind=='until':
                    stage,frame=command.get('stage'),command.get('frame')
                    if type(stage)!=int or type(frame)!=int or not 1<=stage<=7 or not 0<=frame<=600000:raise ValueError('Invalid target frame')
                    state=debugger.state()
                    if state.get('replayActive') and state.get('stage')==stage and frame<=state.get('replayCursor',0):raise ValueError('目标必须晚于当前帧；倒回请在原版重新播放该关。')
                debugger.command(kind,**{k:command[k] for k in ('stage','frame') if k in command})
                self.send(202,b'{"queued":true}')
            except Exception as error:self.send(400,json.dumps({'error':str(error)},ensure_ascii=False).encode('utf8'))
    server=ThreadingHTTPServer(('127.0.0.1',args.port),Handler)
    print(f'TH12 frame inspector http://127.0.0.1:{args.port}/',flush=True)
    try:server.serve_forever()
    finally:
        if debugger:debugger.closed=True
        server.server_close()

if __name__=='__main__':main()
