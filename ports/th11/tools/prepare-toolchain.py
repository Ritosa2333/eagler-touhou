"""Retain the bundled SDK cache when this pinned development package moves."""
from pathlib import Path
import json

root = Path(__file__).resolve().parents[1]
sdk = root / 'tools/emsdk'
if not sdk.exists():
    # This file is installed at <delivery>/tools/prepare-toolchain.py.
    sdk = root / 'emsdk'
sanity = sdk / 'install/emscripten/cache/sanity.txt'
llvm = sdk / 'install/bin'
if sanity.is_file():
    recorded = sanity.read_text(encoding='utf-8').strip()
    version, separator, old_path = recorded.partition('|')
    # The bundled compiler is pinned; relocating it must not erase offline
    # SDL port sources/libraries. Removing only the old location stamp makes
    # Emscripten perform its normal compiler/node checks and write a new stamp.
    if separator and Path(old_path) != llvm:
        sanity.unlink()
print(json.dumps({'sdk': str(sdk), 'offlineCachePreserved': True}))
