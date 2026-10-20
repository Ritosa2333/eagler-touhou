"""Prepare data-only TH075 image replacements and GG75 glyphs offline."""
import json
import struct
import sys
from pathlib import Path

from PIL import Image, ImageFont
from fontTools.ttLib import TTFont


def build(job_path):
    root = Path(job_path).resolve().parent
    job = json.loads(Path(job_path).read_text(encoding='utf-8'))
    for item in job['images']:
        with Image.open(root / item['input']) as image:
            if image.width > 8192 or image.height > 8192:
                raise ValueError('TH075 image exceeds dimension limit')
            rgba = image.convert('RGBA')
            (root / item['output']).write_bytes(
                struct.pack('<II', *rgba.size) + rgba.tobytes('raw', 'BGRA'))
    font_path = Path(job['font'])
    font = TTFont(font_path, fontNumber=0)
    cmap = font.getBestCmap()
    missing = sorted({ord(g['character']) for g in job['glyphs']} - cmap.keys())
    if missing:
        raise ValueError('Missing language glyphs: ' + ','.join(hex(c) for c in missing))
    records = []
    for height in (12, 16, 20, 24, 32):
        face = ImageFont.truetype(str(font_path), height, index=0)
        for glyph in job['glyphs']:
            text = glyph['character']
            left, top, right, bottom = face.getbbox(text)
            width, rows = max(0, right-left), max(0, bottom-top)
            stride = (width+3) & ~3
            image = Image.new('L', (max(1, stride), max(1, rows)))
            if width and rows:
                from PIL import ImageDraw
                ImageDraw.Draw(image).text((-left, -top), text, font=face, fill=255)
                pixels = bytes((p*16+127)//255 for p in image.tobytes())
            else:
                pixels = b''
                stride = 0
            records.append((glyph['code'], height, width, rows, left, top, stride, pixels))
    offset = 12 + len(records)*20
    directory, payload = [], []
    for code, height, width, rows, left, top, stride, pixels in records:
        directory.append(struct.pack('<HHHHhhHHI', code, height, width, rows,
                                     left, top, stride, 0, offset))
        payload.append(pixels)
        offset += len(pixels)
    (root / 'glyphs.bin').write_bytes(b'GG75' + struct.pack('<II', 1, len(records))
                                    + b''.join(directory) + b''.join(payload))


if __name__ == '__main__':
    build(sys.argv[1])
