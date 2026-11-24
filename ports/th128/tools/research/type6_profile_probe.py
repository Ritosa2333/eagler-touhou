#!/usr/bin/env python3
"""Read-only original profile/ANM evidence for TH128 bullet type6."""
import argparse
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'work/reverse-libs'))
import pefile  # noqa: E402

EXE_SHA = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
EXE = pathlib.Path(r'E:\gee\妖精大战争\th128.exe')
TABLE = 0x4b24d0
TYPE = 6
ADDRESS = TABLE + TYPE * 0x120
BULLET_JSON = ROOT / 'local/generations/8c71c316a4b24e68827f9fc560242050/ja/animations/bullet.json'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description='Read-only type6 profile probe')
    parser.add_argument('--exe', type=pathlib.Path, default=EXE)
    parser.add_argument('--profile', type=pathlib.Path,
                        default=ROOT / 'artifacts/dsh-type6-profile/type6-profile.json')
    parser.add_argument('--fixture', type=pathlib.Path,
                        default=ROOT / 'local/research/type6-profile-fixtures.json')
    args = parser.parse_args()
    exe = args.exe.read_bytes()
    assert sha(exe) == EXE_SHA
    pe = pefile.PE(data=exe, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    raw = pe.get_data(ADDRESS - base, 288)
    assert len(raw) == 288
    words = struct.unpack_from('<72I', raw)
    as_i = lambda word: struct.unpack('<i', struct.pack('<I', word))[0]
    script = as_i(words[0])
    secondary = as_i(words[0x11c // 4])
    hitbox = struct.unpack_from('<f', raw, 0x104)[0]
    auxiliary = struct.unpack_from('<f', raw, 0x108)[0]
    appearance = as_i(words[0x10c // 4])
    selector = as_i(words[0x110 // 4])
    opaque114 = as_i(words[0x114 // 4])
    freeze = as_i(words[0x118 // 4])
    assert (script, secondary, hitbox, auxiliary, appearance, selector, opaque114, freeze) == (
        41, 0, 4.0, 16.0, 3, 6, 30, 0)
    remaps = [[as_i(words[1 + color * 4 + sprite]) for sprite in range(4)] for color in range(16)]
    cancellations = [as_i(words[4 + color * 4]) for color in range(16)]
    assert remaps[0] == [96, 323, 336, 337]
    assert remaps[1] == [97, 324, 337, 339]
    assert cancellations[:4] == [337, 339, 341, 343]
    assert all(337 <= value <= 367 and value % 2 == 1 for value in cancellations)
    profile = {
        'schema': 'th128-original-typed-bullet-profile/1',
        'type': 6, 'sourceExeSha256': EXE_SHA, 'sourceAddress': hex(ADDRESS),
        'bytes': 288, 'sha256': sha(raw), 'hex': raw.hex(),
        'script': script, 'secondaryScript': secondary,
        'hitboxSize': hitbox, 'auxiliarySize': auxiliary,
        'appearance': appearance, 'selector': selector, 'freezePolicy': freeze,
        'opaque114': opaque114,
        'identitySpriteRemap': False,
        'colorRemaps': remaps, 'cancellationScripts': cancellations,
        'callback': 'per-color four-sprite remap table; no identity 0x40ccb0 callback',
        'supportedTypes': [6],
    }
    args.profile.parent.mkdir(parents=True, exist_ok=True)
    args.profile.write_text(json.dumps(profile, indent=2) + '\n', encoding='utf-8')

    metadata = json.loads(BULLET_JSON.read_text(encoding='utf-8'))
    animation = metadata['scripts'][script]
    assert animation['entry'] == 0 and len(animation['commands']) == 39
    op3 = [struct.unpack_from('<i', bytes.fromhex(command['raw']))[0]
           for command in animation['commands'] if command['opcode'] == 3]
    assert op3 == [1, 1, 1, 0, 2]
    for cancellation in cancellations:
        entry = metadata['scripts'][cancellation]
        children = [struct.unpack_from('<i', bytes.fromhex(command['raw']))[0]
                    for command in entry['commands'] if command['opcode'] == 88]
        assert children and all(child == cancellation - 1 for child in children)
    current = {}
    for rel in ('src/game/BulletManager.cpp', 'src/game/BulletVisuals.cpp'):
        data = (ROOT / rel).read_bytes()
        current[rel] = {'sha256': sha(data), 'bytes': len(data)}
    fixture = {
        'schema': 'th128-type6-profile-fixtures/1',
        'sourceExe': {'bytes': len(exe), 'sha256': EXE_SHA},
        'profile': profile,
        'animation': {'script': script, 'entry': animation['entry'],
                      'commands': len(animation['commands']), 'spriteSources': op3},
        'cancellation': {'scripts': cancellations, 'entry': 7,
                         'childrenAreScriptMinusOne': True},
        'semantics': {'freezePolicy': freeze, 'ordinaryFreezeEligible': True,
                      'identitySpriteRemap': False, 'secondary': secondary,
                      'appearance': appearance, 'cancellationMode': selector},
        'currentSources': current,
        'proposedProductionWrites': ['src/game/BulletManager.cpp',
                                     'src/game/BulletVisuals.cpp'],
        'frontendRequest': 'load type6 profile JSON and include 6 in supportedBulletTypes; no play.mjs/WorldExports edit here',
        'acceptanceCases': [
            {'id': 'profile-exact-bytes-sha-script41', 'expected': True},
            {'id': 'color0-remap-96/323/336/337', 'expected': True},
            {'id': 'cancellation-337/339/341/343', 'expected': True},
            {'id': 'hitbox4-aux16-appearance3-mode6-freeze0', 'expected': True},
            {'id': 'wrong-script-or-secondary-rejected', 'expected': False},
            {'id': 'wrong-hitbox-aux-appearance-freeze-rejected', 'expected': False},
            {'id': 'visual-script41-no-secondary-identity-false', 'expected': True},
            {'id': 'visual-90-ticks-without-fault', 'expected': True},
            {'id': 'manager-emit-type6-preserves-profile/color/cancel', 'expected': True},
            {'id': 'freeze-policy0-eligible', 'expected': True},
        ],
    }
    args.fixture.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8')
    print(f'wrote {args.profile.relative_to(ROOT)} and {args.fixture.relative_to(ROOT)}')


if __name__ == '__main__':
    main()
