"""Prepare TH12 thpatch PNG replacements in the retail ANM atlas layout.

Only pixel data is emitted. ANM programs and sprites always remain Japanese.
thpatch images patch each ANM sprite; empty replacement sprites preserve the
source. Declared ANM padding and unrelated texture entries remain unchanged.
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
    source = output.copy()
    # thcrap anm.cpp sprite_patch: an empty replacement sprite is a no-op;
    # opaque destination sprites blend, other destinations are overwritten.
    x, y = struct.unpack_from('<HH', data, at + 20)
    count = struct.unpack_from('<H', data, at + 4)[0]
    rectangles = []
    if count <= 1:
        rectangles.append((0, 0, width, height))
    for index in range(count if count > 1 else 0):
        sprite = at + struct.unpack_from('<I', data, at + 64 + index * 4)[0]
        _, sx, sy, sw, sh = struct.unpack_from('<Iffff', data, sprite)
        sx, sy, sw, sh = map(int, (sx, sy, sw, sh))
        if not sw or not sh:
            continue
        sx, sy, sw, sh = sx % width, sy % height, min(sw, width), min(sh, height)
        split_w, split_h = max(0, sx + sw - width), max(0, sy + sh - height)
        sw, sh = min(sw, width - sx), min(sh, height - sy)
        if split_w and split_h:
            rectangles.append((0, 0, split_w, split_h))
        if split_w:
            rectangles.append((0, sy, split_w, sh))
        if split_h:
            rectangles.append((sx, 0, sw, split_h))
        rectangles.append((sx, sy, sw, sh))
    preserved = []
    for sx, sy, sw, sh in rectangles:
        sw, sh = min(sw, patch.width - x - sx, width - sx), min(sh, patch.height - y - sy, height - sy)
        if sw <= 0 or sh <= 0:
            continue
        replacement = patch.crop((x + sx, y + sy, x + sx + sw, y + sy + sh))
        # Quantize alpha before analysis, as thcrap does for ARGB4444.
        if fmt == 5:
            replacement = replacement.point([v // 16 * 17 for v in range(256)] * 4)
        if replacement.getchannel('A').getextrema()[1] == 0:
            preserved.append((sx, sy, sx + sw, sy + sh))
            continue
        original = output.crop((sx, sy, sx + sw, sy + sh))
        if original.getchannel('A').getextrema()[0] == 255:
            if fmt in (1, 5):
                maximum, shift, factor = (15, 4, 17) if fmt == 5 else (255, 8, 1)
                merged = []
                for dst, rep in zip(original.getdata(), replacement.getdata()):
                    alpha = rep[3] // factor
                    merged.append(tuple((((dst[c] // factor) * (maximum - alpha) + (rep[c] // factor) * alpha) >> shift) * factor for c in range(3)) + (255,))
                replacement.putdata(merged)
        output.paste(replacement, (sx, sy))
    # Atlas sprites can overlap (front10's point icon and front18's stock
    # cell). Transparent fallback sprites retain their entire source rectangle,
    # including edges touched by another replacement's transparent padding.
    for rectangle in preserved:
        output.paste(source.crop(rectangle), rectangle[:2])
    output.save(job['out'], compress_level=9)
    return dict(job, width=output.width, height=output.height)


if __name__ == '__main__':
    print(json.dumps([prepare(job) for job in json.loads(Path(sys.argv[1]).read_text())]))
