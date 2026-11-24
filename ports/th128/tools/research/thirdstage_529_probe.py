import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-thirdstage-529-fixtures/1'

SLICES = {
    'handler529': (0x41a286, 0x41a2ab),
    'alternate529': (0x41a2ab, 0x41a2d0),
    'callback41e730': (0x41e730, 0x41e855),
    'callback41e860': (0x41e860, 0x41e9ae),
    'callback41e9b0': (0x41e9b0, 0x41ea36),
    'callback41ea40': (0x41ea40, 0x41eaf6),
    'tick529': (0x4164e1, 0x41652e),
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


def handler(image, opcode):
    index = read(image, 0x41c7c0 + opcode - 256, 1)[0]
    target = struct.unpack('<I', read(image, 0x41c554 + 4 * index, 4))[0]
    return index, target


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        slices = {name: decode(image, start, end, name) for name, (start, end) in SLICES.items()}
        table = [struct.unpack('<I', read(image, 0x4b1e54 + 4 * index, 4))[0] for index in range(13)]
        panic = json.loads((ROOT / 'local/research/thirdstage-boss-fixtures.json').read_text(encoding='utf-8'))
        missing = {}
        for route in panic['routes']:
            targets = route['bossPath'].get('missingHostTargets', {}).get('529')
            if targets:
                missing.setdefault('targets', []).append(targets)
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'apiReturnsAssigned': False,
            'slices': slices,
            'dispatch': {
                'handlerTable': '0x41c554',
                'argumentTable': '0x41c7c0',
                'entries': {str(opcode): {'index': handler(image, opcode)[0],
                                          'handler': address(handler(image, opcode)[1])}
                            for opcode in (529, 530, 531, 534)},
            },
            'callbackTable': {
                'address': '0x4b1e54',
                'entries': [{'index': index, 'value': address(value)} for index, value in enumerate(table)],
            },
            'selectorMap': {
                '0': {'value': address(table[0]), 'meaning': 'no callback; +3fa0=0, +3fa4=0'},
                '2': {'value': address(table[2]), 'meaning': 'callback 0x41e860; +3fa4=0'},
                '3': {'value': address(table[3]), 'meaning': 'callback 0x41e9b0; +3fa4=0'},
                '1': {'value': address(table[1]), 'meaning': 'callback 0x41e730; +3fa4=0'},
                '4': {'value': address(table[4]), 'meaning': 'callback 0x41ea40; +3fa4=0'},
            },
            'handlerBehavior': {
                'writes': ['+0x3fa0 = table[selector]', '+0x3fa4 = 0'],
                'argumentRead': '0x41e0c0 reads one ECL integer argument',
                'alternateUnreferenced': '0x41a2ab writes +3fa4=1 but no direct caller/dispatch entry',
            },
            'callbackTick': {
                'site': '0x4164ee',
                'order': 'VM 0x46fe20 -> callback [+0x3fa0] -> flags/tail',
                'callbackArgument': 'ECX=entity',
                'fields': ['+0x34/+0x38 position', '+0x24c/+0x250 motion', '+0x3fa4 mode'],
                'globalPool': '0x4b8930+0x64, 2000 * 0x11b8',
            },
            'callbacks': {
                '0x41e730': 'per-bullet distance/motion helper; no initial direct dispatch',
                '0x41e860': 'iterates 2000 pool objects, filter word +0xa2a==1, calls 0x41f750, adds motion',
                '0x41e9b0': 'iterates pool, filter +0xa2a==1 and +0x11b0!=0x19, calls 0x40a990, adds motion',
                '0x41ea40': 'iterates pool, calls 0x41f750, adds motion',
            },
            'stageUse': {
                'missingHostTargets': missing.get('targets', []),
                'threeBosses': 'callbacks are entity-scoped (ECX) and shared by Boss2A/B/C, Boss3A/B/C and later three-boss records',
            },
            'limitations': [
                'No source patch applied; no browser/full-stage run.',
                'No exact StageWorld mapping for +0x24c/+0x250 or pool object +0xa2a/+0x11b0.',
                '0x41f750/0x40a990 are not ported and remain required callback helpers.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/thirdstage-529-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/thirdstage-529-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('529 slices %d, handler %s, selectors %s/%s/%s, callbacks %s/%s/%s'
          % (len(fixture['slices']), fixture['dispatch']['entries']['529']['handler'],
             fixture['selectorMap']['0']['value'], fixture['selectorMap']['2']['value'],
             fixture['selectorMap']['3']['value'], fixture['callbacks']['0x41e860'][:8],
             fixture['callbacks']['0x41e9b0'][:8], fixture['callbacks']['0x41ea40'][:8]))


if __name__ == '__main__':
    main()
