"""准备本作本地站点的封面与站点图标。"""
from pathlib import Path
from PIL import Image
import hashlib
import json
import shutil

ROOT = Path(__file__).resolve().parents[1]
WORKSPACE = ROOT.parents[1]

def main():
    output = ROOT / 'build/site-inputs'
    artwork = output / 'artwork'
    (artwork / 'pwa').mkdir(parents=True, exist_ok=True)
    source = ROOT / 'artwork/cover.jpg'
    shutil.copy2(source, artwork / 'thkkd-card.jpg')
    site_icon = WORKSPACE / 'launcher/public/assets/favicon.png'
    with Image.open(site_icon).convert('RGBA') as image:
        for name, size in [('icon-192.png', 192), ('icon-512.png', 512), ('icon-maskable-512.png', 512), ('apple-touch-icon.png', 180)]:
            image.resize((size, size), Image.Resampling.LANCZOS).save(artwork / 'pwa' / name)
    shutil.copy2(WORKSPACE / 'launcher/favicon.ico', artwork / 'favicon.ico')
    manifest = {
        'schema': 'thkkd/site-inputs/1',
        'font': 'GNU Unifont 15.1.05; full original glyph set and naming retained',
        'coverSource': 'ports/thkkd/artwork/cover.jpg',
        'files': {p.relative_to(output).as_posix(): {'bytes': p.stat().st_size, 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in artwork.rglob('*') if p.is_file()},
    }
    (output / 'provenance.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'files': len(manifest['files'])}))

if __name__ == '__main__':
    main()
