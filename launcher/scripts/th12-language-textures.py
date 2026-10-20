"""Prepare TH12 thpatch PNG replacements in the retail ANM atlas layout.

Only pixel data is emitted. ANM programs and sprites always remain Japanese.
thpatch images replace their named texture rectangle, including transparent
pixels; declared ANM padding and unrelated texture entries remain unchanged.
"""
import io
import json
import struct
import sys
from pathlib import Path
from PIL import Image


def prepare(job):
    data = Path(job['input']).read_bytes()
    at = 0
    for _ in range(job['index']):
        step = struct.unpack_from('<I', data, at + 36)[0]
        if not step:
            raise ValueError('ANM entry missing')
        at += step
    td = at + struct.unpack_from('<I', data, at + 28)[0]
    if data[td:td + 4] != b'THTX':
        raise ValueError('Invalid THTX')
    fmt, width, height, size = struct.unpack_from('<HHHI', data, td + 6)
    logical_width, logical_height, texture_format = struct.unpack_from('<HHH', data, at + 10)
    patch = Image.open(job['png']).convert('RGBA')
    raw = data[td + 16:td + 16 + size]
    if fmt == 1:
        pixels = Image.frombytes('RGBA', (width, height), raw, 'raw', 'BGRA')
    elif fmt in (3, 5, 7):
        rgba = bytearray()
        values = raw if fmt == 7 else (v[0] for v in struct.iter_unpack('<H', raw))
        for value in values:
            color = (0, 0, 0, value) if fmt == 7 else (
                (((value >> 8) & 15) * 17, ((value >> 4) & 15) * 17,
                 (value & 15) * 17, ((value >> 12) & 15) * 17) if fmt == 5 else
                (((value >> 11) & 31) * 255 // 31, ((value >> 5) & 63) * 255 // 63,
                 (value & 31) * 255 // 31, 255))
            rgba.extend(color)
        pixels = Image.frombytes('RGBA', (width, height), bytes(rgba))
    else:
        raise ValueError(f'Unsupported THTX format {fmt}')
    output = Image.new('RGBA', (max(width, logical_width), max(height, logical_height)),
                       (0, 0, 0, 255 if texture_format == 3 else 0))
    output.paste(pixels, (0, 0))
    # thcrap supports cropped patches (title01 is 256x351 in a 256x512
    # texture). Uncovered pixels retain their Japanese source values.
    x, y = struct.unpack_from('<HH', data, at + 20)
    right, bottom = min(patch.width, x + width), min(patch.height, y + height)
    if right <= x or bottom <= y:
        raise ValueError(f"{job['name']}: patch does not overlap the texture")
    output.paste(patch.crop((x, y, right, bottom)), (0, 0))
    output.save(job['out'], compress_level=9)
    return dict(job, width=output.width, height=output.height)


if __name__ == '__main__':
    print(json.dumps([prepare(job) for job in json.loads(Path(sys.argv[1]).read_text())]))
