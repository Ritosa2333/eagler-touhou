"""Build the verified-layout TH03 native round/match surrender bridge."""
import hashlib
import json
from pathlib import Path
import struct
import subprocess
from inspect_th03 import executable

ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_SHA = 'f41fde47ea36bf4d985ff9127b67fe93d5ecb58cc36e7cffab86959db7f2ce6b'
INSERT = 0xe8f0
CS_BASE = 0x96e0
OVERLAP_TAIL = bytes.fromhex('ff207c9a5f5ec9c3')


def build_startup(nasm, previous):
    subprocess.run([str(nasm), '-f', 'bin', 'patches/music.asm', '-o', 'web/native/music.com'], cwd=ROOT, check=True)
    subprocess.run([str(nasm), '-f', 'bin', 'patches/start.asm', '-o', 'build/start.com',
                    '-l', 'build/start.lst'], cwd=ROOT, check=True)
    out = ROOT / 'web/native'
    launcher = (ROOT / 'build/start.com').read_bytes()
    (out / 'start.com').write_bytes(launcher)
    music = (out / 'music.com').read_bytes()
    startup = {'music': {'url': 'native/music.com', 'bytes': len(music),
                        'sha256': hashlib.sha256(music).hexdigest(),
                        'signature': 'TH03LOCALBGMv01!',
                        'mailbox_com_offset': music.index(b'TH03LOCALBGMv01!') + 0x100},
               'launcher': {'url': 'native/start.com', 'bytes': len(launcher),
                           'sha256': hashlib.sha256(launcher).hexdigest()}, 'batches': {}}
    for lang in ('jp', 'cn'):
        original = executable(lang, 'YUMEZIKU/GAME.BAT')
        lines = original.replace(b'\r\r\n', b'\r\n').splitlines(keepends=True)
        if sum(line.strip().lower() == b'op' for line in lines) != 6:
            raise ValueError('Original sound-driver launch branches changed')
        if sum(line.strip().lower() == b'zun -3' for line in lines) != 6:
            raise ValueError('Original ZUN Soft animation branches changed')
        batch = b''.join(b'webstart\r\nif errorlevel 1 goto fin\r\n' + line
                         if line.strip().lower() == b'op' else line
                         for line in lines if line.strip().lower() != b'zun -3')
        batch = batch.replace(b'webstart\r\n', b'webmusic\r\nif errorlevel 1 goto fin\r\nwebstart\r\n')
        for driver in (b'pmd', b'pmdb2', b'pmd86'):
            batch = batch.replace(b'\n' + driver + b' /R\r\n', b'\nwebmusic /R\r\n' + driver + b' /R\r\n')
        sha = hashlib.sha256(batch).hexdigest()
        original_sha = hashlib.sha256(original).hexdigest()
        accepted = {original_sha, sha}
        accepted.update(previous.get('startup', {}).get('batches', {}).get(lang, {}).get('acceptedHashes', []))
        (out / f'game-{lang}.bat').write_bytes(batch)
        startup['batches'][lang] = {'url': f'native/game-{lang}.bat', 'bytes': len(batch),
                                  'sha256': sha, 'originalSha256': original_sha,
                                  'acceptedHashes': sorted(accepted)}
    return startup


def build():
    originals = [executable(lang) for lang in ('jp', 'cn')]
    if any(hashlib.sha256(exe).hexdigest() != ORIGINAL_SHA for exe in originals):
        raise ValueError('Unsupported TH03 MAIN.EXE revision')
    build_dir = ROOT / 'build'
    build_dir.mkdir(exist_ok=True)
    nasm = ROOT.parent / 'th04-coop-lab/tools/nasm-2.16.03/nasm.exe'
    subprocess.run([str(nasm), '-f', 'bin', 'patches/pause.asm', '-o', 'build/pause.bin',
                    '-l', 'build/pause.lst'], cwd=ROOT, check=True)
    payload = (build_dir / 'pause.bin').read_bytes()
    gap = (len(OVERLAP_TAIL) + len(payload) + 15) & ~15
    entries = struct.unpack_from('<5H', payload)
    if INSERT - CS_BASE + gap >= 0x10000:
        raise ValueError('Native hook exceeds code segment')
    original = originals[0]
    header_size = struct.unpack_from('<H', original, 8)[0] * 16
    header = bytearray(original[:header_size])
    image = bytearray(original[header_size:])
    # main_01's final CMP straddles the next segment's paragraph base. Keep
    # its operand, backward JL and epilogue at their original near addresses.
    if image[INSERT:INSERT + len(OVERLAP_TAIL)] != OVERLAP_TAIL:
        raise ValueError('Overlapping code-segment tail changed')
    if image[INSERT - 1:INSERT + 3] != bytes.fromhex('83ff207c'):
        raise ValueError('Original final loop instruction changed')
    relocation_count = struct.unpack_from('<H', header, 6)[0]
    table = struct.unpack_from('<H', header, 24)[0]
    relocations = []
    for index in range(relocation_count):
        off, seg = struct.unpack_from('<HH', header, table + index * 4)
        location = seg * 16 + off
        target = struct.unpack_from('<H', image, location)[0]
        relocations.append(location)
        if target * 16 >= INSERT:
            struct.pack_into('<H', image, location, target + gap // 16)
        moved = location + (gap if location >= INSERT else 0)
        struct.pack_into('<HH', header, table + index * 4, moved % 16, moved // 16)
    for at in (14, 22):
        segment = struct.unpack_from('<H', header, at)[0]
        if segment * 16 >= INSERT:
            struct.pack_into('<H', header, at, segment + gap // 16)
    hooks = [(0x977e, 'e8b70c', entries[0]), (0x972f, 'e84600', entries[1]),
             (0xdaca, 'e851fc', entries[3]), (0x990b, 'e80a46', entries[4])]
    for at, expected, target in hooks:
        if image[at:at + 3] != bytes.fromhex(expected):
            raise ValueError(f'Native call anchor changed: {at:#x}')
        if any(at - 1 <= location < at + 3 for location in relocations):
            raise ValueError('Hook overlaps a relocation')
        image[at:at + 3] = b'\xe8' + struct.pack('<H', (CS_BASE + target - at - 3) & 0xffff)
    if image[0x9842:0x9845] != bytes.fromhex('e8602f'):
        raise ValueError('Original pause anchor changed')
    image[0x9842:0x9845] = b'\x90' * 3
    image[INSERT:INSERT] = OVERLAP_TAIL + payload + bytes(gap - len(OVERLAP_TAIL) - len(payload))
    length = len(header) + len(image)
    struct.pack_into('<HH', header, 2, length % 512, (length + 511) // 512)
    patched = bytes(header + image)
    out = ROOT / 'web/native'
    out.mkdir(exist_ok=True)
    (out / 'main.exe').write_bytes(patched)
    sha = hashlib.sha256(patched).hexdigest()
    previous = ROOT / 'web/native-patch.json'
    accepted = {ORIGINAL_SHA, sha}
    old = {}
    if previous.exists():
        old = json.loads(previous.read_text(encoding='utf-8'))
        accepted.update(old.get('acceptedMainHashes', []))
    manifest = {'version': 1, 'signature': 'TH03LOCALPAUSE1!', 'mailboxSize': 44,
                'url': 'native/main.exe', 'sha256': sha, 'bytes': len(patched),
                'originalSha256': ORIGINAL_SHA, 'acceptedMainHashes': sorted(accepted),
                'insertImageOffset': INSERT, 'insertSize': gap, 'codeSegment': CS_BASE // 16,
                'overlapTail': {'imageOffset': INSERT, 'bytes': OVERLAP_TAIL.hex()},
                'dataSegment': 0x1d56 + gap // 16, 'mailboxCsOffset': entries[2],
                'hooks': [{'imageOffset': at, 'original': expected, 'target': target}
                          for at, expected, target in hooks],
                'disabledNativePause': {'imageOffset': 0x9842, 'original': 'e8602f'},
                'fields': {'ticks': 16, 'ds': 20, 'phase': 22, 'command': 23,
                           'seat': 24, 'ack': 25, 'focus': 26, 'points': 27, 'generation': 28,
                           'alwaysPoint': 31, 'touch': 32},
                'focus': {'speedDivisor': 2, 'playerVelocity': 0x6586,
                          'fieldShiftX': 0x659c, 'vramRows': 200}}
    manifest['startup'] = build_startup(nasm, old)
    previous.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    print(f'Built TH03 pause bridge: {gap} inserted bytes, {len(patched)} byte MAIN.EXE')


if __name__ == '__main__':
    build()
