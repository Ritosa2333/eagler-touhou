import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-thirdstage-529-callees/1'

SLICES = {
    'setVelocity41f750': (0x41f750, 0x41f770),
    'selector2Call': (0x41e95b, 0x41e986),
    'selector3Call': (0x41e9e0, 0x41ea12),
    'convertType25': (0x40a990, 0x40aaf3),
}


def address(value):
    return f'0x{value:08x}'


def read(image, start, size):
    if type(size) is not int or not 0 < size <= 65536:
        raise ValueError('bounded read size')
    data = bytes(image.get_data(start - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def dword(image, location):
    return struct.unpack('<I', read(image, location, 4))[0]


def flt(image, location):
    return struct.unpack('<f', read(image, location, 4))[0]


def decode(image, start, end, name):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    instructions = list(decoder.disasm(code, start))
    if sum(item.size for item in instructions) != len(code):
        raise ValueError(f'undecodable original slice {name}')
    return {'start': address(start), 'endExclusive': address(end),
            'sha256': hashlib.sha256(code).hexdigest(),
            'instructions': [{'address': address(item.address), 'bytes': item.bytes.hex(),
                              'text': f'{item.mnemonic} {item.op_str}'.strip()}
                             for item in instructions]}


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'apiReturnsAssigned': False,
            'slices': {name: decode(image, start, end, name) for name, (start, end) in SLICES.items()},
            'selector2': {
                'callee': '0x41f750',
                'callSite': '0x41e97d',
                'outputPointer': 'bullet+0x9e4',
                'stackFloat8': 'bullet+0x9f4 (angle)',
                'stackFloatC': 'bullet+0x9f0 (radius/speed)',
                'writes': ['bullet+0x9e4 = cos(angle)*radius', 'bullet+0x9e8 = sin(angle)*radius'],
                'notChanged': ['type', 'state', 'motion state', 'life'],
            },
            'selector3': {
                'callee': '0x40a990',
                'callSite': '0x41ea0d',
                'receiver': 'EDI=pool bullet object',
                'writes': [
                    ['+0x11b0', 'type word = 0x19 (25)'],
                    ['+0x11ac', '0x4b40f0'],
                    ['+0x9f8', '6.0'],
                    ['+0x9fc', '6.0'],
                    ['+0xa00', '22.0'],
                    ['+0xa40', '0x4b41fc=2'],
                    ['+0x4e8', 'callback 0x40ccb0'],
                    ['+0x4ec', 'self pointer'],
                    ['+0x9a0', 'clear bit0, conditionally set bit0'],
                    ['+0xa1c', 'jump-table selected integer'],
                    ['+0', 'OR 0x10'],
                ],
                'externalServices': ['[0x4b8930]+0x8a7f9c manager', '0x4595f0 call', '[0x4b4200] jump index', '[0x4b420c] flag'],
            },
            'constants': {
                '0x4b41f4': flt(image, 0x4b41f4),
                '0x4b41f8': flt(image, 0x4b41f8),
                '0x4b41fc': dword(image, 0x4b41fc),
                '0x4b4200': dword(image, 0x4b4200),
                '0x4b420c': dword(image, 0x4b420c),
                '0x4b40f0': dword(image, 0x4b40f0),
            },
            'currentModel': {
                'birthVelocityFields': ['state.velocity.x', 'state.velocity.y'],
                'birthTypeField': 'type',
                'birthProfileValue': 'state.profileValue',
                'birthHitbox': 'state.hitboxSize',
                'missing': ['type25 +0x4e8 callback/+0x4ec owner',
                            'native +0x9f8/+0x9fc/+0xa00 exact typed fields',
                            'manager +0x8a7f9c/0x4595f0 service',
                            'entity +0x24c/+0x250 motion fields'],
            },
            'entityMotion': {
                'nativeFields': ['+0x24c', '+0x250'],
                'selector2Write': 'entity+0x24c += entity+0x250 at 0x41e9a1',
                'stageWorldMapping': 'not found in EnemySnapshot/StageWorld Entity',
                'verdict': 'unmapped; do not guess angle/speed',
            },
            'limitations': [
                'No source patch applied; no browser/full-stage run.',
                '0x41f750 is a pure fsincos vector setter; 0x40a990 is a type25 bullet converter.',
                '0x40ccb0 callback and 0x4595f0 manager call are not ported.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/thirdstage-529-callees-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/thirdstage-529-callees-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('529 callees: 41f750 velocity, 40a990 type25; slices %d; type %d; motion %s'
          % (len(fixture['slices']), fixture['selector3']['writes'][0][1].count('25'),
             fixture['entityMotion']['verdict']))


if __name__ == '__main__':
    main()
