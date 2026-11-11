"""Build audited mobile WASM and adapt generated loaders; no deployment tooling needed."""
from pathlib import Path
import subprocess

HERE=Path(__file__).resolve().parent/'mobile-runtime'

def optimize_mobile(source,vendor,mode='local'):
    subprocess.run(['node',str(HERE/'build.cjs'),mode,str(vendor)],check=True)
    return subprocess.run(['node',str(HERE/'patch.cjs'),mode],input=source,
                          stdout=subprocess.PIPE,encoding='utf-8',check=True).stdout
