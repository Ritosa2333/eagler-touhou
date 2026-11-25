"""Generate the neutral placeholder application icon shipped with this repository.

The icon is pure geometry drawn here, so it carries no third-party artwork.
Replace app_icon.ico with your own 24x24-based icon if you want something else;
do not commit artwork taken from the original game.
"""
import pathlib
import struct

SIZES = (16, 24, 32, 48, 64)
BACKDROP = (0x7A, 0x1B, 0x22)
INK = (0xF2, 0xF4, 0xF7)


def pixels(size):
    """Deep-red seal with a white ring and a centred diamond."""
    cx = cy = (size - 1) / 2.0
    outer = size * 0.42
    inner = size * 0.30
    diamond = size * 0.13
    rows = []
    for y in range(size):
        row = []
        for x in range(size):
            dx, dy = abs(x - cx), abs(y - cy)
            distance = (dx * dx + dy * dy) ** 0.5
            inside = distance <= outer and distance >= inner
            if dx + dy <= diamond:
                inside = True
            row.append(INK + (255,) if inside else BACKDROP + (255,))
        rows.append(row)
    return rows


def dib(size):
    """BITMAPINFOHEADER + bottom-up BGRA pixels + empty AND mask."""
    header = struct.pack("<IiiHHIIiiII", 40, size, size * 2, 1, 32, 0, 0, 0, 0, 0, 0)
    body = bytearray()
    for row in reversed(pixels(size)):
        for r, g, b, a in row:
            body += bytes((b, g, r, a))
    mask_stride = ((size + 31) // 32) * 4
    body += bytes(mask_stride * size)
    return header + bytes(body)


def main():
    images = [(size, dib(size)) for size in SIZES]
    offset = 6 + 16 * len(images)
    directory = struct.pack("<HHH", 0, 1, len(images))
    entries, payload = b"", b""
    for size, data in images:
        entries += struct.pack("<BBBBHHII", size % 256, size % 256, 0, 0, 1, 32,
                               len(data), offset + len(payload))
        payload += data
    target = pathlib.Path(__file__).resolve().parent.parent / "src" / "app_icon.ico"
    target.write_bytes(directory + entries + payload)
    print(f"{target} {target.stat().st_size} bytes, sizes={','.join(map(str, SIZES))}")


if __name__ == "__main__":
    main()
