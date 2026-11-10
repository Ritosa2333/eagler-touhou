"""Pack the reviewed TH04 low-visibility player sprites for the native renderer."""
import argparse
import struct
from pathlib import Path

from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('--assets', type=Path, required=True)
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()

body = bytearray()
for character in ('reimu', 'marisa'):
    for variant in ('p1-original', 'p2-blue', 'p3-green'):
        for frame in range(3):
            path = args.assets / 'th04-body' / character / variant / f'player-{frame:02}.png'
            image = Image.open(path).convert('RGBA')
            if image.size != (32, 48):
                raise ValueError(f'{path}: expected a 32x48 sprite')
            for r, g, b, alpha in image.get_flattened_data():
                body += struct.pack('<I', (0xff000000 | b << 16 | g << 8 | r) if alpha else 0)

args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_bytes(body)
print(f'{args.output}: {len(body)} bytes, 18 sprites')
