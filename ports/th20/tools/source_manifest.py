"""Create or verify a SHA-256 manifest; never changes original game files."""
from pathlib import Path
import argparse
import hashlib
import json

ROOT = Path(__file__).resolve().parents[1]
DEFAULT = Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders')
def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()
def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source', type=Path, default=DEFAULT)
    ap.add_argument('--verify', action='store_true')
    args = ap.parse_args()
    target = ROOT / 'reports/source_manifest.json'
    if args.verify:
        manifest = json.loads(target.read_text('utf-8'))
        failures = [f['name'] for f in manifest['files'] if not (args.source / f['name']).is_file()
                    or digest(args.source / f['name']) != f['sha256']]
        print(json.dumps({'unchanged': not failures, 'checked': len(manifest['files']), 'mismatches': failures}))
        return int(bool(failures))
    if target.exists():
        raise SystemExit('Manifest already exists; use --verify to preserve the baseline')
    files = [{'name': p.name, 'size': p.stat().st_size, 'sha256': digest(p)}
             for p in sorted(args.source.iterdir()) if p.is_file()]
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(json.dumps({'source_directory': str(args.source.resolve()), 'files': files}, indent=2), 'utf-8')
    print(f'Created baseline for {len(files)} files: {target}')
if __name__ == '__main__':
    raise SystemExit(main())
