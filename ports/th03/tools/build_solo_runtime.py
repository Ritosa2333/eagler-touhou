from mobile_runtime import optimize_mobile
"""Cache audited static dispatch while preserving the original solo scheduler."""
from pathlib import Path
import hashlib
import json
from optimize_np21_dispatch import optimize_dispatch

ROOT = Path(__file__).resolve().parents[1]


def build():
    vendor = ROOT / 'web/vendor/np2'
    source = optimize_dispatch((vendor / 'np21.js').read_text(encoding='utf-8'), vendor)
    anchor = 'var iterFunc=getWasmTableEntry(func);'
    assert source.count(anchor) == 1
    source = source.replace(anchor, anchor +
        'const rawLocalFrame=iterFunc;iterFunc=()=>{const observe=Module.observeLocalFrame;'
        'if(!observe){rawLocalFrame();return;}const at=performance.now();'
        'rawLocalFrame();observe(performance.now()-at);};')
    source=optimize_mobile(source,vendor,'local')
    (vendor / 'np21-solo.js').write_text(source, encoding='utf-8', newline='\n')
    manifest = {'dispatch': 'static-table-cache-v1', 'scheduler': 'raf-batch-2',
                'native_clock': 'unchanged', 'wasm': 'np21-mobile.wasm',
                'source_sha256': hashlib.sha256((vendor / 'np21.js').read_bytes()).hexdigest(),
                'wasm_sha256': hashlib.sha256((vendor / 'np21-mobile.wasm').read_bytes()).hexdigest(),
                'adapter_sha256': hashlib.sha256(source.encode()).hexdigest()}
    (vendor / 'solo-runtime.json').write_text(json.dumps(manifest, indent=2)+'\n', encoding='utf-8')
    return manifest


if __name__ == '__main__':
    build()
    print('Built solo dispatch cache; original scheduling, audio and WASM preserved')
