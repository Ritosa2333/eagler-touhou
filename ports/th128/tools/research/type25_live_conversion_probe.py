#!/usr/bin/env python3
"""Read-only evidence for selector3 / 0x40a990 live-bullet type25 conversion."""
import argparse
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'work/reverse-libs'))
import capstone  # noqa: E402
import pefile  # noqa: E402

EXE_SHA = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
EXE = pathlib.Path(r'E:\gee\妖精大战争\th128.exe')
CALLER = (0x41e9b0, 0x41ea36)
CALLEE = (0x40a990, 0x40aaf3)
CONSTANTS = {'radiusSquared': 0x4a6820, 'hitbox': 0x4b41f4, 'auxiliary': 0x4b41f8,
             'appearance': 0x4b41fc, 'selector': 0x4b4200, 'secondary': 0x4b420c}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description='Read-only type25 live-conversion probe')
    parser.add_argument('--exe', type=pathlib.Path, default=EXE)
    parser.add_argument('--output', type=pathlib.Path,
                        default=ROOT / 'local/research/type25-live-conversion-fixtures.json')
    args = parser.parse_args()
    exe = args.exe.read_bytes()
    assert sha(exe) == EXE_SHA
    pe = pefile.PE(data=exe, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    def code(bounds):
        start, end = bounds
        return pe.get_data(start - base, end - start), start

    caller, caller_start = code(CALLER)
    callee, callee_start = code(CALLEE)
    assert bytes.fromhex('8b3d30894b00') in caller              # edi = [0x4b8930]
    assert bytes.fromhex('83c764') in caller                    # edi += 0x64
    assert bytes.fromhex('81c7b8110000') in caller              # stride 0x11b8
    assert bytes.fromhex('bbd0070000') in caller                # 2000 slots
    assert bytes.fromhex('6683bf2a0a000001') in caller          # motionState == 1
    assert bytes.fromhex('6683bfb011000019') in caller          # type != 25
    assert bytes.fromhex('c787b011000019000000') in callee      # type = 25
    assert bytes.fromhex('c787ac110000f0404b00') in callee      # profile 0x4b40f0
    assert bytes.fromhex('c787e8040000b0cc4000') in callee      # visual callback
    assert bytes.fromhex('89bfec040000') in callee              # self callback arg
    assert bytes.fromhex('e8bf84ffff') in callee                # reset primary 0x402ea0
    assert bytes.fromhex('e88b84ffff') in callee                # reset secondary 0x402ea0
    assert bytes.fromhex('e8e9eb0400') in callee                # bind primary 0x4595f0
    assert bytes.fromhex('e8a5eb0400') in callee                # bind secondary 0x4595f0
    assert bytes.fromhex('830f10') in callee                   # flags |= 0x10
    assert bytes.fromhex('83a7a0090000fe') in callee            # clear +0x9a0 bit0

    constants = {name: {'address': hex(addr), 'bytes': pe.get_data(addr - base, 4).hex(),
                        'int': struct.unpack('<i', pe.get_data(addr - base, 4))[0]}
                 for name, addr in CONSTANTS.items()}
    constants['radiusSquared']['float'] = struct.unpack('<f', pe.get_data(0x4a6820 - base, 4))[0]
    constants['hitbox']['float'] = struct.unpack('<f', pe.get_data(0x4b41f4 - base, 4))[0]
    constants['auxiliary']['float'] = struct.unpack('<f', pe.get_data(0x4b41f8 - base, 4))[0]

    existing_path = ROOT / 'local/research/thirdstage-529-mapping-fixtures.json'
    existing = json.loads(existing_path.read_text(encoding='utf-8'))
    assert existing['sourceExeSha256'] == EXE_SHA
    assert existing['type25']['profileSha256'] == \
        '080190229ed68ee1120238bb7308ae44c16cdc903c2a84fba65a7c273b2236dd'
    assert existing['type25']['values']['script'] == 102
    assert existing['type25']['values']['secondaryScript'] == 103
    assert existing['baseProof']['stateBase'] == 'Enemy+0x11c8'

    source_paths = ['src/game/BulletManager.hpp', 'src/game/BulletManager.cpp',
                    'src/game/BulletVisuals.hpp', 'src/game/BulletVisuals.cpp',
                    'src/runtime/WorldExports.cpp']
    sources = {}
    for rel in source_paths:
        data = (ROOT / rel).read_bytes()
        sources[rel] = {'sha256': sha(data), 'bytes': len(data)}
    manager_h = (ROOT / 'src/game/BulletManager.hpp').read_text(encoding='utf-8')
    visual_h = (ROOT / 'src/game/BulletVisuals.hpp').read_text(encoding='utf-8')
    exporter = (ROOT / 'src/runtime/WorldExports.cpp').read_text(encoding='utf-8')
    assert 'convert' not in manager_h
    assert 'createConverted' not in visual_h
    assert 'bulletAnimations.emplace' in exporter

    writes = [
        {'address': '0x40a994', 'offset': '0x11b0', 'value': 'word/type=25', 'meaning': 'type'},
        {'address': '0x40a9ae', 'offset': '0x9fc', 'value': 'profile+0x104 float6', 'meaning': 'hitbox duplicate'},
        {'address': '0x40a9b4', 'offset': '0x9f8', 'value': 'profile+0x104 float6', 'meaning': 'hitbox'},
        {'address': '0x40a9c0', 'offset': '0x11ac', 'value': '0x4b40f0', 'meaning': 'profile pointer'},
        {'address': '0x40a9ca', 'offset': '0xa00', 'value': 'profile+0x108 float22', 'meaning': 'auxiliary'},
        {'address': '0x40a9d6', 'offset': '0xa40', 'value': 'profile+0x10c int2', 'meaning': 'appearance/profileValue'},
        {'address': '0x40a9e6', 'offset': '0x4e8', 'value': '0x40ccb0', 'meaning': 'visual callback'},
        {'address': '0x40a9f0', 'offset': '0x4ec', 'value': 'self bullet', 'meaning': 'callback owner'},
        {'address': '0x40aa07', 'offset': '0x0', 'value': 'flags |= 0x10', 'meaning': 'conversion flag'},
        {'address': '0x40aa15', 'offset': '0x9a0', 'value': 'bit0=0 then set if secondary', 'meaning': 'secondary presence'},
        {'address': '0x40aa61..0x40aae9', 'offset': '0xa1c', 'value': 'conditional switch only; selector8 default does not write',
         'meaning': 'appearance/effect field; type25 selector8 reaches default ret'},
    ]
    fixture = {
        'schema': 'th128-type25-live-conversion/1',
        'sourceExe': {'bytes': len(exe), 'sha256': EXE_SHA},
        'sourceEvidence': {'type25': 'local/research/thirdstage-529-mapping-fixtures.json',
                           'type25Sha256': sha(existing_path.read_bytes())},
        'profile': existing['type25'],
        'caller': {
            'entry': '0x41e9b0', 'callee': '0x40a990',
            'edi': 'bullet base [0x4b8930]+0x64',
            'ecx': 'entity state base Enemy+0x11c8 (incoming; callee overwrites ECX for binds)',
            'slotBase': '[0x4b8930]+0x64', 'stride': 0x11b8, 'slots': 2000,
            'checks': ['[bullet]&1 active', '[bullet+0xa2a] motionState==1', '[bullet+0x11b0] type !=25',
                       'squared XY distance from entity+0x34/+0x38 <= 0x4a6820 (9216.0 = 96^2)'],
            'afterLoop': 'entity+0x24c += entity+0x250',
            'executesOnlyWhen': 'active, motionState1, non-type25, within radius',
        },
        'callee': {
            'entry': '0x40a990', 'edi': 'bullet base', 'esi': 'temporary VM pointer',
            'branch': 'bounded original slice; external VM reset/bind calls are not executed/return-faked',
            'externalCalls': ['0x402ea0 reset primary +8', '0x4595f0 bind primary 102',
                              '0x402ea0 reset secondary +4f0', '0x4595f0 bind secondary 103'],
            'vmOrder': 'reset primary, bind primary, flags, reset secondary, bind secondary; no post-tail extra tick observed',
            'writes': writes,
            'noWritesObserved': ['id/slot', 'color +0x11b2', 'position +0x9d8/+9dc/+9e0',
                                 'velocity +0x9e4/+9e8/+9ec', 'speed/angle +0x9f0/+0x9f4',
                                 'age +0x1188', 'life +0x1198/+119c/+11a0',
                                 'extensions', 'cancellationScript', 'other flag bits'],
            'preserved': 'existing Birth fields not listed above, including RNG-owned extensions and identity',
        },
        'constants': constants,
        'missingApi': {
            'BulletManager': 'no slot+id preflight/commit; emit only',
            'BulletVisuals': 'no conversion create without the birth post-tail tick',
            'WorldExports': 'bulletAnimations only inserted at birth; no atomic map swap/old-VM retirement',
            'needed': ['Manager preflight+commit preserving Birth', 'Visual createConverted bind order',
                       'WorldExports map swap + old visual retirement'],
        },
        'currentSources': sources,
        'acceptanceCases': [
            {'id': 'valid-type25-conversion-preserves-position-velocity-age-life', 'expected': True},
            {'id': 'payload-id-slot-mismatch-rejected', 'expected': False},
            {'id': 'non-live-or-inactive-slot-rejected', 'expected': False},
            {'id': 'profile-not-6/22/2/1/rejected', 'expected': False},
            {'id': 'new-primary102-secondary103-bind-inside-transaction', 'expected': True},
            {'id': 'old-visual-retained-until-commit', 'expected': True},
            {'id': 'source-visual-fault-sticky', 'expected': False},
            {'id': 'type25-already-converted-skip', 'expected': False},
            {'id': 'radius-outside-96-rejected-by-caller', 'expected': False},
            {'id': 'selector3-entity-250-delta-not-new-field', 'expected': True},
        ],
        'boundedLimitations': [
            'No original process execution; external VM reset/bind calls are stop boundaries.',
            'No full conversion success or rendered frame claimed.',
            'No StageWorld/529 callback wiring or profile reload in this read-only task.',
            'No raw padding copy; only known Birth fields/visual owner are in scope.',
        ],
    }
    args.output.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8')
    print(f'wrote {args.output.relative_to(ROOT)}: {len(writes)} writes, '
          f'{len(fixture["acceptanceCases"])} acceptance cases')


if __name__ == '__main__':
    main()
