"""Extract only named, user-provided test assets; never scan a workspace/disk."""
from pathlib import Path
import argparse
import hashlib
import json
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'artifacts/presentation-lab/input'
EXPECTED_DATA = '9d7edf43b8ddd347cbb641836f6b5050745dd936f688daebbf9382ca557043bb'

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--zip', type=Path, required=True, help='Existing, legally owned test ZIP')
    args = parser.parse_args()
    members = {'th08_web/dist/data/th08.dat': 'th08.dat'}
    members.update({f'th08_web/reference/demorpy{i}.rpy': f'demo{i}.rpy' for i in range(4)})
    OUT.mkdir(parents=True, exist_ok=True)
    manifest = {}
    with zipfile.ZipFile(args.zip) as archive:
        for source, target in members.items():
            info = archive.getinfo(source)
            if info.file_size > 128 * 1024 * 1024:
                raise ValueError(f'Unexpected asset size: {source}')
            destination = OUT / target
            with archive.open(info) as inp, destination.open('wb') as output:
                shutil.copyfileobj(inp, output, 1024 * 1024)
            digest = hashlib.sha256(destination.read_bytes()).hexdigest()
            if target == 'th08.dat' and digest != EXPECTED_DATA:
                raise ValueError('Test DATA identity differs from the current TH08 product declaration')
            manifest[target] = {'bytes': info.file_size, 'sha256': digest, 'sourceMember': source}
    (OUT / 'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({'prepared': manifest, 'output': str(OUT)}, ensure_ascii=False))

if __name__ == '__main__':
    main()
