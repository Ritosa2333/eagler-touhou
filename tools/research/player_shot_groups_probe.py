import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-player-shot-groups-fixtures/1'

SLICES = {
    'selectionSetup': (0x43d389, 0x43d3d0),
    'slot1Handles': (0x43d3dc, 0x43d4f3),
    'slot1Delete': (0x43d4f3, 0x43d51a),
    'slot0Positions': (0x43d554, 0x43d6b6),
    'slot0Store': (0x43da6b, 0x43da7b),
    'spawn': (0x4670b0, 0x467177),
    'customConstructor': (0x41f2a0, 0x41f34f),
    'registerFirst': (0x466de0, 0x466e57),
    'deleteLifecycle': (0x467480, 0x4674e7),
    'callbackSetup': (0x41219c, 0x4121a8),
    'callbackDefault': (0x412b40, 0x412b43),
    'stageClearSlots': (0x41fe20, 0x41ffbc),
    'groupUpdateCallSite': (0x43b7e0, 0x43b7f3),
    'normalInitCall': (0x43ac40, 0x43ac56),
}

ANCHORS = {
    'selectionSetup': (
        'mov ecx, dword ptr [0x4b4cf4]',
        'mov eax, dword ptr [0x4b4cf8]',
        'shl eax, 6',
        'add eax, 0x4b7370',
        'mov eax, dword ptr [0x4b4cc8]',
        'idiv dword ptr [0x4b4d38]',
        'call 0x4271c0',
        'cmp eax, dword ptr [0x4b4d34]',
    ),
    'slot1Handles': (
        'lea esi, [edi + 0x8868]',
        'call 0x41f2a0',
        'mov dword ptr [esi], eax',
        'add esi, 0xe4',
    ),
    'slot1Delete': (
        'lea esi, [edi + 0x8868]',
        'mov dword ptr [esp + 0x14], 8',
        'mov ebx, 1',
        'call 0x467480',
        'add esi, 0xe4',
    ),
    'slot0Positions': (
        'lea esi, [edi + 0x8824]',
        'push 0xb',
        'push 0xc',
        'push 0',
        'call 0x4670b0',
    ),
    'slot0Store': ('mov dword ptr [esi + 0x40], ecx',),
    'spawn': ('inc dword ptr [edi + 0x130]', 'call 0x468010',
              'or dword ptr [esi + 0x4b4], 2'),
    'customConstructor': (
        'mov dword ptr [esi + 0x20], ebx',
        'call 0x4593b0',
        'call 0x466de0',
    ),
    'registerFirst': ('mov dword ptr [edx + 0x8b9704], ecx',),
    'deleteLifecycle': (
        'mov eax, dword ptr [esi + 0x4c8]',
        'mov word ptr [esi + 0x3f8], bx',
        'mov esi, dword ptr [esi + 0x14]',
    ),
    'callbackSetup': (
        'mov edx, dword ptr [0x4b1eac]',
        'mov dword ptr [esi + 0x4c8], edx',
    ),
    'callbackDefault': ('xor eax, eax', 'ret'),
    'stageClearSlots': (
        'or dword ptr [ebx + 0x438ec], 0x10',
        'mov edx, 3',
        'mov word ptr [esi + 0x3f8], ax',
        'mov dword ptr [edx + 0x438f0], 0',
    ),
    'groupUpdateCallSite': ('call 0x43d380',),
    'normalInitCall': ('call 0x43a180', 'call 0x43d380'),
}

SLOT0_SCRIPTS = [0xb, 0xc, 0xb, 0xc, 0xf, 0x10]
SLOT1_SCRIPTS = [0xe, 0xf, 0xd, 0xe, 0x11, 0x12]


def address(value):
    return f'0x{value:08x}'


def read(image, start, size):
    if type(size) is not int or not 0 < size <= 65536:
        raise ValueError('bounded read size')
    result = bytes(image.get_data(start - 0x400000, size))
    if len(result) != size:
        raise ValueError('truncated original read')
    return result


def decode(image, start, end, name):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    instructions = list(decoder.disasm(code, start))
    if sum(item.size for item in instructions) != len(code):
        raise ValueError(f'undecodable original slice {name}')
    return code, [{'address': address(item.address), 'bytes': item.bytes.hex(),
                   'text': f'{item.mnemonic} {item.op_str}'.strip()} for item in instructions]


def require_anchors(name, instructions):
    text = '\n'.join(item['text'] for item in instructions)
    missing = [anchor for anchor in ANCHORS[name] if anchor not in text]
    if missing:
        raise ValueError(f'{name} missing original anchors: {missing}')


def source_hash(path):
    data = (ROOT / path).read_bytes()
    return {'path': path, 'sha256': hashlib.sha256(data).hexdigest(), 'bytes': len(data)}


def load_catalog():
    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
    raw = (ROOT / 'local' / catalog['variants']['ja']['manifest']).parent / 'raw'
    return raw.resolve()


def decode_sht(raw):
    count = struct.unpack_from('<H', raw, 2)[0]
    offsets = [struct.unpack_from('<I', raw, 0x268 + index * 8)[0] for index in range(count)]
    groups = []
    records = 0
    for index, start in enumerate(offsets):
        successor = min((value for value in offsets if value > start), default=len(raw))
        cursor = start
        group_records = []
        while cursor < successor:
            period = struct.unpack_from('<b', raw, cursor)[0]
            if period < 0:
                break
            if period == 0 or successor - cursor < 52:
                raise ValueError('invalid actual SHT group record')
            values = struct.unpack_from('<2f', raw, cursor + 4)
            group_records.append({'offset': address(cursor), 'period': period,
                                  'relativeX': values[0], 'relativeY': values[1]})
            records += 1
            cursor += 52
        groups.append({'index': index, 'offset': address(start), 'recordCount': len(group_records),
                       'records': group_records[:8]})
    return {'groupCount': count, 'recordCount': records, 'groups': groups}


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        slices = {}
        for name, (start, end) in SLICES.items():
            code, instructions = decode(image, start, end, name)
            require_anchors(name, instructions)
            slices[name] = {'start': address(start), 'endExclusive': address(end),
                            'sha256': hashlib.sha256(code).hexdigest(),
                            'instructions': instructions}
        slot0_branches = list(struct.unpack('<6I', read(image, 0x43dbe0, 24)))
        slot1_branches = list(struct.unpack('<6I', read(image, 0x43dbc8, 24)))
        selector_table = list(struct.unpack('<96i', read(image, 0x4b7370, 0x180)))
        default_callback = struct.unpack('<I', read(image, 0x4b1eac, 4))[0]
        if default_callback != 0x412b40:
            raise ValueError('unexpected default ANM +0x4c8 callback')
        raw = load_catalog()
        sht_path = raw / 'pl00a.sht'
        anm_path = raw / 'pl00.anm'
        metadata_path = raw.parent / 'animations/pl00.json'
        sht = sht_path.read_bytes()
        animation = anm_path.read_bytes()
        metadata = json.loads(metadata_path.read_text(encoding='utf-8'))
        scripts = [item['index'] for item in metadata['scripts']] if 'scripts' in metadata else []
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'apiReturnsAssigned': False,
            'slices': slices,
            'selection': {
                'selectorTableAddress': address(0x4b7370),
                'selectorTable': [selector_table[index * 16:(index + 1) * 16] for index in range(6)],
                'slot0JumpTable': address(0x43dbe0),
                'slot1JumpTable': address(0x43dbc8),
                'slot0Branches': [address(value) for value in slot0_branches],
                'slot1Branches': [address(value) for value in slot1_branches],
                'slot0Scripts': SLOT0_SCRIPTS,
                'slot1Scripts': SLOT1_SCRIPTS,
                'layer': 0xc,
                'groupStride': 0xe4,
                'groupCount': 8,
                'slotsPerGroup': 2,
                'positionFields': [4, 8],
                'callbackCodes': {'groupDelete': 1, 'normalInit': 2, 'stageClear': 3},
                'groupDeleteCallSite': address(0x43d4f3),
                'stageClearCallSite': address(0x41fe20),
                'normalInitCallSite': address(0x43a180),
            },
            'nativeCallback': {
                'default4c8': address(default_callback),
                'returnsZero': True,
                'state3f8Offset': 0x3f8,
                'state3f8WriteCodes': {'groupDelete': 1, 'normalInit': 2, 'stageClear': 3},
                'childChain': '+0x18 conditional, +0x14 -> [node], +4 next',
                'vmInterruptEquivalent': False,
            },
            'resources': {
                'sht': {'path': str(sht_path.relative_to(ROOT)).replace('\\', '/'),
                        'sha256': hashlib.sha256(sht).hexdigest(), 'bytes': len(sht),
                        **decode_sht(sht)},
                'animation': {'path': str(anm_path.relative_to(ROOT)).replace('\\', '/'),
                              'sha256': hashlib.sha256(animation).hexdigest(), 'bytes': len(animation),
                              'scripts': scripts, 'requiredScriptsPresent':
                                  all(index in scripts for index in range(0xb, 0x13))},
            },
            'currentModel': {
                'playerShotHost': source_hash('src/game/PlayerShotHost.cpp'),
                'playerShotGroups': source_hash('src/game/PlayerShotGroups.cpp'),
                'sharedGroupSlotWrites': False,
                'selectionModel': 'actual pl00a.sht group records, first eight relative positions',
                'unresolved': '0x4b7370/SHT array position variant and non-pl00a resources',
            },
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/player-shot-groups-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = Path(arguments.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/player-shot-groups-fixtures.json')
    fixture = probe(Path(arguments.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('player shot-group slices %d, SHT groups %d records %d, scripts %s/%s'
          % (len(fixture['slices']), fixture['resources']['sht']['groupCount'],
             fixture['resources']['sht']['recordCount'], fixture['selection']['slot0Scripts'],
             fixture['selection']['slot1Scripts']))


if __name__ == '__main__':
    main()
