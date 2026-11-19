"""Build a TH02 low-visibility table from the reviewed PNG assets."""
import argparse
import json
import struct
from pathlib import Path

from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--assets', type=Path, required=True)
parser.add_argument('--table', type=Path, required=True)
parser.add_argument('--original-assets', type=Path, required=True)
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()

table = bytearray(args.table.read_bytes())
variants = ('p1-original', 'p2-blue', 'p3-green')
for seat, variant in enumerate(variants):
    for frame in range(10):
        image = Image.open(args.assets / 'th02-body' / variant / f'player-{frame:02}.png').convert('RGBA')
        for pixel, (r, g, b, alpha) in enumerate(image.get_flattened_data()):
            offset = (1 + seat * 18432 + frame * 1536 + pixel) * 4
            if struct.unpack_from('<I', table, offset)[0] and alpha:
                struct.pack_into('<I', table, offset, 0xff000000 | b << 16 | g << 8 | r)

projectiles = json.loads((args.original_assets / 'projectiles-manifest.json').read_text(encoding='utf-8'))
for seat, variant in enumerate(variants):
    colors = {}
    for frame in projectiles['frames']:
        width, height = frame['size']
        start = frame['source_frame'] * width * height
        indices = (args.original_assets / 'sources' / (frame['source'] + '.indices.bin')).read_bytes()[start:start + width * height]
        image = Image.open(args.assets / 'th02-shot' / variant / 'projectiles' / frame['file']).convert('RGBA')
        for index, (r, g, b, alpha) in zip(indices, image.get_flattened_data()):
            if alpha:
                colors[index] = (r, g, b)
    for index, (r, g, b) in colors.items():
        struct.pack_into('<I', table, (60016 + seat * 16 + index) * 4,
                         0xff000000 | b << 16 | g << 8 | r)

manifest = json.loads((args.assets / 'manifest.json').read_text(encoding='utf-8'))
args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_bytes(table)
print(json.dumps({'output': str(args.output), 'bytes': len(table), 'body_frames': 30,
                  'source_files': len(manifest['files'])}))
