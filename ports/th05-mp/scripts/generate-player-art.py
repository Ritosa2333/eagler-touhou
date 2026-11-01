"""Import reviewed PNGs as exact, indexed RGBA; no runtime image conversion."""
from pathlib import Path
from zipfile import ZipFile
import argparse, hashlib, io, json

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--zip', type=Path)
parser.add_argument('--check', action='store_true')
args = parser.parse_args()
target = root / 'Game/Coop/PlayerArt.inc'
source = root / 'resources/player-art/source.json'
if args.check:
    info = json.loads(source.read_text(encoding='utf-8'))
    assert hashlib.sha256(target.read_bytes()).hexdigest() == info['generated']['sha256']
    print('Reviewed TH05 player art: verified')
    raise SystemExit()
if not args.zip:
    parser.error('--zip is required to regenerate the reviewed sprites')
from PIL import Image

palette, pixels, records, colors, sources, hashes = [], [], [], [], {}, {}
base = 'th05-four-player-colors/'
with ZipFile(args.zip) as archive:
    manifest = json.loads(archive.read(base + 'manifest.json'))
    for character in ['reimu', 'marisa', 'mima', 'yuuka']:
        variants = manifest.get('character_variants', {}).get(character, manifest['default_variants'])
        color = []
        for variant in variants:
            row = []
            names = [*[f'player-{n:02d}.png' for n in range(3)], *[f'shots-{n:02d}.png' for n in range(16)], 'ghost-32.png']
            for name in names:
                path = f'{character}/{variant}/{name}'
                png = archive.read(base + path)
                sources[path] = hashlib.sha256(png).hexdigest()
                image = Image.open(io.BytesIO(png)).convert('RGBA')
                width, height = image.size
                offset, rgba_bytes = len(pixels), bytearray()
                for y in range(height - 1, -1, -1):
                    for x in range(width):
                        rgba = image.getpixel((x, y))
                        rgba = rgba if rgba[3] else (0, 0, 0, 0)
                        if rgba not in palette:
                            palette.append(rgba)
                        pixels.append(palette.index(rgba))
                        rgba_bytes.extend(rgba)
                hash_value = 2166136261
                for byte in rgba_bytes:
                    hash_value = ((hash_value ^ byte) * 16777619) & 0xffffffff
                hashes[path] = hash_value
                row.append((offset, width, height))
            records.append(row)
            replacements = manifest['characters'][character]['variants'][variant]['shots_index_replacements']
            key = max(map(int, replacements), default=3 if character == 'marisa' else 7 if character in ['reimu', 'yuuka'] else 9)
            color.append(replacements.get(str(key), manifest['characters'][character]['groups']['shots']['palette_rgb'][key]))
        colors.append(color)
assert len(palette) <= 256
vector = lambda values: '{' + ','.join(map(str, values)) + '}'
lines = ['// Generated from the reviewed th05-four-player-colors package. See resources/player-art/source.json.',
         'static const unsigned char kArtPalette[][4]={' + ','.join(map(vector, palette)) + '};',
         'static const unsigned char kArtPixels[]={']
lines += [','.join(map(str, pixels[i:i+64])) + ',' for i in range(0, len(pixels), 64)]
lines += ['};', 'static const ArtFrame kArtFrames[4][3][20]={']
for character in range(4):
    lines.append('{')
    for variant in range(3):
        lines.append('{' + ','.join(map(vector, records[character*3+variant])) + '},')
    lines.append('},')
lines += ['};', 'static const unsigned char kArtColors[4][3][3]={' + ','.join('{' + ','.join(map(vector, row)) + '}' for row in colors) + '};']
target.write_text('\n'.join(lines) + '\n', encoding='utf-8')
info = {'schema': 'eagler-touhou/th05-reviewed-player-art/1',
        'archiveSha256': hashlib.sha256(args.zip.read_bytes()).hexdigest(), 'sourceArchive': args.zip.name,
        'referenceCommit': '659e65658d4cd1bcfee1670f4c5cc63d79d2819f',
        'rule': 'same-character occurrence; original, alternate 1, alternate 2',
        'paletteConversion': manifest['palette_conversion'],
        'characterVariants': {c: manifest.get('character_variants', {}).get(c, manifest['default_variants']) for c in manifest['characters']},
        'files': sources, 'textureHashes': hashes,
        'generated': {'path': 'Game/Coop/PlayerArt.inc', 'sha256': hashlib.sha256(target.read_bytes()).hexdigest(), 'colors': len(palette), 'pixels': len(pixels)}}
source.parent.mkdir(parents=True, exist_ok=True)
source.write_text(json.dumps(info, indent=2) + '\n', encoding='utf-8')
print(json.dumps(info['generated']))
