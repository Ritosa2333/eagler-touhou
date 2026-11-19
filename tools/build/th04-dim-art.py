"""Pack the reviewed TH04 low-visibility projectiles into the native RGB table."""
import argparse
import json
import struct
from pathlib import Path

from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--assets', type=Path, required=True)
parser.add_argument('--original-assets', type=Path, required=True)
parser.add_argument('--table', type=Path, required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--original-palette-output', type=Path, required=True)
args = parser.parse_args()

table = bytearray(args.table.read_bytes())
manifest = json.loads((args.original_assets / 'manifest.json').read_text(encoding='utf-8'))
count = 0
original_palettes = bytearray(2 * 16 * 4)
for character, name in enumerate(('reimu', 'marisa')):
    info = manifest['characters']['th04/' + name]
    indices = (args.original_assets / 'sources' / (info['source'] + '.indices.bin')).read_bytes()
    colors = {}
    for frame in info['variants']['p1-original']:
        image = Image.open(args.assets / 'th04-shot' / name / 'p1-original' / frame['file']).convert('RGBA')
        for index, (r,g,b,alpha) in zip(indices[frame['source_frame']*256:],image.get_flattened_data()):
            if alpha:
                color=0xff000000 | b << 16 | g << 8 | r
                if index in colors and colors[index]!=color:
                    raise ValueError(f'{name}: inconsistent color for palette index {index}')
                colors[index]=color
    for index,color in colors.items():
        struct.pack_into('<I',original_palettes,(character*16+index)*4,color)
    for rank, variant in enumerate(('p2-blue', 'p3-green')):
        for frame in info['variants'][variant]:
            image = Image.open(args.assets / 'th04-shot' / name / variant / frame['file']).convert('RGBA')
            base = 16000 + ((character * 2 + rank) * 44 + frame['source_frame']) * 256
            for pixel, (r, g, b, alpha) in enumerate(image.get_flattened_data()):
                offset = (base + pixel) * 4
                if struct.unpack_from('<I', table, offset)[0] and alpha:
                    struct.pack_into('<I', table, offset, 0xff000000 | b << 16 | g << 8 | r)
            count += 1

# The beam renderer uses one native table entry per color and animation phase.
# Read the reviewed strip pixels directly, as with the projectile frames.
for rank, variant in enumerate(('p2-blue', 'p3-green')):
    for phase in range(2):
        image = Image.open(args.assets / 'th04-shot' / 'marisa' / variant / 'laser' /
                           f'laser-style0-phase{phase}-strip.png').convert('RGBA')
        r, g, b, _ = next(pixel for pixel in image.get_flattened_data() if pixel[3])
        struct.pack_into('<I', table, (65000 + rank * 2 + phase) * 4,
                         0xff000000 | b << 16 | g << 8 | r)

args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_bytes(table)
args.original_palette_output.parent.mkdir(parents=True, exist_ok=True)
args.original_palette_output.write_bytes(original_palettes)
print(json.dumps({'output': str(args.output), 'bytes': len(table), 'frames': count}))
