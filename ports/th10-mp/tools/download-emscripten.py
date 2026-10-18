"""Fetch the pinned official SDK with bounded concurrent HTTP range reads."""
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from urllib.request import Request, urlopen
import base64, hashlib, json, os, time, zipfile, shutil
root = Path(__file__).resolve().parent.parent
tag = 'f04ea239d533260dd1db760dd2d668d5f9a88d6b'
url = f'https://storage.googleapis.com/webassembly/emscripten-releases-builds/win/{tag}/wasm-binaries.zip'
out = root / 'tools/emsdk/downloads/wasm-binaries.zip'
out.parent.mkdir(parents=True, exist_ok=True)
with urlopen(Request(url, method='HEAD'), timeout=40) as response:
    size = int(response.headers['Content-Length'])
    hashes = response.headers.get('x-goog-hash', '')
print(f'Official Emscripten 6.0.9: {size / 1048576:.1f} MiB', flush=True)
part = out.with_suffix('.zip.parts'); part.mkdir(exist_ok=True)
block = 4 * 1024 * 1024
def fetch(index):
    start, end = index * block, min(size, (index+1)*block)-1
    dest = part / str(index)
    if dest.exists() and dest.stat().st_size == end-start+1:
        return dest
    for retry in range(4):
        try:
            with urlopen(Request(url, headers={'Range': f'bytes={start}-{end}'}), timeout=55) as r:
                if r.status != 206 or r.headers['Content-Range'] != f'bytes {start}-{end}/{size}':
                    raise ValueError('Server did not honor the exact range')
                data = r.read()
            if len(data) != end-start+1: raise ValueError('Incomplete range')
            dest.write_bytes(data); return dest
        except Exception:
            if retry == 3: raise
count = (size+block-1)//block
with ThreadPoolExecutor(max_workers=12) as pool:
    done = 0
    for f in as_completed([pool.submit(fetch, i) for i in range(count)]):
        f.result(); done += 1
        if done % 8 == 0 or done == count: print(f'Downloaded {done}/{count} blocks', flush=True)
digest=hashlib.md5()
with out.open('wb') as result:
    for i in range(count):
        data=(part/str(i)).read_bytes();result.write(data);digest.update(data)
expected = next((s.strip()[4:] for s in hashes.split(',') if s.strip().startswith('md5=')), None)
if expected and base64.b64encode(digest.digest()).decode() != expected:
    raise ValueError('Official object MD5 verification failed')
with zipfile.ZipFile(out) as archive:
    if archive.testzip() is not None: raise ValueError('SDK ZIP entry CRC verification failed')
    destination = (root/'tools/emsdk').resolve()
    for name in archive.namelist():
        resolved = (destination/name).resolve()
        if not resolved.is_relative_to(destination): raise ValueError('Unsafe archive path')
    print('Extracting: '+archive.namelist()[0], flush=True)
    archive.extractall(destination)
config = root/'tools/emsdk/.emscripten'
node = os.environ.get('TH_NODE') or str(root/'th10_web/tools/node.exe')
if not Path(node).is_file(): node = shutil.which('node')
if not node: raise ValueError('Set TH_NODE to the absolute path of tools/node.exe or install Node.js')
config.write_text('\n'.join([
    'LLVM_ROOT = '+repr(str(root/'tools/emsdk/install/bin')),
    'BINARYEN_ROOT = '+repr(str(root/'tools/emsdk/install')),
    'NODE_JS = ['+repr(node)+']',
    'CACHE = '+repr(str(root/'tools/emsdk/install/emscripten/cache')),
    'EMSCRIPTEN_ROOT = '+repr(str(root/'tools/emsdk/install/emscripten')),
])+'\n', encoding='utf-8')
(root/'tools/emsdk/touhou-sdk.json').write_text(json.dumps({'version':'6.0.9','url':url,'size':size,'md5':digest.hexdigest()},indent=2)+'\n')
print('SDK verified and extracted', flush=True)
