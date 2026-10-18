"""Restore ignored local inputs without adding game assets or binaries to Git."""
from pathlib import Path
import argparse, hashlib, json, shutil

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('delivery', type=Path)
args = parser.parse_args()
source = args.delivery.resolve()
root = Path(__file__).resolve().parents[1]
assert source != root, 'Select the original full development package'
manifest = json.loads((source / 'FILE-SHA256.json').read_text(encoding='utf-8'))
prefixes = (
    '[th15] 东方绀珠传 (汉化版+日文版)/',
    'th15_web/assets/', 'th15_web/reference/assets/', 'th15_web/reference/replays/',
    'th15_web/artifacts/cpp/analysis/',
    'th15_web/artifacts/cpp/verification/font-definitions-original.json',
    'th15_web/artifacts/native-process-replay/', 'th15_web/artifacts/native-process-extra/',
    'th15_web/tools/', 'th08_web/node_modules/', 'th09_web/node_modules/', 'th10_web/node_modules/',
    'th10_web/tools/', 'tools/architecture/typescript/', 'tools/architecture/python/',
)
native_suffixes = {'.wasm', '.dll', '.bin', '.decoded'}
paths = [name for name in manifest if name.startswith(prefixes) or
         (name.startswith('th15_web/reference/native/') and Path(name).suffix in native_suffixes)]
for name in paths:
    src = (source / name).resolve()
    out = (root / name).resolve()
    assert src.is_relative_to(source) and out.is_relative_to(root), 'Unsafe path: ' + name
    digest = hashlib.sha256(src.read_bytes()).hexdigest()
    assert digest == manifest[name], 'Input hash mismatch: ' + name
for name in paths:
    out = root / name
    out.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source / name, out)
print(json.dumps({'verifiedAndRestored': len(paths), 'inputsRemainUntracked': True}))
