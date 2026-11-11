"""DOSBox-X build entry. Does not run a game or browser."""
import argparse
from pathlib import Path
import subprocess
ROOT = Path(__file__).resolve().parents[1]
def write_fingerprints():
    subprocess.run(['node', str(ROOT / 'tools/write_dosbox_manifest.cjs')], cwd=ROOT, check=True)
def build():
    subprocess.run(['node', str(ROOT / 'tools/build_dosbox_runtime.cjs')], cwd=ROOT, check=True)
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--fingerprints-only', action='store_true')
    args = parser.parse_args()
    write_fingerprints() if args.fingerprints_only else build()
