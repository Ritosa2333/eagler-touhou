"""准备本作本地站点的封面、站点图标和 GNU Unifont 公共字体。"""
from pathlib import Path
from PIL import Image
from fontTools.ttLib import TTFont, TTCollection
import hashlib
import json
import os
import shutil

ROOT = Path(__file__).resolve().parents[1]
WORKSPACE = ROOT.parents[1]

def main():
    output = ROOT / 'build/site-inputs'
    artwork = output / 'artwork'
    (artwork / 'pwa').mkdir(parents=True, exist_ok=True)
    fonts = output / 'shared'
    fonts.mkdir(parents=True, exist_ok=True)
    source = WORKSPACE / 'launcher/public/assets/favicon.png'
    with Image.open(source) as image:
        image.save(artwork / 'thkkd-card.webp', lossless=True)
    os.utime(artwork / 'thkkd-card.webp', ns=(source.stat().st_atime_ns, source.stat().st_mtime_ns))
    site_icon = WORKSPACE / 'launcher/public/assets/favicon.png'
    with Image.open(site_icon).convert('RGBA') as image:
        for name, size in [('icon-192.png', 192), ('icon-512.png', 512), ('icon-maskable-512.png', 512), ('apple-touch-icon.png', 180)]:
            image.resize((size, size), Image.Resampling.LANCZOS).save(artwork / 'pwa' / name)
    shutil.copy2(WORKSPACE / 'launcher/favicon.ico', artwork / 'favicon.ico')
    font = TTFont(ROOT / 'third_party/unifont.woff2', recalcTimestamp=False)
    font.flavor = None
    font.save(fonts / 'unifont.otf')
    collection = TTCollection()
    collection.fonts = [font]
    collection.save(fonts / 'unifont.ttc')
    manifest = {
        'schema': 'thkkd/site-inputs/1',
        'font': 'GNU Unifont 15.1.05; full original glyph set and naming retained',
        'compatibilityFont': 'The shared vanillaFont role uses a genuine Unifont TTC collection; the generic host writes it at its compatibility path shared/msgothic.ttc.',
        'coverSource': 'launcher/public/assets/favicon.png',
        'files': {p.relative_to(output).as_posix(): {'bytes': p.stat().st_size, 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in output.rglob('*') if p.is_file() and p.name != 'provenance.json'},
    }
    (output / 'provenance.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'files': len(manifest['files'])}))

if __name__ == '__main__':
    main()
