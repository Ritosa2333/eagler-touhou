"""Build movement-only assistance; retain stock boot, music, pause and game flow."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
from inspect_th03 import executable
from build_pause import ROOT, ORIGINAL_SHA, INSERT, CS_BASE, OVERLAP_TAIL


def build():
    original = executable()
    assert all(hashlib.sha256(executable(lang)).hexdigest() == ORIGINAL_SHA for lang in ('jp', 'cn'))
    nasm = ROOT.parent / 'th04-coop-lab/tools/nasm-2.16.03/nasm.exe'
    (ROOT / 'build').mkdir(exist_ok=True)
    subprocess.run([str(nasm), '-f', 'bin', 'patches/solo-assist.asm', '-o', 'build/solo-assist.bin'], cwd=ROOT, check=True)
    payload = (ROOT / 'build/solo-assist.bin').read_bytes()
    entries = struct.unpack_from('<5H', payload)
    assert payload[entries[2] - 0x5218:entries[2] - 0x5218 + 16] == b'TH03SOLOASSIST1!'
    gap = (len(OVERLAP_TAIL) + len(payload) + 15) & ~15
    assert INSERT - CS_BASE + gap < 0x10000
    hsize = struct.unpack_from('<H', original, 8)[0] * 16
    header, image = bytearray(original[:hsize]), bytearray(original[hsize:])
    assert image[INSERT:INSERT + len(OVERLAP_TAIL)] == OVERLAP_TAIL
    count, table = struct.unpack_from('<H', header, 6)[0], struct.unpack_from('<H', header, 24)[0]
    relocations = []
    for index in range(count):
        off, seg = struct.unpack_from('<HH', header, table + index * 4)
        at = seg * 16 + off
        relocations.append(at)
        target = struct.unpack_from('<H', image, at)[0]
        if target * 16 >= INSERT:
            struct.pack_into('<H', image, at, target + gap // 16)
        moved = at + (gap if at >= INSERT else 0)
        struct.pack_into('<HH', header, table + index * 4, moved % 16, moved // 16)
    for at in (14, 22):
        segment = struct.unpack_from('<H', header, at)[0]
        if segment * 16 >= INSERT:
            struct.pack_into('<H', header, at, segment + gap // 16)
    hooks = [(0x977e, 'e8b70c', entries[0]), (0x972f, 'e84600', entries[1]),
             (0xdaca, 'e851fc', entries[3]), (0x9842, 'e8602f', entries[4])]
    for at, expected, target in hooks:
        assert image[at:at + 3] == bytes.fromhex(expected)
        assert not any(at - 1 <= location < at + 3 for location in relocations)
        image[at:at + 3] = b'\xe8' + struct.pack('<H', (CS_BASE + target - at - 3) & 0xffff)
    image[INSERT:INSERT] = OVERLAP_TAIL + payload + bytes(gap - len(OVERLAP_TAIL) - len(payload))
    length = len(header) + len(image)
    struct.pack_into('<HH', header, 2, length % 512, (length + 511) // 512)
    patched = bytes(header + image)
    out = ROOT / 'web/native'
    out.mkdir(exist_ok=True)
    (out / 'solo-main.exe').write_bytes(patched)
    (out / 'original-main.exe').write_bytes(original)
    meta = {'version': 1, 'signature': 'TH03SOLOASSIST1!', 'mailboxSize': 44,
            'url': 'native/solo-main.exe', 'sha256': hashlib.sha256(patched).hexdigest(), 'bytes': len(patched),
            'originalUrl': 'native/original-main.exe', 'originalSha256': ORIGINAL_SHA,
            'insertImageOffset': INSERT, 'insertSize': gap, 'codeSegment': CS_BASE // 16,
            'dataSegment': 0x1d56 + gap // 16, 'mailboxCsOffset': entries[2],
            'hooks': [{'imageOffset': at, 'original': expected, 'target': target} for at, expected, target in hooks],
            'fields': {'ticks': 16, 'ds': 20, 'phase': 22, 'command': 23, 'seat': 24, 'ack': 25,
                       'focus': 26, 'points': 27, 'generation': 28, 'alwaysPoint': 31, 'touch': 32}}
    (ROOT / 'web/solo-assist.json').write_text(json.dumps(meta, indent=2) + '\n', encoding='utf-8')
    print(f'Built solo movement bridge: {gap} inserted bytes; original startup/music/score files untouched')


if __name__ == '__main__':
    build()
